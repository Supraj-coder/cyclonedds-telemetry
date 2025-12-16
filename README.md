CycloneDDS Telemetry Example

Goal
- Small multi-process C++ project skeleton to implement a sensor_hub -> monitor telemetry pipeline over DDS.

Quick build (out-of-tree, requires CMake >= 3.15)

# without DDS (fast local dev)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DENABLE_DDS=OFF ..
cmake --build . -- -j

# with CycloneDDS installed at /home/suprajs/cyclonedds_install and /home/suprajs/cyclonedds-cxx_install
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DENABLE_DDS=ON \
  -DCMAKE_PREFIX_PATH="/home/suprajs/cyclonedds_install;/home/suprajs/cyclonedds-cxx_install" ..
cmake --build . -- -j

Run tests
ctest --verbose

Run applications (sensor_hub publishes, monitor subscribes and computes rolling stats)
# terminal 1: start monitor
./apps/monitor/monitor

# terminal 2: start sensor_hub
./apps/sensor_hub/sensor_hub

Expected output:
- sensor_hub: prints published telemetry (temp, pressure, flow)
- monitor: displays rolling stats (min, max, avg) per sensor, detects stale data (>2s old) and out-of-range warnings

Notes & references
- Onboarding task: multi-threaded sensor hub, monitor, optional logger. (extracted requirements in `/tmp/onboarding_task.txt` if available)
- CycloneDDS Getting started: https://cyclonedds.io/docs/cyclonedds/0.10.5/getting_started.html
- CycloneDDS options: https://github.com/eclipse-cyclonedds/cyclonedds/blob/master/docs/manual/options.md
- CycloneDDS CXX docs/howto: https://cyclonedds.io/docs/cyclonedds-cxx/latest/ and https://cyclonedds.io/docs/cyclonedds-cxx/latest/howto.html

Project layout
- `src/core` : core utilities (thread-safe channel, telemetry types, serialization)
  - `Channel.h` : header-only thread-safe queue (MPSC)
  - `telemetry.hpp` : TelemetryMessage + JSON (de)serialization
  - `idl/Telemetry.idl` : DDS IDL type definition (native DDS types when ENABLE_DDS=ON)
- `src/dds_io` : DDS publisher/subscriber wrapper (conditionally built when ENABLE_DDS=ON)
  - `dds_wrapper.hpp` / `.cpp` : Cyclone DDS CXX-based publisher and subscriber
- `apps/sensor_hub` : multi-threaded publisher process (spawns sensor threads, batches data, publishes via DDS)
- `apps/monitor` : subscriber + rolling stats + staleness/range warning detection
- `tests` : unit tests (Catch2; serialization, channel, pipeline)

Build artifacts
- `build/apps/sensor_hub/sensor_hub` : executable
- `build/apps/monitor/monitor` : executable
- `build/tests/test_serialization` : test executable

Next steps (optional enhancements)
- Implement logger process (optional in onboarding task)
- Add CI/CD (GitHub Actions for multi-config build)
- Expand monitor: configurable thresholds, CSV export
- Add stress testing and benchmarking (ddsperf-style latency/throughput measurements)
