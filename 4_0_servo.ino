#include <ESP32_Servo.h>
Servo myservo;

void setup() {
  myservo.attach(19);
  Serial.begin(115200);
}

void loop() {
  // поворот на угол
  myservo.write(170);
  Serial.println(myservo.read());
  delay(1000);
  myservo.write(10);
  Serial.println(myservo.read());
  delay(1000);

}
