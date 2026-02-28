#pragma once

#include <cstdint>
#include <cstddef>

namespace telemetry {

class PacketWriter;
class PacketReader;

// Status flag bitmask constants
namespace FrameFlags {
    constexpr uint32_t kNone         = 0x00;
    constexpr uint32_t kLowVoltage   = 0x01;
    constexpr uint32_t kHighVelocity = 0x02;
    constexpr uint32_t kTempWarning  = 0x04;
}

// One telemetry packet. Fields are serialized in declaration order.
struct TelemetryFrame {
    uint64_t timestamp_ms  = 0;    // milliseconds since simulation start
    float    temperature_c = 0.0f; // degrees Celsius
    float    voltage_v     = 0.0f; // volts
    float    position_x    = 0.0f; // meters
    float    position_y    = 0.0f; // meters
    float    velocity_mps  = 0.0f; // meters per second
    uint32_t status_flags  = 0;    // bitmask for error/status flags

    // Serialized size: 8 + 4 + 4 + 4 + 4 + 4 + 4 = 32 bytes
    static constexpr std::size_t kSerializedSize = 32;

    // Serialize this frame into a PacketWriter payload.
    void serialize(PacketWriter& writer) const;

    // Deserialize the frame, assigning struct members from a packet.
    void deserialize(PacketReader& reader);

    // Convenience flag helpers
    bool has_flag(uint32_t flag) const { return (status_flags & flag) != 0; }
    void set_flag(uint32_t flag)       { status_flags |= flag; }
};

} // namespace telemetry
