# Architecture Overview

## Module Dependency Graph

```
capstone_cli
    └── Commands
            ├── TelemetrySimulator ──► TelemetryRecorder ──► CRC
            │                    └──► PacketWriter
            ├── TelemetryReader ──────────────────────────► CRC
            │                   └──► PacketReader
            ├── TelemetryFrame ──► PacketWriter / PacketReader
            ├── Exporter ────────► TelemetryFrame
            └── Logger (used throughout)
```

## Module Responsibilities

| Module              | Role                                                       |
|---------------------|------------------------------------------------------------|
| `TelemetryFormat`   | Protocol constants: magic, version, field sizes            |
| `CRC`               | CRC32 checksum computation (polynomial `0x04C11DB7`)       |
| `Logger`            | Timestamped severity-filtered file logging                 |
| `PacketWriter`      | Endian-aware binary packet builder (Builder pattern)       |
| `PacketReader`      | Endian-aware binary packet reader (Cursor pattern)         |
| `TelemetryFrame`    | Frame data struct + serialize/deserialize methods          |
| `TelemetryRecorder` | Framed binary file writer with CRC32 per packet            |
| `TelemetryReader`   | Defensive binary file parser; throws `ParseError` on bad data |
| `TelemetrySimulator`| Configurable frame generator with physics/noise model      |
| `Exporter`          | CSV and JSON export from a collection of frames            |
| `Config`            | CLI argument parsing                                       |

## Design Patterns Used

- **Builder** — `PacketWriter`: fluent interface for composing binary payloads
- **Cursor** — `PacketReader`: sequential reads advance internal position
- **Factory** — `TelemetryReader::open()`: validates file before construction
- **RAII** — `Logger`, `TelemetryRecorder`, `TelemetryReader`: automatic resource cleanup
- **Zero-copy** — `std::span` used throughout for non-owning buffer views
- **Defensive parsing** — `TelemetryReader` validates magic, version, size, and CRC before trusting any data