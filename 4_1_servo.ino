#include <ESP32_Servo.h>
Servo myservo;

void setup() {
  myservo.attach(19);
}

void loop() {
  for (int i = 0; i < 181; i++) { // цикл плавного поворота
    myservo.write(i);
    delay(30);
  }
  for (int i = 180; i > 0; i--) {
    myservo.write(i);
    delay(30);
  }
}
