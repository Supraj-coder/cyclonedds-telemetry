# Pull Request Workflow Guide

This project uses feature branches and multiple PRs to organize code contributions. The codebase is split into two main feature areas that should be reviewed and merged separately.

## Feature Branches Overview

### PR #1: `feature/core-and-tests` - Core Types & Tests
**Purpose**: Establish application-level types and validation tests without external DDS dependencies.

**Files included**:
- `src/core/telemetry.hpp` — Application `TelemetryMessage` struct with JSON serialization helpers
- `src/core/idl/Telemetry.idl` — DDS IDL schema (used by both core and DDS integration)
- `src/core/Channel.h` — Inter-thread message channel (MPSC pattern)
- `src/core/CMakeLists.txt` — Build config for core library
- `tests/test_serialization.cpp` — Catch2 unit tests for JSON round-trip serialization
- `tests/CMakeLists.txt` — Build config for tests
- Top-level `CMakeLists.txt` — Root build configuration with `ENABLE_DDS` option
- `.gitignore` — Standard C++ build artifacts

**Build command**:
```bash
mkdir build && cd build
cmake -DENABLE_DDS=OFF -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j$(nproc)
```

**Test command**:
```bash
ctest -j2 --output-on-failure
# or
./tests/test_serialization
```

**PR Checklist**:
- [ ] Code compiles without warnings
- [ ] All tests pass (`ctest` or `./tests/test_serialization`)
- [ ] JSON serialization/deserialization works correctly
- [ ] No external dependencies beyond C++17 standard library and Catch2 (via FetchContent)

**Review Focus**:
- Type design: Is `TelemetryMessage` flexible enough for future extensions?
- JSON mapping: Does the key naming align with requirements?
- Test coverage: Are all serialization paths tested?
- IDL schema: Is the message structure correct for the DDS integration?

---

### PR #2: `feature/dds-integration` - DDS Publisher/Subscriber & Applications
**Purpose**: Add CycloneDDS-based pub/sub, wrap IDL-generated types, and build sensor/monitor applications.

**Dependencies**: Requires PR #1 (core-and-tests) to be merged first.

**Files included**:
- `src/dds_io/dds_wrapper.hpp` — Public API for Publisher and Subscriber classes
- `src/dds_io/dds_wrapper.cpp` — PIMPL implementation using CycloneDDS C++ API
- `src/dds_io/CMakeLists.txt` — IDL code generation (via `idlcxx_generate`) and `dds_io` library build
- `apps/sensor_hub/` — Telemetry publisher application (simulates sensors)
- `apps/monitor/` — Telemetry subscriber application (computes stats and detects anomalies)
- Updates to top-level `CMakeLists.txt` — Conditionally build DDS targets via `ENABLE_DDS`
- `cmake/idlc_wrapper.sh` — Helper script for IDL compilation

**Build command**:
```bash
export CMAKE_PREFIX_PATH="$HOME/cyclonedds_install;$HOME/cyclonedds-cxx_install"
export LD_LIBRARY_PATH="$HOME/cyclonedds_install/lib:$HOME/cyclonedds-cxx_install/lib:$LD_LIBRARY_PATH"
mkdir build_dds && cd build_dds
cmake -DENABLE_DDS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" ..
cmake --build . -- -j$(nproc)
```

**Test command** (from build_dds):
```bash
# In terminal 1: run monitor (subscriber)
./apps/monitor/monitor

# In terminal 2: run sensor_hub (publisher)
./apps/sensor_hub/sensor_hub
```

**Expected output**:
- `sensor_hub` prints: `PUBLISH: <sensor_name> <value> seq=<n>`
- `monitor` prints: Received messages and computed statistics

