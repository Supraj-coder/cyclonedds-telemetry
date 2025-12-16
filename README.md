# Telemetry System using CycloneDDS C++

# Goal
- Small multi-process C++ project skeleton to implement a **sensor_hub -> monitor** telemetry pipeline over DDS.

**Requirements:**
- CMake >= 3.15
- CycloneDDS
- CycloneDDS C++ binding and its dependencies

**Building and running**
- mkdir build && cd build
- cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DENABLE_DDS=ON -DCMAKE_PREFIX_PATH="/cyclonedds/install/location;/cyclonedds/cxx/install/location" ..
- cmake --build . -- -j

**Run tests**
- ctest --verbose

# Run applications (sensor_hub publishes, monitor subscribes and computes rolling stats)
**Terminal 1: start monitor**
- ./apps/monitor/monitor

**Terminal 2: start sensor_hub**
- ./apps/sensor_hub/sensor_hub

**Expected output:**
- **sensor_hub**: prints published telemetry (temp, pressure, flow)
- **monitor**: displays rolling stats (min, max, avg) per sensor, detects stale data (>2s old) and out-of-range warnings

# Notes & references
- **CycloneDDS Getting started:** https://cyclonedds.io/docs/cyclonedds/0.10.5/getting_started.html
- **CycloneDDS options**: https://github.com/eclipse-cyclonedds/cyclonedds/blob/master/docs/manual/options.md
- **CycloneDDS CXX docs/howto**: https://cyclonedds.io/docs/cyclonedds-cxx/latest/ and https://cyclonedds.io/docs/cyclonedds-cxx/latest/howto.html
