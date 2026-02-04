#pragma once

namespace airuntime {

struct PerceptionInput {
    bool seesThreat{false};
    double threatDistance{0.0};
    bool isHungry{false};
    double hunger{0.0};
};

class IPerception {
public:
    virtual ~IPerception() = default;
    virtual const PerceptionInput& input() const = 0;
    virtual void set_input(const PerceptionInput& input) = 0;
};

class PerceptionStub final : public IPerception {
public:
    const PerceptionInput& input() const override;
    void set_input(const PerceptionInput& input) override;

private:
    PerceptionInput input_{};
};

}
