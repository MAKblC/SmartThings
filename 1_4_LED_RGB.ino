#include <FastLED.h> 
#include <FastLED_GFX.h>
#include <FastLEDMatrix.h>
#define NUM_LEDS 8  
CRGB leds[NUM_LEDS]; 
#define LED_PIN 18 
#define COLOR_ORDER GRB  
#define CHIPSET WS2812   
uint16_t half = NUM_LEDS/2;

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); 
  FastLED.setBrightness(90); 
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
