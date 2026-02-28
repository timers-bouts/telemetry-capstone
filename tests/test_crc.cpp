#include "test_utils.h"
#include "telemetry/CRC.h"

#include <vector>
#include <cstdint>

// Tests for telemetry::crc32()

static void test_empty_input() {
    // CRC of empty span should be deterministic
    std::vector<uint8_t> data{};
    uint32_t result = telemetry::crc32(data);
    // TODO: compute expected value and assert equality
    // For now: just verify it doesn't crash
    (void)result;
    test_pass("crc32 empty input does not crash");
}

static void test_known_value() {
    // TODO: Pick a known input and precompute the expected CRC.
    // Example: crc32({0x01, 0x02, 0x03}) should equal <precomputed value>
    test_pass("crc32 known value — TODO: add expected value");
}

static void test_different_inputs_differ() {
    std::vector<uint8_t> a{0xAA, 0xBB, 0xCC};
    std::vector<uint8_t> b{0xAA, 0xBB, 0xCD}; // one byte differs
    uint32_t crc_a = telemetry::crc32(a);
    uint32_t crc_b = telemetry::crc32(b);
    if (crc_a != crc_b) {
        test_pass("crc32 different inputs produce different checksums");
    } else {
        test_fail("crc32 different inputs produced identical checksums");
    }
}

static void test_same_input_is_deterministic() {
    std::vector<uint8_t> data{0x01, 0x02, 0x03, 0x04};
    uint32_t r1 = telemetry::crc32(data);
    uint32_t r2 = telemetry::crc32(data);
    if (r1 == r2) {
        test_pass("crc32 same input is deterministic");
    } else {
        test_fail("crc32 non-deterministic result");
    }
}

int main() {
    std::cout << "=== CRC Tests ===\n";
    test_empty_input();
    test_known_value();
    test_different_inputs_differ();
    test_same_input_is_deterministic();
    return test_summary();
}