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
                         const std::vector<std::uint8_t>& bytes) {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    f.write(reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
}

static void write_raw(const std::filesystem::path& path,
                        const std::vector<std::uint8_t>& bytes) {
    std::ofstream file(path, std::ios::binary | std::ios::app);
    file.write(reinterpret_cast<const char*>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()));
}

static void write_header(const std::filesystem::path& path) {
    write_binary(path, {'T','L','R','Y', 0x01, 0x00, 0x00, 0x00});
}

static void test_invalid_magic() {
    const std::filesystem::path path = "test_reader_bad_magic.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
    write_binary(path, {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00});
    telemetry::Logger logger("test_reader.log", telemetry::LogLevel::Warn);
    try {
        telemetry::TelemetryReader::open(path, logger);
        test_fail("invalid magic should throw ParseError");
    } catch (const telemetry::ParseError&) {
        test_pass("invalid magic throws ParseError");
    }
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
}

static void test_unsupported_version() {
    const std::filesystem::path path = "test_reader_bad_version.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
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
    std::filesystem::remove(logger_path);
}

static void test_truncated_header() {
    const std::filesystem::path path = "test_reader_truncated.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
    write_binary(path, {'T', 'L'}); // only 2 bytes
    telemetry::Logger logger("test_reader.log", telemetry::LogLevel::Warn);
    try {
        telemetry::TelemetryReader::open(path, logger);
        test_fail("truncated header should throw ParseError");
    } catch (const telemetry::ParseError&) {
        test_pass("truncated header throws ParseError");
    }
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
}

static void test_truncated_size_field() {
    const std::filesystem::path path = "test_truncated_size_field.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

    // Write a correct header
    write_header(path);

    // Write a truncated size filed
    write_raw(path, {0x01, 0x02});

    auto logger = telemetry::Logger(logger_path, telemetry::LogLevel::Info);
    // Try opening a reader using the file. Should not throw
    try {
        auto reader = telemetry::TelemetryReader::open(
            path,
            logger
        );
        try {
            // Try read_next(). Should throw
            std::vector<std::uint8_t> bytes;
            reader.read_next(bytes);
            test_fail("test_truncated_size_field should throw on read_next()");
        } catch (telemetry::ParseError& e) {
            test_pass("test_truncated_size_field throws on incorrect size field");
        }
    } catch (const telemetry::ParseError& e) {
        test_fail("test_truncated_size_field throws for correct header");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

}

static void test_truncated_payload() {

    const std::filesystem::path path = "test_truncated_payload.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

    // Write a correct header
    write_header(path);

    // Write a correct size
    write_raw(path, {0x14, 0x00, 0x00, 0x00});

    // Write a payload whose size is different than the size written (20)
    write_raw(path, {0x01, 0x02, 0x03});

    auto logger = telemetry::Logger(logger_path, telemetry::LogLevel::Info);

    // Try opening a reader using the file. Should not throw
    try {
        auto reader = telemetry::TelemetryReader::open(
            path,
            logger
        );
        try {
            // Try read_next(). Should throw
            std::vector<std::uint8_t> bytes;
            reader.read_next(bytes);
            test_fail("test_truncated_payload should throw on read_next()");
        } catch (telemetry::ParseError& e) {
            test_pass("test_truncated_payload throws on truncated payload");
        }
    } catch (const telemetry::ParseError& e) {
        test_fail("test_truncated_payload throws for correct header");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

}

static void test_zero_size_packet() {
    const std::filesystem::path path = "test_zero_size_packet.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

    // Write a correct header
    write_header(path);

    // Write an invalid size (0)
    write_raw(path, {0x00, 0x00, 0x00, 0x00});

    // Write a random payload (Doesn't matter. Shouldn't be read)
    write_raw(path, {0x01, 0x02, 0x03});

    auto logger = telemetry::Logger(logger_path, telemetry::LogLevel::Info);

    // Try opening a reader using the file. Should not throw
    try {
        auto reader = telemetry::TelemetryReader::open(
            path,
            logger
        );
        try {
            // Try read_next(). Should throw
            std::vector<std::uint8_t> bytes;
            reader.read_next(bytes);
            test_fail("test_zero_size_packet should throw on read_next()");
        } catch (telemetry::ParseError& e) {
            test_pass("test_zero_size_packet throws on invalid packet size (0)");
        }
    } catch (const telemetry::ParseError& e) {
        test_fail("test_zero_size_packet throws for correct header");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
}

static void test_oversized_packet() {
    const std::filesystem::path path = "test_oversized_packet.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

    // Write a correct header
    write_header(path);

    // Write a valid size
    write_raw(path, {0xFF, 0xFF, 0x01, 0x00});

    // Write a random payload (doesn't matter. Shouldn't be read)
    write_raw(path, {0x01, 0x02, 0x03});

    auto logger = telemetry::Logger(logger_path, telemetry::LogLevel::Info);

    // Try opening a reader using the file. Should not throw
    try {
        telemetry::TelemetryReader::Options options;
        options.max_packet_size = 1 <<16;
        auto reader = telemetry::TelemetryReader::open(
            path,
            options,
            logger
        );
        try {
            // Try read_next(). Should throw
            std::vector<std::uint8_t> bytes;
            reader.read_next(bytes);
            test_fail("test_oversized_packet should throw on read_next()");
        } catch (telemetry::ParseError& e) {
            test_pass("test_oversized_packet throws on invalid packet size (131071)");
        }
    } catch (const telemetry::ParseError& e) {
        test_fail("test_oversized_packet throws for correct header");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
}

static void test_crc_mismatch() {
    const std::filesystem::path path = "test_crc_mismatch.bin";
    const std::filesystem::path logger_path = "test_reader.log";
    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);

    // Write a correct header
    write_header(path);

    // Write valid size
    write_raw(path, {0x07, 0x00, 0x00, 0x00});

    // Write a payload that is size - 4 (crc)
    write_raw(path, {0x01, 0x02, 0x03});

    // Write an incorrect crc value
    write_raw(path, {0xDE, 0xAD, 0xBE, 0xEF});

    auto logger = telemetry::Logger(logger_path, telemetry::LogLevel::Info);

    // Try opening a reader using the file. Should not throw
    try {
        telemetry::TelemetryReader::Options options;
        options.max_packet_size = 1 <<16;
        auto reader = telemetry::TelemetryReader::open(
            path,
            options,
            logger
        );
        try {
            // Try read_next(). Should throw
            std::vector<std::uint8_t> bytes;
            reader.read_next(bytes);
            test_fail("test_crc_mismatch should throw on read_next()");
        } catch (telemetry::ParseError& e) {
            test_pass("test_crc_mismatch throws on incorrect crc");
        }
    } catch (const telemetry::ParseError& e) {
        test_fail("test_crc_mismatch throws for correct header");
    }

    std::filesystem::remove(path);
    std::filesystem::remove(logger_path);
}

int main() {
    std::cout << "=== TelemetryReader Defensive Tests ===\n";
    test_invalid_magic();
    test_unsupported_version();
    test_truncated_header();
    test_truncated_size_field();
    test_truncated_payload();
    test_zero_size_packet();
    test_oversized_packet();
    test_crc_mismatch();
    return test_summary();
}