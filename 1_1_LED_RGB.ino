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
