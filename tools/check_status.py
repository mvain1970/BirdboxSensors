import argparse
import time

import serial


def main() -> None:
    parser = argparse.ArgumentParser(description="Wait for a Birdbox Sensors status message.")
    parser.add_argument("--port", default="COM15")
    parser.add_argument("--timeout", type=float, default=35)
    args = parser.parse_args()

    with serial.Serial(args.port, 115200, timeout=0.25) as connection:
        deadline = time.monotonic() + args.timeout
        received = bytearray()
        while time.monotonic() < deadline:
            received.extend(connection.read(4096))
            for line in received.splitlines():
                if b"Birdbox Sensors online" in line:
                    print(line.decode("utf-8", "replace"))
                    return

    raise RuntimeError("No online status message was received.")


if __name__ == "__main__":
    main()
