#include <WiFi.h>  // библиотека для Wi-Fi
#include <FastLED.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>                                  // библиотека Телеграм

#define WIFI_SSID "XXXXX"                                          // логин Wi-Fi
#define WIFI_PASSWORD "XXXXXXXXX"                                  // пароль Wi-Fi
#define BOT_TOKEN "917XXXXXXX:AAEzXXXXXXXXXXXXXXXXXXXXXXXXXXXXc"   // токен
const unsigned long BOT_MTBS = 1000;                               // время обновления
WiFiClientSecure secured_client;                                   // клиент SSL (TLS)
UniversalTelegramBot bot(BOT_TOKEN, secured_client);               // экземпляр бота
unsigned long bot_lasttime;                                        // счетчик времени

#define LED_PIN1 4   // пин, к которому подключена лента
#define LED_NUM1 30  // количество светодиодов на ленте
CRGB led1[LED_NUM1];

#define PHOTO 35 // пин для подключения фоторезистора
#define GAUGE 34 // пин для подключения потенциометра

// переменные
String chat_id, mail, sms;
String text;
int bright;
int message_id;
float val;
float lux;
int luix, i, c;
bool sw;

void setup() {
  Serial.begin(115200);
  pinMode(PHOTO, INPUT);
  pinMode(GAUGE, INPUT);
  FastLED.addLeds< WS2812, LED_PIN1, GRB>(led1, LED_NUM1);
  FastLED.setBrightness(100);
  delay(512);
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                 // подключение
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // сертификат
  while (WiFi.status() != WL_CONNECTED)                 // проверка подключения
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  if (millis() - bot_lasttime > BOT_MTBS)  // периодическая проверка
  {
    // постановка нового сообщения в очередь обработки
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    // если сообщение пришло
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    // отслеживание периодичности опроса
    bot_lasttime = millis();
  }

  // измерение показания потенциометра
  bright = analogRead(GAUGE);
  bright = map(bright, 0, 4095, 0, 100);
  FastLED.setBrightness(c * bright / 100);
  lux = analogRead(PHOTO);  // считывание освещенности
  // определение яркости в зависимости от режима
  switch (sw) {
    case 0:
      FastLED.setBrightness(c * bright / 100);
      break;
    case 1:
      c = map(lux, 0, 3000, 255, 0);
      c = constrain(c, 0, 255);
      FastLED.setBrightness(c * bright / 100);
      break;
  }
  FastLED.show();
}

// функция обработки нового сообщения
void handleNewMessages(int numNewMessages) {
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  // обработка сообщений
  for (int i = 0; i < numNewMessages; i++) {
    String mail = "";
    String sms = "[[{\"text\" : \"Автоматическая яркость\", \"callback_data\" : \"auto\"}, {\"text\" : \"Ручная яркость\", \"callback_data\" : \"hand\"}]]";
    // считывание ID сообщения
    message_id = bot.messages[i].message_id;
    chat_id = bot.messages[i].chat_id;
    text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;  // имя пользователя
    if (from_name == "") from_name = "Guest";
    // обработка команд
    // если сообщение имеет тип обратного запроса
    if (text == "/start") {
      String mail = "Привет! 👋\n";
      mail += "Этот бот может управлять твоей умной лампой\n\n";
      String keyboardJson = "[[\"🔴\", \"🟠\", \"🟡\"],[\"🟢\",\"🔵\",\"🟣\"],[\"⚪\", \"⚫\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Выберите цвет", "", keyboardJson, true);
      bot.sendMessageWithInlineKeyboard(chat_id, mail, "", sms);
    }
    if (text == "🔴") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран красный", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(255, 0, 0);
      }
    } else if (text == "🟠") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран оранжевый", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(255, 150, 0);
      }
    } else if (text == "🟡") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран желтый", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(255, 255, 0);
      }
    } else if (text == "🟢") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран зеленый", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(0, 255, 0);
      }
    } else if (text == "🔵") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран синий", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(0, 0, 255);
      }
    } else if (text == "🟣") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран фиолетовый", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(255, 0, 255);
      }
    } else if (text == "⚪") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Выбран белый", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(255, 255, 255);
      }
    } else if (text == "⚫") {
      bot.sendMessageWithInlineKeyboard(chat_id, "Лампа выключена", "", sms);
      for (int i = 0; i < LED_NUM1; i++) {
        led1[i] = CRGB(0, 0, 0);
      }
    }
    if (text == "auto") {
      sw = 1;
    } else if (text == "hand") {
      sw = 0;
      bot.sendMessage(chat_id, "Выберите яркость от 0 ддо 255 ", "");
      String led_status = text;
      // ждем, пока не придет новое значение
      while (led_status == text) {
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        led_status = bot.messages[i].text;
      }
      // преобразовываем текст в int
      c = led_status.toInt();
      FastLED.setBrightness(c * bright / 100);
    }
    FastLED.show();
    sms = "[[{\"text\" : \"Автоматическая яркость\", \"callback_data\" : \"/red\"}, {\"text\" : \"Ручная яркость\", \"callback_data\" : \"/green\"}]]";
  }
}
