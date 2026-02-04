#include "airuntime/blackboard.h"
#include "doctest/doctest.h"

TEST_CASE("Blackboard set/get/erase") {
    airuntime::Blackboard blackboard;
    blackboard.set("flag", true);
    blackboard.set("count", int64_t{7});

    REQUIRE(blackboard.has("flag"));
    CHECK(blackboard.get<bool>("flag").value_or(false) == true);
    CHECK(blackboard.get<int64_t>("count").value_or(0) == 7);

    blackboard.erase("flag");
    CHECK(!blackboard.has("flag"));
}
