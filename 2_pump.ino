#define PUMP 16 // пин, к которому подключено реле

void setup() {
  pinMode(PUMP, OUTPUT); // пин на выход
}

void loop() {
  digitalWrite(PUMP, HIGH); // вкл насос
  delay(1000);
  digitalWrite(PUMP, LOW); // выкл насос
  delay(1000);
}
