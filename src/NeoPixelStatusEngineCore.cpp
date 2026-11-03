#include "NeoPixelStatusEngineCore.h"

NeoPixelStatusEngineCore::NeoPixelStatusEngineCore()
    : base_(Boot), alertDurationMs_(clampDuration(NEOPIXEL_STATUS_ALERT_DURATION_MS)),
      errorDurationMs_(clampDuration(NEOPIXEL_STATUS_ERROR_DURATION_MS)),
      alertRemainingMs_(0), errorRemainingMs_(0), lastUpdateMs_(0),
      initialized_(false) {}

void NeoPixelStatusEngineCore::setStatus(NeoPixelStatus status) {
  // BootFail remains latched until the caller selects another base status.
  base_ = status;
}
uint32_t NeoPixelStatusEngineCore::clampDuration(uint32_t duration) {
  return duration < 1000U ? 1000U : (duration > 600000U ? 600000U : duration);
}
void NeoPixelStatusEngineCore::setAlertDuration(uint32_t ms) {
  alertDurationMs_ = clampDuration(ms);
}
void NeoPixelStatusEngineCore::setErrorDuration(uint32_t ms) {
  errorDurationMs_ = clampDuration(ms);
}
void NeoPixelStatusEngineCore::setAlert(uint32_t now) {
  update(now); // do not charge time before this event
  if (alertRemainingMs_ == 0) alertRemainingMs_ = alertDurationMs_;
}
void NeoPixelStatusEngineCore::setError(uint32_t now) {
  update(now);
  if (errorRemainingMs_ == 0) errorRemainingMs_ = errorDurationMs_;
}
void NeoPixelStatusEngineCore::update(uint32_t now) {
  if (!initialized_) {
    lastUpdateMs_ = now;
    initialized_ = true;
    return;
  }
  const uint32_t elapsed = now - lastUpdateMs_; // wrap-safe for regular ticks
  lastUpdateMs_ = now;
  // BootFail is latched and suspends all transient timers.
  if (base_ == BootFail) return;
  // Error suspends the Alert countdown: no invisible consumed alert time.
  if (errorRemainingMs_ != 0) {
    errorRemainingMs_ = elapsed >= errorRemainingMs_ ? 0 : errorRemainingMs_ - elapsed;
    return;
  }
  if (alertRemainingMs_ != 0) {
    alertRemainingMs_ = elapsed >= alertRemainingMs_ ? 0 : alertRemainingMs_ - elapsed;
  }
}
NeoPixelDisplayedStatus NeoPixelStatusEngineCore::displayedStatus() const {
  if (base_ == BootFail) return NeoPixelDisplayedStatus::BootFail;
  if (errorRemainingMs_ != 0) return NeoPixelDisplayedStatus::Error;
  if (alertRemainingMs_ != 0) return NeoPixelDisplayedStatus::Alert;
  switch (base_) {
    case Boot: return NeoPixelDisplayedStatus::Boot;
    case LoopIdle: return NeoPixelDisplayedStatus::LoopIdle;
    case LoopRGB: return NeoPixelDisplayedStatus::LoopRGB;
    case BootFail: return NeoPixelDisplayedStatus::BootFail;
  }
  return NeoPixelDisplayedStatus::BootFail;
}
uint8_t NeoPixelStatusEngineCore::triangle(uint8_t phase) {
  return phase < 128 ? static_cast<uint8_t>(phase * 2U)
                     : static_cast<uint8_t>((255U - phase) * 2U);
}
uint32_t NeoPixelStatusEngineCore::pack(uint8_t r, uint8_t g, uint8_t b) {
  return (static_cast<uint32_t>(r) << 16) |
         (static_cast<uint32_t>(g) << 8) | b;
}
uint32_t NeoPixelStatusEngineCore::wheel(uint8_t position) {
  uint8_t p = static_cast<uint8_t>(255U - position);
  if (p < 85) return pack(255U - p*3U, 0, p*3U);
  if (p < 170) {p -= 85; return pack(0, p*3U, 255U - p*3U);}
  p -= 170; return pack(p*3U, 255U - p*3U, 0);
}
uint32_t NeoPixelStatusEngineCore::color(uint32_t now) const {
  switch (displayedStatus()) {
    case NeoPixelDisplayedStatus::Boot:
      return 0x9900FFUL;
    case NeoPixelDisplayedStatus::BootFail:
      return (now / 350U) % 2U == 0U ? 0xB000FFUL : 0xFF0000UL;
    case NeoPixelDisplayedStatus::LoopRGB:
      return wheel(static_cast<uint8_t>(now / 24U));
    case NeoPixelDisplayedStatus::LoopIdle: {
      const uint8_t v = triangle(static_cast<uint8_t>(now / 18U));
      return pack(0, static_cast<uint8_t>(25U + (uint16_t(v) * 180U) / 255U), 0);
    }
    case NeoPixelDisplayedStatus::Alert: {
      const uint8_t v = triangle(static_cast<uint8_t>(now / 12U));
      const uint8_t r = static_cast<uint8_t>(65U + (uint16_t(v)*190U)/255U);
      return pack(r, static_cast<uint8_t>((uint16_t(r)*150U)/255U), 0);
    }
    case NeoPixelDisplayedStatus::Error: {
      const uint8_t v = triangle(static_cast<uint8_t>(now / 6U));
      return pack(static_cast<uint8_t>(75U+(uint16_t(v)*180U)/255U), 0, 0);
    }
  }
  return 0xFF0000UL;
}
