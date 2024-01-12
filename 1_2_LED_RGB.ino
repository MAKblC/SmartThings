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
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHue(i * 255 / NUM_LEDS);
  }
  FastLED.show();
}

void loop() {
}
