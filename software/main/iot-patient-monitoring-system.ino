const int tempSensor = A0;
const int buzzer = 8;
const int led = 13;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

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
  }

  delay(1000);
}