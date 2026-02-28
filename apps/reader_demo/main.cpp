#include <iostream>
#include <vector>

#include "telemetry/Logger.h"
#include "telemetry/TelemetryReader.h"
#include "telemetry/PacketReader.h"
#include "telemetry/TelemetryFrame.h"

// reader_demo: Reads a binary telemetry file and prints each frame.
//
// TODO:
//   1. Construct a Logger (e.g. "reader_demo.log")
//   2. Open a TelemetryReader on "data/recorder_demo.bin" (or a file passed as argv[1])
//   3. Loop: call reader.read_next(packet), deserialize each into TelemetryFrame
//   4. Print a formatted table of frame fields
//   5. Handle ParseError exceptions (print error, continue or abort)
//   6. Print final summary: total frames read, any CRC failures

int main() {
    std::cout << "reader_demo: not yet implemented\n";
    return 0;
}