#include <FastLED.h> 
#include <FastLED_GFX.h>
#include <FastLEDMatrix.h>
#define NUM_LEDS 8 
CRGB leds[NUM_LEDS]; 
#define LED_PIN 18 
#define COLOR_ORDER GRB 
#define CHIPSET WS2812    

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS); 
  FastLED.setBrightness(90); 
  // цикл установки цвета каждого светодиода
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHue(i * 255 / NUM_LEDS);
  }
  FastLED.show();
}

void loop() {
}
