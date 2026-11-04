#requires -Version 5.1
# Offline-only compile: no upload, COM access, fan or pump control.
[CmdletBinding()]
param(
  [string]$CliPath,
  [string]$ConfigPath,
  [string]$LibrariesPath,
  [string]$PythonPath
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$legacyTools = Join-Path $env:LOCALAPPDATA 'CyberFanBuildTools\ArduinoCliV1'
$cliCommand = Get-Command arduino-cli -ErrorAction SilentlyContinue
if (-not $CliPath) {
  if ($cliCommand) { $CliPath = $cliCommand.Source }
  else { $CliPath = Join-Path $legacyTools 'arduino-cli.exe' }
}
if (-not (Test-Path -LiteralPath $CliPath -PathType Leaf)) {
  throw "Arduino CLI not found (use -CliPath): $CliPath"
}
if (-not $ConfigPath) {
  $legacyConfig = Join-Path $legacyTools 'arduino-cli.yaml'
  if (Test-Path -LiteralPath $legacyConfig -PathType Leaf) {
    $ConfigPath = $legacyConfig
  }
}
if ($ConfigPath -and -not (Test-Path -LiteralPath $ConfigPath -PathType Leaf)) {
  throw "Arduino CLI config not found: $ConfigPath"
}
if (-not $LibrariesPath) {
  $LibrariesPath = Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Arduino\libraries'
}
$neoPixelMetadata = Join-Path $LibrariesPath 'Adafruit_NeoPixel\library.properties'
if (-not (Test-Path -LiteralPath $neoPixelMetadata -PathType Leaf)) {
  throw "Adafruit NeoPixel not found: $neoPixelMetadata (use -LibrariesPath)"
}
$pythonArguments = @()
if (-not $PythonPath) {
  $pythonLauncher = Get-Command py -ErrorAction SilentlyContinue
  if ($pythonLauncher) {
    $PythonPath = $pythonLauncher.Source
    $pythonArguments = @('-3')
  } else {
    $pythonCommand = Get-Command python -ErrorAction SilentlyContinue
    if ($pythonCommand) { $PythonPath = $pythonCommand.Source }
  }
}
if (-not $PythonPath -or -not (Test-Path -LiteralPath $PythonPath -PathType Leaf)) {
  throw 'Python 3 not found (use -PythonPath)'
}
& $PythonPath @pythonArguments (Join-Path $root 'tests\test_library_static.py')
if ($LASTEXITCODE -ne 0) { throw 'Static tests FAILED' }

$build = Join-Path $env:TEMP 'NeoPixelStatusEngine_v0_2_2_Offline'
New-Item -ItemType Directory -Path $build -Force | Out-Null
$cliArguments = @('compile', '--fqbn', 'esp32:esp32:adafruit_feather_esp32_v2')
if ($ConfigPath) { $cliArguments += @('--config-file', $ConfigPath) }
$cliArguments += @('--build-path', $build, '--warnings', 'more',
  '--library', $root, '--libraries', $LibrariesPath,
  (Join-Path $root 'examples\StatusDemo'))
& $CliPath @cliArguments
if ($LASTEXITCODE -ne 0) { throw 'Arduino Feather V2 compilation FAILED' }
Write-Host 'PASS: Arduino Feather ESP32 V2 offline example compilation (no upload)'

if (Get-Command g++ -ErrorAction SilentlyContinue) {
  & (Join-Path $root 'tests\Run-HostTests.ps1')
  if ($LASTEXITCODE -and $LASTEXITCODE -ne 0) {
    throw 'Host tests FAILED'
  }
} else {
  Write-Host 'NOT RUN: native C++ host tests (g++ is not installed)'
}
