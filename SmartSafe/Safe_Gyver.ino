#include <Arduino.h>
#include <GyverHub.h>
GyverHub hub("MyDevices", "Safe", "");  // можно настроить тут, но без F-строк!
// логин-пароль от роутера
#define AP_SSID "XXXXX"
#define AP_PASS "XXXXXXXXXX"

#include "FastLED.h"
#define LED_PIN1 4   // пин, к которому подключена лента
#define LED_NUM1 30  // количество светодиодов на ленте
CRGB led1[LED_NUM1];

#include <ESP32_Servo.h>
Servo myservo;

const int pinPhoto = 35; // пин фоторезистора
int raw = 0;

#include <Adafruit_Sensor.h>  // библиотека датчика температуры, влажности и давления
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;

int pin = 34;  // пин потенциометра
bool dsbl, nolbl, notab;
String inp1;

void build(gh::Builder& b) {
  if (b.beginRow()) {
    b.Label_("Temp").label("Температура").color(gh::Colors::Red);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("Light").label("Освещенность").color(gh::Colors::Yellow);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("Poten").label("Регулятор").color(gh::Colors::Green);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Pass_("pass", &inp1).size(3).color(gh::Colors::Blue);
    b.endRow();
  }
  if (b.beginRow()) {
    b.LED_("led1").color(gh::Colors::Red);
    b.LED_("led2").color(gh::Colors::Yellow);
    b.LED_("led3").color(gh::Colors::Green);
    b.LED_("led4").color(gh::Colors::Blue);
    b.endRow();
  }
  if (b.beginRow()) {
    if (b.Button().label("закрыть сейф").icon("f52b").click()) {
      FastLED.clear();
      FastLED.show();
      myservo.write(50);
    }
    b.endRow();
  }
}

void setup() {
  pinMode(pinPhoto, INPUT);
  myservo.attach(13);   // пин сервомотора
  pinMode(pin, INPUT);  
  FastLED.addLeds< WS2812, LED_PIN1, GRB>(led1, LED_NUM1);
  Serial.begin(115200);
  Wire.begin();
  bool bme_status = bme280.begin();
  if (!bme_status) {
    Serial.println("Не найден по адресу 0х77, пробую другой...");
    bme_status = bme280.begin(0x76);
    if (!bme_status)
      Serial.println("Датчик не найден, проверьте соединение");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  hub.config(F("MyDevices"), F("Safe"), F(""));
  hub.onBuild(build);
  hub.begin();
}

void loop() {
  hub.tick();
  static gh::Timer tmr(1000);  // период 1 секунда
  if (tmr) {
    byte key = 0b0000;
    float t = bme280.readTemperature();
    float lux = analogRead(pinPhoto);
    int poten = analogRead(pin);
    hub.update("Temp").value(t);
    hub.update("Light").value(lux);
    hub.update("Poten").value(poten);
    if (t > 26) { // условие 1
      hub.update("led1").value(true);
      led1[28].setRGB(255, 0, 0);
      led1[29].setRGB(255, 0, 0);
      bitSet(key, 0);
    } else {
      hub.update("led1").value(false);
      led1[28].setRGB(0, 0, 0);
      led1[29].setRGB(0, 0, 0);
      bitClear(key, 0);
    }
    if (lux < 200) { // условие 2
      hub.update("led2").value(true);
      led1[26].setRGB(255, 255, 0);
      led1[27].setRGB(255, 255, 0);
      bitSet(key, 1);
    } else {
      hub.update("led2").value(false);
      led1[26].setRGB(0, 0, 0);
      led1[27].setRGB(0, 0, 0);
      bitClear(key, 1);
    }
    if (poten < 1800 and poten > 1700) { // условие 3
      hub.update("led3").value(true);
      led1[24].setRGB(0, 255, 0);
      led1[25].setRGB(0, 255, 0);
      bitSet(key, 2);
    } else {
      hub.update("led3").value(false);
      led1[24].setRGB(0, 0, 0);
      led1[25].setRGB(0, 0, 0);
      bitClear(key, 2);
    }
    if (inp1 == "25cents") { // условие 4
      hub.update("led4").value(true);
      led1[22].setRGB(0, 0, 255);
      led1[23].setRGB(0, 0, 255);
      bitSet(key, 3);
    } else {
      hub.update("led4").value(false);
      led1[22].setRGB(0, 0, 0);
      led1[23].setRGB(0, 0, 0);
      bitClear(key, 3);
    }
    if (key == 0b1111) {
      myservo.write(150);
      for (int i = 0; i < 22; i++) {
        led1[i].setRGB(255, 255, 255);
      }
    }
    FastLED.show();
  }
}
