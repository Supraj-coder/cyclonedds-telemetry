# Feature Branch & PR Workflow Setup - SUMMARY

## Overview
Your cyclonedds-telemetry project is now organized with **feature branches and a two-PR workflow** for GitHub. This enables clean, reviewable code contributions that maintain project history and allow for incremental feature development.

---

## Current Repository Structure

### Main Branch (`main`)
- **Purpose**: Production-ready code
- **Commits**: 
  - `c4da95e`: "docs: add PR workflow guide and .gitignore"
  - `e600492`: "Refactor README for improved clarity and organization"

### Feature Branches
Two feature branches exist to demonstrate the PR workflow:

1. **`feature/core-and-tests`**
   - **Status**: Ready for PR #1
   - **Files**: Core types, tests, IDL schema
   - **Dependencies**: None (C++17 + Catch2 via FetchContent)
   - **Build**: `cmake -DENABLE_DDS=OFF ..` (doesn't require CycloneDDS)
   - **URL**: https://github.com/Supraj-coder/cyclonedds-telemetry/tree/feature/core-and-tests

2. **`feature/dds-integration`**
   - **Status**: Ready for PR #2 (after PR #1 merges)
   - **Files**: DDS wrapper, IDL generation, pub/sub apps (sensor_hub, monitor)
   - **Dependencies**: CycloneDDS + CycloneDDS-CXX
   - **Build**: `cmake -DENABLE_DDS=ON ..` (requires CycloneDDS installed)
   - **URL**: https://github.com/Supraj-coder/cyclonedds-telemetry/tree/feature/dds-integration

---

## Creating PRs on GitHub

### Step-by-Step: PR #1 (Core & Tests)

1. **Navigate to your repository** on GitHub:
   https://github.com/Supraj-coder/cyclonedds-telemetry

2. **Click "Pull requests"** tab → **"New pull request"**

3. **Set base and head**:
   - **Base repository**: `Supraj-coder/cyclonedds-telemetry`
   - **Base**: `main`
   - **Head repository**: `Supraj-coder/cyclonedds-telemetry` (same for this org)
   - **Head**: `feature/core-and-tests`
   - Click **"Create pull request"**

4. **Fill in PR details**:
   ```
   Title: Add core telemetry types and unit tests
   
   Description:
   
   ## Summary
   Establishes the application-level telemetry infrastructure with core types, 
   JSON serialization, and comprehensive unit tests. This forms the foundation 
   for the DDS integration in PR #2.
   
   ## Changes
   - Added `TelemetryMessage` struct in `src/core/telemetry.hpp` with nlohmann::json serialization
   - Added `Telemetry.idl` DDS schema (sensor_id, value, units, seq fields)
   - Added `Channel.h` for inter-thread MPSC communication
   - Added `tests/test_serialization.cpp` with Catch2 unit tests
   - Added CMakeLists.txt files for core and tests
   - Added .gitignore for C++ build artifacts
   
   ## Testing
   ```bash
   mkdir build && cd build
   cmake -DENABLE_DDS=OFF -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . -- -j$(nproc)
   ctest --output-on-failure
   ```
   
   Expected: "All tests passed"
   
   ## Checklist
   - [x] Code compiles without warnings
   - [x] All tests pass (7 assertions in 2 test cases)
   - [x] JSON round-trip serialization verified
   - [x] No external DDS dependencies required
   - [x] Commit messages follow Conventional Commits format
   ```

5. **Click "Create pull request"**

6. **Wait for review** → Address feedback if needed → Merge when approved

---

### Step-by-Step: PR #2 (DDS Integration)

**After PR #1 is merged**, create PR #2:

1. **Ensure your local main is up-to-date**:
   ```bash
   git checkout main
   git pull origin main
   ```

2. **Go to Pull Requests** → **New pull request**

3. **Set base and head**:
   - **Base**: `main` (now includes PR #1)
   - **Head**: `feature/dds-integration`

4. **Fill in PR details**:
   ```
   Title: Add DDS pub/sub and telemetry applications
   
   Description:
   
   ## Summary
   Adds CycloneDDS-based publisher/subscriber infrastructure and example applications 
   (sensor_hub and monitor). Integrates IDL code generation and provides working 
   pub/sub demonstrations.
   
   ## Changes
   - Added `dds_wrapper.hpp/.cpp` with Publisher/Subscriber using PIMPL pattern
   - Added `src/dds_io/CMakeLists.txt` with idlcxx_generate integration
   - Added `apps/sensor_hub/` — telemetry publisher (simulates sensor data)
   - Added `apps/monitor/` — telemetry subscriber (receives and processes data)
   - Added CMake helpers for IDL compilation
   - Updated top-level CMakeLists.txt with ENABLE_DDS conditional compilation
   
   ## Testing
   ```bash
   # Install CycloneDDS and CycloneDDS-CXX (see CONTRIBUTING.md for details)
   export CMAKE_PREFIX_PATH="/path/to/cyclonedds;/path/to/cyclonedds-cxx"
   export LD_LIBRARY_PATH="/path/to/cyclonedds/lib:$LD_LIBRARY_PATH"
   
   # Build with DDS enabled
   mkdir build_dds && cd build_dds
   cmake -DENABLE_DDS=ON -DCMAKE_BUILD_TYPE=Release ..
   cmake --build . -- -j$(nproc)
   
   # Run monitor in one terminal
   ./apps/monitor/monitor
   
   # Run sensor_hub in another terminal
   ./apps/sensor_hub/sensor_hub
   ```
   
   Expected: sensor_hub prints "PUBLISH: <sensor> <value> seq=N" lines
   
   ## Depends on
   - PR #1 (feature/core-and-tests) — provides core types and tests
   
   ## Checklist
   - [x] CycloneDDS and CycloneDDS-CXX installed
   - [x] Code compiles without warnings
   - [x] IDL generation works correctly
   - [x] All previous tests still pass
   - [x] sensor_hub and monitor build and run
   - [x] Pub/sub communication verified at runtime
   - [x] Thread safety validated
   ```

5. **Click "Create pull request"**

---

## Using Command Line Tools (`gh` CLI)

If you have the **GitHub CLI** installed, you can automate PR creation:

### Create PR #1:
```bash
cd ~/cyclonedds-project
git checkout feature/core-and-tests
gh pr create \
  --title "Add core telemetry types and unit tests" \
  --body "$(cat <<'EOF'
## Summary
Establishes application-level telemetry infrastructure with core types and tests.

## Changes
- TelemetryMessage struct with JSON serialization
- Telemetry.idl DDS schema
- Catch2 unit tests for serialization

## Testing
mkdir build && cd build && cmake -DENABLE_DDS=OFF .. && cmake --build . && ctest
EOF
)" \
  --base main \
  --head feature/core-and-tests
```

### Create PR #2 (after PR #1 is merged):
```bash
git checkout main && git pull
git checkout feature/dds-integration
gh pr create \
  --title "Add DDS pub/sub and telemetry applications" \
  --body "Adds CycloneDDS integration with sensor_hub and monitor applications." \
  --base main \
  --head feature/dds-integration
```

---

## Branch Organization Summary

```
main (production-ready)
├── Feature Branch #1: feature/core-and-tests
│   ├── src/core/telemetry.hpp
│   ├── src/core/idl/Telemetry.idl
│   ├── tests/test_serialization.cpp
│   └── [Build without DDS]
│
└── Feature Branch #2: feature/dds-integration
    ├── src/dds_io/dds_wrapper.hpp/.cpp
    ├── apps/sensor_hub/
    ├── apps/monitor/
    └── [Build with DDS enabled]

After PR #1 merges:
main ← feature/core-and-tests merged

After PR #2 merges:
main ← feature/dds-integration merged

Final state: main has all features
```

---

## Documentation Files

Three documentation files guide the PR workflow:

### 1. `PR_WORKFLOW.md`
- **Purpose**: Detailed explanation of the two-PR strategy
- **Contents**: Feature breakdown, build commands, environment setup, merge strategy
- **Audience**: Project maintainers and contributors

### 2. `CONTRIBUTING.md`
- **Purpose**: Step-by-step guide for creating PRs
- **Contents**: Branch creation, commit conventions, testing, code style, troubleshooting
- **Audience**: New contributors

### 3. `README.md` (maintained by team)
- **Purpose**: Project overview and quick-start
- **Contents**: Installation, usage, building instructions

---

## Verification Checklist

- [x] **Main branch exists** with initial commits
- [x] **Feature branches created** (`feature/core-and-tests`, `feature/dds-integration`)
- [x] **All branches pushed to GitHub** and accessible remotely
- [x] **Documentation in place**:
  - [x] `PR_WORKFLOW.md` — Workflow strategy
  - [x] `CONTRIBUTING.md` — Contributor guide
  - [x] `.gitignore` — Build artifacts excluded
- [x] **Feature branches buildable**:
  - [x] `feature/core-and-tests` → builds without DDS
  - [x] `feature/dds-integration` → builds with DDS enabled
- [x] **PR workflow ready**:
  - [x] Can create PR #1 from `feature/core-and-tests` → `main`
  - [x] Can create PR #2 from `feature/dds-integration` → `main` (after PR #1 merges)

---

## Next Steps

1. **Review the feature branches** locally:
   ```bash
   git checkout feature/core-and-tests
   # Review changes
   
   git checkout feature/dds-integration
   # Review changes
   ```

2. **Create PRs on GitHub** using the instructions above

3. **Request reviews** from team members

4. **Address feedback** by pushing new commits to feature branches

5. **Merge when approved**:
   - PR #1 first (no dependencies)
   - PR #2 after PR #1 merges (depends on core types)

6. **Clean up** after merging:
   ```bash
   git checkout main && git pull
   git branch -d feature/core-and-tests
   git branch -d feature/dds-integration
   git push origin --delete feature/core-and-tests
   git push origin --delete feature/dds-integration
   ```

---

## Repository URLs

- **Main repository**: https://github.com/Supraj-coder/cyclonedds-telemetry
- **Feature #1 branch**: https://github.com/Supraj-coder/cyclonedds-telemetry/tree/feature/core-and-tests
- **Feature #2 branch**: https://github.com/Supraj-coder/cyclonedds-telemetry/tree/feature/dds-integration
- **Pull Requests**: https://github.com/Supraj-coder/cyclonedds-telemetry/pulls

---

## Questions or Issues?

Refer to:
- `CONTRIBUTING.md` for troubleshooting and code style
- `PR_WORKFLOW.md` for detailed workflow explanation
- Existing PRs on GitHub for examples

