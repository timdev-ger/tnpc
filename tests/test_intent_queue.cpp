#include "airuntime/intent_queue.h"
#include "doctest/doctest.h"

TEST_CASE("IntentQueue priority stable ordering") {
    airuntime::IntentQueue queue;
    queue.push(airuntime::Intent{airuntime::IntentType::Idle, "", {}, "A", 1});
    queue.push(airuntime::Intent{airuntime::IntentType::MoveTo, "", {}, "B", 5});
    queue.push(airuntime::Intent{airuntime::IntentType::TakeCover, "", {}, "C", 5});

    auto first = queue.pop_next();
    REQUIRE(first.has_value());
    CHECK(first->payload == "B");

    auto second = queue.pop_next();
    REQUIRE(second.has_value());
    CHECK(second->payload == "C");

    auto third = queue.pop_next();
    REQUIRE(third.has_value());
    CHECK(third->payload == "A");
}
