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
  leds[0].setRGB(255, 0, 0);   // красный
  leds[2].setRGB(255, 255, 0);   // желтый
  leds[4].setRGB(0, 255, 0);   // зеленый
  leds[6].setRGB(255, 0, 255);   // розовый
  FastLED.show();
  delay(300);
  FastLED.clear();
  leds[1].setRGB(255, 0, 0);   // красный
  leds[3].setRGB(255, 255, 0);   // желтый
  leds[5].setRGB(0, 255, 0);   // зеленый
  leds[7].setRGB(255, 0, 255);   // розовый
  FastLED.show();
  delay(300);
  FastLED.clear();
}
