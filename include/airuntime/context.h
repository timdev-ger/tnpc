#pragma once

#include "airuntime/blackboard.h"
#include "airuntime/debug_trace.h"
#include "airuntime/intent_queue.h"
#include "airuntime/memory.h"
#include "airuntime/perception.h"
#include "airuntime/rng.h"

namespace airuntime {

struct Context {
    Blackboard& blackboard;
    Memory& memory;
    IPerception& perception;
    IntentQueue& intents;
    DebugTrace& trace;
    RNG& rng;
    double delta_seconds{0.0};
    double time_seconds{0.0};
};

}
