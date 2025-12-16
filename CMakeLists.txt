cmake_minimum_required(VERSION 3.15)
project(cyclonedds-telemetry CXX)

option(ENABLE_DDS "Enable building DDS-dependent code (find CycloneDDS)" OFF)
option(BUILD_TESTS "Enable building tests" ON)
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

if(BUILD_TESTS)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
  )
  FetchContent_MakeAvailable(Catch2)
endif()

if(ENABLE_DDS)
  message(STATUS "Looking for CycloneDDS packages")
  find_package(CycloneDDS REQUIRED)
  find_package(CycloneDDS-CXX REQUIRED)
  message(STATUS "CycloneDDS found: ${CycloneDDS_DIR}")
  message(STATUS "CycloneDDS-CXX found: ${CycloneDDS-CXX_DIR}")
endif()

add_subdirectory(src/core)
add_subdirectory(src/dds_io)
add_subdirectory(apps/sensor_hub)
add_subdirectory(apps/monitor)
if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
