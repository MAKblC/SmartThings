#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
const char* ntpServer = "pool.ntp.org";  // отсюда считается текущие дата/время
const long gmtOffset_sec = 10800;
const int daylightOffset_sec = 0;

int t_max = 27; // максимальная температура
int h_min = 40; // минимальная влажность
int raw_min = 1000; // порог включения подсветки

#define DELAY 1  // периодичность считывания в минутах
const char* ssid = "XXXXXXX";              // логин Wi-Fi
const char* password = "XXXXXXXXX";  // пароль Wi-Fi

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbx7XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXaMxVuy1SH4";  // Script ID

#include <ArduinoJson.h>
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?lat=59.9386&lon=30.3141&APPID=";
const String key = "90cXXXXXXXXXXXXXXXXXXXca41248";
const String units = "&units=metric";
int humidity;
int temperature;
int pressure;

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;

const int pinPhoto = 35;
int raw = 0;

#include <FastLED.h>     // конфигурация матрицы
#define NUM_LEDS 30      // количество светодиодов в матрице
CRGB leds[NUM_LEDS];     // определяем матрицу (FastLED библиотека)
#define LED_PIN 4        // пин к которому подключена матрица
#define COLOR_ORDER GRB  // порядок цветов матрицы
#define CHIPSET WS2812   // тип светодиодов

void setup() {
  delay(1000);
  Serial.begin(115200);
  Wire.begin();
  bool bme_status = bme280.begin();
  if (!bme_status) {
    Serial.println("Не найден по адресу 0х77, пробую другой...");
    bme_status = bme280.begin(0x76);
    if (!bme_status)
      Serial.println("Датчик не найден, проверьте соединение");
  }
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(pinPhoto, INPUT);
  delay(1000);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));  // заполнить всю матрицу выбранным цветом
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  HTTPClient http;
  http.begin(endpoint + key + units);
  int httpCode = http.GET();  // Делаем запрос
  // проверяем успешность запроса
  if (httpCode > 0) {
    // выводим ответ сервера
    String payload = http.getString();
    Serial.println(httpCode);
    handleReceivedMessage(payload);
  } else {
    Serial.println("Ошибка HTTP-запроса");
  }
  //  ----------------------------------------
  float t = bme280.readTemperature();
  float h = bme280.readHumidity();
  float p = bme280.readPressure() / 133.3F;
  if (t > t_max) {
    for (int i = 0; i < 5; i++) {
      leds[i].setRGB(255, 0, 0);
    }
  } else {
    for (int i = 0; i < 5; i++) {
      leds[i].setRGB(0, 0, 0);
    }
  }
  if (h < h_min) {
    for (int i = 5; i < 10; i++) {
      leds[i].setRGB(0, 0, 255);
    }
  } else {
    for (int i = 5; i < 10; i++) {
      leds[i].setRGB(0, 0, 0);
    }
  }
  raw = analogRead(pinPhoto);
  if (raw < raw_min) {
    for (int i = 10; i < NUM_LEDS; i++) {
      leds[i].setRGB(255, 255, 255);
    }
  } else {
    for (int i = 10; i < NUM_LEDS; i++) {
      leds[i].setRGB(0, 0, 0);
    }
  }
  FastLED.show();

  if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50];                                                               //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);  // получение даты и времени
    String asString(timeStringBuff);
    asString.replace(" ", "-");
    Serial.print("Time:");
    Serial.println(asString);
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "date=" + asString + "&temp=" + String(t, 1) + "&hum=" + String(h, 0) + "&press=" + String(p, 0) + "&temp1=" + String(temperature) + "&hum1=" + String(humidity) + "&press1=" + String(pressure) + "&light=" + String(raw);  // запрос, составленный также, как и в ручном режиме (пункт 8 урока)
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload: " + payload);
    }
    //http.end();
    //---------------------------------------------------------------------
  }
  http.end();
  delay(DELAY * 60000);
}

void handleReceivedMessage(String message) {
  StaticJsonDocument<1500> doc;  // Выделен объем памяти для JSON-объекта
  DeserializationError error = deserializeJson(doc, message);
  // проверка
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  pressure = doc["main"]["pressure"];
  pressure = pressure / 1.33;
  Serial.print("Давление: ");
  Serial.print(pressure);
  Serial.println("мм рт ст");

  humidity = doc["main"]["humidity"];
  Serial.print("Влажность: ");
  Serial.print(humidity);
  Serial.println("%");

  temperature = doc["main"]["temp"];
  Serial.print("Температура: ");
  Serial.print(temperature);
  Serial.println("С");
}
