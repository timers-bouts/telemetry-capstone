# Usage Examples

## Building

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

To also build tests:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
make -j$(nproc)
ctest --output-on-failure
```

---

## CLI Commands (once implemented)

### Generate frames (console only)
```bash
./capstone_cli generate --frames 50 --seed 42
```

### Record frames to file
```bash
./capstone_cli record --frames 100 --output data/telemetry.bin
```

### Replay and validate a file
```bash
./capstone_cli replay --input data/telemetry.bin
```

### Export to CSV
```bash
./capstone_cli export --input data/telemetry.bin --output data/telemetry.csv --format csv
```

### Export to JSON
```bash
./capstone_cli export --input data/telemetry.bin --output data/telemetry.json --format json
```

---

## Demo Apps

```bash
./recorder_demo     # Writes a short simulation to data/recorder_demo.bin
./reader_demo       # Reads and validates data/recorder_demo.bin
./simulator_demo    # Prints a tabular frame summary to stdout
```

---

## TODO

- Add plotting example (Python + matplotlib reading the CSV export)
- Add corrupted file example demonstrating CRC rejection