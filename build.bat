@echo off
echo Compiling TOML Validator...

REM Check if g++ is available
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found
    echo Please ensure MinGW or TDM-GCC is installed and added to PATH
    pause
    exit /b 1
)

REM Remove old executable if exists
if exist toml-verify.exe (
    echo Removing old executable...
    del /F /Q toml-verify.exe
    timeout /t 1 /nobreak >nul
)

REM Compile
g++ -std=c++17 -Wall -Wextra -o toml-verify.exe toml_verify.cpp

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Compilation successful!
    echo Executable: toml-verify.exe
    echo ========================================
    echo.
    echo Usage:
    echo 1. Drag and drop a TOML file onto toml-verify.exe
    echo 2. Or run in command line: toml-verify.exe ^<filepath^>
    echo.
) else (
    echo.
    echo Compilation failed, please check error messages
)

pause
