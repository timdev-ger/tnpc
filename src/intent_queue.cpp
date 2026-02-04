#include "airuntime/intent_queue.h"

namespace airuntime {

void IntentQueue::push(const Intent& intent) {
    intents_.push_back(intent);
}

std::optional<Intent> IntentQueue::pop_next() {
    if (intents_.empty()) {
        return std::nullopt;
    }

    size_t best_index = 0;
    int best_priority = intents_.front().priority;
    for (size_t i = 1; i < intents_.size(); ++i) {
        if (intents_[i].priority > best_priority) {
            best_priority = intents_[i].priority;
            best_index = i;
        }
    }

    Intent next = intents_[best_index];
    intents_.erase(intents_.begin() + static_cast<std::ptrdiff_t>(best_index));
    return next;
}

void IntentQueue::clear() {
    intents_.clear();
}

size_t IntentQueue::size() const {
    return intents_.size();
}

}
