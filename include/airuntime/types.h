#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace airuntime {

struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

enum class IntentType {
    MoveTo,
    TakeCover,
    Attack,
    Interact,
    Speak,
    Idle
};

struct Intent {
    IntentType type{IntentType::Idle};
    std::string target_id;
    Vec3 position{};
    std::string payload;
    int priority{0};
};

using Variant = std::variant<bool, int64_t, double, std::string, Vec3>;

std::string to_string(IntentType type);
std::string to_string(const Vec3& value);
std::string to_string(const Intent& intent);
std::string to_string(const Variant& value);

}
