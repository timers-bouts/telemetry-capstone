#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "telemetry/Logger.h"

namespace telemetry {

    std::string Logger::make_timestamp() const {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);

        std::ostringstream ts;
        ts << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::string timestamp = ts.str();
        
        return timestamp;
    }

    int Logger::severity(LogLevel level) {
        switch (level) {
            case LogLevel::Info: return 0;
            case LogLevel::Warn: return 1;
            case LogLevel::Error: return 2;
        }

        return -1;

    }

    void Logger::log(const std::string& message, LogLevel level) {
        
        if (severity(level) < severity(min_level)) {
            return;
        }

        if (!file.is_open()) {
            return;
        }

        std::string timestamp = make_timestamp();
        std::ostringstream oss;
        oss << "[" << to_string(level) << "] " << timestamp << " " << message << "\n";
        std::string line = oss.str();
        file << line;
        if (mirror_to_console_) {
            std::cout << line;
        }
    }

    Logger::Logger(const std::string& filename, LogLevel minLevel, bool mirrorToConsole) 

    : file(filename, std::ios::app),
      min_level(minLevel),
      mirror_to_console_(mirrorToConsole)

    {
        if (!file.is_open()) {
            std::cerr << "Failed to open file!\n";
        }
    }

    Logger::~Logger() {
        if (file.is_open()) {
            file.close();
        }
    }

    void Logger::info(const std::string& message) {
        log(message, LogLevel::Info);
    }

    void Logger::warn(const std::string& message) {
        log(message, LogLevel::Warn);
    }

    void Logger::error(const std::string& message) {
        log(message, LogLevel::Error);
    }

    const char* Logger::to_string(LogLevel level) {
        switch (level) {
            case LogLevel::Info: return "INFO";
            case LogLevel::Warn: return "WARN";
            case LogLevel::Error: return "ERROR";
        }

        return "UNKNOWN";

    }

}