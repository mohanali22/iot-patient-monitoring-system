"""
Patient Vital Signs Monitoring System - Simulation
Supports both simulated random data and live Arduino serial input.
Logs all readings to a CSV file.
"""

import random
import time
import csv
import os
import sys
import argparse
from datetime import datetime

# --- Thresholds (must match main.ino) ---
TEMP_WARNING  = 37.5
TEMP_CRITICAL = 38.5
HR_WARNING    = 100
HR_CRITICAL   = 110
HR_LOW        = 50

LOG_FILE = "results/vitals_log.csv"

# -------------------------------------------------------
def get_status(heart_rate: int, temperature: float) -> str:
    if temperature >= TEMP_CRITICAL or heart_rate >= HR_CRITICAL or heart_rate <= HR_LOW:
        return "CRITICAL"
    if temperature >= TEMP_WARNING or heart_rate >= HR_WARNING:
        return "WARNING"
    return "NORMAL"

# -------------------------------------------------------
def print_reading(timestamp: str, heart_rate: int, temperature: float,
                  status: str, patient_id: str = "P001") -> None:
    print(f"[{timestamp}] Patient: {patient_id}")
    print(f"  Heart Rate  : {heart_rate} bpm")
    print(f"  Temperature : {temperature:.1f} °C")
    print(f"  Status      : {status}")
    if status == "CRITICAL":
        print("  🚨 ALERT: Immediate medical attention required!")
    elif status == "WARNING":
        print("  ⚠️  Warning: Patient condition needs attention.")
    print("-" * 45)

# -------------------------------------------------------
def log_reading(writer, timestamp: str, patient_id: str,
                heart_rate: int, temperature: float, status: str) -> None:
    writer.writerow([timestamp, patient_id, heart_rate, f"{temperature:.1f}", status])

# -------------------------------------------------------
def simulated_source():
    """Yields (heart_rate, temperature) from random simulation."""
    while True:
        yield random.randint(45, 125), round(random.uniform(36.0, 39.5), 1)
        time.sleep(2)

# -------------------------------------------------------
def serial_source(port: str, baud: int = 9600):
    """Yields (heart_rate, temperature) parsed from Arduino serial output."""
    try:
        import serial
    except ImportError:
        print("pyserial not installed. Run: pip install pyserial")
        sys.exit(1)

    ser = serial.Serial(port, baud, timeout=2)
    print(f"Connected to {port} at {baud} baud.")

    while True:
        line = ser.readline().decode("utf-8", errors="ignore").strip()
        # Expected format: "[123s] Temp: 37.2 C  HR: 78 bpm  Status: NORMAL"
        if "Temp:" in line and "HR:" in line:
            try:
                temp_part = line.split("Temp:")[1].split("C")[0].strip()
                hr_part   = line.split("HR:")[1].split("bpm")[0].strip()
                yield int(float(hr_part)), float(temp_part)
            except (IndexError, ValueError):
                continue  # skip malformed lines

# -------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(description="Vital Signs Monitor Simulation")
    parser.add_argument("--port",      type=str, default=None,
                        help="Serial port for live Arduino data (e.g. COM3 or /dev/ttyUSB0)")
    parser.add_argument("--baud",      type=int, default=9600)
    parser.add_argument("--patient",   type=str, default="P001",
                        help="Patient ID label")
    parser.add_argument("--log",       type=str, default=LOG_FILE,
                        help="CSV log file path")
    args = parser.parse_args()

    os.makedirs(os.path.dirname(args.log), exist_ok=True)

    source = serial_source(args.port, args.baud) if args.port else simulated_source()

    print("=== Patient Vital Signs Monitoring System ===")
    print(f"Mode    : {'Live Serial' if args.port else 'Simulation'}")
    print(f"Patient : {args.patient}")
    print(f"Log     : {args.log}")
    print("Press Ctrl+C to stop.\n")

    write_header = not os.path.exists(args.log)

    try:
        with open(args.log, "a", newline="") as csvfile:
            writer = csv.writer(csvfile)
            if write_header:
                writer.writerow(["timestamp", "patient_id", "heart_rate_bpm",
                                 "temperature_c", "status"])

            for heart_rate, temperature in source:
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                status    = get_status(heart_rate, temperature)

                print_reading(timestamp, heart_rate, temperature, status, args.patient)
                log_reading(writer, timestamp, args.patient, heart_rate, temperature, status)
                csvfile.flush()

    except KeyboardInterrupt:
        print("\nMonitoring stopped.")

# -------------------------------------------------------
if __name__ == "__main__":
    main()
