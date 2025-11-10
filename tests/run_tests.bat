@echo off
REM Run all tests for Student Records CMS
REM Windows batch file

echo =====================================
echo Running Student Records CMS Tests
echo =====================================
echo.

REM Build tests first
call build_tests.bat
if %ERRORLEVEL% NEQ 0 (
    echo Failed to build tests
    exit /b 1
)

REM Run each test suite
echo.
echo =====================================
echo Running Test Suites
echo =====================================
echo.

set TOTAL_FAILURES=0

echo [1/4] Running test_utils...
build\test_utils.exe
set /a TOTAL_FAILURES+=%ERRORLEVEL%

echo.
echo [2/4] Running test_database...
build\test_database.exe
set /a TOTAL_FAILURES+=%ERRORLEVEL%

echo.
echo [3/4] Running test_summary...
build\test_summary.exe
set /a TOTAL_FAILURES+=%ERRORLEVEL%

echo.
echo [4/4] Running test_commands...
build\test_commands.exe
set /a TOTAL_FAILURES+=%ERRORLEVEL%

echo.
echo =====================================
echo Test Results Summary
echo =====================================

if %TOTAL_FAILURES%==0 (
    echo ALL TESTS PASSED!
    exit /b 0
) else (
    echo SOME TESTS FAILED
    exit /b 1
)

