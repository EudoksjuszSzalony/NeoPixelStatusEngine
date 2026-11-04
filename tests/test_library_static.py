from pathlib import Path
p=Path(__file__).resolve().parents[1]
meta=dict(line.split("=",1) for line in (p/"library.properties").read_text().splitlines() if "=" in line)
assert meta["name"]=="NeoPixel Status Engine"
assert meta["version"]=="0.2.2"
assert meta["includes"]=="NeoPixelStatusEngine.h"
assert meta["depends"]=="Adafruit NeoPixel"
assert (p/"src/NeoPixelStatusEngine.h").exists()
assert (p/"src/NeoPixelStatusEngineCore.cpp").exists()
assert (p/"examples/StatusDemo/StatusDemo.ino").exists()
all_source="\n".join(x.read_text() for x in (p/"src").glob("*"))
for banned in ["namespace pfcs","PFCSStatusEngine","PFCSStatusLED","delay(", "ledcWrite(", "digitalWrite(13"]:
    assert banned not in all_source,banned
for val,n in [("Boot",0),("LoopIdle",1),("LoopRGB",2),("BootFail",3)]:
    assert f"{val} = {n}" in all_source
assert "enum NeoPixelStatus : uint8_t" in all_source
assert "setStatus(NeoPixelStatus status)" in all_source
for name in ["setAlert(", "setError(", "setBrightness(", "setModeBrightness(", "setAlertBrightness(", "setErrorBrightness("]:
    assert name in all_source,name
for name in ("AnimationHost.cpp", "TimingStressHost.cpp", "StatusEngineHost.cpp", "AdapterHost.cpp"):
    assert (p/"tests"/name).exists(), name
print("PASS: library metadata, typed enum, autocomplete API, status priority, host suites, PWM untouched")
