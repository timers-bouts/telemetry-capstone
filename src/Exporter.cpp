#include "telemetry/Exporter.h"

#include <fstream>
#include <iomanip>
#include <format>

namespace telemetry {

Exporter::Exporter(Logger& logger) : logger_(logger) {}

bool Exporter::export_to(const std::span<const TelemetryFrame> frames,
                         const std::string& output_path,
                         Format format) const {
    if (format == Format::CSV) {
        return write_csv(frames, output_path);
    }
    return write_json(frames, output_path);
}

bool Exporter::write_csv(const std::span<const TelemetryFrame> frames,
                         const std::string& path) const {

    std::ofstream out(path);
    if (!out) {
        logger_.error(std::format("Unable to open file: {}", path));
        return false;
    }

    out << "timestamp_ms,temperature_c,voltage_v,position_x,position_y,velocity_mps,status_flags\n";

    // Set the precision for decimals in the csv
    out << std::fixed << std::setprecision(2);
    for (const auto& frame : frames) {
        out << frame.timestamp_ms << ","
            << frame.temperature_c << ","
            << frame.voltage_v << ","
            << frame.position_x << ","
            << frame.position_y << ","
            << frame.velocity_mps << ","
            << frame.status_flags << "\n";
    }

    logger_.info(std::format("exported {} frames to {}",
                frames.size(), path));

    return true;
}

bool Exporter::write_json(const std::span<const TelemetryFrame> frames,
                          const std::string& path) const {

    std::ofstream out(path);
    if (!out) {
        logger_.error(std::format("Unable to open file: {}", path));
        return false;
    }

    out << std::fixed << std::setprecision(2);
    out << "[\n";

    for (size_t i = 0; i < frames.size(); i++) {
        out << "  { "
            << "\"timestamp_ms\": "
            << frames[i].timestamp_ms << ", "
            << "\"temperature_c\": "
            << frames[i].temperature_c << ", "
            << "\"voltage_v\": "
            << frames[i].voltage_v << ", "
            << "\"position_x\": "
            << frames[i].position_x << ", "
            << "\"position_y\": "
            << frames[i].position_y << ", "
            << "\"velocity_mps\": "
            << frames[i].velocity_mps << ", "
            << "\"status_flags\": "
            << frames[i].status_flags
            << " }";
        if (i < (frames.size() - 1)) {
            // Dont write a trailing comma after the last entry
            out << ",";
        }
        out << "\n";
    }

    out << "]\n";

    logger_.info(std::format("exported {} frames to {}",
                frames.size(), path));

    return true;
}

} // namespace telemetry