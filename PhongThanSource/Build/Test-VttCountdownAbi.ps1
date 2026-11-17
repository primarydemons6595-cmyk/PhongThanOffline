$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$folder=Join-Path (Split-Path -Parent $project) 'analysis/vtt_client_services_20261019'
$r=& "$project/Tests/Test-LuaApiBinaryRegistry.ps1" -ProjectRoot $project -IncludeAll
$api=@($r.AllRegistrations | Where-Object Name -eq 'ShowMissionCountdown')
if($api.Count -ne 1){throw 'Countdown API registration missing'}
$fixture=Join-Path $folder 'countdown_abi.tsv'
[IO.File]::WriteAllText($fixture,"ShowMissionCountdown`t$($api[0].FunctionRva)`tnumber,number`n",[Text.Encoding]::ASCII)
$runner=Join-Path $project 'Output/Server/VttCountdownSmoke.exe'
Copy-Item -LiteralPath "$project/Output/Tools/LuaApiBinarySmoke.exe" -Destination $runner
try {
 $output=@(& $runner "$project/Output/Server" $fixture)
 $exit=$LASTEXITCODE
 $output | Set-Content (Join-Path $folder 'countdown_abi.log')
 $output
 if($exit){throw 'Countdown ABI failed'}
} finally {Remove-Item -LiteralPath $runner}
