@echo off
echo Installing EVolocityArduino dependencies...
echo.

where arduino-cli >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: arduino-cli not found.
    echo Download it from https://arduino.github.io/arduino-cli/latest/installation/
    exit /b 1
)

arduino-cli lib install RF24
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to install RF24.
    exit /b 1
)

echo.
echo All dependencies installed successfully.
