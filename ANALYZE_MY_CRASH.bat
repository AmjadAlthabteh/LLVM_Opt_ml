@echo off
REM ============================================
REM   AI Debugger - Analyze Your Crash
REM   Drag and drop your crash file here!
REM ============================================

if "%~1"=="" (
    echo.
    echo ========================================
    echo    AI Debugger - Crash Analysis
    echo ========================================
    echo.
    echo HOW TO USE:
    echo.
    echo Option 1: Drag and drop your crash file onto this script
    echo Option 2: Run from command line:
    echo           ANALYZE_MY_CRASH.bat crash_file.log
    echo.
    echo Option 3: Enter the path now:
    echo.
    set /p CRASH_FILE="Enter path to crash file: "
    if "%CRASH_FILE%"=="" (
        echo No file specified. Exiting.
        pause
        exit /b 1
    )
) else (
    set CRASH_FILE=%~1
)

if not exist "%CRASH_FILE%" (
    echo.
    echo [ERROR] File not found: %CRASH_FILE%
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo    Analyzing: %CRASH_FILE%
echo ========================================
echo.

.\build\examples\Release\cli_tool.exe -v "%CRASH_FILE%"

echo.
echo ========================================
echo    Analysis Complete!
echo ========================================
echo.
echo Want to generate tests?
echo   Run: .\build\examples\Release\cli_tool.exe --generate-tests "%CRASH_FILE%"
echo.
echo Want to save the report?
echo   Run: .\build\examples\Release\cli_tool.exe -o report.txt "%CRASH_FILE%"
echo.
pause
