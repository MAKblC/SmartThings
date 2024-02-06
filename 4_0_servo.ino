#include <ESP32_Servo.h> // библиотека для сервопривода
Servo myservo; // создаем объект

void setup() {
  myservo.attach(19); // пин сервопривода
  Serial.begin(115200);
}

void loop() {
  // поворот на угол
  myservo.write(170);
  Serial.println(myservo.read()); // считывание текущей позиции
  delay(1000);
  myservo.write(10);
  Serial.println(myservo.read());
  delay(1000);

}
