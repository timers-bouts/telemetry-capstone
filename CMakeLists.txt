cmake_minimum_required(VERSION 3.20)
project(telemetry_capstone VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for clangd IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# ── Core library ──────────────────────────────────────────────────────────────
add_library(telemetry_lib STATIC
    src/CRC.cpp
    src/Logger.cpp
    src/PacketWriter.cpp
    src/PacketReader.cpp
    src/TelemetryFrame.cpp
    src/TelemetryRecorder.cpp
    src/TelemetryReader.cpp
    src/TelemetrySimulator.cpp
    src/Exporter.cpp
    src/Config.cpp
)
target_include_directories(telemetry_lib PUBLIC include)

# ── Applications ─────────────────────────────────────────────────────────────
add_executable(capstone_cli
    apps/capstone_cli/main.cpp
    apps/capstone_cli/Commands.cpp
)
target_include_directories(capstone_cli PRIVATE apps/capstone_cli)
target_link_libraries(capstone_cli PRIVATE telemetry_lib)

add_executable(recorder_demo apps/recorder_demo/main.cpp)
target_link_libraries(recorder_demo PRIVATE telemetry_lib)

add_executable(reader_demo apps/reader_demo/main.cpp)
target_link_libraries(reader_demo PRIVATE telemetry_lib)

add_executable(simulator_demo apps/simulator_demo/main.cpp)
target_link_libraries(simulator_demo PRIVATE telemetry_lib)

# ── Tests (enable with: cmake .. -DBUILD_TESTS=ON) ───────────────────────────
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()