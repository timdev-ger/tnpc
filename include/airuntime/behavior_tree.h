#pragma once

#include "airuntime/context.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace airuntime {

enum class Status {
    Success,
    Failure,
    Running
};

struct ActionOutcome {
    Status status{Status::Success};
    std::string decision;
    std::vector<std::string> blackboard_changes;
    std::vector<Intent> emitted_intents;
};

class Node {
public:
    explicit Node(std::string name);
    virtual ~Node() = default;

    const std::string& name() const;
    virtual Status tick(Context& context) = 0;

protected:
    void record_debug(Context& context,
                      const std::string& decision,
                      const std::vector<std::string>& changes,
                      const std::vector<Intent>& intents) const;

private:
    std::string name_;
};

class Sequence final : public Node {
public:
    explicit Sequence(std::string name);
    void add_child(std::unique_ptr<Node> child);
    Status tick(Context& context) override;

private:
    std::vector<std::unique_ptr<Node>> children_;
    size_t running_index_{0};
};

class Selector final : public Node {
public:
    explicit Selector(std::string name);
    void add_child(std::unique_ptr<Node> child);
    Status tick(Context& context) override;

private:
    std::vector<std::unique_ptr<Node>> children_;
    size_t running_index_{0};
};

class Condition final : public Node {
public:
    using Predicate = std::function<bool(Context&)>;

    Condition(std::string name, Predicate predicate);
    Status tick(Context& context) override;

private:
    Predicate predicate_;
};

class Action final : public Node {
public:
    using Handler = std::function<ActionOutcome(Context&)>;

    Action(std::string name, Handler handler);
    Status tick(Context& context) override;

private:
    Handler handler_;
};

class BehaviorTree {
public:
    explicit BehaviorTree(std::unique_ptr<Node> root);
    Status tick(Context& context);

private:
    std::unique_ptr<Node> root_;
};

}
