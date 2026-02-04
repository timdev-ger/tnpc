#include "airuntime/memory.h"

namespace airuntime {

void Memory::add_fact(const std::string& key, const Variant& value, double ttl_seconds) {
    facts_[key] = Fact{value, time_seconds_, ttl_seconds};
}

std::optional<Variant> Memory::query_fact(const std::string& key) const {
    auto it = facts_.find(key);
    if (it == facts_.end() || is_expired(it->second)) {
        return std::nullopt;
    }
    return it->second.value;
}

void Memory::tick(double delta_seconds) {
    time_seconds_ += delta_seconds;
    for (auto it = facts_.begin(); it != facts_.end();) {
        if (is_expired(it->second)) {
            it = facts_.erase(it);
        } else {
            ++it;
        }
    }
}

double Memory::time_seconds() const {
    return time_seconds_;
}

bool Memory::is_expired(const Fact& fact) const {
    return (time_seconds_ - fact.timestamp) >= fact.ttl_seconds;
}

}
