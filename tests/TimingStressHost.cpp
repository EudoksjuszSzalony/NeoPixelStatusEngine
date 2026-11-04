#include "../src/NeoPixelStatusEngineCore.h"
#include <cassert>
#include <cstdio>

// Equivalent event sequences must produce the same result whether update()
// executes every millisecond or only occasionally. No real delays or hardware.
static void verifyTimeline(uint32_t origin) {
  NeoPixelStatusEngineCore dense;
  NeoPixelStatusEngineCore sparse;
  dense.setStatus(LoopIdle);
  sparse.setStatus(LoopIdle);
  dense.setAlertDuration(3500);
  sparse.setAlertDuration(3500);
  dense.setErrorDuration(6500);
  sparse.setErrorDuration(6500);

  for (uint32_t step = 0; step <= 30000; ++step) {
    const uint32_t now = origin + step;  // Also exercises millis() wraparound.
    const bool event = step == 100 || step == 500 || step == 1500 ||
                       step == 5000 || step == 9000 || step == 14500 ||
                       step == 17000 || step == 19000;
    const bool check = event || (step % 997U == 0U) || step == 30000;
    dense.update(now);
    if (check) sparse.update(now);

    if (step == 100 || step == 1500) {
      dense.setAlert(now);
      sparse.setAlert(now);
    }
    if (step == 500 || step == 5000 || step == 19000) {
      dense.setError(now);
      sparse.setError(now);
    }
    if (step == 9000) {
      dense.setStatus(BootFail);
      sparse.setStatus(BootFail);
    }
    if (step == 14500) {
      dense.setStatus(LoopRGB);
      sparse.setStatus(LoopRGB);
    }
    if (step == 17000) {
      dense.setStatus(LoopIdle);
      sparse.setStatus(LoopIdle);
    }
    if (check) {
      assert(dense.displayedStatus() == sparse.displayedStatus());
      assert(dense.alertPending() == sparse.alertPending());
      assert(dense.errorPending() == sparse.errorPending());
      assert(dense.color(now) == sparse.color(now));
    }
  }
}
int main() {
  verifyTimeline(0);
  verifyTimeline(0xFFFFF000UL);
  std::puts("PASS timing stress: dense/delayed updates, overlapping events, BootFail, millis wrap");
}
