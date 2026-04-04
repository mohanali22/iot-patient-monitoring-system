int tempSensor = A0;
int buzzer = 8;
int led = 13;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  int value = analogRead(tempSensor);
  float temperature = (value * 5.0 / 1023.0) * 100;

  Serial.print("Temperature: ");
  Serial.println(temperature);

  if (temperature > 37.5) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    Serial.println("ALERT: High Temperature!");
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
  }

  delay(1000);
}