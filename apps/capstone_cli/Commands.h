#pragma once

#include "telemetry/Config.h"

namespace telemetry::cli {

// Each command returns 0 on success, non-zero on failure (suitable for main() return).
int run_generate(const Config& cfg);
int run_record(const Config& cfg);
int run_replay(const Config& cfg);
int run_export(const Config& cfg);

} // namespace telemetry::cli