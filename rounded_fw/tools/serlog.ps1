# Runs serlog.py with the ESP-IDF Python (it has pyserial). Output: tools/serial.log
param([string]$port = "COM5", [int]$seconds = 15)
. 'C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1' *> $null
$py = Join-Path $env:IDF_PYTHON_ENV_PATH 'Scripts\python.exe'
& $py -I (Join-Path $PSScriptRoot 'serlog.py') $port $seconds *>&1 | Out-File -Encoding utf8 (Join-Path $PSScriptRoot 'serial.log')
