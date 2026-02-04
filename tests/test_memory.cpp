#include "airuntime/memory.h"
#include "doctest/doctest.h"

TEST_CASE("Memory TTL expires facts") {
    airuntime::Memory memory;
    memory.add_fact("saw_player", true, 1.0);

    CHECK(memory.query_fact<bool>("saw_player").value_or(false) == true);

    memory.tick(0.6);
    CHECK(memory.query_fact<bool>("saw_player").value_or(false) == true);

    memory.tick(0.5);
    CHECK(!memory.query_fact<bool>("saw_player").has_value());
}
