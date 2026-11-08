#!/usr/bin/env bash
# Export the UI, build and flash the firmware, and show the boot log.
#
# When the exported C code is older than the XML in ../roundedui, this sends
# Ctrl+E to the running LVGL Pro Editor (tools/export_ui.ps1) and waits for the
# new export. Then it builds, flashes, captures the boot log and prints a summary.
#
#   tools/flash.sh               export if needed, build, flash, 15 s of log
#   tools/flash.sh --no-wait     skip the wait (firmware-only changes)
#   PORT=COM4 LOG_SECONDS=30 tools/flash.sh
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
UI="$HERE/../../roundedui"
PORT="${PORT:-COM5}"
LOG_SECONDS="${LOG_SECONDS:-15}"
WAIT_TIMEOUT="${WAIT_TIMEOUT:-300}"

newest() { find "$UI" "$@" -printf '%T@\n' 2>/dev/null | sort -n | tail -1 | cut -d. -f1; }
ps() { powershell.exe -NoProfile -WindowStyle Hidden -ExecutionPolicy Bypass -File "$@"; }

if [ "${1:-}" != "--no-wait" ]; then
    start=$(date +%s)
    asked=0
    while :; do
        xml=$(newest -path "$UI/sim" -prune -o -path "$UI/preview-build" -prune -o -name '*.xml' -print)
        gen=$(newest -maxdepth 1 -name '*_gen.c')
        [ -n "$gen" ] && [ "$gen" -ge "$xml" ] && break
        # Ask the Editor to export, again every 60 s if nothing happens
        if [ $(( $(date +%s) - asked )) -ge 60 ]; then
            ps "$HERE/export_ui.ps1"
            asked=$(date +%s)
        fi
        if [ $(( $(date +%s) - start )) -ge "$WAIT_TIMEOUT" ]; then
            echo "TIMEOUT: no export newer than the XML after ${WAIT_TIMEOUT}s. Is the LVGL Pro Editor open on roundedui?"
            exit 2
        fi
        sleep 3
    done
    echo "Export is up to date ($(( $(date +%s) - start ))s wait)"
fi

ps "$HERE/idf.ps1" -cmd "-p $PORT build flash"
if ! grep -q "EXIT: 0" "$HERE/idf.log"; then
    echo "BUILD/FLASH FAILED:"
    grep -E "error:|FAILED|Error|No such|could not open port" "$HERE/idf.log" | head -30
    exit 1
fi
grep -E "binary size .* free" "$HERE/idf.log" | tail -1
echo "Flashed to $PORT"

ps "$HERE/serlog.ps1" -port "$PORT" -seconds "$LOG_SECONDS"
echo "---- boot log (errors, warnings, our modules, sysmon) ----"
grep -a -E "E \(|W \(|abort|panic|Guru|main:|imu:|pmu:|gps:|settings:|sysmon" "$HERE/serial.log" | grep -v "co5300_spi: The 3Ah\|GESTURE_RECOGNITION" | tail -40
