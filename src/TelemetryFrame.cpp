#include "telemetry/TelemetryFrame.h"
#include "telemetry/PacketWriter.h"
#include "telemetry/PacketReader.h"

namespace telemetry {

    void TelemetryFrame::serialize(PacketWriter& writer) const {
        // Serialize fields in this order for consistent binary format
        writer.add_u64(timestamp_ms);
        writer.add_float(temperature_c);
        writer.add_float(voltage_v);
        writer.add_float(position_x);
        writer.add_float(position_y);
        writer.add_float(velocity_mps);
        writer.add_u32(status_flags);

    };

    void TelemetryFrame::deserialize(PacketReader& reader) {
        timestamp_ms = reader.read_u64();
        temperature_c = reader.read_float();
        voltage_v = reader.read_float();
        position_x = reader.read_float();
        position_y = reader.read_float();
        velocity_mps = reader.read_float();
        status_flags   = reader.read_u32();
    }

}