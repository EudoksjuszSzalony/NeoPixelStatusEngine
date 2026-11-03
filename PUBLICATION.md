# Preparing Arduino Library Manager release

GitHub repository: https://github.com/EudoksjuszSzalony/NeoPixelStatusEngine
Arduino Library Manager requires a public repository with library.properties
at the root, src/, examples/ and a semver tag that matches library.properties.

This repository deliberately has NO GitHub Actions workflow. Use local testing
and Arduino Lint; do not trigger Actions without separate approval.
The copyright holder must approve the distribution license before release.
Review possible name conflicts, choose a LICENSE, run Arduino Lint locally,
and then tag a tested semver release and submit it through
https://github.com/arduino/library-registry .

This is source-only: physical LED, supply, GPIO0 strap and integrated
Controller PWM/D5 safety are NOT hardware-tested.
