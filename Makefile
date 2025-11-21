# Makefile for AI Debugger
# Convenience wrapper around CMake

.PHONY: all build clean test install examples format help

all: build

build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build .

release:
	@mkdir -p build
	@cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .

debug:
	@mkdir -p build
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

test: build
	@cd build && ctest --output-on-failure

clean:
	@rm -rf build

install: build
	@cd build && cmake --install .

examples: build
	@echo "Running simple example..."
	@./build/examples/simple_example

cli: build
	@./build/examples/cli_tool

format:
	@clang-format -i src/*.cpp include/ai_debugger/*.h examples/*.cpp

lint:
	@clang-tidy src/*.cpp -- -I./include

docs:
	@doxygen Doxyfile

help:
	@echo "AI Debugger Build System"
	@echo "======================="
	@echo ""
	@echo "Available targets:"
	@echo "  make build      - Build the library (default)"
	@echo "  make release    - Build optimized release version"
	@echo "  make debug      - Build with debug symbols"
	@echo "  make test       - Run all tests"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make install    - Install library system-wide"
	@echo "  make examples   - Build and run examples"
	@echo "  make cli        - Run command-line tool"
	@echo "  make format     - Format code with clang-format"
	@echo "  make lint       - Run clang-tidy"
	@echo "  make docs       - Generate documentation"
	@echo "  make help       - Show this message"
