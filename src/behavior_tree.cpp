#include "airuntime/behavior_tree.h"

#include <utility>

namespace airuntime {

Node::Node(std::string name) : name_(std::move(name)) {}

const std::string& Node::name() const {
    return name_;
}

void Node::record_debug(Context& context,
                        const std::string& decision,
                        const std::vector<std::string>& changes,
                        const std::vector<Intent>& intents) const {
    DebugEvent event{};
    event.time_seconds = context.time_seconds;
    event.node_name = name_;
    event.decision = decision;
    event.blackboard_changes = changes;
    event.emitted_intents = intents;
    context.trace.add_event(event);
}

Sequence::Sequence(std::string name) : Node(std::move(name)) {}

void Sequence::add_child(std::unique_ptr<Node> child) {
    children_.push_back(std::move(child));
}

Status Sequence::tick(Context& context) {
    for (size_t i = running_index_; i < children_.size(); ++i) {
        auto status = children_[i]->tick(context);
        if (status == Status::Running) {
            running_index_ = i;
            return Status::Running;
        }
        if (status == Status::Failure) {
            running_index_ = 0;
            return Status::Failure;
        }
    }
    running_index_ = 0;
    return Status::Success;
}

Selector::Selector(std::string name) : Node(std::move(name)) {}

void Selector::add_child(std::unique_ptr<Node> child) {
    children_.push_back(std::move(child));
}

Status Selector::tick(Context& context) {
    for (size_t i = running_index_; i < children_.size(); ++i) {
        auto status = children_[i]->tick(context);
        if (status == Status::Running) {
            running_index_ = i;
            return Status::Running;
        }
        if (status == Status::Success) {
            running_index_ = 0;
            return Status::Success;
        }
    }
    running_index_ = 0;
    return Status::Failure;
}

Condition::Condition(std::string name, Predicate predicate)
    : Node(std::move(name)), predicate_(std::move(predicate)) {}

Status Condition::tick(Context& context) {
    const bool result = predicate_(context);
    record_debug(context, result ? "true" : "false", {}, {});
    return result ? Status::Success : Status::Failure;
}

Action::Action(std::string name, Handler handler) : Node(std::move(name)), handler_(std::move(handler)) {}

Status Action::tick(Context& context) {
    ActionOutcome outcome = handler_(context);
    for (const auto& intent : outcome.emitted_intents) {
        context.intents.push(intent);
    }

    const std::string decision = outcome.decision.empty() ? (outcome.status == Status::Success ? "success" : "failure")
                                                          : outcome.decision;
    record_debug(context, decision, outcome.blackboard_changes, outcome.emitted_intents);
    return outcome.status;
}

BehaviorTree::BehaviorTree(std::unique_ptr<Node> root) : root_(std::move(root)) {}

Status BehaviorTree::tick(Context& context) {
    return root_ ? root_->tick(context) : Status::Failure;
}

}
