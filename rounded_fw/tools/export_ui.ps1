# Ask the running LVGL Pro Editor to export the C code (its Ctrl+E), without clicking.
# Brings the Editor window to the front, checks it really is in front, sends Ctrl+E,
# then gives the focus back to the window that had it. Exit 0 = sent, 1 = not sent.
Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class Win {
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
}
"@

$editor = Get-Process LVGL_Pro_Editor -ErrorAction SilentlyContinue |
          Where-Object { $_.MainWindowHandle -ne [IntPtr]::Zero } | Select-Object -First 1
if (-not $editor) { Write-Output "LVGL Pro Editor window not found"; exit 1 }

$hwnd = $editor.MainWindowHandle
$previous = [Win]::GetForegroundWindow()

if ([Win]::IsIconic($hwnd)) { [Win]::ShowWindow($hwnd, 9) | Out-Null }   # SW_RESTORE
# Windows only lets a process take the focus right after a key event: tap Alt first
[Win]::keybd_event(0x12, 0, 0, [UIntPtr]::Zero); [Win]::keybd_event(0x12, 0, 2, [UIntPtr]::Zero)
[Win]::SetForegroundWindow($hwnd) | Out-Null
Start-Sleep -Milliseconds 400

# Only type if the Editor really is in front, so Ctrl+E can never land in another app
$pid_front = 0
[Win]::GetWindowThreadProcessId([Win]::GetForegroundWindow(), [ref]$pid_front) | Out-Null
$editor_pids = (Get-Process LVGL_Pro_Editor).Id
if ($editor_pids -notcontains [int]$pid_front) { Write-Output "Could not bring the Editor to the front: nothing sent"; exit 1 }

[Win]::keybd_event(0x11, 0, 0, [UIntPtr]::Zero)      # Ctrl down
[Win]::keybd_event(0x45, 0, 0, [UIntPtr]::Zero)      # E
[Win]::keybd_event(0x45, 0, 2, [UIntPtr]::Zero)
[Win]::keybd_event(0x11, 0, 2, [UIntPtr]::Zero)      # Ctrl up
Start-Sleep -Milliseconds 300

if ($previous -ne [IntPtr]::Zero -and $previous -ne $hwnd) { [Win]::SetForegroundWindow($previous) | Out-Null }
Write-Output "Sent Ctrl+E to the LVGL Pro Editor"
exit 0
