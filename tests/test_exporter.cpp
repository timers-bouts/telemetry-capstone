#include "test_utils.h"
#include "telemetry/Exporter.h"
#include "telemetry/Logger.h"
#include "telemetry/TelemetryFrame.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Tests for telemetry::Exporter

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
    const std::filesystem::path logger_path = "test_csv.log";
    const std::filesystem::path csv_path = "test.csv";
    std::filesystem::remove(logger_path);
    std::filesystem::remove(csv_path);
    auto logger = telemetry::Logger(logger_path,
                                    telemetry::LogLevel::Info
                                    );
    auto exporter = telemetry::Exporter(logger);
    exporter.export_to(make_test_frames(),
                    csv_path,
                    telemetry::Exporter::Format::CSV);
    auto file_string = read_file(csv_path);
    std::string headers = "timestamp_ms,temperature_c,voltage_v,position_x,position_y,velocity_mps,status_flags";
    std::string values = "100,20.00,12.00,1.00,2.00,0.50,0\n";
    size_t first_line_index = file_string.find('\n');
    if (first_line_index == std::string::npos) {
        test_fail("test_csv_export no newline found");
    } else if (file_string.substr(0, first_line_index) != headers) {
            test_fail("test_csv_export incorrect headers. Expected: " +
                        headers +
                        " but got: " +
                        file_string.substr(0, first_line_index)
            );
        } else if (file_string.substr(first_line_index + 1) != values) {
            test_fail("test_csv_export incorrect values. Expected: " +
                        values +
                        " but got: " +
                        file_string.substr(first_line_index + 1)      
            );
        } else {
            test_pass("test_csv_export");
        }

    std::filesystem::remove(logger_path);
    std::filesystem::remove(csv_path);

}

static void test_json_export() {
    const std::filesystem::path logger_path = "test_json.log";
    const std::filesystem::path json_path = "test.json";
    std::filesystem::remove(logger_path);
    std::filesystem::remove(json_path);
    auto logger = telemetry::Logger(logger_path,
                                    telemetry::LogLevel::Info
                                    );
    auto exporter = telemetry::Exporter(logger);
    exporter.export_to(make_test_frames(),
                    json_path,
                    telemetry::Exporter::Format::JSON);
    auto file_string = read_file(json_path);

    if (file_string.front() != '[') {
        test_fail("test_json_export incorrect first character. Expected: \"[\"");
    } else if (file_string[file_string.size() - 2] != ']') {
        // Last character is '\n', so second to last should be ']'
        test_fail("test_json_export incorrect final character. Expected: \"]\"");
    } else if (file_string.find("\"timestamp_ms\"") == std::string::npos) {
        test_fail("test_json_export missing timestamp_ms field");
    } else if (file_string.find("100") == std::string::npos) {
        test_fail("test_json_export missing or incorrect timestamp value");
    } else if (file_string.find("\"temperature_c\"") == std::string::npos) {
        test_fail("test_json_export missing temperature_c field");
    } else if (file_string.find("20.00") == std::string::npos) {
        test_fail("test_json_export missing or incorrect temperature_c value");
    } else if (file_string.find("\"voltage_v\"") == std::string::npos) {
        test_fail("test_json_export missing voltage_v field");
    } else if (file_string.find("12.00") == std::string::npos) {
        test_fail("test_json_export missing or incorrect voltage_v value");
    } else if (file_string.find("\"position_x\"") == std::string::npos) {
        test_fail("test_json_export missing position_x field");
    } else if (file_string.find("\"position_x\": 1.00") == std::string::npos) {
        test_fail("test_json_export missing or incorrect position_x value");
    } else if (file_string.find("\"position_y\"") == std::string::npos) {
        test_fail("test_json_export missing position_y field");
    } else if (file_string.find("2.00") == std::string::npos) {
        test_fail("test_json_export missing or incorrect position_y value");
    } else if (file_string.find("\"velocity_mps\"") == std::string::npos) {
        test_fail("test_json_export missing velocity_mps field");
    } else if (file_string.find("0.50") == std::string::npos) {
        test_fail("test_json_export missing or incorrect velocity_mps value");
    } else if (file_string.find("\"status_flags\"") == std::string::npos) {
        test_fail("test_json_export missing status_flags field");
    } else if (file_string.find("\"status_flags\": 0") == std::string::npos) {
        test_fail("test_json_export missing or incorrect status_flags value");
    } else {
        test_pass("test_json_export");
    }
    std::filesystem::remove(logger_path);
    std::filesystem::remove(json_path);
}

int main() {
    std::cout << "=== Exporter Tests ===\n";
    test_csv_export();
    test_json_export();
    return test_summary();
}