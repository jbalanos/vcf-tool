# VCF Tool

A modular C++ project for processing and analyzing VCF (Variant Call Format) files with MongoDB integration.

## Features

- High-performance VCF file parsing
- Concurrent processing with thread pool
- MongoDB integration for variant storage
- Modular architecture with clean dependency separation
- Comprehensive logging and error handling

## Quick Start

For detailed instructions on building, running, and deploying the project, see **[HowToRun.md](HowToRun.md)**.

### Minimal Setup

```bash
# 1. Initialize vcpkg submodule
git submodule update --init --recursive
./external/vcpkg/bootstrap-vcpkg.sh

# 2. Start MongoDB services
docker compose up -d

# 3. Build and run
make config
make build
make run ARGS="--vcf data/assignment.vcf.gz"
```

## Project Structure

```
vcf-tool/
├── CMakeLists.txt              # Root CMake configuration
├── CMakePresets.json           # CMake preset configurations
├── Makefile                    # Convenient build shortcuts
├── vcpkg.json                  # C++ dependency manifest
├── compose.yaml                # Docker services (MongoDB)
├── .env                        # Environment variables
│
├── cmake/                      # Custom CMake modules
│   ├── StandardProjectSettings.cmake
│   └── CompilerWarnings.cmake
│
├── external/                   # External dependencies
│   └── vcpkg/                  # vcpkg package manager (submodule)
│
├── apps/                       # Application executables
│   └── vcf_tool/               # Main VCF processing application
│       ├── CMakeLists.txt
│       └── main.cpp
│
├── src/                        # Library modules (layered architecture)
│   ├── utils/                  # Generic infrastructure layer
│   │   ├── include/vcf_tool/utils/
│   │   │   ├── Logger.h        # Logging façade (wraps spdlog)
│   │   │   ├── Format.h        # Formatting utilities (wraps fmt)
│   │   │   ├── Json.h          # JSON utilities (wraps nlohmann-json)
│   │   │   └── Utils.h         # Convenience header
│   │   └── CMakeLists.txt
│   │
│   ├── core/                   # App-specific abstractions
│   │   ├── include/vcf_tool/core/
│   │   │   ├── Config.h        # Configuration management
│   │   │   └── Core.h          # Convenience header
│   │   └── CMakeLists.txt
│   │
│   └── domain/                 # Business logic and domain models
│       ├── include/vcf_tool/domain/
│       └── CMakeLists.txt
│
├── tests/                      # Test suites 
│   ├── unit/                   # Unit tests
│   └── integration/            # Integration tests
│
├── docs/                       # Documentation
├── scripts/                    # Build and utility scripts
└── data/                       # Sample VCF files
```

## Architecture

The project follows a **layered library architecture** with strict dependency rules:

```
apps/vcf_tool/          # Main executable (owns CLI11)
    └─> src/core/       # App-specific abstractions (Config, etc.)
        └─> src/utils/  # Generic infrastructure (Logger, Format, Json)
            └─> Third-party libraries (fmt, spdlog, nlohmann-json)
    └─> src/domain/     # Domain models (uses utils for infrastructure)
```

### Dependency Rules

1. **`src/utils/`** - Owns all third-party infrastructure libraries
   - Wraps fmt, spdlog, nlohmann-json behind clean interfaces
   - Provides generic utilities only (no app-specific logic)

2. **`src/core/`** - App-specific abstractions
   - Depends on `utils` for infrastructure
   - Provides app-specific types (Config, AppError, etc.)

3. **`src/domain/`** - Business logic
   - Contains domain models and processing logic
   - Can use `utils` directly for logging, formatting
   - Depends on `core` (privately) for app integration

4. **`apps/*`** - Application entry points
   - Owns CLI parsing (CLI11)
   - Orchestrates core and domain modules

## Dependencies

Managed via [vcpkg.json](vcpkg.json) (automatically installed during build):

| Library | Purpose |
|---------|---------|
| **fmt** | Modern string formatting |
| **spdlog** | Fast logging library |
| **CLI11** | Command-line argument parsing |
| **nlohmann-json** | JSON serialization/parsing |
| **Catch2** | Testing framework |
| **concurrentqueue** | Lock-free concurrent queue |
| **mongo-cxx-driver** | MongoDB C++ driver |

## Build System

- **CMake 3.15+** - Modern build system with modular configuration
- **vcpkg** - Cross-platform C++ package manager (included as submodule)
- **C++20** - Modern C++ standard
- **Compiler warnings** - Strict warnings enforced (GCC/Clang/MSVC)


## Documentation

- **[HowToRun.md](HowToRun.md)** - Comprehensive guide for building, running, and deploying


## Requirements

- **C++20 compatible compiler** (GCC 10+, Clang 10+, MSVC 2019+)
- **CMake 3.15+**
- **Git** (for submodule management)
- **Docker & Docker Compose** (for MongoDB services)

