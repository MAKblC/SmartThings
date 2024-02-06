#define WIND 17

void setup() {
  pinMode(WIND, OUTPUT);
}

void loop() {
  digitalWrite(WIND, HIGH);
  delay(1000);
  digitalWrite(WIND, LOW);
  delay(1000);
}
