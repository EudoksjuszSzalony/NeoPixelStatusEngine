#requires -Version 5.1
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gpp) {throw "Native g++ not installed; C++ unit tests NOT RUN"}
$out = Join-Path $env:TEMP 'NeoPixelStatusEngineHostTests'
New-Item -ItemType Directory -Path $out -Force | Out-Null
& $gpp.Source -std=c++11 -Wall -Wextra -Werror -I (Join-Path $root 'src') (Join-Path $root 'src\NeoPixelStatusEngineCore.cpp') (Join-Path $root 'tests\StatusEngineHost.cpp') -o (Join-Path $out 'core.exe')
if($LASTEXITCODE -ne 0){throw 'Core compilation FAILED'}
& (Join-Path $out 'core.exe')
if($LASTEXITCODE -ne 0){throw 'Core unit tests FAILED'}
& $gpp.Source -std=c++11 -Wall -Wextra -Werror -I (Join-Path $root 'tests\stubs') -I (Join-Path $root 'src') (Join-Path $root 'src\NeoPixelStatusEngineCore.cpp') (Join-Path $root 'src\NeoPixelStatusEngine.cpp') (Join-Path $root 'tests\AdapterHost.cpp') -o (Join-Path $out 'adapter.exe')
if($LASTEXITCODE -ne 0){throw 'Adapter compilation FAILED'}
& (Join-Path $out 'adapter.exe')
if($LASTEXITCODE -ne 0){throw 'Adapter tests FAILED'}
Write-Host 'PASS: native C++ unit tests'
