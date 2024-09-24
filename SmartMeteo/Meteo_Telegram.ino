#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme280;

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "XXXXX"
#define WIFI_PASSWORD "XXXXXXX"
#define BOT_TOKEN "917994XXXXXXXXXXXXXXXXXDvr-RPiM4PUc"
#define your_chat_ID "2XXXXXXXXX7"  // ID вашего чата с ботом
int t_max = 27;
int h_min = 40;
int raw_min = 2000;

const int pinPhoto = 35;

const unsigned long BOT_MTBS = 1000;       // период обновления сканирования новых сообщений
const unsigned long sensor_alert = 10000;  // время отправки нового уведомления (10 сек)
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime, sensor_monitor;
String sensor_status;
bool alert = true;

#include <FastLED.h>     // конфигурация матрицы
#define NUM_LEDS 30      // количество светодиодов в матрице
CRGB leds[NUM_LEDS];     // определяем матрицу (FastLED библиотека)
#define LED_PIN 4        // пин к которому подключена матрица
#define COLOR_ORDER GRB  // порядок цветов матрицы
#define CHIPSET WS2812   // тип светодиодов

#include <iocontrol.h>
const char* myPanelName = "XXXXX";
int status;
// Название переменных как на сайте iocontrol.ru
const char* VarName_temperature = "temperature";
const char* VarName_humidity = "humidity";
const char* VarName_pressure = "pressure";
const char* VarName_light = "light";
WiFiClient client;
iocontrol mypanel(myPanelName, client);

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

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  pinMode(pinPhoto, INPUT);

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

  fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));  // заполнить всю матрицу выбранным цветом
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();

  mypanel.begin();
  bot.sendMessage(your_chat_ID, "Бот запущен", "");  // сообщение о запуске отправлено в Ваш чат
}

void loop() {
  float t = bme280.readTemperature();
  float h = bme280.readHumidity();
  float p = bme280.readPressure() / 133.3F;
  float raw = analogRead(pinPhoto);
  if (alert and (t > t_max or h < h_min)) {          // если значение превышено
    if (millis() - sensor_monitor > sensor_alert) {  // если уже прошло 10 секунд с последней отправки
      String welcome = "Превышены нормы датчиков:\n";
      welcome += "🌡 Температура воздуха: " + String(t, 1) + "°C (" + t_max + "°C)\n";
      welcome += "💧 Влажность воздуха: " + String(h, 0) + "% (" + h_min + " %)\n";
      welcome += "---------------------------------\n";
      welcome += "/alertsoff - отключить уведомления\n";
      bot.sendMessage(your_chat_ID, welcome, "");  // отправить уведомление
      sensor_monitor = millis();                   // сброс таймера
    }
  }
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
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
  mypanel.write(VarName_temperature, t);  // Записали значение
  mypanel.write(VarName_humidity, h);     // Записали значение
  mypanel.write(VarName_pressure, p);     // Записали значение
  mypanel.write(VarName_light, raw);      // Записали значение
  status = mypanel.writeUpdate();
  // Если статус равен константе OK...
  if (status == OK) {
    // Выводим текст в последовательный порт
    Serial.println("------- Write OK -------");
  }
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    text.toLowerCase();
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    if (text == "/sensors") {
      float t = bme280.readTemperature();
      float h = bme280.readHumidity();
      float p = bme280.readPressure() / 133.3F;
      float raw = analogRead(pinPhoto);
      String welcome = "Показания датчиков:\n";
      welcome += "🌡 Температура воздуха: " + String(t, 1) + " °C\n";
      welcome += "💧 Влажность воздуха: " + String(h, 0) + " %\n";
      welcome += "☁ Атмосферное давление: " + String(p, 0) + " мм рт.ст.\n";
      welcome += "☀ Освещенность: " + String(raw, 0) + " Лк\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }

    if (text == "/start") {
      bot.sendMessage(chat_id, "Привет, " + from_name + "!", "");
      bot.sendMessage(chat_id, "Я контроллер Йотик 32. Команды смотрите в меню слева от строки ввода", "");
      String sms = "Команды:\n";
      sms += "/sensors - посмотреть показания\n";
      sms += "/temp - изменить значение критической температуры\n";
      sms += "/hum - изменить значение критической влажности\n";
      sms += "/alertson - включить уведомления\n";
      bot.sendMessage(chat_id, sms, "Markdown");
    }
    if (text == "/temp") {
      bot.sendMessage(chat_id, "Введите критическое значение температуры", "");
      sensor_status = text;
      // ждем, пока не придет новое значение
      while (sensor_status == text) {
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        sensor_status = bot.messages[i].text;
      }
      // преобразовываем текст в int
      t_max = sensor_status.toInt();
      bot.sendMessage(chat_id, "Критическая температура теперь: " + String(t_max, 1) + "°C", "");
    }
    if (text == "/hum") {
      bot.sendMessage(chat_id, "Введите критическое значение влажности", "");
      sensor_status = text;
      // ждем, пока не придет новое значение
      while (sensor_status == text) {
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        sensor_status = bot.messages[i].text;
      }
      // преобразовываем текст в int
      h_min = sensor_status.toInt();
      bot.sendMessage(chat_id, "Критическая влажность теперь: " + String(h_min) + "%", "");
    }
    if (text == "/alertson") {
      alert = true;
      bot.sendMessage(chat_id, "Уведомления включены ", "");
    }
    if (text == "/alertsoff") {
      alert = false;
      bot.sendMessage(chat_id, "Уведомления выключены ", "");
    }
  }
}
