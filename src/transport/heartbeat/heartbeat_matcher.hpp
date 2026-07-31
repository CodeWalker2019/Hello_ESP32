/**
 * @file heartbeat_matcher.hpp
 * @brief Streaming byte-sequence matcher for locating a fixed magic
 *        header within an unstructured, continuous byte stream.
 *
 * Designed for parsing raw serial/UART streams where framing is not
 * guaranteed at the transport level (e.g. USB serial shared with plain
 * text debug logs). Rather than requiring a fixed-size read of exactly
 * N bytes at once, this matcher consumes one byte at a time and tracks
 * partial progress toward the target sequence.
 *
 * This design is resynchronization-safe: if a byte breaks the current
 * partial match, the matcher does not simply reset to zero — it checks
 * whether that same byte could itself be the start of a new match
 * (e.g. the first magic byte appearing again), so a single corrupted
 * or unexpected byte in the stream cannot permanently desynchronize
 * detection of subsequent, valid sequences.
 *
 * Usage is protocol-agnostic: the same class can match a short
 * heartbeat signature (e.g. 0xAA 0x55 0x02) or, with a different
 * magic buffer, the start of a binary telemetry packet header
 * (e.g. 0xAA 0x55 <device_id>).
 */

#ifndef HEARTBEAT_MATCHER_HPP
#define HEARTBEAT_MATCHER_HPP

#include <cstdint>
#include <cstddef>

class HeartbeatMatcher {
public:
    /**
     * @brief Constructs a matcher for a specific byte sequence.
     *
     * The matcher does not copy or own the magic sequence — the
     * caller must ensure `magic` remains valid for the lifetime of
     * this object (in practice, a static/constexpr array is expected).
     *
     * @param magic Pointer to the byte sequence to match against.
     * @param magicLen Number of bytes in the sequence.
     */
    HeartbeatMatcher(const uint8_t* magic, size_t magicLen);

    /**
     * @brief Feeds a single incoming byte to the matcher.
     *
     * Call this once per byte read from the transport, in the order
     * bytes arrive. Internally advances (or resets/resynchronizes)
     * the match progress based on whether the byte continues the
     * expected sequence.
     *
     * @param byte The next byte read from the incoming stream.
     * @return true if this byte completes a full match of the target
     *         sequence (the matcher automatically resets and is ready
     *         to detect the next occurrence); false otherwise.
     */
    bool feed(uint8_t byte);

private:
    const uint8_t* magic_;   ///< Target byte sequence to detect (not owned).
    size_t magicLen_;        ///< Length of the target sequence in bytes.
    size_t matched_ = 0;     ///< Number of consecutive bytes matched so far.
};

#endif