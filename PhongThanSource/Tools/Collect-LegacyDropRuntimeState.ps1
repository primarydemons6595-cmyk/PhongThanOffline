$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_live'
$game=@(Get-Process GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
$state=[ordered]@{CollectedUtc=[DateTime]::UtcNow.ToString('o');GameServers=$game.Count;Ready=$false;Stopping=$false;Mode='UNKNOWN';CoreServerSha256=(Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash;EngineSha256=(Get-FileHash -LiteralPath (Join-Path $server 'Engine.dll')).Hash;ProductionGate='CLOSED';Parity='KINGSOFT_PARITY_UNVERIFIED'}
$text=[IO.File]::ReadAllText((Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini'))
$mode=[regex]::Match($text,'(?m)^Mode=([^\r\n]*)');if($mode.Success){$state.Mode=$mode.Groups[1].Value}
if($game.Count -eq 1){
 $state.Pid=$game[0].Id;$state.StartUtc=$game[0].StartTime.ToUniversalTime().ToString('o')
 try{$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($game[0].Id)");try{$state.Stopping=$event.WaitOne(0)}finally{$event.Dispose()}}catch{}
 $ready=Join-Path $server 'native_world_ready.log'
 $state.Ready=(!$state.Stopping -and (Test-Path -LiteralPath $ready) -and (Get-Item -LiteralPath $ready).LastWriteTimeUtc -ge $game[0].StartTime.ToUniversalTime())
 $state.WorkingSetBytes=$game[0].WorkingSet64;$state.PrivateMemoryBytes=$game[0].PrivateMemorySize64
}
$json=$state|ConvertTo-Json
[IO.File]::WriteAllText((Join-Path $report 'runtime_state.json'),$json,[Text.UTF8Encoding]::new($false))
$json
