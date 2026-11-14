@echo off
rem Download every ride log (.CSV) from the SD card over the USB-C cable.
rem Double-click it, or run:  get_rides.bat [COM port]   (default COM5)
rem The files go to rounded_fw\rides. Stop recording on the device first.

setlocal
set "PORT=%~1"
if "%PORT%"=="" set "PORT=COM5"
for %%I in ("%~dp0..\rides") do set "OUT=%%~fI"

rem The ESP-IDF Python already has pyserial; otherwise the normal one (pip install pyserial)
set "PY=C:\Espressif\tools\python\v6.1\venv\Scripts\python.exe"
if not exist "%PY%" set "PY=python"

echo Downloading ride logs from %PORT% to %OUT%
"%PY%" -I "%~dp0sdcard.py" %PORT% get all -o "%OUT%"
if errorlevel 1 (
    echo.
    echo Download failed. Is the board plugged in on %PORT% and not recording?
) else (
    echo.
    echo Done.
)

rem Keep the window open when started by double-click
echo %CMDCMDLINE% | "%SystemRoot%\System32\find.exe" /i "%~0" >nul && pause
endlocal
