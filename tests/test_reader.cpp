#include "test_utils.h"
#include "telemetry/Logger.h"
#include "telemetry/TelemetryReader.h"
#include "telemetry/TelemetryFormat.h"

#include <filesystem>
#include <fstream>
#include <vector>
#include <cstdint>

// Defensive parsing tests for TelemetryReader

static void write_binary(const std::filesystem::path& path,
                         const std::vector<uint8_t>& bytes) {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    f.write(reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
}

static void test_invalid_magic() {
    const std::filesystem::path path = "test_reader_bad_magic.bin";
    write_binary(path, {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00});
    telemetry::Logger logger("test_reader.log", telemetry::LogLevel::Warn);
    try {
        telemetry::TelemetryReader::open(path, logger);
        test_fail("invalid magic should throw ParseError");
    } catch (const telemetry::ParseError&) {
        test_pass("invalid magic throws ParseError");
    }
    std::filesystem::remove(path);
}

static void test_unsupported_version() {
    const std::filesystem::path path = "test_reader_bad_version.bin";
    // Valid magic, version=99
    write_binary(path, {'T','L','R','Y', 0x63, 0x00, 0x00, 0x00});
    telemetry::Logger logger("test_reader.log", telemetry::LogLevel::Warn);
    try {
        telemetry::TelemetryReader::open(path, logger);
        test_fail("unsupported version should throw ParseError");
    } catch (const telemetry::ParseError&) {
        test_pass("unsupported version throws ParseError");
    }
    std::filesystem::remove(path);
}

static void test_truncated_header() {
    const std::filesystem::path path = "test_reader_truncated.bin";
    write_binary(path, {'T', 'L'}); // only 2 bytes
    telemetry::Logger logger("test_reader.log", telemetry::LogLevel::Warn);
    try {
        telemetry::TelemetryReader::open(path, logger);
        test_fail("truncated header should throw ParseError");
    } catch (const telemetry::ParseError&) {
        test_pass("truncated header throws ParseError");
    }
    std::filesystem::remove(path);
}

// TODO: Add tests for:
//   - Truncated size field mid-record
//   - Truncated payload
//   - CRC mismatch (flip a byte in the payload)
//   - Oversized packet (size field > max_packet_size)
//   - Zero-size packet

int main() {
    std::cout << "=== TelemetryReader Defensive Tests ===\n";
    test_invalid_magic();
    test_unsupported_version();
    test_truncated_header();
    return test_summary();
}