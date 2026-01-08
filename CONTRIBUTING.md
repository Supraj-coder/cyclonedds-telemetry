# Contributing to Cyclone DDS Telemetry Project

Thank you for contributing! This document outlines the process for submitting PRs with feature branches.

## Quick Start: Creating a PR

### Prerequisites
1. Fork the repository on GitHub (if not a collaborator)
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/cyclonedds-telemetry.git
   cd cyclonedds-telemetry
   ```

### Step 1: Create a Feature Branch
```bash
# Update main to latest
git checkout main
git pull origin main

# Create a feature branch with a descriptive name
git checkout -b feature/your-feature-name
# Examples: feature/core-and-tests, feature/dds-integration
```

### Step 2: Make Your Changes
```bash
# Edit files, stage, and commit
git add <files>
git commit -m "feat: description of your changes"

# Optionally, make multiple logical commits
git commit -m "refactor: related changes"
```

**Commit message format** (follows Conventional Commits):
- `feat:` new feature
- `fix:` bug fix
- `docs:` documentation changes
- `test:` test additions/changes
- `refactor:` code refactoring
- `chore:` build, tooling, dependencies

Example:
```
feat: add DDS pub/sub wrapper with PIMPL pattern

- Implement Publisher class with PIMPL for DDS DataWriter encapsulation
- Implement Subscriber with background reader thread
- Add CMake IDL generation integration using idlcxx_generate

Fixes #42
```

### Step 3: Push to Remote
```bash
git push -u origin feature/your-feature-name
```

### Step 4: Create the PR on GitHub

#### Option A: Using GitHub CLI (`gh`)
```bash
gh pr create \
  --title "Your PR Title" \
  --body "Description of changes\n\nFixes #123" \
  --base main \
  --head feature/your-feature-name
```

#### Option B: Using GitHub Web UI
1. Go to https://github.com/YOUR_USERNAME/cyclonedds-telemetry
2. Click **Compare & Pull Request** (appears after pushing a new branch)
3. Ensure:
   - **Base** is set to `main` (from original repo)
   - **Head** is set to `feature/your-feature-name` (your branch)
4. Fill in PR title and description:
   ```
   Title: Add core telemetry types and tests
   
   Description:
   ## Changes
   - Added TelemetryMessage struct with JSON serialization
   - Added Catch2 unit tests for serialization round-trip
   - Added CMakeLists.txt for core and tests targets
   
   ## Testing
   ```bash
   mkdir build && cd build
   cmake -DENABLE_DDS=OFF ..
   cmake --build .
   ctest --output-on-failure
   ```
   
   ## Checklist
   - [x] Code compiles without warnings
   - [x] Tests pass
   - [x] No external dependencies beyond C++17 and Catch2
   ```
5. Click **Create pull request**

### Step 5: Address Review Comments
```bash
# Make requested changes
git add <files>
git commit -m "review: address feedback from code review"
git push origin feature/your-feature-name
# PR will auto-update with new commits
```

### Step 6: Merge
Once approved:
- Maintainers will merge using **Squash and merge** (to keep history clean) or **Create a merge commit**
- Your branch may be automatically deleted on GitHub

---

## Project Organization: Two Main Feature Areas

### PR #1: Core Types & Tests (`feature/core-and-tests`)
**Scope**: Application types, JSON serialization, and unit tests  
**Files**:
- `src/core/` — Core types and channel implementation
- `tests/` — Catch2 unit tests
- `CMakeLists.txt` — Root config with `ENABLE_DDS` option
- `Telemetry.idl` — IDL schema (shared between PRs)

**Build** (no DDS required):
```bash
mkdir build && cd build
cmake -DENABLE_DDS=OFF ..
cmake --build .
ctest --output-on-failure
```

**Merge after**: This should be merged before PR #2

---

### PR #2: DDS Integration & Applications (`feature/dds-integration`)
**Scope**: DDS pub/sub, IDL code generation, and example applications  
**Files**:
- `src/dds_io/` — Publisher/Subscriber wrapper using CycloneDDS
- `apps/` — `sensor_hub` and `monitor` applications
- `cmake/` — IDL compilation helpers

**Build** (requires CycloneDDS installed):
```bash
export CMAKE_PREFIX_PATH="/path/to/cyclonedds;/path/to/cyclonedds-cxx"
export LD_LIBRARY_PATH="/path/to/cyclonedds/lib:$LD_LIBRARY_PATH"
mkdir build_dds && cd build_dds
cmake -DENABLE_DDS=ON ..
cmake --build .
```

**Merge after**: Depends on PR #1 being merged first

---

## Code Style & Standards

### C++ Style
- **Standard**: C++17
- **Naming**: 
  - Classes: `PascalCase` (e.g., `Publisher`)
  - Methods: `camelCase` (e.g., `publish()`)
  - Members: `snake_case_` (with trailing `_` for private members)
  - Constants: `SCREAMING_SNAKE_CASE`
- **Formatting**: 4-space indentation, no tabs
- **PIMPL**: Use for DDS types to hide implementation details

### Header Guards
```cpp
#pragma once
// or
#ifndef NAMESPACE_CLASSNAME_H
#define NAMESPACE_CLASSNAME_H
// ... content ...
#endif
```

### Documentation
- Add comments for non-obvious logic
- Document public APIs with brief descriptions
- Include example usage in headers if beneficial

---

## Testing

### Unit Tests
All new code should include tests. Use Catch2:

```cpp
#include <catch2/catch_test_macros.hpp>
#include "telemetry.hpp"

