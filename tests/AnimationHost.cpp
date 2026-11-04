#include "../src/NeoPixelStatusEngineCore.h"
#include <cassert>
#include <cstdio>

int main() {
  NeoPixelStatusEngineCore core;
  assert(core.color(0) == 0x9900FFUL);

  core.setStatus(LoopRGB);
  // The RGB wheel visits the primary colors, with no blocking delay.
  assert(core.color(0) == 0xFF0000UL);
  assert(core.color(85U * 24U) == 0x00FF00UL);
  assert(core.color(170U * 24U) == 0x0000FFUL);

  core.setStatus(LoopIdle);
  const uint32_t dark = core.color(0);
  const uint32_t bright = core.color(128U * 18U);
  assert((dark & 0xFF00FFUL) == 0);
  assert((bright & 0xFF00FFUL) == 0);
  assert(((bright >> 8) & 0xFF) > ((dark >> 8) & 0xFF));

  core.setAlert(100);
  for (uint32_t t = 100; t < 2100; t += 79) {
    const uint32_t amber = core.color(t);
    assert((amber & 0xFF) == 0);
    assert(((amber >> 8) & 0xFF) > 0);
  }
  core.setError(200);
  for (uint32_t t = 200; t < 900; t += 17) {
    const uint32_t red = core.color(t);
    assert((red & 0xFFFF) == 0);
  }

  core.setStatus(BootFail);
  assert(core.color(0) == 0xB000FFUL);
  assert(core.color(349) == 0xB000FFUL);
  assert(core.color(350) == 0xFF0000UL);
  assert(core.color(699) == 0xFF0000UL);
  assert(core.color(700) == 0xB000FFUL);
  std::puts("PASS animation: Boot, RGB wheel, green breathing, amber/red, BootFail cadence");
}
