@echo off
echo =================================
echo AI Debugger - Quick Test (Windows)
echo =================================
echo.

echo Test 1: Building simple demo...
echo --------------------------------
echo.

g++ -std=c++17 demo\simple_test.cpp -o demo_test.exe 2>&1

if %ERRORLEVEL% EQU 0 (
    echo [OK] Compilation successful
    echo.
    echo Running demo...
    echo.
    demo_test.exe
    echo.
) else (
    echo [FAIL] Compilation failed
    echo.
    echo Trying with MSVC...
    cl /EHsc /std:c++17 demo\simple_test.cpp /Fe:demo_test.exe 2>&1
    if %ERRORLEVEL% EQU 0 (
        demo_test.exe
    )
)

echo.
echo =================================
echo Test 2: Checking project files
echo =================================
echo.

echo Header files:
dir /s /b include\*.h 2>nul | find /c /v ""

echo.
echo Source files:
dir /s /b src\*.cpp 2>nul | find /c /v ""

echo.
echo Test files:
dir /s /b tests\*.cpp 2>nul | find /c /v ""

echo.
echo Example files:
dir /s /b examples\*.cpp 2>nul | find /c /v ""

echo.
echo =================================
echo Test 3: CMake configuration check
echo =================================
echo.

where cmake >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] CMake found
    cmake --version | findstr /C:"cmake version"
    echo.
    if exist CMakeLists.txt (
        echo [OK] Main CMakeLists.txt exists
    )
) else (
    echo [WARN] CMake not found (needed for full build)
)

echo.
echo =================================
echo Test 4: Build system check
echo =================================
echo.

if exist Makefile (
    echo [OK] Makefile exists
)

if exist build.bat (
    echo [OK] Build script exists
)

if exist package.json (
    echo [OK] Package configuration exists
)

echo.
echo =================================
echo Quick Test Complete
echo =================================
echo.
echo Next steps:
echo   1. Run full build: build.bat
echo   2. Run tests: cd build ^&^& ctest
echo   3. Try examples: build\examples\Release\simple_example.exe
echo.

pause
