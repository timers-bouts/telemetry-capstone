#include "test_utils.h"
#include "telemetry/PacketWriter.h"
#include "telemetry/PacketReader.h"

#include <stdexcept>
#include <vector>

// Tests for telemetry::PacketReader

static void test_read_u64_roundtrip() {
    telemetry::PacketWriter pw;
    pw.add_u64(0xDEADBEEFCAFEBABEULL);
    telemetry::PacketReader pr(pw.bytes());
    uint64_t val = pr.read_u64();
    if (val == 0xDEADBEEFCAFEBABEULL) {
        test_pass("PacketReader read_u64 roundtrip");
    } else {
        test_fail("PacketReader read_u64 roundtrip", "value mismatch");
    }
}

static void test_remaining_decrements() {
    telemetry::PacketWriter pw;
    pw.add_u32(0x12345678);
    telemetry::PacketReader pr(pw.bytes());
    size_t before = pr.remaining();
    pr.read_u16();
    size_t after = pr.remaining();
    if (before == 4 && after == 2) {
        test_pass("PacketReader remaining() decrements after read");
    } else {
        test_fail("PacketReader remaining() incorrect");
    }
}

static void test_out_of_range_throws() {
    std::vector<uint8_t> empty{};
    telemetry::PacketReader pr(empty);
    try {
        pr.read_u8();
        test_fail("PacketReader out-of-range should throw");
    } catch (const std::out_of_range&) {
        test_pass("PacketReader out-of-range throws std::out_of_range");
    }
}

static void test_skip() {
    telemetry::PacketWriter pw;
    pw.add_u8(0xFF);
    pw.add_u8(0xAB);
    telemetry::PacketReader pr(pw.bytes());
    pr.skip(1);
    uint8_t val = pr.read_u8();
    if (val == 0xAB) {
        test_pass("PacketReader skip() advances cursor correctly");
    } else {
        test_fail("PacketReader skip() incorrect");
    }
}

int main() {
    std::cout << "=== PacketReader Tests ===\n";
    test_read_u64_roundtrip();
    test_remaining_decrements();
    test_out_of_range_throws();
    test_skip();
    return test_summary();
}