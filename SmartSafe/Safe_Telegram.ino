#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "FastLED.h"
// параметры сети
#define WIFI_SSID "XXXX"
#define WIFI_PASSWORD "XXXXXXX"
// токен вашего бота
#define BOT_TOKEN "9179XXXXXXXXXXXXXXXXXXXXXX4PUc"
const unsigned long BOT_MTBS = 500;  
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;

//Переменные для сейфа
int level = 0;
bool startBreake = false;
String last_chat_id = "";
String last_msg = "";
bool msgArived = false;

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

void setup() {
  pinMode(pinPhoto, INPUT);
  myservo.attach(13);  // пин сервомотора
  myservo.write(50);
  pinMode(pin, INPUT);  
  FastLED.addLeds< WS2812, LED_PIN1, GRB>(led1, LED_NUM1);
  Serial.begin(115200);
  delay(512);
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Wire.begin();
  bool bme_status = bme280.begin();
  if (!bme_status) {
    Serial.println("Не найден по адресу 0х77, пробую другой...");
    bme_status = bme280.begin(0x76);
    if (!bme_status)
      Serial.println("Датчик не найден, проверьте соединение");
  }
}

// функция обработки новых сообщений
void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    last_chat_id = chat_id;
    String text = bot.messages[i].text;
    text.toLowerCase();
    last_msg = text;
    if (level == 5 or level == 4) msgArived = true;
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";


    if ((text == "/start") or (text == "/help"))  // команда для вызова помощи
    {
      bot.sendMessage(chat_id, "Привет, " + from_name + "!", "");
      bot.sendMessage(chat_id, "Я контроллер Йотик 32. Команды смотрите в меню слева от строки ввода", "");
      String sms = "Команды:\n";
      sms += "/startbreak - начать взлом\n";
      sms += "/stopbreak - завершить взлом\n";
      bot.sendMessage(chat_id, sms, "Markdown");
    }

    if (text == "/startbreak") {
      startBreake = true;
      for (int i = 0; i < 20; i++) {
        led1[i].setRGB(0, 0, 0);
      }
      for (int i = 21; i < 25; i++) {
        led1[i].setRGB(0, 255, 0);
      }
      FastLED.show();
      myservo.write(50);
      level = 0;
    }

    if (text == "/stopbreak") {
      startBreake = false;
      FastLED.clear();
      for (int i = 21; i < 25; i++) {
        led1[i].setRGB(0, 255, 0);
      }
      FastLED.show();
      myservo.write(50);
      bot.sendMessage(chat_id, "Взлом остановлен", "");
    }
  }
}

void loop()  // вызываем функцию обработки сообщений через определенный период
{
  if (startBreake == true) {
    safeChekerSensor();
  }

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

void safeChekerSensor() {
  if (level == 0) {
    sendMSG("Тайны прячутся в темноте");
    level = 1;
    //lvlComplite();
  }

  raw = analogRead(pinPhoto);
  Serial.println(raw);
  // уровень 1: фоторезистор
  if (level == 1 and raw < 1000) {
    lvlComplite();
    sendMSG("Теплее...");
    level = 2;
  }

  float t = bme280.readTemperature();
  // уровень 2: датчик температуры
  if (level == 2 and t > 26) {
    lvlComplite();
    sendMSG("сохраняй баланс");
    level = 3;
  }

  int poten = analogRead(pin);
  Serial.println(poten);
  // уровень 3: позиция потенциометра
  if (poten < 1900 and poten > 1800 and level == 3) {
    lvlComplite();
    sendMSG("2 в 🎲");
    level = 4;
  }

  if (level == 4 && msgArived == true) {
    msgArived = false;
    // уровень 4: ответ на вопрос "два в кубе"
    if (last_msg == "8") {
      lvlComplite();
      sendMSG("Сколько ядер в ЙоТике?");
      level = 5;
    } else {
      sendMSG("Неа, что-то не сходится");
    }
  }
  if (level == 5 && msgArived == true) {
    msgArived = false;
    // уровень 5: ответ на вопрос "сколько ядер в йотике"
    if (last_msg == "2") {
      lvlComplite();
      sendMSG("Сейф взломан, вы хакеры что надо!");
      for (int i = 0; i < 20; i++) {
        led1[i].setRGB(255, 255, 255);
      }
      FastLED.show();
      myservo.write(150);
      level = 6;
    } else {
      sendMSG("Неа, что-то не сходится");
    }
  }
}


// включение светодиодов при прохождении уровня
void lvlComplite() {
  led1[30 - level].setRGB(0, 255, 0);
  FastLED.show();
}

String bufer_msg = "";

void sendMSG(String msg) {
  if (bufer_msg != msg) {
    bot.sendMessage(last_chat_id, msg, "");
    bufer_msg = msg;
  }
}