TEST_CASE("TelemetryMessage JSON serialization") {
    TelemetryMessage msg{"sensor1", 42.5, "°C", 1};
    nlohmann::json j = msg;
    
    REQUIRE(j["sensor_id"] == "sensor1");
    REQUIRE(j["value"] == 42.5);
}
```

Run tests:
```bash
ctest --output-on-failure
# or directly:
./tests/test_serialization
```

### Integration Tests (for DDS PRs)
Run both `sensor_hub` and `monitor` in separate terminals and verify data flows:

```bash
# Terminal 1
./apps/monitor/monitor

# Terminal 2
./apps/sensor_hub/sensor_hub
# Expected output: PUBLISH: <sensor> <value> seq=<n>
```

---

## Common Issues & Help

### "CMake can't find CycloneDDS"
Ensure:
1. CycloneDDS is installed to `$PREFIX`
2. `CMAKE_PREFIX_PATH` includes `$PREFIX` and `$PREFIX-cxx`
3. Verify: `find $PREFIX -name CycloneDDSConfig.cmake`

### "IDL compilation fails"
1. Check `Telemetry.idl` syntax (no reserved word conflicts)
2. Set `LD_LIBRARY_PATH` to include both DDS C and CXX lib dirs
3. Try manual compile: `idlc -l /path/to/libcycloneddsidlcxx.so Telemetry.idl`

### "Tests fail after my changes"
1. Run tests locally before pushing: `ctest -j2 --output-on-failure`
2. Ensure no unintended side effects in shared code
3. Review test output for specific assertion failures

---

## PR Review Checklist

**Before submitting:**
- [ ] Code compiles without warnings
- [ ] All tests pass (`ctest`)
- [ ] Commit messages follow Conventional Commits format
- [ ] No large binary files or generated code committed
- [ ] PR description includes: what changed, why, and testing instructions

**For reviewers:**
- [ ] Code quality and style adherence
- [ ] Test coverage adequate?
- [ ] API design clear and extensible?
- [ ] Dependencies justified?
- [ ] Documentation (comments, commit messages) adequate?

---

## Merging

### Strategy
1. **"Squash and merge"**: Recommended for feature branches (keeps main history clean)
2. **"Create a merge commit"**: Alternative if you want to preserve full commit history

### Workflow
```bash
# After merge, update local main
git checkout main
git pull origin main

# Delete local feature branch (optional)
git branch -d feature/your-feature-name

# Delete remote branch (if not auto-deleted)
git push origin --delete feature/your-feature-name
```

---

## Questions?

See `PR_WORKFLOW.md` for detailed workflow documentation, or contact maintainers.
