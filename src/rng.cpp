#include "airuntime/rng.h"

#include <algorithm>

namespace airuntime {

RNG::RNG(uint64_t seed_value) {
    seed(seed_value);
}

void RNG::seed(uint64_t seed_value) {
    state_ = seed_value;
}

uint64_t RNG::next_u64() {
    uint64_t z = (state_ += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

double RNG::next_double() {
    constexpr double kInv = 1.0 / static_cast<double>(1ull << 53);
    return static_cast<double>(next_u64() >> 11) * kInv;
}

int64_t RNG::next_int64(int64_t min_inclusive, int64_t max_inclusive) {
    if (min_inclusive > max_inclusive) {
        std::swap(min_inclusive, max_inclusive);
    }
    const uint64_t range = static_cast<uint64_t>(max_inclusive - min_inclusive + 1);
    return static_cast<int64_t>(min_inclusive + (next_u64() % range));
}

}
