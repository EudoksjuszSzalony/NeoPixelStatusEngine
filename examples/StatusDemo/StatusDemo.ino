// Compile for Adafruit Feather ESP32 V2, or any board with an external NeoPixel.
// No PWM, network, fan or pump activity. The status LED is independent from D13.
#include <NeoPixelStatusEngine.h>
#ifdef PIN_NEOPIXEL
constexpr int16_t kPixelPin=PIN_NEOPIXEL;
#else
constexpr int16_t kPixelPin=6;
#endif
#ifdef NEOPIXEL_I2C_POWER
constexpr int16_t kPowerPin=NEOPIXEL_I2C_POWER;
#else
constexpr int16_t kPowerPin=-1;
#endif

NeoPixelStatusEngine led(1,kPixelPin,220,kPowerPin);
bool loopStarted=false, alertSent=false, errorSent=false, bootFailSent=false;

void setup() {
  led.begin();
  led.setStatus(Boot);
  // SetStatus also accepts the scoped form; autocomplete after NeoPixelStatus::
  led.setStatus(NeoPixelStatus::Boot);
  led.setAlertDuration(4000);
  led.setErrorDuration(8000);
  led.setBrightness(220);
  led.setModeBrightness(Boot,130);
  led.setModeBrightness(LoopRGB,90);
  led.setModeBrightness(LoopIdle,60);
  led.setModeBrightness(BootFail,255);
  led.setAlertBrightness(210);
  led.setErrorBrightness(255);
}
void loop() {
  const uint32_t t=millis();
  if(t>2000 && !loopStarted) {
    led.setStatus(NeoPixelStatus::LoopRGB);
    loopStarted=true;
  }
  if(t>4000 && !alertSent) { led.setAlert(); alertSent=true; }
  // Amber automatically ends after 4 sec; no clearAlert needed.
  if(t>10000 && !errorSent) { led.setError(); errorSent=true; }
  // Red automatically ends after 8 sec; no clearError needed.
  if(t>22000 && !bootFailSent) {
    led.setStatus(NeoPixelStatus::BootFail);
    bootFailSent=true;
  }
  // BootFail stays purple/red until manually selecting another status.
  led.update();
}
