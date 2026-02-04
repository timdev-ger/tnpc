#include "airuntime/types.h"

#include <iomanip>
#include <sstream>

namespace airuntime {

std::string to_string(IntentType type) {
    switch (type) {
        case IntentType::MoveTo:
            return "MoveTo";
        case IntentType::TakeCover:
            return "TakeCover";
        case IntentType::Attack:
            return "Attack";
        case IntentType::Interact:
            return "Interact";
        case IntentType::Speak:
            return "Speak";
        case IntentType::Idle:
            return "Idle";
        default:
            return "Unknown";
    }
}

std::string to_string(const Vec3& value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << "(" << value.x << ", " << value.y << ", " << value.z << ")";
    return out.str();
}

std::string to_string(const Intent& intent) {
    std::ostringstream out;
    out << to_string(intent.type);
    if (!intent.target_id.empty()) {
        out << " target=" << intent.target_id;
    }
    if (!intent.payload.empty()) {
        out << " payload=" << intent.payload;
    }
    if (intent.position.x != 0.0 || intent.position.y != 0.0 || intent.position.z != 0.0) {
        out << " pos=" << to_string(intent.position);
    }
    out << " priority=" << intent.priority;
    return out.str();
}

std::string to_string(const Variant& value) {
    return std::visit(
        [](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, bool>) {
                return std::string(v ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream out;
                out << std::fixed << std::setprecision(3) << v;
                return out.str();
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else {
                return to_string(v);
            }
        },
        value);
}

}
