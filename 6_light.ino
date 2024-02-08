const int pinPhoto = 35; // пин фоторезистора (А7)
int raw = 0; // переменная для хранения данных

void setup() {
  Serial.begin(115200);
  pinMode(pinPhoto, INPUT); // пин на вход
}

void loop() {
  raw = analogRead(pinPhoto); // считываем значение
  Serial.println(raw);
  delay(200);
}
