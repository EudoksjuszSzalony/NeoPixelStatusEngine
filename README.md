# NeoPixel Status Engine v0.2.2 (development)

A reusable, non-blocking Arduino library for RGB NeoPixel/WS2812 indicators.
Single external dependency: **Adafruit NeoPixel**. Does not depend on PFCS,
CyberFan, OLED, network or ESP32. Firmware PWM/actuators are never touched.

## Developer-friendly typed status values

The public status type is a C++11 named enum with fixed numeric values:
Boot=0, LoopIdle=1, LoopRGB=2, BootFail=3. You never need to type numbers.

Both short and qualified syntax work:

    #include <NeoPixelStatusEngine.h>
    NeoPixelStatusEngine led(1, PIN_NEOPIXEL, 220, NEOPIXEL_I2C_POWER);
    void setup() {
      led.begin();
      led.setStatus(Boot);
      // Equivalent, with reliable enum autocomplete:
      led.setStatus(NeoPixelStatus::Boot);
      led.setBrightness(220);
      led.setModeBrightness(LoopRGB,90);
      led.setModeBrightness(BootFail,255);
      led.setAlertBrightness(210);
      led.setErrorBrightness(255);
      led.setAlertDuration(4000);
      led.setErrorDuration(8000);
    }
    void loop() {
      led.update(); // Call continuously, no delay().
    }

In **Arduino IDE 2**, type **led.** for method suggestions, and
**NeoPixelStatus::** for valid enum values. The latter is the most reliable
autocomplete trigger, and also avoids ambiguity when another library defines
a global identifier named Boot. The short form led.setStatus(Boot) is valid.
Suggestions come from Arduino IDE's C++ language server, **not** keywords.txt
(which controls legacy syntax coloring). Let the language server index files;
try a Verify run if completion does not appear.

## Statuses and timing

| Display | Meaning | Duration |
| --- | --- | --- |
| Boot | Purple | Until changed |
| LoopIdle | Green breathing | Until changed |
| LoopRGB | RGB wheel | Until changed |
| BootFail | High-brightness alternating purple/red (~350 ms) | Latched until changed |
| Alert | Amber breathing, priority over normal states | One-shot 4 seconds by default |
| Error | Red breathing, priority over Alert | One-shot 8 seconds by default |

Priority: **BootFail > Error > Alert > base status**.
Call setAlert() or setError() **once per incident**, not every loop.
Each expires automatically; there is no clearAlert()/clearError() call.
While Error is displayed, Alert's timer pauses, so its warning stays visible
for the remaining duration afterward. BootFail never auto-expires.

Use setAlertDuration(ms)/setErrorDuration(ms) to adjust hold times; a value
outside 1,000–600,000 ms is clamped. Compile-time defaults can be overridden
with NEOPIXEL_STATUS_ALERT_DURATION_MS and
NEOPIXEL_STATUS_ERROR_DURATION_MS **for the library compilation unit**.

The effective brightness is master × mode / 255. Default master=220; mode
levels Boot=125, LoopIdle=75, LoopRGB=85, BootFail=255, Alert=205, Error=245.
All brightness levels 0–255.

## Feather ESP32 V2 electrical details

- Built-in NeoPixel data: GPIO0 (PIN_NEOPIXEL).
- Shared NeoPixel/STEMMA QT power: GPIO2 (NEOPIXEL_I2C_POWER), HIGH enables.
  Passing the power pin causes begin() to ENABLE but never disable that rail.
- D13/GPIO13 is a SEPARATE red LED and optional physical heartbeat; the
  library never touches it.
- Boot GPIO0 is also a strapping pin: electrical validation required before
  attaching extra external NeoPixels.

## Building

Open examples/StatusDemo/StatusDemo.ino in Arduino IDE 2 with Feather ESP32 V2
board selected and Adafruit NeoPixel installed. The build script
Build-NeoPixelStatusEngineOffline.ps1 compiles via local Arduino CLI but never
uploads. Host tests use C++11 with a stubbed NeoPixel implementation.

The separate repo holds the single source of truth for this library:
https://github.com/EudoksjuszSzalony/NeoPixelStatusEngine

**Not yet submitted** to Arduino Library Manager and no tagged release until
Arduino Lint, license approval, ESP32 physical checks and API review.


## Development update v0.2.2

Corrects timer accounting when a busy application calls update() late: if Error
expires within one update interval, the remaining elapsed time counts toward
the paused Alert. Priority and default durations remain unchanged. Host tests
include this stalled-loop regression. This commit is **not** a hardware release.

The optional Windows offline build script discovers arduino-cli on PATH and
Python from the py launcher or PATH; it keeps legacy local tool locations as a
fallback. No external downloads, firmware upload or device-control commands
are part of the script. Override paths when needed:

    .\Build-NeoPixelStatusEngineOffline.ps1 -CliPath "C:\tools\arduino-cli.exe" -LibrariesPath "$HOME\Documents\Arduino\libraries"

No release tag is created. Feather V2 hardware, LED electrical behavior and
power rail handling remain unverified on physical hardware.
