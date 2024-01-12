#include <FastLED.h> // библиотеки для FastLED
#include <FastLED_GFX.h>
#include <FastLEDMatrix.h>
#define NUM_LEDS 8 // количество светодиодов в матрице  
CRGB leds[NUM_LEDS]; // определяем матрицу (FastLED библиотека)
#define LED_PIN 18 // пин к которому подключена матрица 
#define COLOR_ORDER GRB // порядок цветов матрицы 
#define CHIPSET WS2812 // тип светодиодов    

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); // конфигурация матрицы // matrix configuration
  FastLED.setBrightness(90); // яркость (0-255), не рекомендуется выставлять много, особенно при питании через USB
}

void loop() {
  fill_solid( leds, NUM_LEDS, CRGB(255, 0, 0)); // заполнить всю матрицу выбранным цветом
  FastLED.show();
  delay(500);
  fill_solid( leds, NUM_LEDS, CRGB(0, 255, 255)); // заполнить всю матрицу выбранным цветом
  FastLED.show();
  delay(500);
  FastLED.clear();
  FastLED.show();
  delay(500);
}
