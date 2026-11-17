$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
if(Test-Path (Join-Path $report 'handoff.json')){throw 'Handoff baseline exists; do not reset evidence offsets after player testing'}
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
$processes=Get-Content -Raw (Join-Path $report 'processes.json')|ConvertFrom-Json
$worldRecord=$processes|Where-Object Name -eq 'GameServer.exe';$world=Get-Process -Id $worldRecord.Pid
if($world.Path -ine (Join-Path $iso.Server 'GameServer.exe')){throw 'World path mismatch'}
$ready=Get-Item (Join-Path $iso.Server 'native_world_ready.log');if($ready.LastWriteTimeUtc -lt $world.StartTime.ToUniversalTime()){throw 'Stale WORLD_READY'}
$events=@(Get-Content (Join-Path $iso.Server 'legacy_drop_debug.jsonl')|ForEach-Object {$_|ConvertFrom-Json}|Where-Object pid -eq ([string]$world.Id))
$latest=@($events|Where-Object event -eq 'P2_BLUE_STATUS')[-1]
if($latest.status -ne 'READY' -or $latest.chance_per_million -ne '1000000'){throw 'P2 policy/logger not READY'}
$ports=netstat -ano -p tcp
foreach($entry in $processes){$proc=Get-Process -Id $entry.Pid;if($proc.Path -ine (Join-Path $iso.Server $entry.Name)){throw 'Service path mismatch'};$pattern='\s[^\s]+:'+ $entry.Port +'\s+[^\s]+\s+LISTENING\s+'+$entry.Pid+'\s*$';if(!($ports -match $pattern)){throw 'Service port not listening'}}
$clientRecord=Get-Content -Raw (Join-Path $report 'client_process.json')|ConvertFrom-Json
$client=Get-Process -Id $clientRecord.Pid;if($client.Path -ine (Join-Path $iso.Client 'Game.exe')){throw 'Real Client path mismatch'}
$legacy=[IO.File]::ReadAllText((Join-Path $iso.Server 'settings/phongthan/LegacyDropExperiment.ini'))
if($legacy -notmatch '(?m)^Mode=LIVE_TEST\s*$' -or $legacy -notmatch '(?m)^SelfTestEnabled=0\s*$'){throw 'Legacy mode/selftest mismatch'}
$quarantine=Join-Path $iso.Server 'settings/phongthan/LegacyDropQuarantine.txt'
if((Get-FileHash $quarantine).Hash -ne $iso.QuarantineSha256){throw 'Quarantine changed'}
$before=Join-Path $iso.Backup 'CharacterStore';$unchanged=$true
foreach($file in Get-ChildItem $before -File -Filter '*.pthc'){if((Get-FileHash $file.FullName).Hash -ne (Get-FileHash (Join-Path $iso.Server ('CharacterStore/'+$file.Name))).Hash){$unchanged=$false}}
$live=Get-Process -Id 49460;$hash=(Get-FileHash (Join-Path $workspace 'PhongThanRuntime-Loose/Server/CoreServer.dll')).Hash
if($hash -ne 'BAC0354CC1C691F35801A73EB1067A555D6B8E031299DD52AA65DDE7A5D79907'){throw 'Active Loose baseline changed'}
$serverMagic=Join-Path $iso.Server 'p1_blue_magic.jsonl';$clientMagic=Join-Path $iso.Client 'p1_blue_magic.jsonl'
$receipt=@{Status='READY_FOR_PLAYER_TEST';P2Acceptance='WAITING_PLAYER_LOGS';StartedUtc=$iso.Utc;CheckedUtc=[DateTime]::UtcNow.ToString('o');GameServerPid=$world.Id;ClientPid=$client.Id;WorldReadyLine=(Get-Content $ready.FullName -Tail 1);LegacyMode='LIVE_TEST';BlueStatus=$latest;DeathEvents=@($events|Where-Object event -eq 'NPC_DEATH').Count;BlueApplyEvents=@($events|Where-Object event -eq 'BLUE_APPLY').Count;SelfTestEvents=@($events|Where-Object event -like 'SELF_TEST*').Count;TestCharacterStoreUnchangedBeforeHandoff=$unchanged;ServerMagicLogOffset=0;ClientMagicLogOffset=0;LivePid=$live.Id;LiveStartUtc=$live.StartTime.ToUniversalTime().ToString('o');LiveCoreSha256=$hash;Source='REAL_PROCESS_PORT_WORLD_READY_AND_RUNTIME_STATUS_LOG'}
[IO.File]::WriteAllText((Join-Path $report 'handoff.json'),($receipt|ConvertTo-Json -Depth 8),[Text.UTF8Encoding]::new($false))
Copy-Item -LiteralPath (Join-Path $iso.Server 'legacy_drop_debug.jsonl') -Destination (Join-Path $report 'pre_handoff_runtime.jsonl')
$receipt|ConvertTo-Json -Depth 8
