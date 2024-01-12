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
}

void loop() {
  // "весь спектр"
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHSV(i * 255 / NUM_LEDS, 255, 255);
  }
  FastLED.show();
  delay(2000);
  // "стремление от белого"
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHSV(255, i * 255 / NUM_LEDS, 255);
  }
  FastLED.show();
  delay(2000);
  // "стремление от черного"
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHSV(255, 255, i * 255 / NUM_LEDS);
  }
  FastLED.show();
  delay(2000);
}




