/* алгоритм подбора
1. откройте монитор порта
2. справа выберите режим "No Line Ending / Нет конца строки"
3. Напишите в строку сообщения "777", в ответ придет включение режима Насос
4. Теперь, введя число от 1 до 9, насос будет включаться на соответствующее количество секунд. Таким образом вы можете понять, какая длительность полива подходит для наполнения миски
5. Введите "666". Включится режим сервопривода. Теперь, введя число от 1 до 180 вы можете выставлять определенный угол на сервоприводе. Так вы можете понять, какой угол соответствует открытой кормушке, а какой - закрытой.
6. Введите "222". Включится режим порции. Теперь введя число от 1 до 9 вы сможете регулировать объем порции (размер задержки между открытым и закрытым клапаном для корма)
7. Введите "333". Включится режим open. Введите число, соответствующее открытой кормушке
8. Введите "444". Включится режим closed. Введите число, соответствующее закрытой кормушке
9. Введите "555". Активируется сценарий подачи корма. Можно теперь использовать пункты 7-9, чтобы отрегулировать требуемый объем порции.
*/

#define pump 23  // пин насоса
#include <ESP32_Servo.h>
Servo myservo;
int mode = 0;
int opened = 10;
int closed = 170;
int delayS = 6;
int zone = 0;

void setup() {
  Serial.begin(115200);
  myservo.attach(5);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
}

void loop() {
  while (Serial.available() > 0) {
    zone = Serial.parseInt();
    Serial.println(zone);
    if (zone == 222) {
      mode = 2;
      Serial.println("введите порцию (1-9");
    }
    if (zone == 777) {
      mode = 0;
      Serial.println("режим насоса");
    }
    if (zone == 666) {
      mode = 1;
      Serial.println("режим сервопривода");
    }
    if (zone == 444) {
      mode = 4;
      Serial.println("настройка close");
    }
    if (zone == 333) {
      mode = 3;
      Serial.println("настройка open");
    }
    if (mode == 1 and zone > 0 and zone < 181) {
      myservo.write(zone);
    }
    if (mode == 0 and zone > 0 and zone < 10) {
      digitalWrite(pump, HIGH);
      Serial.println("Включено поение");
      delay(zone * 1000);
      digitalWrite(pump, LOW);
    }
    if (mode == 4 and zone > 0 and zone < 181) {
      closed = zone;
      Serial.print(closed);
       Serial.print("close теперь " + String(closed));
      //Serial.println(close);
    }
    if (mode == 3 and zone > 0 and zone < 181) {
      opened = zone;
      Serial.println(opened);
    }
    if (mode == 2 and zone > 0 and zone < 10) {
      delayS = zone;
      Serial.println(delayS);
    }
    if (zone == 555) {
      myservo.write(opened);
      Serial.println(opened);
      delay(delayS * 50);
      Serial.println("Кормление");
      myservo.write(closed);
      Serial.println(closed);
    }
  }
}
