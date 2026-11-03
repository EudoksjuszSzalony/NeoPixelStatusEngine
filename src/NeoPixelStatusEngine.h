#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "NeoPixelStatusEngineCore.h"

/**
 * Non-blocking NeoPixel status indication for Arduino projects.
 * Type led. to see public methods, NeoPixelStatus:: to complete enum values.
 */
class NeoPixelStatusEngine {
 public:
  NeoPixelStatusEngine(uint16_t count, int16_t pixelPin,
      uint8_t brightness = 220, int16_t enablePowerPin = -1,
      bool powerActiveHigh = true);

  /** Initialise NeoPixel (and enable optional shared power regulator). */
  void begin();

  /** Update animations and expire timed events; call every loop iteration. */
  void update(uint32_t nowMs);
  void update() { update(millis()); }

  /**
   * Select a persistent status, never a numeric duty or a timed Alert/Error.
   * status: Boot, LoopIdle, LoopRGB or BootFail.
   * Both led.setStatus(Boot) and led.setStatus(NeoPixelStatus::Boot) work.
   */
  void setStatus(NeoPixelStatus status) { core_.setStatus(status); }

  /** One-shot amber Alert, auto-expires (default: 4000 milliseconds). */
  void setAlert() { setAlert(millis()); }
  void setAlert(uint32_t nowMs) { core_.setAlert(nowMs); }

  /** One-shot red Error, auto-expires (default: 8000 milliseconds). */
  void setError() { setError(millis()); }
  void setError(uint32_t nowMs) { core_.setError(nowMs); }

  /** Duration in milliseconds, clamped between 1000 and 600000. */
  void setAlertDuration(uint32_t durationMs) { core_.setAlertDuration(durationMs); }
  void setErrorDuration(uint32_t durationMs) { core_.setErrorDuration(durationMs); }

  /** Global brightness 0..255, combined with the selected mode brightness. */
  void setBrightness(uint8_t masterBrightness);
  uint8_t brightness() const { return masterBrightness_; }

  /** Per-persistent-mode brightness: Boot/LoopIdle/LoopRGB/BootFail. */
  void setModeBrightness(NeoPixelStatus status, uint8_t brightness);
  void setAlertBrightness(uint8_t value);
  void setErrorBrightness(uint8_t value);

  uint8_t modeBrightness(NeoPixelDisplayedStatus mode) const;
  uint8_t effectiveBrightness() const;
  NeoPixelDisplayedStatus displayedStatus() const { return core_.displayedStatus(); }
  const NeoPixelStatusEngineCore& core() const { return core_; }
  bool isReady() const { return ready_; }

 private:
  static uint8_t modeIndex(NeoPixelDisplayedStatus mode);
  NeoPixelStatusEngineCore core_;
  Adafruit_NeoPixel pixels_;
  int16_t powerPin_;
  bool powerActiveHigh_;
  uint8_t masterBrightness_;
  uint8_t brightnessPerMode_[6]; // Boot, Idle, RGB, BootFail, Alert, Error
  bool ready_;
  bool rendered_;
  uint32_t lastFrameMs_;
  uint32_t lastColor_;
  uint8_t lastBrightness_;
  NeoPixelDisplayedStatus lastMode_;
};
