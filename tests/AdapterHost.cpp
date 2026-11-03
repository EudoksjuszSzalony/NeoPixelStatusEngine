#include "../src/NeoPixelStatusEngine.h"
#include <cassert>
#include <cstdio>
uint32_t testMillis=0;
int lastPinModePin=-1,lastDigitalWritePin=-1,lastDigitalWriteValue=-1;
uint8_t testPixelBrightness=0;
uint32_t testPixelColor=0;
int testShowCount=0;
int main(){
 NeoPixelStatusEngine led(1,0,220,2);
 led.begin();
 assert(led.isReady());
 assert(lastPinModePin==2 && lastDigitalWritePin==2 && lastDigitalWriteValue==HIGH);
 led.setStatus(NeoPixelStatus::Boot);
 led.update(100);
 assert(testPixelColor==0x9900FF);
 led.setBrightness(255);
 led.setModeBrightness(Boot,100);
 led.update(200);
 assert(testPixelBrightness==100);
 led.setStatus(NeoPixelStatus::LoopIdle);
 led.setModeBrightness(LoopIdle,60);
 led.update(300);
 assert(testPixelBrightness==60);
 led.setAlertBrightness(210);
 led.setAlert(500);led.update(500);
 assert(led.displayedStatus()==NeoPixelDisplayedStatus::Alert);
 assert(testPixelBrightness==210);
 led.update(4501);
 assert(led.displayedStatus()==NeoPixelDisplayedStatus::LoopIdle);
 led.setErrorBrightness(230);
 led.setError(5000);led.update(5000);
 assert(testPixelBrightness==230);
 led.setStatus(BootFail);
 led.setModeBrightness(BootFail,255);
 led.update(5300);
 assert(testPixelBrightness==255 && testPixelColor==0xFF0000);
 led.update(5600);
 assert(testPixelColor==0xB000FF);
 assert(lastDigitalWritePin==2);
 std::puts("PASS NeoPixel adapter: brightness, GPIO2, typed enum, no D13");
}
