#include "airuntime/perception.h"

namespace airuntime {

const PerceptionInput& PerceptionStub::input() const {
    return input_;
}

void PerceptionStub::set_input(const PerceptionInput& input) {
    input_ = input;
}

}
