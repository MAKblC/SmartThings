#include <Wire.h>
#include <Arduino.h>
#include "FastLED.h"
#include <GyverHub.h>
GyverHub hub("MyDevices", "Pot", "");  // можно настроить тут, но без F-строк!
// логин-пароль от роутера
#define AP_SSID "XXXXXX"
#define AP_PASS "XXXXXXXXX"

#include "time.h"
const char* ntpServer = "pool.ntp.org";  // отсюда считается текущие дата/время
const long gmtOffset_sec = 10800;        // +3 часа для времени по МСК
const int daylightOffset_sec = 0;
int hour;
String time_buffer;
String water_last = "0:00";

#include <Adafruit_BME280.h>  // BME280
Adafruit_BME280 bme280;       //

const int pinPhoto = 35;
#define pump 23  // пин насоса

#include <FastLED.h>     // конфигурация матрицы
#define NUM_LEDS 30      // количество светодиодов в матрице
CRGB leds[NUM_LEDS];     // определяем матрицу (FastLED библиотека)
#define LED_PIN 4        // пин к которому подключена матрица
#define COLOR_ORDER GRB  // порядок цветов матрицы
#define CHIPSET WS2812   // тип светодиодов

int water_uptime;
unsigned long water_timer;
unsigned long t_del = 0;
float lux, t, h, p;
uint16_t size = 12;
static int inp1 = 8;
static int inp2 = 20;
static int inp3 = 2;
static int inp4 = 1440;
static int inp5 = 600;
static int inp6 = 29;
static int inp7 = 20;

void build(gh::Builder& b) {
  if (b.beginRow()) {
    b.Label_("Temp").label("Температура").color(gh::Colors::Red);
    b.Label_("Hum").label("Влажность").color(gh::Colors::Blue);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("Press").label("Давление").color(gh::Colors::Green);
    b.Label_("Light").label("Освещенность").color(gh::Colors::Yellow);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_(F("lab1")).label(" ").value("Световой день:").fontSize(size).size(1).color(gh::Colors::Yellow);
    b.Input(&inp1).label("Начало").size(1).color(gh::Colors::Yellow);
    b.Input(&inp2).label("Конец").size(1).color(gh::Colors::Yellow);
    b.Input(&inp5).label("Порог включения").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_(F("lab2")).label(" ").value("Полив:").fontSize(size).size(1).color(gh::Colors::Blue);
    b.Input(&inp3).label("Длительность в секундах").size(1);
    b.Input(&inp4).label("Периодичность в минутах").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("last").label("Последний полив:").size(1);
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_("next").label("Следующий полив через:").fontSize(size).size(1);
    b.endRow();
  }

  if (b.beginRow()) {
    b.Label_(F("lab4")).label(" ").value("Границы датчиков:").fontSize(size).size(1);
    b.Input(&inp6).label("температуры").size(1).color(gh::Colors::Red);
    b.Input(&inp7).label("влажности").size(1).color(gh::Colors::Blue);
    b.endRow();
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  bool bme_status = bme280.begin();
  if (!bme_status) {
    Serial.println("Не найден по адресу 0х77, пробую другой...");
    bme_status = bme280.begin(0x76);
    if (!bme_status)
      Serial.println("Датчик не найден, проверьте соединение");
  }
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
  hub.config(F("MyDevices"), F("Pot"), F(""));
  hub.onBuild(build);
  hub.begin();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // получение времени
}

void loop() {
  hub.tick();
  getTime();
  static gh::Timer tmr(1000);  // период 1 секунда
  if (millis() - water_timer > inp4 * 60000) {
    digitalWrite(pump, HIGH);
    Serial.println("Включен полив");
    delay(inp3 * 1000);
    digitalWrite(pump, LOW);
    Serial.println("Выключен полив");
    getTime();
    water_last = time_buffer;
    water_timer = millis();
    hub.update("last").value(water_last);
  }

  if (tmr) {
    lux = analogRead(pinPhoto);  // считывание освещенности
    t = bme280.readTemperature();
    h = bme280.readHumidity();
    p = bme280.readPressure() / 133.3F;
    water_uptime = (inp4*60000 - (millis() - water_timer)) / 60000;
    String water_wait = String(water_uptime) + " минут / ~" + String(water_uptime / 60) + " часов";
    hub.update(F("Light")).value(lux);
    hub.update("Temp").value(t);
    hub.update("Hum").value(h);
    hub.update("Press").value(p);
    hub.update("next").value(water_wait);
    if (lux < inp5 and hour >= inp1 and hour < inp2) {
      for (int i = 12; i < NUM_LEDS; i++) {
        leds[i].setRGB(255, 0, 127);
      }
      FastLED.show();
    } else {
      for (int i = 12; i < NUM_LEDS; i++) {
        leds[i].setRGB(0, 0, 0);
      }
      FastLED.show();
    }

    if (t > inp6) {
      for (int i = 0; i < 6; i++) {
        leds[i].setRGB(255, 0, 0);
      }
      FastLED.show();
    } else {
      for (int i = 0; i < 6; i++) {
        leds[i].setRGB(0, 0, 0);
      }
      FastLED.show();
    }

    if (h < inp7) {
      for (int i = 6; i < 12; i++) {
        leds[i].setRGB(0, 0, 255);
      }
      FastLED.show();
    } else {
      for (int i = 6; i < 12; i++) {
        leds[i].setRGB(0, 0, 0);
      }
      FastLED.show();
    }
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
  // day = timeinfo.tm_mday;
  // month = timeinfo.tm_mon + 1;
  hour = timeinfo.tm_hour;
  // minute = timeinfo.tm_min;
}
