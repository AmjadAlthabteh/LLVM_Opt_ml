@echo off
REM AI Debugger - Quick Start Script for Windows
REM This script helps you get started with AI Debugger quickly

echo ========================================
echo   AI Debugger - Quick Start
echo ========================================
echo.

REM Check if already built
if exist "build\examples\Release\cli_tool.exe" (
    echo [OK] AI Debugger is already built!
    echo.
    goto :menu
)

echo [INFO] AI Debugger needs to be built first.
echo [INFO] This will take 2-5 minutes...
echo.
choice /C YN /M "Would you like to build it now"
if errorlevel 2 goto :end
if errorlevel 1 goto :build

:build
echo.
echo Building AI Debugger...
echo.
call build.bat Release ON ON
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed. Please check the error messages above.
    echo [TIP] Make sure you're running this from a Visual Studio Developer Command Prompt
    pause
    goto :end
)
echo.
echo [OK] Build completed successfully!
echo.

:menu
echo What would you like to do?
echo.
echo  1. Try a simple example (null pointer crash)
echo  2. Try all examples
echo  3. Analyze your own crash file
echo  4. Show version info
echo  5. Show help
echo  6. Exit
echo.
choice /C 123456 /N /M "Enter your choice (1-6): "
set choice=%errorlevel%

if %choice%==1 goto :example1
if %choice%==2 goto :allexamples
if %choice%==3 goto :custom
if %choice%==4 goto :version
if %choice%==5 goto :help
if %choice%==6 goto :end

:example1
echo.
echo ========================================
echo  Running: Null Pointer Crash Example
echo ========================================
echo.
.\build\examples\Release\cli_tool.exe demo\null_pointer_crash.txt
echo.
echo ========================================
echo  Example Complete!
echo ========================================
echo.
pause
goto :menu

:allexamples
echo.
echo ========================================
echo  Running All Examples
echo ========================================
echo.

for %%f in (demo\*.txt demo\*.log) do (
    echo.
    echo ----------------------------------------
    echo Testing: %%f
    echo ----------------------------------------
    .\build\examples\Release\cli_tool.exe %%f
    echo.
    timeout /t 2 >nul
)

echo.
echo ========================================
echo  All Examples Complete!
echo ========================================
echo.
pause
goto :menu

:custom
echo.
echo ========================================
echo  Analyze Your Own Crash File
echo ========================================
echo.
set /p crashfile="Enter path to your crash file: "
if not exist "%crashfile%" (
    echo [ERROR] File not found: %crashfile%
    echo.
    pause
    goto :menu
)
echo.
echo Analyzing: %crashfile%
echo.
.\build\examples\Release\cli_tool.exe -v "%crashfile%"
echo.
echo ========================================
echo  Analysis Complete!
echo ========================================
echo.
echo Would you like to:
echo  1. Generate tests for this crash
echo  2. Save report to file
echo  3. Return to menu
echo.
choice /C 123 /N /M "Enter choice (1-3): "
set subchoice=%errorlevel%

if %subchoice%==1 (
    echo.
    echo Generating tests...
    .\build\examples\Release\cli_tool.exe --generate-tests "%crashfile%"
    echo.
    pause
)
if %subchoice%==2 (
    set /p reportfile="Enter report filename (e.g., report.txt): "
    echo.
    echo Saving report...
    .\build\examples\Release\cli_tool.exe -o "%reportfile%" "%crashfile%"
    echo.
    echo Report saved to: %reportfile%
    pause
)
goto :menu

:version
echo.
.\build\examples\Release\cli_tool.exe --version
echo.
pause
goto :menu

:help
echo.
.\build\examples\Release\cli_tool.exe --help
echo.
echo ========================================
echo  Additional Resources
echo ========================================
echo.
echo  - GETTING_STARTED.md : Complete beginner guide
echo  - SIMPLE_README.md   : Quick overview
echo  - README.md          : Full documentation
echo  - demo\              : Sample crash files
echo.
pause
goto :menu

:end
echo.
echo Thanks for using AI Debugger!
echo.
echo Next steps:
echo  - Read GETTING_STARTED.md for a complete guide
echo  - Try demo examples in the demo\ folder
echo  - Use on your own crash files!
echo.
echo For help, run: cli_tool.exe --help
echo.
