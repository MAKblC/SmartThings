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
  cd77_colorwipe(CRGB::Red , 200);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.delay(50);

  cd77_colorwipe_reverse(CRGB::Blue, 200);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.delay(50);

  cd77_colorwipe_dot(CRGB::Red, 100);

  cd77_colorwipe_half(CRGB::Blue, 200);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.delay(50);

  cd77_colorwipe_half_dot(CRGB::Red, 100);
}

void cd77_colorwipe_dot(CRGB color, uint16_t wait) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    FastLED.delay(wait);
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

void cd77_colorwipe(CRGB color, uint16_t wait) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    FastLED.delay(wait);
  }
}
void cd77_colorwipe_reverse(CRGB color, uint16_t wait) {
  for (uint16_t i = NUM_LEDS - 1; i > 0; i--) {
    leds[i] = color;
    FastLED.delay(wait);
  }
}

void cd77_colorwipe_half_dot(CRGB color, uint16_t wait) {
  for (uint16_t i = 0; i < half; i++) {
    leds[half - i] = color;
    leds[i + half] = color;
    FastLED.delay(wait);
    leds[half - i] = CRGB::Black;
    leds[i + half] = CRGB::Black;
    FastLED.show();
  }
}

void cd77_colorwipe_half(CRGB color, uint16_t wait) {
  for (uint16_t i = 0; i < half; i++) {
    leds[half - i] = color;
    leds[i + half] = color;
    FastLED.delay(wait);
  }
}
