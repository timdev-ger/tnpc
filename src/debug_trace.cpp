#include "airuntime/debug_trace.h"

#include <sstream>
#include <string>

namespace airuntime {

void DebugTrace::clear() {
    events_.clear();
}

void DebugTrace::add_event(const DebugEvent& event) {
    events_.push_back(event);
}

const std::vector<DebugEvent>& DebugTrace::events() const {
    return events_;
}

static std::string join_strings(const std::vector<std::string>& values) {
    std::ostringstream out;
    for (size_t i = 0; i < values.size(); ++i) {
        out << values[i];
        if (i + 1 < values.size()) {
            out << ", ";
        }
    }
    return out.str();
}

static std::string escape_json(const std::string& value) {
    std::ostringstream out;
    for (char ch : value) {
        switch (ch) {
            case '"':
                out << "\\\"";
                break;
            case '\\':
                out << "\\\\";
                break;
            case '\n':
                out << "\\n";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\t':
                out << "\\t";
                break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    out << "\\u" << std::hex << std::uppercase << static_cast<int>(ch) << std::dec;
                } else {
                    out << ch;
                }
                break;
        }
    }
    return out.str();
}

static std::string json_string_array(const std::vector<std::string>& values) {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < values.size(); ++i) {
        out << "\"" << escape_json(values[i]) << "\"";
        if (i + 1 < values.size()) {
            out << ",";
        }
    }
    out << "]";
    return out.str();
}

std::vector<std::string> DebugTrace::to_lines(bool include_intents) const {
    std::vector<std::string> lines;
    lines.reserve(events_.size());

    for (const auto& event : events_) {
        std::ostringstream out;
        out << "[t=" << event.time_seconds << "] ";
        out << "Node=" << event.node_name << " decision=" << event.decision;

        if (!event.blackboard_changes.empty()) {
            out << " changes=[" << join_strings(event.blackboard_changes) << "]";
        }
        if (include_intents && !event.emitted_intents.empty()) {
            std::vector<std::string> intent_lines;
            intent_lines.reserve(event.emitted_intents.size());
            for (const auto& intent : event.emitted_intents) {
                intent_lines.push_back(to_string(intent));
            }
            out << " intents=[" << join_strings(intent_lines) << "]";
        }

        lines.push_back(out.str());
    }

    return lines;
}

std::string DebugTrace::to_json(bool include_intents) const {
    std::ostringstream out;
    out << "{";
    out << "\"events\":[";

    for (size_t i = 0; i < events_.size(); ++i) {
        const auto& event = events_[i];
        out << "{";
        out << "\"time\":" << event.time_seconds << ",";
        out << "\"node\":\"" << escape_json(event.node_name) << "\",";
        out << "\"decision\":\"" << escape_json(event.decision) << "\",";
        out << "\"blackboardChanges\":" << json_string_array(event.blackboard_changes);

        if (include_intents) {
            out << ",\"intents\":[";
            for (size_t j = 0; j < event.emitted_intents.size(); ++j) {
                const auto& intent = event.emitted_intents[j];
                out << "{";
                out << "\"type\":\"" << escape_json(to_string(intent.type)) << "\",";
                out << "\"targetId\":\"" << escape_json(intent.target_id) << "\",";
                out << "\"payload\":\"" << escape_json(intent.payload) << "\",";
                out << "\"priority\":" << intent.priority << ",";
                out << "\"position\":{";
                out << "\"x\":" << intent.position.x << ",";
                out << "\"y\":" << intent.position.y << ",";
                out << "\"z\":" << intent.position.z << "}";
                out << "}";
                if (j + 1 < event.emitted_intents.size()) {
                    out << ",";
                }
            }
            out << "]";
        }

        out << "}";
        if (i + 1 < events_.size()) {
            out << ",";
        }
    }

    out << "]}";
    return out.str();
}

}
