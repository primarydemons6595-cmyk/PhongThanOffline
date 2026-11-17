$ErrorActionPreference='Stop'
$testRoot=Join-Path ([IO.Path]::GetTempPath()) ('pt-byte-hook-'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $testRoot | Out-Null
$path=Join-Path $testRoot 'fixture.cpp'
# Invalid UTF-8 and legacy Vietnamese bytes surround the ASCII hook.
$fixture=[byte[]](0x2f,0x2f,0x20,0xff,0xb5,0xae,0x0d,0x0a,0x48,0x4f,0x4f,0x4b,0x0d,0x0a,0xe1,0xa9)
[IO.File]::WriteAllBytes($path,$fixture)
$hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
& $hook -Path $path -ExpectedSha256 $hash -Old 'HOOK' -New 'REPLACEMENT' | Out-Null
if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $hash) { throw 'Dry run wrote bytes.' }
& $hook -Path $path -ExpectedSha256 $hash -Old 'HOOK' -New 'REPLACEMENT' -Apply | Out-Null
$actual=[IO.File]::ReadAllBytes($path)
$expected=[byte[]](0x2f,0x2f,0x20,0xff,0xb5,0xae,0x0d,0x0a)+[Text.Encoding]::ASCII.GetBytes('REPLACEMENT')+[byte[]](0x0d,0x0a,0xe1,0xa9)
if ([Convert]::ToBase64String($actual) -cne [Convert]::ToBase64String($expected)) { throw 'Legacy byte preservation failed.' }
$rejected=$false
try { & $hook -Path $path -ExpectedSha256 $hash -Old 'REPLACEMENT' -New 'BAD' -Apply | Out-Null } catch { $rejected=$true }
if (-not $rejected) { throw 'Stale hash accepted.' }
$hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
$rejected=$false
try { & $hook -Path $path -ExpectedSha256 $hash -Old 'MISSING' -New 'BAD' -Apply | Out-Null } catch { $rejected=$true }
if (-not $rejected) { throw 'Missing hook accepted.' }
'PASS: dry run; raw legacy byte preservation; stale hash rejection; missing hook rejection.'
"Fixture retained: $path"
