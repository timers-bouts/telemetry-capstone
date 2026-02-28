#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>

#include "telemetry/Logger.h"

namespace telemetry {

class TelemetryRecorder {
    public:
        enum class OpenMode {
            Truncate, // overwrite existing file
            Append    // append new packets to end
        };

        TelemetryRecorder(const std::filesystem::path& path,
                        telemetry::Logger& logger,
                        OpenMode mode);

        ~TelemetryRecorder();

        // Non-copyable (owning a file handle should not be copied)
        TelemetryRecorder(const TelemetryRecorder& recorder) = delete;
        TelemetryRecorder& operator=(const TelemetryRecorder& recorder) = delete;

        // Write one framed packet:
        // [u32 size][packet bytes...][u32 crc]
        void write_packet(std::span<const std::uint8_t> packet_bytes);

        void flush();

    private:
        std::ofstream out_;
        telemetry::Logger& logger_;
        void write_u32_le(std::uint32_t);
    };

} // namespace telemetry
