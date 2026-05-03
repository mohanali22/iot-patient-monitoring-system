import random
import time
from datetime import datetime
import matplotlib.pyplot as plt
import os

if not os.path.exists("graphs"):
    os.makedirs("graphs")

# Lists to store data
times = []
heart_rates = []
temperatures = []

def simulate_arduino():
    temperature = round(random.uniform(36.0, 39.5), 1)
    return f"TEMP:{temperature}"

def get_status(heart_rate, temperature):
    if heart_rate > 110 or temperature > 38.5:
        return "CRITICAL"
    elif heart_rate > 100 or temperature > 37.5:
        return "WARNING"
    else:
        return "NORMAL"

print("=== Patient Monitoring System with Graph Saving ===\n")

plt.ion()

while True:
    # Simulated data
    line = simulate_arduino()
    temperature = float(line.split(":")[1])

    heart_rate = random.randint(60, 120)
    status = get_status(heart_rate, temperature)
    current_time = datetime.now().strftime("%H:%M:%S")

    # Store data
    times.append(current_time)
    heart_rates.append(heart_rate)
    temperatures.append(temperature)

    # Keep last 10 points
    times = times[-10:]
    heart_rates = heart_rates[-10:]
    temperatures = temperatures[-10:]

    print(f"[{current_time}] HR: {heart_rate} bpm | Temp: {temperature}°C | {status}")

    # Plot
    plt.clf()
    plt.plot(times, heart_rates, marker='o', label="Heart Rate (bpm)")
    plt.plot(times, temperatures, marker='x', label="Temperature (°C)")

    plt.title("Patient Monitoring")
    plt.xlabel("Time")
    plt.ylabel("Values")
    plt.legend()
    plt.xticks(rotation=45)
    plt.tight_layout()

    # ✅ Save graph as image
    filename = f"graph_{current_time.replace(':','-')}.png"
    filepath = os.path.join("graphs", filename)
    plt.savefig(filepath)

    plt.pause(0.5)
    time.sleep(2)