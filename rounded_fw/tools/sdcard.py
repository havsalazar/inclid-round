"""Read the ride logs from the SD card over the USB-C cable, without taking the card out.

Stop recording first (the record button shows the round dot).

usage:
  python sdcard.py COM5 ls                   list the files on the card
  python sdcard.py COM5 get RIDE0001.CSV     download one file into ./rides
  python sdcard.py COM5 get all              download every .CSV file into ./rides
  python sdcard.py COM5 del RIDE0001.CSV     delete a file from the card
  add  -o <folder>  to download somewhere else

The firmware side is main/sd_serial.c. Lines that don't start with # are the
board's normal log output and are skipped.
"""
import argparse
import base64
import os
import sys
import time
import zlib

import serial

LINE_TIMEOUT_S = 5      # no answer for this long = give up
RETRIES = 3


def open_port(name):
    # DTR and RTS both low before opening: other combinations can reset the ESP32-S3
    port = serial.Serial()
    port.port = name
    port.baudrate = 115200          # ignored by the USB serial, required by pyserial
    port.timeout = 0.2
    port.dtr = False
    port.rts = False
    port.open()
    time.sleep(0.2)
    port.reset_input_buffer()
    return port


def lines(port):
    """Our protocol lines (starting with #), with the log output filtered out"""
    buf = b""
    last = time.time()
    while True:
        chunk = port.read(4096)
        if chunk:
            last = time.time()
            buf += chunk
            *done, buf = buf.split(b"\n")
            for raw in done:
                text = raw.rstrip(b"\r").decode("ascii", "replace")
                if text.startswith("#"):
                    yield text
        elif time.time() - last > LINE_TIMEOUT_S:
            raise TimeoutError("no answer from the board (wrong port, or old firmware?)")


def command(port, cmd):
    port.reset_input_buffer()
    port.write((cmd + "\n").encode("ascii"))
    port.flush()
    return lines(port)


def check_error(line):
    if line.startswith("#ERR"):
        sys.exit("board says: " + line[5:])


def do_ls(port):
    files = []
    for line in command(port, "LS"):
        check_error(line)
        if line == "#END":
            return files
        if line.startswith("#LS "):
            name, size = line[4:].rsplit(" ", 1)
            files.append((name, int(size)))


def do_get(port, name, folder):
    for attempt in range(1, RETRIES + 1):
        data = bytearray()
        size = None
        shown = -1
        start = time.time()
        for line in command(port, "GET " + name):
            check_error(line)
            if line.startswith("#FILE "):
                size = int(line.rsplit(" ", 1)[1])
            elif line.startswith("#D "):
                data += base64.b64decode(line[3:])
                pct = len(data) * 100 // max(size or 1, 1)
                if size and pct // 10 != shown // 10:
                    shown = pct
                    print(f"\r  {name}: {pct}%", end="", flush=True)
            elif line.startswith("#END"):
                crc = int(line.split(" ")[1], 16)
                break
        ok = zlib.crc32(data) == crc and (size is None or len(data) == size)
        if ok:
            os.makedirs(folder, exist_ok=True)
            path = os.path.join(folder, name)
            with open(path, "wb") as f:
                f.write(data)
            secs = time.time() - start
            print(f"\r  {name}: {len(data)} bytes in {secs:.1f} s -> {path}")
            return
        print(f"\r  {name}: transfer damaged, retrying ({attempt}/{RETRIES})")
    sys.exit(f"could not download {name}")


def do_del(port, name):
    for line in command(port, "DEL " + name):
        check_error(line)
        if line == "#OK":
            print(f"deleted {name}")
            return


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("port")
    ap.add_argument("action", choices=["ls", "get", "del"])
    ap.add_argument("name", nargs="?")
    ap.add_argument("-o", "--out", default="rides", help="download folder (default: ./rides)")
    args = ap.parse_args()
    if args.action != "ls" and not args.name:
        ap.error(f"{args.action} needs a file name")

    port = open_port(args.port)
    try:
        if args.action == "ls":
            files = do_ls(port)
            if not files:
                print("(card is empty)")
            for name, size in files:
                print(f"{name:14} {size:>10} bytes")
        elif args.action == "get":
            if args.name.lower() == "all":
                names = [n for n, _ in do_ls(port) if n.upper().endswith(".CSV")]
                if not names:
                    print("no .CSV files on the card")
                for n in names:
                    do_get(port, n, args.out)
            else:
                do_get(port, args.name, args.out)
        else:
            do_del(port, args.name)
    finally:
        port.close()


if __name__ == "__main__":
    main()
