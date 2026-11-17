$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/blue_magic_p0_20261018'
$games=@(Get-Process GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
$state=[ordered]@{CollectedUtc=[DateTime]::UtcNow.ToString('o');GameServers=$games.Count;Ready=$false;RuntimeDeploymentPerformed=$false;CoreServerSha256=(Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash;EngineSha256=(Get-FileHash -LiteralPath (Join-Path $server 'Engine.dll')).Hash;LegacyDropConfigSha256=(Get-FileHash -LiteralPath (Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini')).Hash}
if($games.Count -eq 1){$state.Pid=$games[0].Id;$state.StartUtc=$games[0].StartTime.ToUniversalTime().ToString('o');$ready=Join-Path $server 'native_world_ready.log';$state.Ready=((Test-Path -LiteralPath $ready) -and (Get-Item -LiteralPath $ready).LastWriteTimeUtc -ge $games[0].StartTime.ToUniversalTime())}
[IO.File]::WriteAllText((Join-Path $report 'runtime_state.json'),($state|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$state|ConvertTo-Json
