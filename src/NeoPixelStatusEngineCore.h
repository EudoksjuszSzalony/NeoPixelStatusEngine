#pragma once
#include <stdint.h>

/**
 * @brief Persistent indications. Fixed numeric values help diagnostics.
 *
 * C++11 unscoped named enum supports BOTH forms:
 *   led.setStatus(Boot);
 *   led.setStatus(NeoPixelStatus::Boot);
 * Type NeoPixelStatus:: in Arduino IDE to request all available values.
 * Alert and Error are timed events; use setAlert()/setError().
 */
enum NeoPixelStatus : uint8_t {
  Boot = 0,         ///< Purple during initialisation.
  LoopIdle = 1,     ///< Green breathing during normal operation.
  LoopRGB = 2,      ///< Continuous rainbow wheel.
  BootFail = 3      ///< Latched, alternating purple/red failure.
};

enum class NeoPixelDisplayedStatus : uint8_t {
  Boot, LoopIdle, LoopRGB, BootFail, Alert, Error
};

#ifndef NEOPIXEL_STATUS_ALERT_DURATION_MS
#define NEOPIXEL_STATUS_ALERT_DURATION_MS 4000UL
#endif
#ifndef NEOPIXEL_STATUS_ERROR_DURATION_MS
#define NEOPIXEL_STATUS_ERROR_DURATION_MS 8000UL
#endif

// Arduino-independent C++11 status machine: unit-testable on any host.
class NeoPixelStatusEngineCore {
 public:
  NeoPixelStatusEngineCore();
  void setStatus(NeoPixelStatus status);
  NeoPixelStatus baseStatus() const { return base_; }
  void setAlert(uint32_t nowMs);
  void setError(uint32_t nowMs);
  void setAlertDuration(uint32_t durationMs);
  void setErrorDuration(uint32_t durationMs);
  uint32_t alertDuration() const { return alertDurationMs_; }
  uint32_t errorDuration() const { return errorDurationMs_; }
  bool alertPending() const { return alertRemainingMs_ != 0; }
  bool errorPending() const { return errorRemainingMs_ != 0; }
  void update(uint32_t nowMs);
  NeoPixelDisplayedStatus displayedStatus() const;
  uint32_t color(uint32_t nowMs) const; // RGB without brightness scaling

 private:
  static uint32_t clampDuration(uint32_t requested);
  static uint8_t triangle(uint8_t phase);
  static uint32_t pack(uint8_t r, uint8_t g, uint8_t b);
  static uint32_t wheel(uint8_t phase);
  NeoPixelStatus base_;
  uint32_t alertDurationMs_;
  uint32_t errorDurationMs_;
  uint32_t alertRemainingMs_;
  uint32_t errorRemainingMs_;
  uint32_t lastUpdateMs_;
  bool initialized_;
};
