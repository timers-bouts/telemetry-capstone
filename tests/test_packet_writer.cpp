#include "test_utils.h"
#include "telemetry/PacketWriter.h"
#include "telemetry/PacketReader.h"

#include <cstdint>

// Tests for telemetry::PacketWriter

static void test_add_u8_roundtrip() {
    telemetry::PacketWriter pw;
    pw.add_u8(0xAB);
    auto buf = pw.bytes();
    if (buf.size() == 1 && buf[0] == 0xAB) {
        test_pass("PacketWriter add_u8 roundtrip");
    } else {
        test_fail("PacketWriter add_u8 roundtrip");
    }
}

static void test_add_u32_big_endian() {
    telemetry::PacketWriter pw(telemetry::PacketWriter::Endianness::Big);
    pw.add_u32(0x01020304);
    auto buf = pw.bytes();
    if (buf.size() == 4 && buf[0] == 0x01 && buf[1] == 0x02 && buf[2] == 0x03 && buf[3] == 0x04) {
        test_pass("PacketWriter add_u32 big-endian byte order");
    } else {
        test_fail("PacketWriter add_u32 big-endian byte order");
    }
}

static void test_add_u32_little_endian() {
    telemetry::PacketWriter pw(telemetry::PacketWriter::Endianness::Little);
    pw.add_u32(0x01020304);
    auto buf = pw.bytes();
    if (buf.size() == 4 && buf[0] == 0x04 && buf[1] == 0x03 && buf[2] == 0x02 && buf[3] == 0x01) {
        test_pass("PacketWriter add_u32 little-endian byte order");
    } else {
        test_fail("PacketWriter add_u32 little-endian byte order");
    }
}

static void test_add_float_roundtrip() {
    telemetry::PacketWriter pw;
    pw.add_float(3.14f);
    auto buf = pw.bytes();
    telemetry::PacketReader pr(buf);
    float val = pr.read_float();
    if (val == 3.14f) {
        test_pass("PacketWriter add_float roundtrip");
    } else {
        test_fail("PacketWriter add_float roundtrip", "value mismatch");
    }
}

static void test_clear_resets_size() {
    telemetry::PacketWriter pw;
    pw.add_u32(0xDEADBEEF);
    pw.clear();
    if (pw.size() == 0) {
        test_pass("PacketWriter clear resets size to zero");
    } else {
        test_fail("PacketWriter clear did not reset size");
    }
}

int main() {
    std::cout << "=== PacketWriter Tests ===\n";
    test_add_u8_roundtrip();
    test_add_u32_big_endian();
    test_add_u32_little_endian();
    test_add_float_roundtrip();
    test_clear_resets_size();
    return test_summary();
}