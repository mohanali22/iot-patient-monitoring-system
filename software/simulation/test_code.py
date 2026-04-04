import random
import time
from datetime import datetime

def get_status(heart_rate, temperature):
    if heart_rate > 110 or temperature > 38.5:
        return "CRITICAL"
    elif heart_rate > 100 or temperature > 37.5:
        return "WARNING"
    else:
        return "NORMAL"

print("=== Patient Vital Signs Monitoring System ===\n")

while True:
    heart_rate = random.randint(60, 120)
    temperature = round(random.uniform(36.0, 39.5), 1)

    status = get_status(heart_rate, temperature)
    current_time = datetime.now().strftime("%H:%M:%S")

    print(f"[{current_time}]")
    print(f"Heart Rate   : {heart_rate} bpm")
    print(f"Temperature  : {temperature} °C")
    print(f"Status       : {status}")

    if status == "CRITICAL":
        print("🚨 ALERT: Immediate medical attention required!")
    elif status == "WARNING":
        print("⚠️ Warning: Patient condition needs attention.")

    print("-" * 40)

    time.sleep(2)