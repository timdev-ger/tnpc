#pragma once

#include "airuntime/types.h"

#include <optional>
#include <vector>

namespace airuntime {

class IntentQueue {
public:
    void push(const Intent& intent);
    std::optional<Intent> pop_next();
    void clear();
    size_t size() const;

private:
    std::vector<Intent> intents_;
};

}
