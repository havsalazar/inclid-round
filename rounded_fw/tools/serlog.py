"""Reset the board through RTS and print its serial log for N seconds.
usage: python serlog.py COM5 15"""
import sys
import time

import serial

port = serial.Serial(sys.argv[1], 115200, timeout=0.2)
port.dtr = False
port.rts = True
time.sleep(0.1)
port.rts = False
end = time.time() + float(sys.argv[2])
out = b""
while time.time() < end:
    out += port.read(4096)
sys.stdout.buffer.write(out)
