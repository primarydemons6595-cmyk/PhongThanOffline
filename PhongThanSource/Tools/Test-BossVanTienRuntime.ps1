param([string]$ExpectedCoreSha256)
$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$server=Join-Path $workspace 'PhongThanRuntime-Loose\Server'
$batch=Join-Path $workspace 'analysis\restoration_20261016\boss_van_tien'
$expected=Join-Path $server 'GameServer.exe'
$games=@(Get-CimInstance Win32_Process | Where-Object ExecutablePath -EQ $expected)
if($games.Count -ne 1){throw 'Expected one loose GameServer'}
$game=Get-Process -Id $games[0].ProcessId
$start=$game.StartTime.ToUniversalTime()
$epoch=[DateTime]::SpecifyKind([DateTime]'1970-01-01',[DateTimeKind]::Utc)
$startUtc=[long][Math]::Floor(($start-$epoch).TotalSeconds)
$bishops=@(Get-CimInstance Win32_Process | Where-Object ExecutablePath -EQ (Join-Path $server 'Bishop.exe'))
if($bishops.Count -ne 1){throw 'Expected one loose Bishop'}
$bishopPid=$bishops[0].ProcessId
$readyPath=Join-Path $server 'native_world_ready.log'
$readyText=[IO.File]::ReadAllText($readyPath)
$activeIds=@([regex]::Matches([IO.File]::ReadAllText((Join-Path $server 'maps\WorldSet.ini')),'(?m)^World\d+=(\d+)') | ForEach-Object {[int]$_.Groups[1].Value})
$expectedCount=$activeIds.Count
$arenasRegistered=@(1067,1068,1069,1070 | Where-Object {$_ -notin $activeIds}).Count -eq 0
$worldReady=(Get-Item $readyPath).LastWriteTimeUtc -ge $start -and $readyText -match "WORLD_READY bishop_pid=$bishopPid service=1001 maps=$expectedCount"
$scheduleText=[IO.File]::ReadAllText((Join-Path $server 'original_schedule_status.log'))
$scheduleReady=$false
foreach($match in [regex]::Matches($scheduleText,'READY utc=(\d+) rows=63 utc_offset=420 enabled=1')){
    if([long]$match.Groups[1].Value -ge $startUtc){$scheduleReady=$true}
}
$plan=Get-Content -Raw (Join-Path $batch 'plan.json') | ConvertFrom-Json
$sha=(Get-FileHash (Join-Path $server 'CoreServer.dll') -Algorithm SHA256).Hash.ToLowerInvariant()
if(-not $ExpectedCoreSha256){$ExpectedCoreSha256=$plan.binaries[0].sha256}
if($sha -ne $ExpectedCoreSha256.ToLowerInvariant()){throw 'Unexpected published DLL; pass the SHA from the newer reviewed publication with -ExpectedCoreSha256'}
$result=[ordered]@{Status=$(if($worldReady -and $scheduleReady -and $arenasRegistered){'READY'}else{'LOADING'});GameServerPid=$game.Id;BishopPid=$bishopPid;StartUtc=$start.ToString('o');VerifiedUtc=[DateTime]::UtcNow.ToString('o');ActiveMapCount=$expectedCount;WorldReady=$worldReady;VanTienArenasRegistered=$arenasRegistered;ScheduleRows63=$scheduleReady;UtcOffsetMinutes=420;CoreServerSha256=$sha;LivePlayerAcceptance=$false}
$json=$result | ConvertTo-Json
[IO.File]::WriteAllText((Join-Path $batch 'runtime_verification.json'),$json,[Text.UTF8Encoding]::new($false))
Write-Output $json
