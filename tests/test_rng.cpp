#include "airuntime/rng.h"
#include "doctest/doctest.h"

TEST_CASE("RNG deterministic sequence") {
    airuntime::RNG rng_a(42);
    airuntime::RNG rng_b(42);

    for (int i = 0; i < 5; ++i) {
        CHECK(rng_a.next_u64() == rng_b.next_u64());
    }
}
