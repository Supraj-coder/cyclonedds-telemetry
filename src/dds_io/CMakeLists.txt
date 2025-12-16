# DDS IO is optional — only build if ENABLE_DDS is ON and CycloneDDS was found.
if(NOT ENABLE_DDS)
  return()
endif()

# Create placeholder files so CMake doesn't complain during config
file(TOUCH "${CMAKE_CURRENT_BINARY_DIR}/Telemetry.hpp" "${CMAKE_CURRENT_BINARY_DIR}/Telemetry.cpp")

# Generate IDL-based C++ types from Telemetry.idl
idlcxx_generate(TARGET ddscxxTelemetry FILES ${CMAKE_SOURCE_DIR}/src/core/idl/Telemetry.idl WARNINGS no-implicit-extensibility)

# Create a library with the wrapper and link to the generated type library
add_library(dds_io STATIC 
  dds_wrapper.cpp 
  dds_wrapper.hpp
)

target_include_directories(dds_io PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/core)
target_link_libraries(dds_io PUBLIC core nlohmann_json::nlohmann_json ddscxxTelemetry CycloneDDS-CXX::ddscxx)

# Ensure the IDL files are properly generated before building dds_io
add_dependencies(dds_io ddscxxTelemetry_generate)
