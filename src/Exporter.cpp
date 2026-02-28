#include "telemetry/Exporter.h"

#include <fstream>
#include <iomanip>

namespace telemetry {

Exporter::Exporter(Logger& logger) : logger_(logger) {}

bool Exporter::export_to(const std::vector<TelemetryFrame>& frames,
                         const std::string& output_path,
                         Format format) const {
    if (format == Format::CSV) {
        return write_csv(frames, output_path);
    }
    return write_json(frames, output_path);
}

bool Exporter::write_csv(const std::vector<TelemetryFrame>& frames,
                         const std::string& path) const {
    // TODO: Implement CSV export.
    //
    // Suggested format:
    //   timestamp_ms,temperature_c,voltage_v,position_x,position_y,velocity_mps,status_flags
    //   <row per frame>
    //
    // Steps:
    //   1. Open std::ofstream at path
    //   2. Write the header row
    //   3. Loop frames, write each field separated by commas
    //   4. Log success/failure and return true/false

    (void)frames;
    (void)path;
    logger_.warn("Exporter::write_csv not yet implemented");
    return false;
}

bool Exporter::write_json(const std::vector<TelemetryFrame>& frames,
                          const std::string& path) const {
    // TODO: Implement JSON export.
    //
    // Suggested format:
    //   [
    //     { "timestamp_ms": 0, "temperature_c": 20.0, ... },
    //     ...
    //   ]
    //
    // Note: no external JSON library needed — write the JSON manually with ofstream.
    // Steps:
    //   1. Open std::ofstream at path
    //   2. Write opening '['
    //   3. Loop frames, write each as a JSON object (handle trailing comma)
    //   4. Write closing ']'
    //   5. Log success/failure and return true/false

    (void)frames;
    (void)path;
    logger_.warn("Exporter::write_json not yet implemented");
    return false;
}

} // namespace telemetry