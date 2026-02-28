# Telemetry Binary File Format

Version 1 — `TLRY` format

---

## File Header (8 bytes)

| Offset | Size | Type    | Value      | Description                  |
|--------|------|---------|------------|------------------------------|
| 0      | 4    | bytes   | `TLRY`     | Magic identifier             |
| 4      | 2    | u16 LE  | `0x0001`   | Format version (currently 1) |
| 6      | 2    | u16 LE  | `0x0000`   | Flags (reserved, must be 0)  |

## Records (repeated)

Each record immediately follows the header (or the previous record):

| Offset | Size      | Type    | Description                               |
|--------|-----------|---------|-------------------------------------------|
| 0      | 4         | u32 LE  | Total record size = payload_size + 4      |
| 4      | size − 4  | bytes   | Payload (serialized `TelemetryFrame`)     |
| last 4 | 4         | u32 LE  | CRC32 of payload bytes only               |

**Note:** The size field includes the 4-byte CRC. The CRC is computed over the payload bytes only — not the size field and not the CRC field itself.

---

## TelemetryFrame Payload (32 bytes, Big-endian)

| Offset | Size | Type   | Field          | Units              |
|--------|------|--------|----------------|--------------------|
| 0      | 8    | u64 BE | `timestamp_ms` | ms since sim start |
| 8      | 4    | f32 BE | `temperature_c`| degrees Celsius    |
| 12     | 4    | f32 BE | `voltage_v`    | volts              |
| 16     | 4    | f32 BE | `position_x`   | meters             |
| 20     | 4    | f32 BE | `position_y`   | meters             |
| 24     | 4    | f32 BE | `velocity_mps` | meters/second      |
| 28     | 4    | u32 BE | `status_flags` | bitmask (see below)|

**Total payload size:** 32 bytes
**Total record size:** 4 (size field) + 32 (payload) + 4 (CRC) = 40 bytes per frame

### Status Flags Bitmask

| Bit | Constant            | Meaning                       |
|-----|---------------------|-------------------------------|
| 0   | `kLowVoltage`       | Voltage dropped below 11.5 V  |
| 1   | `kHighVelocity`     | Velocity exceeded 5.0 m/s     |
| 2   | `kTempWarning`      | Temperature out of range      |

---

## CRC32 Algorithm

- Polynomial: `0x04C11DB7` (same as Ethernet/ZIP)
- Initial value: `0xFFFFFFFF`
- Final XOR: `0xFFFFFFFF`
- Input: payload bytes only (not size or CRC fields)
- Output: 4 bytes, little-endian in file

---

## Example: Minimal valid file (1 frame)

```
54 4C 52 59   -- Magic: "TLRY"
01 00         -- Version: 1 (LE)
00 00         -- Flags: 0
24 00 00 00   -- Record size: 36 = 32 payload + 4 CRC (LE)
<32 bytes>    -- TelemetryFrame payload (Big-endian fields)
<4 bytes>     -- CRC32 of payload (LE)
```