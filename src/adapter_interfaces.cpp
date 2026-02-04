#include "airuntime/adapters/adapter_interfaces.h"

namespace airuntime::adapters {

void UnityAdapterStub::pull_from_engine(PerceptionInput& out_input) {
    out_input = PerceptionInput{};
}

void UnityAdapterStub::execute(const Intent&) {}

void UnityAdapterStub::begin_tick(Context&) {}

void UnityAdapterStub::end_tick(Context&) {}

void UnrealAdapterStub::pull_from_engine(PerceptionInput& out_input) {
    out_input = PerceptionInput{};
}

void UnrealAdapterStub::execute(const Intent&) {}

void UnrealAdapterStub::begin_tick(Context&) {}

void UnrealAdapterStub::end_tick(Context&) {}

}
