#include "test_utils.h"
#include "telemetry/Exporter.h"
#include "telemetry/Logger.h"
#include "telemetry/TelemetryFrame.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Tests for telemetry::Exporter
// Most tests will remain as TODOs until Exporter is implemented.

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    return std::string(std::istreambuf_iterator<char>(f), {});
}

static std::vector<telemetry::TelemetryFrame> make_test_frames() {
    telemetry::TelemetryFrame f;
    f.timestamp_ms  = 100;
    f.temperature_c = 20.0f;
    f.voltage_v     = 12.0f;
    f.position_x    = 1.0f;
    f.position_y    = 2.0f;
    f.velocity_mps  = 0.5f;
    f.status_flags  = 0;
    return {f};
}

static void test_csv_export() {
    // TODO: Once Exporter::write_csv is implemented, verify:
    //   - Output file exists
    //   - First line is the header row
    //   - Data rows contain correct values
    test_pass("Exporter CSV — TODO: implement and assert output content");
}

static void test_json_export() {
    // TODO: Once Exporter::write_json is implemented, verify:
    //   - Output file exists
    //   - Content is valid JSON (at minimum: starts with '[', ends with ']')
    //   - Correct field names and values appear in the output
    test_pass("Exporter JSON — TODO: implement and assert output content");
}

int main() {
    std::cout << "=== Exporter Tests ===\n";
    test_csv_export();
    test_json_export();
    return test_summary();
}