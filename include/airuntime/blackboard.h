#pragma once

#include "airuntime/types.h"

#include <optional>
#include <string>
#include <unordered_map>

namespace airuntime {

class Blackboard {
public:
    void set(const std::string& key, const Variant& value);

    template <typename T>
    void set(const std::string& key, T value) {
        set(key, Variant{value});
    }

    template <typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = values_.find(key);
        if (it == values_.end()) {
            return std::nullopt;
        }
        if (const auto* typed = std::get_if<T>(&it->second)) {
            return *typed;
        }
        return std::nullopt;
    }

    bool has(const std::string& key) const;
    void erase(const std::string& key);

private:
    std::unordered_map<std::string, Variant> values_;
};

}
