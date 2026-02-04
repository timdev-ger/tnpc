#include "airuntime/behavior_tree.h"
#include "airuntime/context.h"
#include "airuntime/intent_queue.h"
#include "airuntime/perception.h"
#include "airuntime/rng.h"
#include "airuntime/types.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

struct Options {
    uint64_t seed{42};
    int ticks{30};
    int agents{2};
    bool verbose{false};
};

void print_usage() {
    std::cout << "sim_demo --seed <int> --ticks <int> --agents <int> --verbose\n";
}

Options parse_args(int argc, char** argv) {
    Options options{};
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--seed" && i + 1 < argc) {
            options.seed = static_cast<uint64_t>(std::stoull(argv[++i]));
        } else if (arg == "--ticks" && i + 1 < argc) {
            options.ticks = std::max(1, std::stoi(argv[++i]));
        } else if (arg == "--agents" && i + 1 < argc) {
            options.agents = std::max(1, std::stoi(argv[++i]));
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else {
            print_usage();
            std::exit(1);
        }
    }
    return options;
}

}

namespace {

struct Agent {
    std::string name;
    airuntime::RNG rng;
    airuntime::Blackboard blackboard;
    airuntime::Memory memory;
    airuntime::PerceptionStub perception;
    airuntime::IntentQueue intents;
    airuntime::DebugTrace trace;
    airuntime::BehaviorTree tree;
    airuntime::Context context;

    Agent(std::string label, uint64_t seed)
        : name(std::move(label)),
          rng(seed),
          tree(nullptr),
          context{blackboard, memory, perception, intents, trace, rng, 1.0, 0.0} {
        blackboard.set("hunger", 0.65);
        blackboard.set("state", std::string("idle"));
        blackboard.set("seeking_ticks", int64_t{0});

        auto root = std::make_unique<airuntime::Selector>("RootSelector");

        auto threat_sequence = std::make_unique<airuntime::Sequence>("ThreatSequence");
        threat_sequence->add_child(std::make_unique<airuntime::Condition>(
            "SeesThreat", [](airuntime::Context& ctx) { return ctx.perception.input().seesThreat; }));
        threat_sequence->add_child(std::make_unique<airuntime::Action>(
            "TakeCoverAction", [](airuntime::Context& ctx) {
                airuntime::ActionOutcome outcome{};
                outcome.status = airuntime::Status::Success;
                outcome.decision = "Emit TakeCover";
                airuntime::Intent intent{airuntime::IntentType::TakeCover, "", {}, "threat", 100};
                outcome.emitted_intents.push_back(intent);
                outcome.blackboard_changes.push_back("state=afraid");
                ctx.blackboard.set("state", std::string("afraid"));
                ctx.blackboard.set("seeking_ticks", int64_t{0});
                return outcome;
            }));

        auto hunger_sequence = std::make_unique<airuntime::Sequence>("HungerSequence");
        hunger_sequence->add_child(std::make_unique<airuntime::Condition>(
            "IsHungry", [](airuntime::Context& ctx) { return ctx.perception.input().hunger > 0.7; }));
        hunger_sequence->add_child(std::make_unique<airuntime::Action>(
            "SeekFoodAction", [](airuntime::Context& ctx) {
                airuntime::ActionOutcome outcome{};
                outcome.status = airuntime::Status::Success;
                outcome.decision = "Emit MoveTo(food)";
                airuntime::Intent intent{airuntime::IntentType::MoveTo, "food", {}, "seek_food", 50};
                outcome.emitted_intents.push_back(intent);
                outcome.blackboard_changes.push_back("state=seeking_food");
                ctx.blackboard.set("state", std::string("seeking_food"));
                return outcome;
            }));

        auto idle_action = std::make_unique<airuntime::Action>(
            "IdleAction", [](airuntime::Context& ctx) {
                airuntime::ActionOutcome outcome{};
                outcome.status = airuntime::Status::Success;
                outcome.decision = "Emit Idle";
                airuntime::Intent intent{airuntime::IntentType::Idle, "", {}, "", 0};
                outcome.emitted_intents.push_back(intent);
                outcome.blackboard_changes.push_back("state=idle");
                ctx.blackboard.set("state", std::string("idle"));
                return outcome;
            });

        root->add_child(std::move(threat_sequence));
        root->add_child(std::move(hunger_sequence));
        root->add_child(std::move(idle_action));

        tree = airuntime::BehaviorTree(std::move(root));
    }
};

void update_agent_state(Agent& agent, int tick) {
    auto& ctx = agent.context;
    ctx.time_seconds = static_cast<double>(tick) * ctx.delta_seconds;
    agent.memory.tick(ctx.delta_seconds);

    auto hunger = agent.blackboard.get<double>("hunger").value_or(0.0);
    auto state = agent.blackboard.get<std::string>("state").value_or("idle");
    auto seeking_ticks = agent.blackboard.get<int64_t>("seeking_ticks").value_or(0);

    if (state == "seeking_food") {
        seeking_ticks += 1;
        hunger = std::min(1.0, hunger + 0.03);
        if (seeking_ticks >= 3) {
            hunger = std::max(0.1, hunger - 0.7);
            state = "idle";
            seeking_ticks = 0;
            airuntime::DebugEvent event{};
            event.time_seconds = ctx.time_seconds;
            event.node_name = agent.name + "::SimUpdate";
            event.decision = "food found";
            event.blackboard_changes = {"state=idle", "hunger=" + airuntime::to_string(hunger)};
            agent.trace.add_event(event);
        }
    } else {
        hunger = std::min(1.0, hunger + 0.03);
    }

    agent.blackboard.set("hunger", hunger);
    agent.blackboard.set("state", state);
    agent.blackboard.set("seeking_ticks", static_cast<int64_t>(seeking_ticks));

    airuntime::PerceptionInput input{};
    input.seesThreat = (tick >= 10 && tick < 20);
    input.threatDistance = input.seesThreat ? 5.0 : 0.0;
    input.hunger = hunger;
    input.isHungry = hunger > 0.7;
    agent.perception.set_input(input);

    agent.tree.tick(ctx);
}

}

