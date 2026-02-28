#include "Commands.h"

#include "telemetry/Logger.h"
#include "telemetry/TelemetryRecorder.h"
#include "telemetry/TelemetryReader.h"
#include "telemetry/TelemetrySimulator.h"
#include "telemetry/PacketReader.h"
#include "telemetry/TelemetryFrame.h"
#include "telemetry/Exporter.h"

#include <iostream>
#include <vector>

namespace telemetry::cli {

int run_generate(const Config& cfg) {
    (void)cfg;
    // TODO: Create a Logger + TelemetrySimulator, run without recording to file.
    // Print frame table to console (timestamp, temp, voltage, position, velocity).
    std::cout << "[generate] not yet implemented\n";
    return 0;
}

int run_record(const Config& cfg) {
    (void)cfg;
    // TODO: Create Logger + TelemetryRecorder + TelemetrySimulator.
    // Run simulation and write frames to cfg.output_file.
    std::cout << "[record] not yet implemented\n";
    return 0;
}

int run_replay(const Config& cfg) {
    (void)cfg;
    // TODO: Open cfg.input_file with TelemetryReader.
    // Read all packets, deserialize each into TelemetryFrame, print summary table.
    // Report CRC validation results and any ParseErrors.
    std::cout << "[replay] not yet implemented\n";
    return 0;
}

int run_export(const Config& cfg) {
    (void)cfg;
    // TODO: Open cfg.input_file with TelemetryReader, collect all frames.
    // Use Exporter to write to cfg.output_file in cfg.export_format (csv/json).
    std::cout << "[export] not yet implemented\n";
    return 0;
}

} // namespace telemetry::cli