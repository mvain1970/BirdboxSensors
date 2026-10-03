import argparse
import os
import re
import time

import serial


def main() -> None:
    parser = argparse.ArgumentParser(description="Provision Birdbox Sensors over USB serial.")
    parser.add_argument("--port", default="COM12")
    parser.add_argument("--ssid", required=True)
    parser.add_argument("--password", default=os.environ.get("BIRDBOX_WIFI_PASSWORD"))
    args = parser.parse_args()
    if not args.password:
        parser.error("provide --password or set BIRDBOX_WIFI_PASSWORD")

    with serial.Serial(args.port, 115200, timeout=0.25) as connection:
        # The device may already be waiting for input before Windows opens the
        # serial port, so the original prompt is not guaranteed to be visible.
        deadline = time.monotonic() + 3
        received = bytearray()
        while time.monotonic() < deadline:
            received.extend(connection.read(4096))
            if b"PROVISIONING_REQUIRED" in received:
                break

        connection.write((args.ssid + "\n" + args.password + "\n").encode())
        connection.flush()

        deadline = time.monotonic() + 40
        received.clear()
        while time.monotonic() < deadline:
            received.extend(connection.read(4096))
            if b"PROVISIONING_COMPLETE" in received:
                address = re.search(rb"IP address: ([0-9.]+)", received)
                print("Provisioning complete.")
                if address:
                    print(f"IP address: {address.group(1).decode()}")
                return
            if b"Provisioning failed" in received:
                break

    raise RuntimeError("The device could not join the Wi-Fi network.")


if __name__ == "__main__":
    main()
