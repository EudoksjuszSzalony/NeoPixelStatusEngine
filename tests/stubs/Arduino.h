#pragma once
#include <stdint.h>
#define OUTPUT 1
#define LOW 0
#define HIGH 1
extern uint32_t testMillis;
extern int lastPinModePin;
extern int lastDigitalWritePin;
extern int lastDigitalWriteValue;
inline uint32_t millis() {return testMillis;}
inline void pinMode(int p,int m) {lastPinModePin=p;(void)m;}
inline void digitalWrite(int p,int v) {lastDigitalWritePin=p;lastDigitalWriteValue=v;}
