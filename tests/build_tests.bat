@echo off
REM Build script for Student Records CMS Test Suite
REM Windows batch file

echo =====================================
echo Building Student Records CMS Tests
echo =====================================
echo.

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Compiler settings
set CC=gcc
set CFLAGS=-I../include -I./unity -Wall -std=c11
set SRC_FILES=../src/cms_status.c ../src/database.c ../src/summary.c ../src/utils.c

echo [1/4] Compiling test_utils...
%CC% %CFLAGS% -o build/test_utils.exe test_utils.c unity/unity.c %SRC_FILES%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile test_utils
    goto :error
)

echo [2/4] Compiling test_database...
%CC% %CFLAGS% -o build/test_database.exe test_database.c unity/unity.c %SRC_FILES%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile test_database
    goto :error
)

echo [3/4] Compiling test_summary...
%CC% %CFLAGS% -o build/test_summary.exe test_summary.c unity/unity.c %SRC_FILES%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile test_summary
    goto :error
)

echo [4/4] Compiling test_commands...
%CC% %CFLAGS% -o build/test_commands.exe test_commands.c unity/unity.c %SRC_FILES% ../src/commands.c
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile test_commands
    goto :error
)

echo.
echo =====================================
echo Build Successful!
echo =====================================
echo.
echo Test executables created in tests/build/
echo.
echo To run tests:
echo   build\test_utils.exe
echo   build\test_database.exe
echo   build\test_summary.exe
echo   build\test_commands.exe
echo.

goto :end

:error
echo.
echo =====================================
echo Build Failed!
echo =====================================
echo.
exit /b 1

:end

