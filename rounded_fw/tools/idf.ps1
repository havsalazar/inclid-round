# Runs idf.py from Git Bash without a console window and without MSYS confusing it.
#   powershell -NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass -File idf.ps1 -cmd "-p COM5 build flash"
# Output goes to tools/idf.log, with "EXIT: <code>" as the last line.
param([string]$cmd = "build")
foreach ($v in 'MSYSTEM','MSYSTEM_PREFIX','MSYSTEM_CHOST','MSYSTEM_CARCH','MINGW_PREFIX','MINGW_CHOST','MINGW_PACKAGE_PREFIX','OSTYPE','SHELL','TERM','ORIGINAL_PATH','EXEPATH','PLINK_PROTOCOL') { Remove-Item "Env:$v" -ErrorAction SilentlyContinue }
. 'C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1' *> $null
$env:PYTHONUTF8 = '1'
Set-Location (Split-Path $PSScriptRoot -Parent)
$log = Join-Path $PSScriptRoot 'idf.log'
$py = Join-Path $env:IDF_PYTHON_ENV_PATH 'Scripts\python.exe'
$idf = Join-Path $env:IDF_PATH 'tools\idf.py'
& $py $idf $cmd.Split(' ') *>&1 | Out-File -Encoding utf8 $log
"EXIT: $LASTEXITCODE" | Add-Content -Encoding utf8 $log
