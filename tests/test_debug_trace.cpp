#include "airuntime/debug_trace.h"
#include "doctest/doctest.h"

TEST_CASE("DebugTrace JSON export includes events") {
    airuntime::DebugTrace trace;
    airuntime::DebugEvent event{};
    event.time_seconds = 1.0;
    event.node_name = "TestNode";
    event.decision = "did something";
    event.blackboard_changes = {"state=idle"};
    event.emitted_intents.push_back(airuntime::Intent{airuntime::IntentType::Idle, "", {}, "", 0});
    trace.add_event(event);

    const auto json = trace.to_json();
    CHECK(json.find("\"node\":\"TestNode\"") != std::string::npos);
    CHECK(json.find("\"decision\":\"did something\"") != std::string::npos);
    CHECK(json.find("\"type\":\"Idle\"") != std::string::npos);
}
