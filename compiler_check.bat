@echo off
setlocal enabledelayedexpansion

set TMP_CPP=.compiler_check.cpp
set TMP_EXE=.compiler_check.exe

echo #include ^<iostream^> > %TMP_CPP%
echo int main() { std::cout ^<^< "compiler-check-ok\n"; return 0; } >> %TMP_CPP%

where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Using compiler: cl
    cl /EHsc /std:c++17 %TMP_CPP% /Fe:%TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    %TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    goto ok
)

where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Using compiler: g++
    g++ -std=c++17 %TMP_CPP% -o %TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    %TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    goto ok
)

where clang++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Using compiler: clang++
    clang++ -std=c++17 %TMP_CPP% -o %TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    %TMP_EXE% >nul 2>&1
    if %ERRORLEVEL% NEQ 0 goto fail
    goto ok
)

echo [FAIL] No C++ compiler found (cl, g++, clang++).
goto cleanup_fail

:ok
echo [OK] C++17 compiler works
goto cleanup_ok

:fail
echo [FAIL] Compiler failed to build a C++17 program.
goto cleanup_fail

:cleanup_ok
del /q %TMP_CPP% %TMP_EXE% >nul 2>&1
exit /b 0

:cleanup_fail
del /q %TMP_CPP% %TMP_EXE% >nul 2>&1
exit /b 1