**PR Checklist**:
- [ ] CycloneDDS and CycloneDDS-CXX installed and configured (see below)
- [ ] Code compiles without warnings
- [ ] IDL generates successfully → produces `Telemetry.hpp` and `Telemetry.cpp`
- [ ] `sensor_hub` and `monitor` executables build and link correctly
- [ ] Runtime: both apps communicate via DDS topic
- [ ] Tests still pass (includes tests from PR #1)

**Environment Setup** (required for this PR):
1. Install CycloneDDS (C implementation):
```bash
export PREFIX="$HOME/cyclonedds_install"
git clone https://github.com/eclipse-cyclonedds/cyclonedds.git
cd cyclonedds
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target install
```

2. Install CycloneDDS-CXX (C++ wrapper + IDL generator):
```bash
export PREFIX_CXX="$HOME/cyclonedds-cxx_install"
git clone https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git
cd cyclonedds-cxx
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX="$PREFIX_CXX" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$PREFIX" ..
cmake --build . --target install
```

3. Set environment variables before configuring/building:
```bash
export CMAKE_PREFIX_PATH="$HOME/cyclonedds_install;$HOME/cyclonedds-cxx_install"
export LD_LIBRARY_PATH="$HOME/cyclonedds_install/lib:$HOME/cyclonedds-cxx_install/lib:$LD_LIBRARY_PATH"
```

**Review Focus**:
- PIMPL design: Is the Publisher/Subscriber interface clean and extensible?
- DDS integration: Does the mapping between `TelemetryMessage` and `Telemetry::Message` (generated type) work correctly?
- IDL generation: Is the CMake integration robust and idempotent?
- Thread safety: Are pub/sub operations thread-safe? Is the background reader thread correctly managed?
- Applications: Do `sensor_hub` and `monitor` demonstrate the full pub/sub cycle?
- Testing: Do all tests from PR #1 still pass? Any DDS-specific tests needed?

---

## How to Create PRs on GitHub

### From the Command Line (using `gh` CLI):

1. **Ensure your local repository is up-to-date**:
   ```bash
   cd ~/cyclonedds-project
   git fetch origin
   ```

2. **For PR #1 (core-and-tests)**, create and push the branch:
   ```bash
   git checkout -b feature/core-and-tests origin/main
   # (Make your changes, then commit)
   git commit -am "feat: add core telemetry types and tests"
   git push origin feature/core-and-tests
   ```

3. **Create the PR** (using GitHub CLI):
   ```bash
   gh pr create --title "Add core telemetry types and tests" \
     --body "Establishes TelemetryMessage struct, JSON serialization, and unit tests.\n\nFixes: #X\nCloses: #Y" \
     --base main \
     --head feature/core-and-tests
   ```

4. **For PR #2 (dds-integration)**, create it after PR #1 is merged:
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feature/dds-integration
   # (Make your DDS changes)
   git commit -am "feat: add DDS pub/sub and applications"
   git push origin feature/dds-integration
   
   gh pr create --title "Add DDS integration with pub/sub apps" \
     --body "Adds CycloneDDS-based Publisher/Subscriber, IDL code generation, and sensor_hub/monitor applications.\n\nDependends on: #PR_NUMBER_OF_PR1" \
     --base main \
     --head feature/dds-integration
   ```

### From the GitHub Web UI:

1. **Push your feature branch** to the remote:
   ```bash
   git push origin feature/core-and-tests
   ```

2. **Go to GitHub** → repository → **Branches** tab
3. Find your branch in the list and click **Compare & Pull Request**
4. Fill in:
   - **Title**: `Add core telemetry types and tests` (or similar)
   - **Description**: Include PR checklist, testing instructions, and any relevant context
   - **Base**: `main`
   - **Compare**: your feature branch
5. Click **Create pull request**

---

## Merge Strategy

### Recommended merge approach:
- **Use "Squash and merge"** for feature branches to keep main history clean
- Alternative: **Use "Create a merge commit"** if you want to preserve full commit history within each feature

### Merge order:
1. Review and merge PR #1 (feature/core-and-tests) first
2. Update PR #2 branch if needed (`git rebase origin/main` after PR #1 merges)
3. Review and merge PR #2 (feature/dds-integration)

### After merge:
```bash
# Clean up local branches
git branch -d feature/core-and-tests
git branch -d feature/dds-integration

# Or delete from remote (if not auto-deleted)
git push origin --delete feature/core-and-tests
git push origin --delete feature/dds-integration
```

---

## Common Issues & Troubleshooting

### Issue: "IDL compilation fails with syntax error"
- Verify `src/core/idl/Telemetry.idl` uses `#pragma keylist` (not `@key` annotations)
- Ensure field names don't conflict with IDL reserved words (avoid `sequence`)
- Check that `LD_LIBRARY_PATH` includes both CycloneDDS and CycloneDDS-CXX lib directories

### Issue: "CMake can't find CycloneDDS or CycloneDDS-CXX"
- Ensure installations are in the correct locations
- Verify `CMAKE_PREFIX_PATH` is set and exported before running `cmake`
- Use absolute paths and test with `find $HOME -name CycloneDDSConfig.cmake`

### Issue: "PR merges but tests fail on main"
- Ensure both PRs pass CI/CD pipelines before merging
- Check for cross-dependency issues (e.g., does PR #2 build without PR #1?)
- Verify that feature branches were rebased on main after each merge if needed

---

## Additional Notes

### File organization rationale:
- **src/core**: Application types independent of DDS (can be used with other middleware)
- **src/dds_io**: DDS-specific wrapper (hidden behind PIMPL; can be stubbed for testing)
- **apps**: Example applications using the core and DDS modules
- **tests**: Validation tests that run on core layer; can add DDS-specific tests in PR #2

### Future enhancements:
- Add integration tests for DDS pub/sub in a separate test file
- Add benchmarks for serialization performance
- Consider a stub implementation of Publisher/Subscriber for non-DDS builds
- Document QoS settings and topic configuration options

