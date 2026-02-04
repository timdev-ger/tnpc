#include "airuntime/behavior_tree.h"
#include "airuntime/context.h"
#include "airuntime/perception.h"
#include "airuntime/rng.h"
#include "doctest/doctest.h"

#include <vector>

namespace {

std::vector<airuntime::IntentType> run_demo_simulation(int ticks) {
    airuntime::RNG rng(42);
    airuntime::Blackboard blackboard;
    airuntime::Memory memory;
    airuntime::PerceptionStub perception;
    airuntime::IntentQueue intent_queue;
    airuntime::DebugTrace trace;

    blackboard.set("hunger", 0.65);
    blackboard.set("state", std::string("idle"));
    blackboard.set("seeking_ticks", int64_t{0});

    airuntime::Context context{blackboard, memory, perception, intent_queue, trace, rng, 1.0, 0.0};

    auto root = std::make_unique<airuntime::Selector>("RootSelector");
    auto threat_sequence = std::make_unique<airuntime::Sequence>("ThreatSequence");
    threat_sequence->add_child(std::make_unique<airuntime::Condition>(
        "SeesThreat", [](airuntime::Context& ctx) { return ctx.perception.input().seesThreat; }));
    threat_sequence->add_child(std::make_unique<airuntime::Action>(
        "TakeCoverAction", [](airuntime::Context& ctx) {
            airuntime::ActionOutcome outcome{};
            outcome.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::TakeCover, "", {}, "", 100});
            ctx.blackboard.set("state", std::string("afraid"));
            return outcome;
        }));

    auto hunger_sequence = std::make_unique<airuntime::Sequence>("HungerSequence");
    hunger_sequence->add_child(std::make_unique<airuntime::Condition>(
        "IsHungry", [](airuntime::Context& ctx) { return ctx.perception.input().hunger > 0.7; }));
    hunger_sequence->add_child(std::make_unique<airuntime::Action>(
        "SeekFoodAction", [](airuntime::Context& ctx) {
            airuntime::ActionOutcome outcome{};
            outcome.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::MoveTo, "food", {}, "", 50});
            ctx.blackboard.set("state", std::string("seeking_food"));
            return outcome;
        }));

    auto idle_action = std::make_unique<airuntime::Action>(
        "IdleAction", [](airuntime::Context& ctx) {
            airuntime::ActionOutcome outcome{};
            outcome.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::Idle, "", {}, "", 0});
            ctx.blackboard.set("state", std::string("idle"));
            return outcome;
        });

    root->add_child(std::move(threat_sequence));
    root->add_child(std::move(hunger_sequence));
    root->add_child(std::move(idle_action));

    airuntime::BehaviorTree tree(std::move(root));

    std::vector<airuntime::IntentType> intents;
    for (int tick = 0; tick < ticks; ++tick) {
        context.time_seconds = static_cast<double>(tick);
        memory.tick(1.0);

        auto hunger = blackboard.get<double>("hunger").value_or(0.0);
        auto state = blackboard.get<std::string>("state").value_or("idle");
        auto seeking_ticks = blackboard.get<int64_t>("seeking_ticks").value_or(0);

        if (state == "seeking_food") {
            seeking_ticks += 1;
            hunger = std::min(1.0, hunger + 0.03);
            if (seeking_ticks >= 3) {
                hunger = std::max(0.1, hunger - 0.7);
                state = "idle";
                seeking_ticks = 0;
            }
        } else {
            hunger = std::min(1.0, hunger + 0.03);
        }

        blackboard.set("hunger", hunger);
        blackboard.set("state", state);
        blackboard.set("seeking_ticks", seeking_ticks);

        airuntime::PerceptionInput input{};
        input.seesThreat = (tick >= 10 && tick < 20);
        input.hunger = hunger;
        input.isHungry = hunger > 0.7;
        perception.set_input(input);

        tree.tick(context);

        while (auto intent = intent_queue.pop_next()) {
            intents.push_back(intent->type);
        }
    }

    return intents;
}

}

TEST_CASE("Demo intent sequence is deterministic") {
    auto intents = run_demo_simulation(5);
    REQUIRE(intents.size() == 5);
    CHECK(intents[0] == airuntime::IntentType::Idle);
    CHECK(intents[1] == airuntime::IntentType::MoveTo);
    CHECK(intents[2] == airuntime::IntentType::MoveTo);
    CHECK(intents[3] == airuntime::IntentType::MoveTo);
    CHECK(intents[4] == airuntime::IntentType::Idle);
}
