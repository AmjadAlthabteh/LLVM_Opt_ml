@echo off
setlocal enabledelayedexpansion

echo ================================
echo AI Debugger Build Script (Windows)
echo ================================
echo.

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set BUILD_DIR=build
set ENABLE_TESTS=%2
if "%ENABLE_TESTS%"=="" set ENABLE_TESTS=ON

set ENABLE_EXAMPLES=%3
if "%ENABLE_EXAMPLES%"=="" set ENABLE_EXAMPLES=ON

echo Configuration:
echo   Build Type: %BUILD_TYPE%
echo   Tests: %ENABLE_TESTS%
echo   Examples: %ENABLE_EXAMPLES%
echo.

if exist %BUILD_DIR% (
    echo Cleaning previous build...
    rmdir /s /q %BUILD_DIR%
)

mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo Running CMake...
cmake .. ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DBUILD_TESTS=%ENABLE_TESTS% ^
    -DBUILD_EXAMPLES=%ENABLE_EXAMPLES%

if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

echo.
echo Building...
cmake --build . --config %BUILD_TYPE%

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

if "%ENABLE_TESTS%"=="ON" (
    echo.
    echo Running tests...
    ctest --output-on-failure -C %BUILD_TYPE%
)

echo.
echo ================================
echo Build completed successfully!
echo ================================
echo.
echo Binaries location:
echo   Library: %BUILD_DIR%\%BUILD_TYPE%\ai_debugger.lib
if "%ENABLE_EXAMPLES%"=="ON" (
    echo   Examples: %BUILD_DIR%\examples\%BUILD_TYPE%\
)
echo.
echo To install:
echo   cd %BUILD_DIR% ^&^& cmake --install .
echo.

cd ..
