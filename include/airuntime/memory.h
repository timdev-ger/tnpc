#pragma once

#include "airuntime/types.h"

#include <optional>
#include <string>
#include <unordered_map>

namespace airuntime {

class Memory {
public:
    struct Fact {
        Variant value;
        double timestamp{0.0};
        double ttl_seconds{0.0};
    };

    void add_fact(const std::string& key, const Variant& value, double ttl_seconds);

    template <typename T>
    std::optional<T> query_fact(const std::string& key) const {
        auto it = facts_.find(key);
        if (it == facts_.end()) {
            return std::nullopt;
        }
        if (is_expired(it->second)) {
            return std::nullopt;
        }
        if (const auto* typed = std::get_if<T>(&it->second.value)) {
            return *typed;
        }
        return std::nullopt;
    }

    std::optional<Variant> query_fact(const std::string& key) const;

    void tick(double delta_seconds);
    double time_seconds() const;

private:
    bool is_expired(const Fact& fact) const;

    double time_seconds_{0.0};
    std::unordered_map<std::string, Fact> facts_;
};

}
