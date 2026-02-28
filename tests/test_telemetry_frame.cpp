#include "test_utils.h"
#include "telemetry/TelemetryFrame.h"
#include "telemetry/PacketWriter.h"
#include "telemetry/PacketReader.h"

// Tests for telemetry::TelemetryFrame serialization/deserialization

static void test_roundtrip() {
    telemetry::TelemetryFrame original;
    original.timestamp_ms  = 12345;
    original.temperature_c = 25.5f;
    original.voltage_v     = 11.8f;
    original.position_x    = 1.0f;
    original.position_y    = 2.0f;
    original.velocity_mps  = 0.5f;
    original.status_flags  = 0x01;

    telemetry::PacketWriter pw;
    original.serialize(pw);

    telemetry::PacketReader pr(pw.bytes());
    telemetry::TelemetryFrame restored;
    restored.deserialize(pr);

    bool ok = restored.timestamp_ms  == original.timestamp_ms
           && restored.temperature_c == original.temperature_c
           && restored.voltage_v     == original.voltage_v
           && restored.position_x    == original.position_x
           && restored.position_y    == original.position_y
           && restored.velocity_mps  == original.velocity_mps
           && restored.status_flags  == original.status_flags;

    if (ok) {
        test_pass("TelemetryFrame serialize/deserialize roundtrip");
    } else {
        test_fail("TelemetryFrame serialize/deserialize roundtrip", "field mismatch");
    }
}

static void test_serialized_size() {
    telemetry::PacketWriter pw;
    telemetry::TelemetryFrame frame;
    frame.serialize(pw);
    if (pw.size() == telemetry::TelemetryFrame::kSerializedSize) {
        test_pass("TelemetryFrame serialized size matches kSerializedSize (32 bytes)");
    } else {
        test_fail("TelemetryFrame serialized size mismatch",
                  "got " + std::to_string(pw.size()) + " bytes");
    }
}

static void test_flag_helpers() {
    telemetry::TelemetryFrame frame;
    frame.set_flag(telemetry::FrameFlags::kLowVoltage);
    if (frame.has_flag(telemetry::FrameFlags::kLowVoltage)) {
        test_pass("TelemetryFrame set_flag/has_flag work correctly");
    } else {
        test_fail("TelemetryFrame flag helpers failed");
    }
}

int main() {
    std::cout << "=== TelemetryFrame Tests ===\n";
    test_roundtrip();
    test_serialized_size();
    test_flag_helpers();
    return test_summary();
}