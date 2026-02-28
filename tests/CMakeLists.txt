cmake_minimum_required(VERSION 3.20)

# Each test is a standalone executable with its own main().
# Run all tests with: ctest --output-on-failure

function(add_telemetry_test name src)
    add_executable(${name} ${src})
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${name} PRIVATE telemetry_lib)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_telemetry_test(test_crc              test_crc.cpp)
add_telemetry_test(test_logger           test_logger.cpp)
add_telemetry_test(test_packet_writer    test_packet_writer.cpp)
add_telemetry_test(test_packet_reader    test_packet_reader.cpp)
add_telemetry_test(test_telemetry_frame  test_telemetry_frame.cpp)
add_telemetry_test(test_recorder         test_recorder.cpp)
add_telemetry_test(test_reader           test_reader.cpp)
add_telemetry_test(test_exporter         test_exporter.cpp)