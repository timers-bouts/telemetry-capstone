#include "test_utils.h"
#include "telemetry/Logger.h"

#include <filesystem>
#include <fstream>
#include <string>

// Tests for telemetry::Logger

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    return std::string(std::istreambuf_iterator<char>(f), {});
}

static void test_writes_to_file() {
    const std::string path = "test_logger_output.log";
    std::filesystem::remove(path);
    {
        telemetry::Logger logger(path, telemetry::LogLevel::Info);
        logger.info("hello from test");
    }
    std::string content = read_file(path);
    if (content.find("hello from test") != std::string::npos) {
        test_pass("Logger writes message to file");
    } else {
        test_fail("Logger did not write message to file");
    }
    std::filesystem::remove(path);
}

static void test_severity_filter() {
    const std::string path = "test_logger_filter.log";
    std::filesystem::remove(path);
    {
        // Min level = Error: info/warn should be suppressed
        telemetry::Logger logger(path, telemetry::LogLevel::Error);
        logger.info("this should not appear");
        logger.warn("this should not appear");
        logger.error("this should appear");
    }
    std::string content = read_file(path);
    bool suppressed = content.find("should not appear") == std::string::npos;
    bool written    = content.find("should appear") != std::string::npos;
    if (suppressed && written) {
        test_pass("Logger severity filter suppresses lower-priority messages");
    } else {
        test_fail("Logger severity filter not working correctly");
    }
    std::filesystem::remove(path);
}

static void test_appends_on_reopen() {
    const std::string path = "test_logger_append.log";
    std::filesystem::remove(path);
    {
        telemetry::Logger logger(path, telemetry::LogLevel::Info);
        logger.info("first message");
    }
    {
        telemetry::Logger logger(path, telemetry::LogLevel::Info);
        logger.info("second message");
    }
    std::string content = read_file(path);
    if (content.find("first message") != std::string::npos &&
        content.find("second message") != std::string::npos) {
        test_pass("Logger appends to existing file on reopen");
    } else {
        test_fail("Logger did not append on reopen");
    }
    std::filesystem::remove(path);
}

int main() {
    std::cout << "=== Logger Tests ===\n";
    test_writes_to_file();
    test_severity_filter();
    test_appends_on_reopen();
    return test_summary();
}