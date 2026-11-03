#pragma once
#include <stdint.h>
#define NEO_GRB 1
#define NEO_KHZ800 2
extern uint8_t testPixelBrightness;
extern uint32_t testPixelColor;
extern int testShowCount;
class Adafruit_NeoPixel {
public:
  Adafruit_NeoPixel(uint16_t count,int16_t pin,int type) {(void)count;(void)pin;(void)type;}
  void begin() {}
  void clear(){testPixelColor=0;}
  void show(){++testShowCount;}
  void setBrightness(uint8_t value){testPixelBrightness=value;}
  void fill(uint32_t value){testPixelColor=value;}
};
