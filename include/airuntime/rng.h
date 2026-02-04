#pragma once

#include <cstdint>

namespace airuntime {

class RNG {
public:
    explicit RNG(uint64_t seed = 0U);

    void seed(uint64_t seed);
    uint64_t next_u64();
    double next_double();
    int64_t next_int64(int64_t min_inclusive, int64_t max_inclusive);

private:
    uint64_t state_{0U};
};

}
