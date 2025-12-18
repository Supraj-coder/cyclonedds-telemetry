# DDS IO - Generate IDL and link to DDS libraries
# Generate IDL-based C++ types from Telemetry.idl
IDLCXX_GENERATE(
  TARGET ddscxxTelemetry 
  FILES ${CMAKE_SOURCE_DIR}/src/core/idl/Telemetry.idl
  WARNINGS no-implicit-extensibility
)

# Create a library with the wrapper - the ddscxxTelemetry target handles the IDL
add_library(dds_io STATIC 
  dds_wrapper_impl.cpp
  dds_wrapper.cpp 
  dds_wrapper.hpp
)

target_include_directories(dds_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/core)
target_link_libraries(dds_io PUBLIC core nlohmann_json::nlohmann_json ddscxxTelemetry CycloneDDS-CXX::ddscxx)

# Ensure the IDL files are properly generated before building dds_io
add_dependencies(dds_io ddscxxTelemetry)
target_include_directories(dds_io PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
