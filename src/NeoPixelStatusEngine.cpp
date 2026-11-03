#include "NeoPixelStatusEngine.h"

NeoPixelStatusEngine::NeoPixelStatusEngine(uint16_t count, int16_t pin,
    uint8_t brightness, int16_t enablePowerPin, bool powerActiveHigh)
    : core_(), pixels_(count,pin,NEO_GRB+NEO_KHZ800),
      powerPin_(enablePowerPin), powerActiveHigh_(powerActiveHigh),
      masterBrightness_(brightness),
      brightnessPerMode_{125,75,85,255,205,245},
      ready_(false), rendered_(false), lastFrameMs_(0), lastColor_(0),
      lastBrightness_(0), lastMode_(NeoPixelDisplayedStatus::Boot) {}

uint8_t NeoPixelStatusEngine::modeIndex(NeoPixelDisplayedStatus m) {
  return static_cast<uint8_t>(m);
}
void NeoPixelStatusEngine::setBrightness(uint8_t value) {
  masterBrightness_ = value;
  rendered_ = false;
}
void NeoPixelStatusEngine::setModeBrightness(NeoPixelStatus mode, uint8_t value) {
  const uint8_t index = static_cast<uint8_t>(mode);
  if (index >= 4) return;
  brightnessPerMode_[index] = value;
  rendered_ = false;
}
void NeoPixelStatusEngine::setAlertBrightness(uint8_t value) {
  brightnessPerMode_[modeIndex(NeoPixelDisplayedStatus::Alert)] = value;
  rendered_ = false;
}
void NeoPixelStatusEngine::setErrorBrightness(uint8_t value) {
  brightnessPerMode_[modeIndex(NeoPixelDisplayedStatus::Error)] = value;
  rendered_ = false;
}
uint8_t NeoPixelStatusEngine::modeBrightness(NeoPixelDisplayedStatus mode) const {
  const uint8_t index = modeIndex(mode);
  return index < 6 ? brightnessPerMode_[index] : 0;
}
uint8_t NeoPixelStatusEngine::effectiveBrightness() const {
  return static_cast<uint8_t>(
      (uint16_t(masterBrightness_)*modeBrightness(core_.displayedStatus())+127U)/255U);
}
void NeoPixelStatusEngine::begin() {
  if (powerPin_ >= 0) {
    pinMode(powerPin_,OUTPUT);
    digitalWrite(powerPin_,powerActiveHigh_ ? HIGH : LOW);
  }
  pixels_.begin();
  pixels_.clear();
  pixels_.show();
  ready_ = true;
  rendered_ = false;
  update(millis());
}
void NeoPixelStatusEngine::update(uint32_t now) {
  core_.update(now);
  if (!ready_) return;
  const auto mode=core_.displayedStatus();
  const uint8_t currentBrightness=effectiveBrightness();
  if (rendered_ && mode==lastMode_ && currentBrightness==lastBrightness_ &&
      uint32_t(now-lastFrameMs_)<30U) return;
  const uint32_t rgb=core_.color(now);
  if (!rendered_ || rgb!=lastColor_ || currentBrightness!=lastBrightness_ ||
      mode!=lastMode_) {
    pixels_.setBrightness(currentBrightness);
    pixels_.fill(rgb);
    pixels_.show();
    lastColor_=rgb;
    lastBrightness_=currentBrightness;
  }
  lastMode_=mode;
  lastFrameMs_=now;
  rendered_=true;
}
