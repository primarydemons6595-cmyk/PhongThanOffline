$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$target=[IO.Path]::GetFullPath($iso.Root)
$allowed=Join-Path $workspace 'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_'
if(!$target.StartsWith($allowed,[StringComparison]::OrdinalIgnoreCase) -or !(Test-Path -LiteralPath (Join-Path $target 'p0_isolated.flag'))){throw 'Invalid isolation root'}
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq (Join-Path $iso.Client 'Game.exe')}).Count){throw 'Logout and close isolated Client first'}
$exe=Join-Path $iso.Server 'GameServer.exe'
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe})
if($world.Count -ne 1){throw 'Exactly one isolated world required'}
$final=Join-Path $report 'final';if(Test-Path -LiteralPath $final){throw 'Final evidence already exists'}
New-Item -ItemType Directory -Path $final|Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $final 'CharacterStore.before_stop') -Recurse
$proc=$world[0];[void]$proc.Handle
$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native clean stop failed; support services preserved'}
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $final 'CharacterStore') -Recurse
$stopped=@()
foreach($name in 'Bishop','PhongThanAccountServer','Goddess','PhongThanRelay'){
 $path=Join-Path $iso.Server ($name+'.exe')
 foreach($service in @(Get-Process $name -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $path})){
  $stopped+=$service.Id;$service.Kill();[void]$service.WaitForExit(10000)
 }
}
$live=Get-Process -Id 49460
$liveHash=(Get-FileHash -LiteralPath (Join-Path $workspace 'PhongThanRuntime-Loose/Server/CoreServer.dll')).Hash
if($liveHash -ne 'BAC0354CC1C691F35801A73EB1067A555D6B8E031299DD52AA65DDE7A5D79907'){throw 'Live baseline mismatch'}
$receipt=@{WorldPid=$proc.Id;WorldExitCode=$proc.ExitCode;SupportPidsStopped=$stopped;LivePid=$live.Id;LiveStartUtc=$live.StartTime.ToUniversalTime().ToString('o');LiveCoreSha256=$liveHash;Utc=[DateTime]::UtcNow.ToString('o');Scope='P1_ISOLATED_ONLY';DataPreserved=$true}
[IO.File]::WriteAllText((Join-Path $report 'final_stop.json'),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
