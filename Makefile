.PHONY: all config build clean run test rebuild help install

# vcpkg configuration
VCPKG_TOOLCHAIN := external/vcpkg/scripts/buildsystems/vcpkg.cmake
VCPKG_TRIPLET := x64-linux

# Default target
all: build

# Configure the build system
config:
	@echo "Configuring CMake..."
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
		-DVCPKG_TARGET_TRIPLET=$(VCPKG_TRIPLET)

# Configure with specific build type
config-debug:
	@echo "Configuring CMake (Debug)..."
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
		-DVCPKG_TARGET_TRIPLET=$(VCPKG_TRIPLET) \
		-DCMAKE_BUILD_TYPE=Debug

config-release:
	@echo "Configuring CMake (Release)..."
	cmake -B build -S . \
		-DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
		-DVCPKG_TARGET_TRIPLET=$(VCPKG_TRIPLET) \
		-DCMAKE_BUILD_TYPE=Release

# Build the project
build:
	@echo "Building project..."
	cmake --build build

# Build with verbose output
build-verbose:
	@echo "Building project (verbose)..."
	cmake --build build --verbose

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	rm -rf build

# Rebuild from scratch
rebuild: clean config build

# Run the application (pass arguments via ARGS variable)
# Example: make run ARGS="--vcf test.vcf --threads 4"
run:
	@echo "Running VCF Tool..."
	./build/apps/vcf_tool/vcf_tool $(ARGS)

# Run tests
test:
	@echo "Running tests..."
	cd build && ctest --output-on-failure

# Install (requires sudo for system-wide install)
install:
	@echo "Installing..."
	cmake --build build --target install

# Help target
help:
	@echo "VCF Tool Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build the project (default)"
	@echo "  make config       - Configure CMake with default settings"
	@echo "  make config-debug - Configure CMake in Debug mode"
	@echo "  make config-release - Configure CMake in Release mode"
	@echo "  make build        - Build the project"
	@echo "  make build-verbose - Build with verbose output"
	@echo "  make clean        - Remove build directory"
	@echo "  make rebuild      - Clean and rebuild from scratch"
	@echo "  make run [ARGS=\"...\"] - Run the application with optional arguments"
	@echo "  make test         - Run all tests"
	@echo "  make install      - Install the project"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "CMake options (use with config target):"
	@echo "  BUILD_TESTS=ON/OFF        - Build tests (default: ON)"
	@echo "  BUILD_DOCS=ON/OFF         - Build documentation (default: OFF)"
	@echo "  WARNINGS_AS_ERRORS=ON/OFF - Treat warnings as errors (default: ON)"
	@echo ""
	@echo "Examples:"
	@echo "  make config-release && make build && make run"
	@echo "  make run ARGS=\"--vcf test.vcf --threads 4 --log-level debug\""
	@echo "  make run ARGS=\"--help\""
