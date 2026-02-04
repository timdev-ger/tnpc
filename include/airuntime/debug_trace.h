#pragma once

#include "airuntime/types.h"

#include <string>
#include <vector>

namespace airuntime {

struct DebugEvent {
    double time_seconds{0.0};
    std::string node_name;
    std::string decision;
    std::vector<std::string> blackboard_changes;
    std::vector<Intent> emitted_intents;
};

class DebugTrace {
public:
    void clear();
    void add_event(const DebugEvent& event);

    const std::vector<DebugEvent>& events() const;
    std::vector<std::string> to_lines(bool include_intents = true) const;
    std::string to_json(bool include_intents = true) const;

private:
    std::vector<DebugEvent> events_;
};

}
