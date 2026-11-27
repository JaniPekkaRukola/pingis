import argparse
import os
import sys
import time
import serial
import serial.tools.list_ports

DEFAULT_BAUD = 115200


def enable_ansi_windows() -> None:
    if os.name == "nt":
        try:
            import ctypes
            kernel32 = ctypes.windll.kernel32
            handle = kernel32.GetStdHandle(-11)
            mode = ctypes.c_ulong()
            if kernel32.GetConsoleMode(handle, ctypes.byref(mode)):
                kernel32.SetConsoleMode(handle, mode.value | 0x0004)
        except Exception:
            pass


def find_arduino_port() -> str | None:
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        desc = f"{p.description} {p.manufacturer or ''}".lower()
        if any(k in desc for k in ("arduino", "ch340", "usb serial", "acm", "mega")):
            return p.device
    return ports[0].device if ports else None


def clear_screen() -> None:
    sys.stdout.write("\033[2J\033[H")
    sys.stdout.flush()


def render_scoreboard(state: str, p1: int, p2: int, note: str = "") -> None:
    clear_screen()

    match state:
        case "START":
            header = "GAME IN PROGRESS"
        case "UPDATE":
            header = "GAME IN PROGRESS — GOAL!"
        case "END":
            header = "GAME OVER — FINAL SCORE"
        case _:
            header = "WAITING FOR GAME START..."

    width = 46
    lines = [
        "=" * width,
        f"{header:^{width}}",
        "=" * width,
        f"{'Player 1':^22}|{'Player 2':^23}",
        "-" * width,
        f"{p1:^22}|{p2:^23}",
        "=" * width,
    ]

    if state == "END":
        if p1 > p2:
            winner = "Winner: Player 1!"
        elif p2 > p1:
            winner = "Winner: Player 2!"
        else:
            winner = "Draw!"
        lines.extend([f"{winner:^{width}}", "=" * width])
    elif note:
        lines.append(f"{note:^{width}}")

    lines.append("\n(Press Ctrl+C to exit)")
    sys.stdout.write("\n".join(lines) + "\n")
    sys.stdout.flush()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Pingis UART Scoreboard")
    parser.add_argument("port", nargs="?", help="Serial port (e.g. COM3 or /dev/ttyACM0)")
    parser.add_argument("-b", "--baud", type=int, default=DEFAULT_BAUD, help="Baud rate (default: 115200)")
    return parser.parse_args()


def main() -> None:
    enable_ansi_windows()
    args = parse_args()

    port = args.port or find_arduino_port()
    if not port:
        available = ", ".join(p.device for p in serial.tools.list_ports.comports()) or "none"
        sys.exit(f"No Arduino serial port found (available: {available}). Pass it explicitly: python scoreboard.py COM3")

    print(f"Connecting to {port} at {args.baud} baud...")
    p1_score = 0
    p2_score = 0

    try:
        with serial.Serial(port, args.baud, timeout=0.5) as ser:
            time.sleep(2.0)
            ser.reset_input_buffer()
            render_scoreboard("WAIT", p1_score, p2_score)

            while True:
                raw = ser.readline()
                if not raw:
                    continue

                line = raw.decode("utf-8", errors="ignore").strip()
                match line.split(","):
                    case [("START" | "UPDATE" | "END") as event, p1_str, p2_str]:
                        try:
                            new_p1, new_p2 = int(p1_str), int(p2_str)
                        except ValueError:
                            continue
                    case _:
                        continue

                note = ""
                if event == "UPDATE":
                    if new_p1 > p1_score:
                        note = "Player 1 scored!"
                    elif new_p2 > p2_score:
                        note = "Player 2 scored!"

                p1_score, p2_score = new_p1, new_p2
                render_scoreboard(event, p1_score, p2_score, note)

    except serial.SerialException as err:
        sys.exit(f"Serial connection error on {port}: {err}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nScoreboard closed.")