# IoT Patient Vital Signs Monitoring System

## Overview

A continuous patient monitoring system that reads heart rate and body temperature,
triggers local alerts, and transmits data wirelessly to an MQTT broker for remote monitoring.

---

## Hardware

| Component              | Role                          | Connection         |
|------------------------|-------------------------------|--------------------|
| Arduino Uno/Nano       | Main microcontroller          | —                  |
| LM35 Temperature Sensor| Body temperature (°C)         | Analog Pin A0      |
| Pulse Sensor           | Heart rate (BPM)              | Analog Pin A1      |
| Active Buzzer          | Audible alert                 | Digital Pin 8      |
| LED + 220Ω resistor    | Visual alert                  | Digital Pin 13     |
| ESP8266 WiFi Module    | Wireless MQTT transmission    | Pins 2 (RX), 3 (TX)|

See `hardware/circuit_diagram.txt` for full wiring details.

---

## Thresholds

| Vital Sign   | Warning        | Critical              |
|--------------|----------------|-----------------------|
| Temperature  | ≥ 37.5 °C      | ≥ 38.5 °C             |
| Heart Rate   | ≥ 100 bpm      | ≥ 110 bpm or ≤ 50 bpm |

---

## Software

### Arduino Firmware (`software/main/main.ino`)
- Reads LM35 temperature using the internal 1.1V reference for accuracy
- Reads pulse sensor and estimates BPM via peak/trough amplitude detection
- Triggers buzzer and LED alerts with a 5-second cooldown to prevent constant firing
- Uses `millis()` for non-blocking timing
- Sends JSON payloads over SoftwareSerial to the ESP8266 for MQTT publishing

### Python Simulation (`software/simulation/test_code.py`)
- Simulates random vital sign data for testing without hardware
- Can connect to a live Arduino via serial port (`--port` flag)
- Logs all readings to a timestamped CSV file in `results/`
- Supports multiple patient IDs via `--patient` flag

#### Usage
```bash
# Simulated mode
python test_code.py

# Live serial mode
python test_code.py --port COM3 --patient P002

# Custom log file
python test_code.py --log results/ward_a.csv
```

---

## MQTT Data Format

Topic: `patient/vitals`

```json
{"temp": 37.2, "hr": 82, "status": "NORMAL"}
```

---

## Future Improvements

- Add SpO2 monitoring (MAX30102 sensor)
- Web dashboard for real-time visualization
- Multi-patient support with unique MQTT topics per patient
- Battery level monitoring and low-power sleep modes
- Alert escalation (SMS/email via MQTT subscriber service)
