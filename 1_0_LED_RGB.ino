#include <FastLED.h> // библиотеки для FastLED
#include <FastLED_GFX.h>
#include <FastLEDMatrix.h>
#define NUM_LEDS 8 // количество светодиодов 
CRGB leds[NUM_LEDS]; // определяем модуль 
#define LED_PIN 18 // пин к которому подключен модуль 
#define COLOR_ORDER GRB // порядок цветов  
#define CHIPSET WS2812 // тип светодиодов    

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); // конфигурация модуля
  FastLED.setBrightness(90); // яркость (0-255), не рекомендуется выставлять много, особенно при питании через USB
}

void loop() {
  fill_solid( leds, NUM_LEDS, CRGB(255, 0, 0)); // заполнить все выбранным цветом
  FastLED.show(); // функция отображения 
  delay(500);
  fill_solid( leds, NUM_LEDS, CRGB(0, 255, 255)); // заполнить все выбранным цветом
  FastLED.show();
  delay(500);
  FastLED.clear(); // очистить 
  FastLED.show();
  delay(500);
}
