#include "test_utils.h"
#include "telemetry/CRC.h"

#include <cstdint>
#include <format>
#include <vector>

// Tests for telemetry::crc32()

static void test_empty_input() {
    // CRC of empty span should be deterministic
    std::vector<uint8_t> data{};
    uint32_t result = telemetry::crc32(data);
    uint32_t expected_value = 0x00000000;
    if (result == expected_value) {
        test_pass("crc32 empty input");
    } else {
        test_fail("crc32 empty input expected: " +
                    std::format("0x{:08X}", expected_value) +
                    " but got: " +
                    std::format("0x{:08X}", result)
        );
    }
    
}

static void test_known_value() {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03};
    uint32_t result = telemetry::crc32(data);
    uint32_t expected_value = 0xE4F296AE;
    if (result == expected_value) {
        test_pass("crc32 known value — 0xE4F296AE");
    } else {
        test_fail("crc32 known value. Expected: " +
                    std::format("0x{:08X}", expected_value) +
                    " but got: " +
                    std::format("0x{:08X}", result)
        );
    }

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