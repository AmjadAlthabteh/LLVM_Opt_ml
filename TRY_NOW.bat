@echo off
REM ============================================
REM   AI Debugger - INSTANT DEMO
REM   Double-click to see it in action!
REM ============================================

echo.
echo ========================================
echo    AI Debugger - LIVE DEMO
echo ========================================
echo.
echo Running AI analysis on a crash...
echo.
timeout /t 2 >nul

.\build\examples\Release\cli_tool.exe demo\null_pointer_crash.txt

echo.
echo ========================================
echo    Demo Complete!
echo ========================================
echo.
echo What just happened?
echo - AI Debugger analyzed a null pointer crash
echo - Identified the bug type and location
echo - Suggested how to fix it
echo - Gave prevention tips
echo.
echo Want to try more?
echo   Run: quick_start.bat
echo.
echo Have your own crash to analyze?
echo   Run: .\build\examples\Release\cli_tool.exe your_crash.log
echo.
pause
