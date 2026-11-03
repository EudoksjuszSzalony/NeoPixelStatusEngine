#requires -Version 5.1
# Junction, not a copy. Refuse to overwrite other libraries.
[CmdletBinding()]
param([switch]$VerifyOnly)
$ErrorActionPreference='Stop'
$source=(Resolve-Path -LiteralPath $PSScriptRoot).Path.TrimEnd('\')
$book=Join-Path ([Environment]::GetFolderPath('MyDocuments')) 'Arduino\libraries'
if(-not(Test-Path -LiteralPath $book -PathType Container)){
  if($VerifyOnly){throw "Missing libraries directory: $book"}
  New-Item -ItemType Directory -Force -Path $book|Out-Null
}
$dest=Join-Path $book 'NeoPixelStatusEngine'
$existing=Get-Item -Force -LiteralPath $dest -ErrorAction SilentlyContinue
if($existing){
  if($existing.LinkType -notin @('Junction','SymbolicLink')){
    throw "An ordinary folder exists, refusing overwrite: $dest"
  }
  $other=@($existing.Target)|Select-Object -First 1
  if(-not [string]::Equals(([IO.Path]::GetFullPath([string]$other)).TrimEnd('\'),
      $source,[StringComparison]::OrdinalIgnoreCase)){
    throw "Another junction already occupies $dest : $other"
  }
}else{
  if($VerifyOnly){throw 'Junction does not exist'}
  New-Item -ItemType Junction -Path $dest -Target $source -ErrorAction Stop|Out-Null
}
$a=(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $source 'src\NeoPixelStatusEngine.h')).Hash
$b=(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $dest 'src\NeoPixelStatusEngine.h')).Hash
if($a -ne $b){throw 'Library SHA256 mismatch through junction'}
Write-Host "PASS: IDE library junction $dest -> $source"
