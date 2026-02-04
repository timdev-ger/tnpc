#include "airuntime/behavior_tree.h"
#include "airuntime/context.h"
#include "airuntime/perception.h"
#include "airuntime/rng.h"
#include "doctest/doctest.h"

TEST_CASE("BehaviorTree selector chooses threat") {
    airuntime::RNG rng(1);
    airuntime::Blackboard blackboard;
    airuntime::Memory memory;
    airuntime::PerceptionStub perception;
    airuntime::IntentQueue intents;
    airuntime::DebugTrace trace;

    airuntime::Context ctx{blackboard, memory, perception, intents, trace, rng, 1.0, 0.0};

    auto root = std::make_unique<airuntime::Selector>("RootSelector");
    auto threat_seq = std::make_unique<airuntime::Sequence>("ThreatSeq");
    threat_seq->add_child(std::make_unique<airuntime::Condition>(
        "SeesThreat", [](airuntime::Context& c) { return c.perception.input().seesThreat; }));
    threat_seq->add_child(std::make_unique<airuntime::Action>(
        "TakeCover", [](airuntime::Context& c) {
            airuntime::ActionOutcome outcome{};
            outcome.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::TakeCover, "", {}, "", 10});
            return outcome;
        }));

    auto idle_action = std::make_unique<airuntime::Action>(
        "Idle", [](airuntime::Context& c) {
            airuntime::ActionOutcome outcome{};
            outcome.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::Idle, "", {}, "", 0});
            return outcome;
        });

    root->add_child(std::move(threat_seq));
    root->add_child(std::move(idle_action));

    airuntime::BehaviorTree tree(std::move(root));

    airuntime::PerceptionInput input{};
    input.seesThreat = true;
    perception.set_input(input);

    tree.tick(ctx);

    auto intent = intents.pop_next();
    REQUIRE(intent.has_value());
    CHECK(intent->type == airuntime::IntentType::TakeCover);
}
