<<<<<<< HEAD:software/main/main.ino
// ============================================================
// IoT Patient Vital Signs Monitoring System
// Sensors: LM35 (Temperature), Pulse Sensor (Heart Rate)
// Connectivity: ESP8266 WiFi -> MQTT Broker
// ============================================================
=======
const int tempSensor = A0;
const int buzzer = 8;
const int led = 13;
>>>>>>> 581f97b485947394261e0eeb2806fc54a04269aa:software/main/iot-patient-monitoring-system.ino

#include <SoftwareSerial.h>

// --- Pin Definitions ---
#define TEMP_SENSOR_PIN   A0
#define HEART_SENSOR_PIN  A1
#define BUZZER_PIN        8
#define LED_PIN           13
#define ESP_RX_PIN        2
#define ESP_TX_PIN        3

// --- Alert Thresholds ---
#define TEMP_WARNING      37.5
#define TEMP_CRITICAL     38.5
#define HR_WARNING        100
#define HR_CRITICAL       110
#define HR_LOW            50

// --- Timing ---
#define SAMPLE_INTERVAL   1000   // ms between readings
#define ALERT_COOLDOWN    5000   // ms before re-triggering alert

// --- Heart Rate Calculation ---
#define HR_SAMPLE_COUNT   10     // samples averaged per reading

SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN);

// WiFi / MQTT config — update these
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASS     = "YOUR_PASSWORD";
const char* MQTT_HOST     = "broker.hivemq.com";
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "patient/vitals";

unsigned long lastSampleTime = 0;
unsigned long lastAlertTime  = 0;
bool alertActive = false;

// -------------------------------------------------------
float readTemperature() {
  // LM35: 10mV per °C, using internal 1.1V reference for accuracy
  int raw = analogRead(TEMP_SENSOR_PIN);
  float voltage = raw * (1.1 / 1023.0);
  return voltage * 100.0;
}

<<<<<<< HEAD:software/main/main.ino
// -------------------------------------------------------
int readHeartRate() {
  // Simple peak-detection average over HR_SAMPLE_COUNT samples
  long sum = 0;
  int peak = 0, trough = 1023;

  for (int i = 0; i < HR_SAMPLE_COUNT; i++) {
    int val = analogRead(HEART_SENSOR_PIN);
    if (val > peak)   peak   = val;
    if (val < trough) trough = val;
    sum += val;
    delay(10);
=======
void loop() {
  int value = analogRead(tempSensor);

  // Convert to temperature (LM35 style)
  float temperature = (value * 5.0 / 1023.0) * 100;

  // Send clean data
  Serial.print("TEMP:");
  Serial.println(temperature);

  // Alert system
  if (temperature > 37.5) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
>>>>>>> 581f97b485947394261e0eeb2806fc54a04269aa:software/main/iot-patient-monitoring-system.ino
  }

  int amplitude = peak - trough;
  // Map amplitude to a BPM estimate (calibrate per sensor)
  int bpm = map(amplitude, 0, 1023, 40, 180);
  return constrain(bpm, 40, 200);
}

// -------------------------------------------------------
String getStatus(float temp, int hr) {
  if (temp >= TEMP_CRITICAL || hr >= HR_CRITICAL || hr <= HR_LOW)
    return "CRITICAL";
  if (temp >= TEMP_WARNING || hr >= HR_WARNING)
    return "WARNING";
  return "NORMAL";
}

// -------------------------------------------------------
void triggerAlert(bool active) {
  digitalWrite(BUZZER_PIN, active ? HIGH : LOW);
  digitalWrite(LED_PIN,    active ? HIGH : LOW);
}

// -------------------------------------------------------
void sendToESP(float temp, int hr, const String& status) {
  // Build a simple JSON payload and send over SoftwareSerial to ESP8266
  String payload = "{";
  payload += "\"temp\":"   + String(temp, 1) + ",";
  payload += "\"hr\":"     + String(hr)      + ",";
  payload += "\"status\":\"" + status        + "\"";
  payload += "}";

  // Command format the ESP firmware listens for
  espSerial.println("SEND:" + String(MQTT_TOPIC) + ":" + payload);
}

// -------------------------------------------------------
void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  analogReference(INTERNAL);  // Use 1.1V internal ref for LM35 accuracy

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN,    OUTPUT);

  triggerAlert(false);

  Serial.println(F("=== Patient Vital Signs Monitor ==="));
  Serial.println(F("Initializing..."));

  // Tell ESP to connect to WiFi
  espSerial.println("WIFI:" + String(WIFI_SSID) + ":" + String(WIFI_PASS));
  delay(3000);
  Serial.println(F("Ready."));
}

// -------------------------------------------------------
void loop() {
  unsigned long now = millis();

  if (now - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = now;

    float temp = readTemperature();
    int   hr   = readHeartRate();
    String status = getStatus(temp, hr);

    // Timestamp (seconds since boot)
    unsigned long ts = now / 1000;

    Serial.print(F("["));
    Serial.print(ts);
    Serial.print(F("s] Temp: "));
    Serial.print(temp, 1);
    Serial.print(F(" C  HR: "));
    Serial.print(hr);
    Serial.print(F(" bpm  Status: "));
    Serial.println(status);

    // Alert with cooldown to avoid constant buzzing
    bool shouldAlert = (status == "CRITICAL" || status == "WARNING");
    if (shouldAlert && (now - lastAlertTime >= ALERT_COOLDOWN)) {
      triggerAlert(true);
      lastAlertTime = now;
      alertActive = true;
      Serial.println(status == "CRITICAL"
        ? F("🚨 CRITICAL: Immediate attention required!")
        : F("⚠️  WARNING: Patient needs attention."));
    } else if (!shouldAlert && alertActive) {
      triggerAlert(false);
      alertActive = false;
    }

    // Send data wirelessly
    sendToESP(temp, hr, status);
  }
}
