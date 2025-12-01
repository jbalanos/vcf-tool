# VCF Tool

A modular C++ project for working with VCF files.

## Building

```bash
# Configure the build
cmake -B build -S .

# Build the project
cmake --build build

# Or using make directly
cd build && make
```

## Running

```bash
./build/apps/vcf_tool/vcf_tool
```

## Running Tests

```bash
cd build
ctest
```

## Project Structure

```
vcf-tool/
├── CMakeLists.txt              # Root CMake configuration
├── cmake/                      # Custom CMake modules
├── external/                   # External dependencies
├── apps/                       # Application executables
│   └── vcf_tool/
│       ├── CMakeLists.txt
│       └── main.cpp
├── src/                        # Library modules
│   ├── core/                   # Core functionality
│   │   ├── include/vcf_tool/core/
│   │   ├── src/
│   │   └── CMakeLists.txt
│   ├── domain/                 # Domain models
│   │   ├── include/vcf_tool/domain/
│   │   ├── src/
│   │   └── CMakeLists.txt
│   └── utils/                  # Utility functions
│       ├── include/vcf_tool/utils/
│       ├── src/
│       └── CMakeLists.txt
├── tests/                      # Test suites
│   ├── unit/
│   └── integration/
├── docs/                       # Documentation
└── scripts/                    # Build and utility scripts
```

## Build Options

- `BUILD_TESTS`: Build test suites (default: ON)
- `BUILD_DOCS`: Build documentation (default: OFF)

Example:
```bash
cmake -B build -S . -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
```
