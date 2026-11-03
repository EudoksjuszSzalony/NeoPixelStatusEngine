#include "../src/NeoPixelStatusEngineCore.h"
#include <cassert>
#include <cstdio>
int main() {
 static_assert(static_cast<uint8_t>(NeoPixelStatus::Boot)==0,"enum");
 static_assert(static_cast<uint8_t>(NeoPixelStatus::LoopIdle)==1,"enum");
 static_assert(static_cast<uint8_t>(NeoPixelStatus::LoopRGB)==2,"enum");
 static_assert(static_cast<uint8_t>(NeoPixelStatus::BootFail)==3,"enum");
 NeoPixelStatusEngineCore e;
 assert(e.displayedStatus()==NeoPixelDisplayedStatus::Boot);
 e.setStatus(NeoPixelStatus::LoopIdle);
 e.update(1000);
 e.setAlert(1002);
 e.update(1004);
 assert(e.displayedStatus()==NeoPixelDisplayedStatus::Alert);
 e.update(4999);assert(e.displayedStatus()==NeoPixelDisplayedStatus::Alert);
 e.update(5002);assert(e.displayedStatus()==NeoPixelDisplayedStatus::LoopIdle);
 e.setError(6000);
 e.update(13999);assert(e.displayedStatus()==NeoPixelDisplayedStatus::Error);
 e.update(14000);assert(e.displayedStatus()==NeoPixelDisplayedStatus::LoopIdle);
 e.setAlert(20000);e.update(21000);e.setError(21000);
 e.update(29000);assert(e.displayedStatus()==NeoPixelDisplayedStatus::Alert);
 e.update(32000);assert(e.displayedStatus()==NeoPixelDisplayedStatus::LoopIdle);
 e.setAlert(33000);e.setStatus(NeoPixelStatus::BootFail);
 assert(e.displayedStatus()==NeoPixelDisplayedStatus::BootFail);
 assert(e.color(0)==0xB000FF && e.color(350)==0xFF0000);
 e.update(90000);assert(e.displayedStatus()==NeoPixelDisplayedStatus::BootFail);
 e.setStatus(LoopRGB);
 assert(e.displayedStatus()==NeoPixelDisplayedStatus::Alert);
 e.update(94000);assert(e.displayedStatus()==NeoPixelDisplayedStatus::LoopRGB);
 e.setAlertDuration(2);e.setErrorDuration(0);
 assert(e.alertDuration()==1000 && e.errorDuration()==1000);
 NeoPixelStatusEngineCore wrap;
 wrap.setStatus(LoopRGB);wrap.setError(0xFFFFFF00UL);
 wrap.update(0x000000F0UL);
 assert(wrap.displayedStatus()==NeoPixelDisplayedStatus::Error);
 wrap.update(0x00001E40UL);
 assert(wrap.displayedStatus()==NeoPixelDisplayedStatus::LoopRGB);
 std::puts("PASS core: typed enum, durations, priority, BootFail, millis wrap");
}
