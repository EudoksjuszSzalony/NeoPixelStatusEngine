#requires -Version 5.1
# Offline-only compile, never flash, COM, fan or pump output.
$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$tools=Join-Path $env:LOCALAPPDATA 'CyberFanBuildTools\ArduinoCliV1'
$cli=Join-Path $tools 'arduino-cli.exe'
$cfg=Join-Path $tools 'arduino-cli.yaml'
$libs=Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Arduino\libraries'
$python='C:\Program Files\Python313\python.exe'
foreach($f in @($cli,$cfg,$python,(Join-Path $libs 'Adafruit_NeoPixel\library.properties'))){
    if(-not(Test-Path -LiteralPath $f)){throw "Required build dependency missing: $f"}
}
& $python (Join-Path $root 'tests\test_library_static.py')
if($LASTEXITCODE -ne 0){throw 'Static tests FAILED'}
$build=Join-Path $env:TEMP 'NeoPixelStatusEngine_v0_2_1_Offline'
New-Item -ItemType Directory -Path $build -Force|Out-Null
$argsList=@('compile','--fqbn','esp32:esp32:adafruit_feather_esp32_v2','--config-file',$cfg,
'--build-path',$build,'--warnings','more','--library',$root,'--libraries',$libs,
(Join-Path $root 'examples\StatusDemo'))
& $cli @argsList
if($LASTEXITCODE -ne 0){throw 'Arduino Feather V2 compilation FAILED'}
Write-Host 'PASS: Arduino ESP32 3.3.12 status engine example; no upload'
if(Get-Command g++ -ErrorAction SilentlyContinue){
    & (Join-Path $root 'tests\Run-HostTests.ps1')
}else{
    Write-Host 'NOT RUN: native C++ tests (g++ absent from CAD VM)'
}
