#include <Arduino.h>
#include "FastLED.h"
#include <GyverHub.h>
GyverHub hub("MyDevices", "Feeder", "");  
// логин-пароль от роутера
#define AP_SSID "XXXXXXX"
#define AP_PASS "XXXXXXXXXX"

#include "time.h"
const char* ntpServer = "pool.ntp.org";  // отсюда считается текущие дата/время
const long gmtOffset_sec = 10800;        // +3 часа для времени по МСК
const int daylightOffset_sec = 0;
String time_buffer;
String water_last = "0:00";
String food_last = "0:00";

const int pinPhoto = 35; // пин фоторезистора
#define pump 23  // пин насоса

#include <ESP32_Servo.h>
Servo myservo;
#define OPENED 180 // насыпать корм
#define CLOSED 110 // закрыть кормушку

#include <FastLED.h>     
#define NUM_LEDS 30      // количество светодиодов в матрице
CRGB leds[NUM_LEDS];     
#define LED_PIN 4        // пин к которому подключена матрица
#define COLOR_ORDER GRB  
#define CHIPSET WS2812   

int water_uptime, food_uptime;
unsigned long water_timer, food_timer;
float lux;
uint16_t size = 14;
static int inp2 = 1000;
static int inp3 = 2;
static int inp4 = 1440;
static int inp5 = 2;
static int inp6 = 1440;

void build(gh::Builder& b) {
  if (b.beginRow()) {
    b.Label_("Light").label("Освещенность").color(gh::Colors::Yellow);
    b.Input(&inp2).label("Граница света").size(1).color(gh::Colors::Yellow);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_(F("lab2")).label(" ").value("Вода:").fontSize(size).size(1).color(gh::Colors::Blue);
    b.Input(&inp3).label("Длительность в секундах").size(1).color(gh::Colors::Blue);
    b.Input(&inp4).label("Периодичность в минутах").size(1).color(gh::Colors::Blue);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_(F("lab1")).label(" ").value("Корм:").fontSize(size).size(1).color(gh::Colors::Green);
    b.Input(&inp5).label("Порция (1-10)").size(1);
    b.Input(&inp6).label("Периодичность в минутах").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("last_water").label("Последнее поение:").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("last_food").label("Последнее кормление:").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("next_water").label("Следующее поение через:").fontSize(size).size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("next_food").label("Следующее кормление через:").fontSize(size).size(1);
    b.endRow();
  }
}

void setup() {
  Serial.begin(115200);
  myservo.attach(5); // пин сервопривода
  myservo.write(CLOSED);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(pinPhoto, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  hub.config(F("MyDevices"), F("Feeder"), F(""));
  hub.onBuild(build);
  hub.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // получение времени
}

void loop() {
  hub.tick();
  getTime();
  static gh::Timer tmr(1000); 
  if (millis() - water_timer > inp4 * 60000) {
    digitalWrite(pump, HIGH);
    Serial.println("Включено поение");
    delay(inp3 * 1000); // длительность переливания воды
    digitalWrite(pump, LOW);
    Serial.println("Выключено поение");
    getTime();
    water_last = time_buffer;
    water_timer = millis();
    hub.update("last_water").value(water_last);
  }

  if (millis() - food_timer > inp6 * 60000) {
    myservo.write(OPENED);
    delay(inp5 * 50); // длительность открытой створки кормушки
    Serial.println("Кормление");
    myservo.write(CLOSED);
    getTime();
    food_last = time_buffer;
    food_timer = millis();
    hub.update("last_food").value(food_last);
  }

  if (tmr) {
    lux = analogRead(pinPhoto);  // считывание освещенности
    water_uptime = (inp4 * 60000 - (millis() - water_timer)) / 60000;
    food_uptime = (inp6 * 60000 - (millis() - food_timer)) / 60000;
    String water_wait = String(water_uptime) + " минут / ~" + String(water_uptime / 60) + " часов";
    String food_wait = String(food_uptime) + " минут / ~" + String(food_uptime / 60) + " часов";
    hub.update(F("Light")).value(lux);
    hub.update("next_water").value(water_wait);
    hub.update("next_food").value(food_wait);
    if (lux < inp2) {
      for (int i = 0; i < 15; i++) {
        leds[i].setRGB(255, 255, 255);
      }
    } else {
      for (int i = 0; i < 15; i++) {
        leds[i].setRGB(0, 0, 0);
      }
    }
    for (int i = 15; i < (15 + (7 * (inp6 - food_uptime) / inp6)); i++) {
      leds[i].setRGB(0, 255, 0);
    }
    for (int i = (15 + (7 * (inp6 - food_uptime) / inp6)); i < 22; i++) {
      leds[i].setRGB(0, 0, 0);
    }
    for (int i = 22; i < (22 + (8 * (inp4 - water_uptime) / inp4)); i++) {
      leds[i].setRGB(0, 0, 255);
    }
    for (int i = (22 + (8 * (inp4 - water_uptime) / inp4)); i < 30; i++) {
      leds[i].setRGB(0, 0, 0);
    }
    FastLED.show();
  }
}

void getTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d.%b в %H:%M", &timeinfo);  // получение даты и времени
  String asString(timeStringBuff);
  time_buffer = asString;
}
