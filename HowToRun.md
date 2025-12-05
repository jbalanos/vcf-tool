# How to Run VCF Tool

This guide provides detailed instructions for building and running the VCF Tool project.

---

## 0. Base Setup

### Project Overview

This project is built using:
- **CMake** (minimum version 3.15) - Modern build system for C++ projects
- **vcpkg** - Cross-platform package manager for C++ dependencies
- **Docker Compose** - For MongoDB database services

### vcpkg Setup

vcpkg is included as a Git submodule at [external/vcpkg](external/vcpkg/) (pinned to version 2025.10.17).

#### Initial Setup

If you've just cloned the repository, initialize and update the vcpkg submodule:

```bash
# Initialize and update submodules
git submodule update --init --recursive
```

#### Bootstrap vcpkg

Before first use, vcpkg must be bootstrapped to build its executable:

```bash
# On Linux/macOS
./external/vcpkg/bootstrap-vcpkg.sh

# On Windows
.\external\vcpkg\bootstrap-vcpkg.bat
```

The bootstrap process creates the vcpkg executable that CMake will use to install dependencies automatically.

### Dependencies

The project dependencies are declared in [vcpkg.json](vcpkg.json):
- **fmt** - Modern formatting library
- **spdlog** - Fast logging library
- **CLI11** - Command line parser
- **nlohmann-json** - JSON library
- **Catch2** - Testing framework
- **concurrentqueue** - Lock-free concurrent queue
- **mongo-cxx-driver** - MongoDB C++ driver

Dependencies are automatically installed by CMake during configuration using the `x64-linux` triplet (static linkage).

---

## 1. How to Build

### Option A: Using Make Shortcuts (Recommended)

The project provides convenient Make targets for common operations:

#### Basic Build Commands

```bash
# Show all available targets
make help

# Configure and build (one command)
make              # Same as 'make build'

# Configure CMake with default settings
make config

# Build the project
make build

# Build with verbose output (shows compiler commands)
make build-verbose
```

#### Configuration Variants

```bash
# Configure for Debug build
make config-debug

# Configure for Release build
make config-release
```

#### Clean and Rebuild

```bash
# Remove build directory
make clean

# Clean and rebuild from scratch
make rebuild      # Equivalent to: make clean config build
```

#### Running the Application

```bash
# Run with default settings
make run

# Run with arguments (pass via ARGS variable)
make run ARGS="--vcf data/assignment.vcf.gz --threads 4"
make run ARGS="--log-level debug"
make run ARGS="--help"
```

#### Testing

```bash
# Run all tests
make test
```

#### Installation

```bash
# Install to system (may require sudo)
make install
```

### Option B: Using CMake Presets (Alternative)

CMake presets provide predefined configurations for different build types:

```bash
# Configure Debug build
cmake --preset linux-debug

# Build Debug
cmake --build --preset linux-debug

# Run tests for Debug build
ctest --preset linux-debug

# Configure Release build
cmake --preset linux-release

# Build Release
cmake --build --preset linux-release

# Run tests for Release build
ctest --preset linux-release
```

Build outputs go to separate directories:
- Debug: `build/linux-debug/`
- Release: `build/linux-release/`

### Option C: Manual CMake Configuration

For custom configurations or advanced use cases:

```bash
# Basic configuration
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

#### CMake Build Options

Customize the build with these options:

```bash
# Disable tests
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux \
  -DBUILD_TESTS=OFF

# Release build with warnings as errors disabled
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux \
  -DCMAKE_BUILD_TYPE=Release \
  -DWARNINGS_AS_ERRORS=OFF

# Enable documentation build
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-linux \
  -DBUILD_DOCS=ON
```

Available options:
- `BUILD_TESTS=ON/OFF` - Build test suites (default: ON)
- `BUILD_DOCS=ON/OFF` - Build documentation (default: OFF)
- `WARNINGS_AS_ERRORS=ON/OFF` - Treat compiler warnings as errors (default: ON)
- `DEBUG=ON/OFF` - Enable debug definitions (auto-enabled in Debug builds)

### Debug Build Features

Debug builds automatically define these preprocessor macros:
- `VCF_DEBUG` - Enable debug-only code paths
- `VCF_ENABLE_ASSERTS` - Enable assertion checks
- `VCF_PROFILE` - Enable performance profiling code

---

## 2. Environment Variables

The project uses environment variables for configuration, particularly for MongoDB connection settings.

### Configuration File: `.env`

The [.env](.env) file contains environment variables loaded by the application:

```bash
MONGODB_URI="mongodb://root:example@localhost:27017/?authSource=admin"
MONGODB_DB_NAME="vcf_test_db"
MONGODB_COLLECTION_NAME="vcf_records"
```

### Environment Variable Reference

| Variable | Description | Default Value |
|----------|-------------|---------------|
| `MONGODB_URI` | MongoDB connection string with authentication | `mongodb://root:example@localhost:27017/?authSource=admin` |
| `MONGODB_DB_NAME` | Database name for VCF data | `vcf_test_db` |
| `MONGODB_COLLECTION_NAME` | Collection name for VCF records | `vcf_records` |

### Customizing Configuration

####  Export environment variables

Override settings temporarily in your shell:

```bash
export MONGODB_URI="mongodb://localhost:27017/"
export MONGODB_DB_NAME="my_custom_db"

# Then run the application
make run ARGS="--vcf data/assignment.vcf.gz"
```


## 3. Docker Compose

The project uses Docker Compose to run MongoDB database services required for VCF data storage.


## Complete Workflow Example

Here's a complete workflow from initial setup to running the application:

```bash
# 1. Clone repository (if not already done)
git clone <repository-url>
cd vcf-tool

# 2. Initialize vcpkg submodule
git submodule update --init --recursive

# 3. Bootstrap vcpkg
./external/vcpkg/bootstrap-vcpkg.sh

# 4. Start MongoDB services
docker compose up -d

# 5. Verify services are running
docker compose ps

# 6. Configure and build the project
make config-release
make build

# 7. Run the application
make run ARGS="--vcf data/assignment.vcf.gz --threads 4"

# 8. Run tests
make test

# 9. View results in Mongo Express
# Open browser to: http://localhost:8081
# Login: mongoexpressuser / mongoexpresspass

# 10. When done, stop services
docker compose down
```

---
