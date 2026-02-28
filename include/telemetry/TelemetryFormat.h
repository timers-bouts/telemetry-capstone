#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace telemetry::format {

inline constexpr std::size_t kMagicSize   = 4;
inline constexpr std::size_t kVersionSize = 2;
inline constexpr std::size_t kFlagsSize   = 2;

inline constexpr std::size_t kHeaderSize =
    kMagicSize + kVersionSize + kFlagsSize;

inline constexpr std::uint16_t kCurrentVersion = 1;

inline constexpr std::array<std::uint8_t, 4> kMagic{'T','L','R','Y'};

// Packet size prefix width (bytes).
// This is part of the on-disk format contract and must remain stable.
// Any change requires a format version bump for backward compatibility.
inline constexpr std::size_t kRecordSizeFieldBytes = sizeof(std::uint32_t);

static_assert(kRecordSizeFieldBytes == 4,
              "Record size field must remain 4 bytes in format version 1");

constexpr size_t CRC32_SIZE = 4; // bytes

} // namespace telemetry::format
