# Telemetry Capstone

[![CI](https://github.com/timers-bouts/telemetry-capstone/actions/workflows/ci.yml/badge.svg)](https://github.com/timers-bouts/telemetry-capstone/actions/workflows/ci.yml)

A C++20 binary telemetry system for simulating, recording, replaying, and exporting sensor data streams. Built as a portfolio project demonstrating systems programming patterns including custom binary serialization, CRC32 integrity verification, and a layered CLI architecture.

---

## Features

- **Simulate** deterministic telemetry frames with configurable RNG seed, frame count, and time step
- **Record** simulation output to a compact binary file with per-packet CRC32 validation
- **Replay** binary files with full integrity checking and tabular display
- **Export** binary data to CSV or JSON for downstream analysis
- Pure C++20 standard library — no external dependencies

---

## Architecture

The project is organized as a static library (`telemetry_lib`) consumed by a CLI application and three standalone demo programs.

```
telemetry_capstone/
├── include/telemetry/     # Public headers (library API)
├── src/                   # Library implementation
│   ├── CRC.cpp            # CRC32 computation
│   ├── Logger.cpp         # Timestamped file logger
│   ├── PacketWriter.cpp   # Binary packet builder
│   ├── PacketReader.cpp   # Binary packet cursor/reader
│   ├── TelemetryFrame.cpp # Frame serialization / deserialization
│   ├── TelemetryRecorder.cpp  # RAII binary file writer
│   ├── TelemetryReader.cpp    # Factory-pattern binary file reader
│   ├── TelemetrySimulator.cpp # Physics-based sensor simulator
│   ├── Exporter.cpp       # CSV and JSON export
│   └── Config.cpp         # CLI argument parsing
├── apps/
│   ├── capstone_cli/      # Main CLI (generate / record / replay / export)
│   ├── recorder_demo/     # Demo: run simulation and write binary file
│   ├── reader_demo/       # Demo: read binary file and print table
│   └── simulator_demo/    # Demo: simulate and print table to stdout
└── tests/                 # Unit tests (no external framework)
```

### Key Design Patterns

| Pattern | Where used |
|---|---|
| Builder | `PacketWriter` — fluent API for constructing binary packets |
| Cursor | `PacketReader` — stateful read position over a byte buffer |
| Factory | `TelemetryReader::open()` — validates header before returning reader |
| RAII | `Logger`, `TelemetryRecorder`, `TelemetryReader` — file handles managed by destructors |

### C++20 Features

- `std::span` throughout for zero-copy buffer views
- `std::optional<Config>` for ergonomic parse failure signaling
- Nested namespace syntax (`telemetry::cli`)
- `std::format` for log message formatting

---

## Binary Format

```
┌─────────────────────────────────────────────────────────┐
│  File Header (8 bytes)                                  │
│  magic[4]='TLRY'  version:u16=1  flags:u16=0            │
├─────────────────────────────────────────────────────────┤
│  Record 0                                               │
│  size:u32LE  │  payload (32 bytes)  │  crc32:u32LE      │
├─────────────────────────────────────────────────────────┤
│  Record 1 ...                                           │
└─────────────────────────────────────────────────────────┘
```

Each payload is a serialized `TelemetryFrame` (32 bytes):

| Field | Type | Size |
|---|---|---|
| `timestamp_ms` | `uint64_t` | 8 bytes |
| `temperature_c` | `float` | 4 bytes |
| `voltage_v` | `float` | 4 bytes |
| `position_x` | `float` | 4 bytes |
| `position_y` | `float` | 4 bytes |
| `velocity_mps` | `float` | 4 bytes |
| `status_flags` | `uint32_t` | 4 bytes |

CRC32 is computed over the payload only. The `size` field includes the 4-byte CRC. Any mismatch raises a `ParseError`.

---

## Build

**Requirements:** CMake ≥ 3.20, a C++20-capable compiler (clang++ 14+ or g++ 12+)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To build with unit tests:

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest --output-on-failure
```

---

## Usage

```
Usage: capstone_cli <command> [options]

Commands:
  generate   Simulate frames and print to console
  record     Simulate frames and record to binary file
  replay     Read and validate a binary telemetry file
  export     Export binary file to CSV or JSON
  help       Show this message
```

### generate

Simulate frames and print a table to stdout. No output file is required.

```bash
./capstone_cli generate --frames 10 --seed 42
```

```
  Time(ms)     Temp(C)   Volt(V)     PosX(m)     PosY(m)    Vel(m/s)     Flags
------------------------------------------------------------------------------
         0       20.81     11.98       -0.05        0.12        0.14         0
       100       20.79     11.99       -0.06        0.00        0.14         0
       200       20.67     11.97        0.10       -0.02        0.11         0
       300       20.49     12.00       -0.13        0.12        0.05         0
       ...
```

### record

Run a simulation and write the result to a binary file.

```bash
./capstone_cli record --output data/session.bin --frames 100 --seed 42
```

```
Recorded 100 frames to data/session.bin
```

### replay

Read a binary file, validate CRC32 on every record, and print the frame table.

```bash
./capstone_cli replay --input data/session.bin
```

### export

Convert a binary file to CSV or JSON.

```bash
./capstone_cli export --input data/session.bin --output data/session.csv --format csv
./capstone_cli export --input data/session.bin --output data/session.json --format json
```

**CSV output:**
```
timestamp_ms,temperature_c,voltage_v,position_x,position_y,velocity_mps,status_flags
0,20.81,11.98,-0.05,0.12,0.14,0
100,20.79,11.99,-0.06,0.00,0.14,0
200,20.67,11.97,0.10,-0.02,0.11,0
300,20.49,12.00,-0.13,0.12,0.05,0
...
```

**JSON output:**
```json
[
  { "timestamp_ms": 0, "temperature_c": 20.81, "voltage_v": 11.98, "position_x": -0.05, "position_y": 0.12, "velocity_mps": 0.14, "status_flags": 0 },
  { "timestamp_ms": 100, "temperature_c": 20.79, "voltage_v": 11.99, "position_x": -0.06, "position_y": 0.00, "velocity_mps": 0.14, "status_flags": 0 },
  { "timestamp_ms": 200, "temperature_c": 20.67, "voltage_v": 11.97, "position_x": 0.10, "position_y": -0.02, "velocity_mps": 0.11, "status_flags": 0 },
  ...
]
```

### Common Options

| Flag | Description | Default |
|---|---|---|
| `--frames N` | Number of frames to simulate | 100 |
| `--step-ms N` | Milliseconds between frames | 100 |
| `--seed N` | RNG seed for deterministic output | 42 |
| `--log FILE` | Log output path | `capstone.log` |
| `-v` | Verbose logging to console | off |

---

## Demo Programs

Three standalone programs demonstrate the core library directly without the CLI layer:

```bash
./recorder_demo    # Writes 50 frames to data/recorder_demo.bin
./reader_demo      # Reads data/recorder_demo.bin and prints table
./simulator_demo   # Simulates 50 frames and prints table to stdout
```

---

## License

MIT
