#pragma once

#include <string>
#include <span>

#include "telemetry/TelemetryFrame.h"
#include "telemetry/Logger.h"

namespace telemetry {

    class Exporter {
    public:
        enum class Format { CSV, JSON };

        explicit Exporter(Logger& logger);

        // Export frames to the given output path in the specified format.
        // Returns true on success, false on failure (file open error, etc.)
        bool export_to(const std::span<const TelemetryFrame> frames,
                       const std::string& output_path,
                       Format format) const;

    private:
        bool write_csv(const std::span<const TelemetryFrame> frames,
                       const std::string& path) const;

        bool write_json(const std::span<const TelemetryFrame> frames,
                        const std::string& path) const;

        Logger& logger_;
    };

} // namespace telemetry
