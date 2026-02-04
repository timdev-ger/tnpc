#pragma once

#include "airuntime/context.h"
#include "airuntime/perception.h"
#include "airuntime/types.h"

#include <string>
#include <vector>

namespace airuntime::adapters {

class IPerceptionAdapter {
public:
    virtual ~IPerceptionAdapter() = default;
    virtual void pull_from_engine(PerceptionInput& out_input) = 0;
};

class IIntentExecutor {
public:
    virtual ~IIntentExecutor() = default;
    virtual void execute(const Intent& intent) = 0;
};

class IEngineBridge {
public:
    virtual ~IEngineBridge() = default;
    virtual void begin_tick(Context& context) = 0;
    virtual void end_tick(Context& context) = 0;
};

class UnityAdapterStub final : public IPerceptionAdapter, public IIntentExecutor, public IEngineBridge {
public:
    void pull_from_engine(PerceptionInput& out_input) override;
    void execute(const Intent& intent) override;
    void begin_tick(Context& context) override;
    void end_tick(Context& context) override;
};

class UnrealAdapterStub final : public IPerceptionAdapter, public IIntentExecutor, public IEngineBridge {
public:
    void pull_from_engine(PerceptionInput& out_input) override;
    void execute(const Intent& intent) override;
    void begin_tick(Context& context) override;
    void end_tick(Context& context) override;
};

}
