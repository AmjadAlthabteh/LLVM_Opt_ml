@echo off
REM ============================================
REM   AI Debugger - Run All Example Crashes
REM   See how it handles different bug types!
REM ============================================

echo.
echo ========================================
echo    AI Debugger - All Demos
echo ========================================
echo.
echo This will run AI Debugger on multiple
echo crash examples to show different bug types:
echo   - Null pointer dereference
echo   - Buffer overflow
echo   - Use-after-free
echo   - And more...
echo.
echo Press any key to start, or Ctrl+C to cancel
pause >nul

for %%f in (demo\*.txt demo\*.log) do (
    echo.
    echo ========================================
    echo Demo: %%~nxf
    echo ========================================
    echo.
    .\build\examples\Release\cli_tool.exe %%f
    echo.
    echo Press any key for next demo...
    pause >nul
)

echo.
echo ========================================
echo    All Demos Complete!
echo ========================================
echo.
echo You've seen how AI Debugger handles:
echo   - Different crash types
echo   - Various debugger formats
echo   - Root cause identification
echo   - Fix suggestions
echo.
echo Ready to try your own crashes?
echo   Run: ANALYZE_MY_CRASH.bat
echo.
pause
