# Installation Guide

## Prerequisites

### Required
- C++17 compatible compiler
  - GCC 7.0+
  - Clang 5.0+
  - MSVC 2017+
- CMake 3.15+

### Optional
- LLVM 10.0+ (for advanced static analysis)
- Google Test (for running tests)
- Python 3.7+ (for ML-enhanced prediction)

## Platform-Specific Instructions

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Optional: Install LLVM
sudo apt-get install -y llvm-dev

# Optional: Install GTest
sudo apt-get install -y libgtest-dev

# Clone and build
git clone https://github.com/yourusername/ai-debugger.git
cd ai-debugger
./build.sh Release ON ON

# Install
cd build
sudo cmake --install .
```

### macOS

```bash
# Install dependencies using Homebrew
brew install cmake llvm

# Optional: Install GTest
brew install googletest

# Clone and build
git clone https://github.com/yourusername/ai-debugger.git
cd ai-debugger
./build.sh Release ON ON

# Install
cd build
sudo cmake --install .
```

### Windows

```powershell
# Using Visual Studio Developer Command Prompt

# Install dependencies (using vcpkg)
vcpkg install llvm gtest

# Clone and build
git clone https://github.com/yourusername/ai-debugger.git
cd ai-debugger
build.bat Release ON ON

# Install
cd build
cmake --install .
```

## Build Options

### Standard Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Debug Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### With LLVM Support

```bash
mkdir build && cd build
cmake -DENABLE_LLVM=ON ..
cmake --build .
```

### Without Tests

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=OFF ..
cmake --build .
```

### Custom Install Location

```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
cmake --build .
cmake --install .
```

## Using Convenience Scripts

### Linux/macOS

```bash
# Release build with tests and examples
./build.sh Release ON ON

# Debug build without tests
./build.sh Debug OFF ON

# Release build, tests only
./build.sh Release ON OFF
```

### Windows

```batch
REM Release build with tests and examples
build.bat Release ON ON

REM Debug build without tests
build.bat Debug OFF ON
```

## Using Makefile (Linux/macOS)

```bash
# Standard build
make

# Release build
make release

# Debug build
make debug

# Run tests
make test

# Install
sudo make install

# Run examples
make examples

# Clean
make clean
```

## Verification

After installation, verify:

```bash
# Check library
ls /usr/local/lib/libai_debugger.a

# Check headers
ls /usr/local/include/ai_debugger/

# Run examples
./build/examples/simple_example

# Run tests
cd build && ctest
```

## Integration with CMake Projects

```cmake
# In your CMakeLists.txt
find_package(AIDebugger REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp AIDebugger::ai_debugger)
```

## Integration with Make Projects

```makefile
# In your Makefile
CXXFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib -lai_debugger

myapp: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
```

## Troubleshooting

### CMake not found

```bash
# Ubuntu
sudo apt-get install cmake

# macOS
brew install cmake

# Windows - Download from cmake.org
```

### Compiler not C++17 compatible

```bash
# Update GCC on Ubuntu
sudo apt-get install gcc-9 g++-9
export CXX=g++-9

# Update Clang
sudo apt-get install clang-10
export CXX=clang++-10
```

### LLVM not found

```bash
# Set LLVM path explicitly
cmake -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm ..
```

### GTest not found

```bash
# Install manually
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
sudo make install
```

### Permission denied on install

```bash
# Use sudo
sudo cmake --install .

# Or install to user directory
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
```

## Uninstallation

```bash
# From build directory
sudo cmake --build . --target uninstall

# Or manually
sudo rm -rf /usr/local/include/ai_debugger
sudo rm -f /usr/local/lib/libai_debugger.a
```

## Docker Installation

```dockerfile
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    llvm-dev \
    libgtest-dev

WORKDIR /app
COPY . .

RUN ./build.sh Release ON ON && \
    cd build && \
    cmake --install .

CMD ["./build/examples/cli_tool"]
```

Build and run:

```bash
docker build -t ai-debugger .
docker run -v /path/to/crashes:/crashes ai-debugger /crashes/trace.log
```
