#include "FastLED.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define PORTION 5        // объем порции (1-10)
#define VOLUME 2         // объем воды (длительность работы насоса в секундах)
#define LIGHT_EDGE 1000  // порог включения освещенности

const char* ssid = "XXXXX";
const char* password = "XXXXXXXX";

const char* mqtt_server = "XXX.wqtt.ru";  // адрес
int mqtt_port = XXXXXX;                   // порт
const char* mqtt_login = "XXXXXXXXX";     // пользователь
const char* mqtt_pass = "XXXXXXXXX";      // пароль

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

const int pinPhoto = 35;  // пин фоторезистора
#define pump 23           // пин насоса

#include <ESP32_Servo.h>
Servo myservo;
#define OPENED 180  // насыпать корм
#define CLOSED 110  // закрыть кормушку

#include <FastLED.h>
#define NUM_LEDS 30  // количество светодиодов в матрице
CRGB leds[NUM_LEDS];
#define LED_PIN 4  // пин к которому подключена матрица
#define COLOR_ORDER GRB
#define CHIPSET WS2812

void setup() {
  Serial.begin(115200);
  myservo.attach(5);  // пин сервопривода
  myservo.write(CLOSED);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(pinPhoto, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int lux = analogRead(pinPhoto);
  if (lux < LIGHT_EDGE) {
    fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
  } else {
    FastLED.clear();
  }
  FastLED.show();
}

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
      client.subscribe("Feeder");
      client.subscribe("Water");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // 5 секунд для повторного подключения
      delay(5000);
    }
  }
}

// функция приёма и обработки пакета данных
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Если принята единица то включить светодиод
  if (String(topic) == "Feeder" and (char) payload[0] == '1') {
    myservo.write(OPENED);
    delay(PORTION * 50);
    Serial.println("Кормление");
    myservo.write(CLOSED);
  }
  if (String(topic) == "Water" and (char) payload[0] == '1') {
    digitalWrite(pump, HIGH);
    Serial.println("Включено поение");
    delay(VOLUME * 1000);
    digitalWrite(pump, LOW);
  }
}

void setup_wifi() {

  delay(10);
  // Подключение к сети
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
