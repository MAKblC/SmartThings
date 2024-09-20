#include <Wire.h>
#include "FastLED.h"
#include <Arduino.h>
#include <GyverHub.h>

int bright;
float lux;
int luix, i;
static uint32_t color;
static int sld4;
static int inp1 = 200;
static int inp2 = 400;
static bool sw1;
static bool sw2;
static bool sw3;
uint16_t size = 12;
const char* Icon;
#define LED_PIN1 4   // пин ленты
#define LED_NUM1 30  // количество светодиодов
CRGB led1[LED_NUM1];

#define PHOTO 35 // пин фоторезистора
#define GAUGE 34 // пин потенциометра

// логин-пароль от роутера
#define AP_SSID "XXXXXX"
#define AP_PASS "XXXXXX"

const char* mqtt_server = "XX.wqtt.ru";  //адрес
int mqtt_port = XXXXX;                   //порт
const char* mqtt_login = "XXXXXXXX";     // пользователь
const char* mqtt_pass = "XXXXXXXX";      //пароль

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//GyverHub hub;
GyverHub hub("MyDevices", "Lamp", "");  // можно настроить тут, но без F-строк!

unsigned long t_del = 0;

// обработчик кнопки (см. ниже)
void btn_cb() {
  Serial.println("click 4");
}

// обработчик кнопки с информацией о билде (см. ниже)
void btn_cb_b(gh::Build& b) {
  Serial.print("click 5 from client ID: ");
  Serial.println(b.client.id);
}

// билдер
void build(gh::Builder& b) {
  // =================== КОНТЕЙНЕРЫ ===================
  // =================== ШИРИНА ===================

  // ширина виджетов задаётся в "долях" - отношении их ширины друг к другу
  // виджеты займут пропорциональное место во всю ширину контейнера
  // если ширина не указана - она принимается за 1
  if (b.beginRow()) {
    b.Label_(F("jon")).label(" ").value("Умная лампа MGBot");
    b.endRow();
  }

  // =============== ПОДКЛЮЧЕНИЕ ПЕРЕМЕННОЙ ===============

  if (b.beginRow()) {
    Icon = "f011";
    b.SwitchIcon_("sw", &sw2).label("Выкл/Вкл").icon(Icon).size(2).click();
    b.endRow();
  }
  if (b.beginRow()) {
    b.Label_(F("Jon")).label(" ").value("Границы работы автояркости:").fontSize(size).size(2);
    b.Input(&inp1).label("Мин.").size(1);
    b.Input(&inp2).label("Макс.").size(1);
    b.Label_(F("label")).label("Освещённость").size(1).value("__");  // с указанием стандартного значения
    b.endRow();
  }

  if (b.beginRow()) {
    b.Label_(F("bon")).label(" ").value("Выбор цвета:").fontSize(size).size(2);
    if (b.Switch(&sw1).label("Выкл/вкл").size(2).click()) {
      Serial.print("switch: ");
      Serial.println(sw1);
    }
    b.Color(&color).label("Цвет").size(2);
    Serial.println(color);
    b.endRow();
  }

  if (b.beginRow()) {
    b.Label_(F("don")).label(" ").value("Управление яркостью:").fontSize(size).size(2);
    b.Switch(&sw3).label("Вкл/Выкл").size(2).click();
    b.Slider(&sld4).label("Яркость").size(2);
    b.endRow();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PHOTO, INPUT);
  pinMode(GAUGE, INPUT);
  Wire.begin();
  FastLED.addLeds< WS2812, LED_PIN1, GRB>(led1, LED_NUM1);

#ifdef GH_ESP_BUILD
  // подключение к роутеру
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  // если нужен MQTT - подключаемся
  hub.mqtt.config("test.mosquitto.org", 1883);
#endif

  // указать префикс сети, имя устройства и иконку
  hub.config(F("MyDevices"), F("Lamp"), F(""));
  // подключить билдер
  hub.onBuild(build);
  // запуск!
  hub.begin();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  hub.tick();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  bright = analogRead(GAUGE);
  bright = map(bright, 0, 4095, 0, 100);
  // Serial.println(bright);
  // =========== ОБНОВЛЕНИЯ ПО ТАЙМЕРУ ===========
  static gh::Timer tmr(10);  // период 1 секунда

  if (millis() - t_del > 500) {
    t_del = millis();
    lux = analogRead(PHOTO);  // считывание освещенности
  }

  luix = lux;
  hub.update(F("label")).value(luix);

  if (sw1 == 0) {
    if (sw2 == 1) {
      for (i = 0; i < LED_NUM1; i++) {
        led1[i].setRGB(255, 255, 255);
      }
      int c = map(lux, inp2, inp1, 0, 255);
      c = constrain(c, 0, 255);
      FastLED.setBrightness(c * bright / 100);
      FastLED.show();
      int b = map(lux, inp2, inp1, 0, 90);
      b = constrain(b, 0, 90);
    }
    if (sw2 == 0) {
      for (i = 0; i < LED_NUM1; i++) {
        led1[i].setRGB(0, 0, 0);
      }
      FastLED.setBrightness(0);
      FastLED.show();
    }
  }
  if (sw1 == 1) {
    if (sw2 == 1) {
      int lit = sld4;
      int a = map(lit, 100, 0, 90, 0);
      int e = map(lit, 100, 0, 255, 0);
      e = constrain(e, 0, 255);
      int c = map(lux, inp2, inp1, 0, 255);
      c = constrain(c, 0, 255);
      int red = color >> 16;
      int green = (color & 0x00ff00) >> 8;
      int blue = (color & 0x0000ff);

      if (sw3 == 0) {
        for (i = 0; i < LED_NUM1; i++) {
          led1[i] = color;
        }
        FastLED.setBrightness(c * bright / 100);
        FastLED.show();
      }
      if (sw3 == 1) {
        for (i = 0; i < LED_NUM1; i++) {
          led1[i] = color;
        }
        FastLED.setBrightness(e * bright / 100);
        FastLED.show();
      }
    }
    if (sw2 == 0) {
      for (i = 0; i < LED_NUM1; i++) {
        led1[i].setRGB(0, 0, 0);
      }
      FastLED.setBrightness(0);
      FastLED.show();
    }
  }
}

// обработка соощения от Алисы
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    sw2 = 1;
    sw1 = 1;
    sw3 = 1;
    sld4 = 100;
    color = 0xFFFFFFFF;
    FastLED.setBrightness(255 * bright / 100);
    fill_solid(led1, LED_NUM1, CRGB(255, 255, 255));
    FastLED.show();
  } else {
    sw2 = 0;
    FastLED.clear();
    FastLED.show();
  }
}

// Подключение к серверу
void reconnect() {
  // Ждем, пока не подсоединимся
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Создается случайный номер клиента
    String clientId = "IoTik32Client-";
    clientId += String(random(0xffff), HEX);
    // Попытка подключения
    if (client.connect(clientId.c_str(), mqtt_login, mqtt_pass)) {
      Serial.println("connected");
      // ... и подписки на топик
      client.subscribe("TopicLight");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // 5 секунд для повторного подключения
      delay(5000);
    }
  }
}