int main(int argc, char** argv) {
    Options options = parse_args(argc, argv);

    std::vector<Agent> agents;
    agents.reserve(static_cast<size_t>(options.agents));
    for (int i = 0; i < options.agents; ++i) {
        agents.emplace_back("NPC" + std::to_string(i + 1), options.seed + static_cast<uint64_t>(i * 17));
    }

    std::map<airuntime::IntentType, int> intent_counts;

    for (int tick = 0; tick < options.ticks; ++tick) {
        for (auto& agent : agents) {
            update_agent_state(agent, tick);

            while (auto intent = agent.intents.pop_next()) {
                intent_counts[intent->type] += 1;
                if (options.verbose) {
                    std::cout << "[tick " << tick << "] " << agent.name
                              << " executed intent: " << airuntime::to_string(*intent) << "\n";
                }
            }
        }
    }

    std::cout << "\nSummary\n";
    for (const auto& pair : intent_counts) {
        std::cout << "  " << airuntime::to_string(pair.first) << ": " << pair.second << "\n";
    }

    if (!agents.empty()) {
        const auto last_state = agents.front().blackboard.get<std::string>("state").value_or("unknown");
        std::cout << "Last state (" << agents.front().name << "): " << last_state << "\n\n";
    }

    if (!agents.empty()) {
        auto lines = agents.front().trace.to_lines();
        if (options.verbose) {
            std::cout << "DebugTrace (" << agents.front().name << ")\n";
            for (const auto& line : lines) {
                std::cout << line << "\n";
            }
        } else {
            std::cout << "DebugTrace (first 5 lines, " << agents.front().name << ")\n";
            for (size_t i = 0; i < std::min<size_t>(5, lines.size()); ++i) {
                std::cout << lines[i] << "\n";
            }
        }
    }

    return 0;
}
