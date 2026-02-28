#include "test_utils.h"
#include "telemetry/Logger.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetryReader.h"

#include <filesystem>
#include <vector>
#include <cstdint>

// Tests for TelemetryRecorder + TelemetryReader round-trip

static const std::filesystem::path kTestFile = "test_recorder_output.bin";

static void cleanup() {
    std::filesystem::remove(kTestFile);
}

static void test_write_and_read_back() {
    cleanup();
    telemetry::Logger logger("test_recorder.log", telemetry::LogLevel::Warn);

    {
        telemetry::TelemetryRecorder recorder(kTestFile, logger,
            telemetry::TelemetryRecorder::OpenMode::Truncate);

        std::vector<uint8_t> payload{0x01, 0x02, 0x03, 0x04};
        recorder.write_packet(payload);
    }

    auto reader = telemetry::TelemetryReader::open(kTestFile, logger);
    std::vector<uint8_t> out;
    bool got = reader.read_next(out);

    if (got && out.size() == 4 && out[0] == 0x01 && out[3] == 0x04) {
        test_pass("TelemetryRecorder write + TelemetryReader read roundtrip");
    } else {
        test_fail("TelemetryRecorder/Reader roundtrip failed");
    }
    cleanup();
}

static void test_clean_eof() {
    cleanup();
    telemetry::Logger logger("test_recorder.log", telemetry::LogLevel::Warn);

    {
        telemetry::TelemetryRecorder recorder(kTestFile, logger,
            telemetry::TelemetryRecorder::OpenMode::Truncate);
        std::vector<uint8_t> payload{0xAA, 0xBB};
        recorder.write_packet(payload);
    }

    auto reader = telemetry::TelemetryReader::open(kTestFile, logger);
    std::vector<uint8_t> out;
    reader.read_next(out);
    bool eof = !reader.read_next(out); // second call should return false (EOF)

    if (eof) {
        test_pass("TelemetryReader returns false on clean EOF");
    } else {
        test_fail("TelemetryReader did not signal clean EOF");
    }
    cleanup();
}

int main() {
    std::cout << "=== Recorder/Reader Tests ===\n";
    test_write_and_read_back();
    test_clean_eof();
    return test_summary();
}