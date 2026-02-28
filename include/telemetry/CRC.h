#pragma once

#include <cstdint>
#include <span>

namespace telemetry {

    std::uint32_t crc32(const std::span<const std::uint8_t>& data);

}
