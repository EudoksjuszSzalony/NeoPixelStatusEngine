#requires -Version 5.1
# Native offline tests only. Never uses a microcontroller, COM, PWM or Arduino upload.
[CmdletBinding()]
param([switch]$AllowSkip)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path

$compiler = $null
$kind = $null
foreach ($name in @('g++', 'clang++', 'cl.exe')) {
    $candidate = Get-Command $name -ErrorAction SilentlyContinue
    if ($candidate) {
        $compiler = $candidate.Source
        $kind = $name
        break
    }
}
if (-not $compiler) {
    $message = 'Native C++ tests NOT RUN: g++, clang++ or cl.exe not available in PATH.'
    if ($AllowSkip) {
        Write-Warning $message
        return
    }
    throw $message
}

$out = Join-Path $env:TEMP 'NeoPixelStatusEngineHostTests'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$sourceDir = Join-Path $root 'src'
$stubDir = Join-Path $root 'tests\stubs'
$coreSource = Join-Path $sourceDir 'NeoPixelStatusEngineCore.cpp'
$adapterSource = Join-Path $sourceDir 'NeoPixelStatusEngine.cpp'

$cases = @(
    [pscustomobject]@{
        Name='core'
        Sources=@($coreSource, (Join-Path $root 'tests\StatusEngineHost.cpp'))
        Includes=@($sourceDir)
    },
    [pscustomobject]@{
        Name='timing'
        Sources=@($coreSource, (Join-Path $root 'tests\TimingStressHost.cpp'))
        Includes=@($sourceDir)
    },
    [pscustomobject]@{
        Name='animation'
        Sources=@($coreSource, (Join-Path $root 'tests\AnimationHost.cpp'))
        Includes=@($sourceDir)
    },
    [pscustomobject]@{
        Name='adapter'
        Sources=@($coreSource, $adapterSource, (Join-Path $root 'tests\AdapterHost.cpp'))
        Includes=@($stubDir, $sourceDir)
    }
)

Write-Host "Native test compiler: $compiler"
Push-Location -LiteralPath $out
try {
    foreach ($test in $cases) {
        $exe = Join-Path $out ($test.Name + '.exe')
        if ($kind -eq 'cl.exe') {
            $arguments = @('/nologo', '/std:c++14', '/EHsc', '/W4', '/WX',
                           '/D_CRT_SECURE_NO_WARNINGS')
            foreach ($dir in $test.Includes) { $arguments += ('/I' + $dir) }
            $arguments += ('/Fe:' + $exe)
            $arguments += $test.Sources
        } else {
            $arguments = @('-std=c++11', '-Wall', '-Wextra', '-Werror')
            foreach ($dir in $test.Includes) { $arguments += @('-I', $dir) }
            $arguments += $test.Sources
            $arguments += @('-o', $exe)
        }

        & $compiler @arguments
        if ($LASTEXITCODE -ne 0) {
            throw "Native $($test.Name) compilation FAILED (exit $LASTEXITCODE)"
        }
        if (-not (Test-Path -LiteralPath $exe -PathType Leaf)) {
            throw "Native $($test.Name) executable missing after compilation: $exe"
        }
        & $exe
        if ($LASTEXITCODE -ne 0) {
            throw "Native $($test.Name) unit test FAILED (exit $LASTEXITCODE)"
        }
    }
} finally {
    Pop-Location
}
Write-Host 'PASS: four native C++11 host test executables'
