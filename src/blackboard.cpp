#include "airuntime/blackboard.h"

namespace airuntime {

void Blackboard::set(const std::string& key, const Variant& value) {
    values_[key] = value;
}

bool Blackboard::has(const std::string& key) const {
    return values_.find(key) != values_.end();
}

void Blackboard::erase(const std::string& key) {
    values_.erase(key);
}

}
