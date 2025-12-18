cmake_minimum_required(VERSION 3.15)
project(cyclonedds-telemetry CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# JSON library for (de)serialization
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# CycloneDDS is now mandatory
message(STATUS "Looking for CycloneDDS packages (DDS is required)")
find_package(CycloneDDS REQUIRED)
find_package(CycloneDDS-CXX REQUIRED)
message(STATUS "CycloneDDS found: ${CycloneDDS_DIR}")
message(STATUS "CycloneDDS-CXX found: ${CycloneDDS-CXX_DIR}")
include(${CycloneDDS-CXX_DIR}/idlcxx/Generate.cmake)

add_subdirectory(src/core)
add_subdirectory(src/dds_io)
add_subdirectory(apps/sensor_hub)
add_subdirectory(apps/monitor)
