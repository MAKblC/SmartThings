#define WIND 17 // пин модуля реле

void setup() {
  pinMode(WIND, OUTPUT); // пин на выход
}

void loop() {
  digitalWrite(WIND, HIGH); // вкл вентилятор
  delay(1000);
  digitalWrite(WIND, LOW); // выкл вентилятор
  delay(1000);
}
