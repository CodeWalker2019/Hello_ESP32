#include "transport/heartbeat/heartbeat_matcher.hpp"

HeartbeatMatcher::HeartbeatMatcher(const uint8_t* magic, size_t magicLen)
    : magic_(magic), magicLen_(magicLen) {}

bool HeartbeatMatcher::feed(uint8_t byte) {
    if (byte != magic_[matched_]) {
        matched_ = (byte == magic_[0]) ? 1 : 0;
        return false;
    }

    matched_++;

    if (matched_ == magicLen_) {
        matched_ = 0;
        return true;
    }

    return false;
}
