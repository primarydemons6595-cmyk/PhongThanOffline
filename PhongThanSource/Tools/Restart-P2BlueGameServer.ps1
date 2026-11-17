param([switch]$DeployCore)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq (Join-Path $iso.Client 'Game.exe')}).Count){throw 'Logout and close isolated Client before restart'}
$exe=Join-Path $iso.Server 'GameServer.exe'
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe})
if($world.Count -ne 1){throw 'Exactly one isolated GameServer required'}
$evidence=Join-Path $report ('restart_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));New-Item -ItemType Directory -Path $evidence|Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $evidence 'CharacterStore.before') -Recurse
$proc=$world[0];[void]$proc.Handle;$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native clean stop failed; no force kill/restart'}
if($DeployCore){Copy-Item -LiteralPath (Join-Path $iso.Server 'CoreServer.dll') -Destination (Join-Path $evidence 'CoreServer.before.dll');Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll') -Destination (Join-Path $iso.Server 'CoreServer.dll')}
$oldP0=$env:PHONGTHAN_P0_MAGIC_ROOT;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT;$oldP2=$env:PHONGTHAN_P2_BLUE_ROOT
try{$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root;$env:PHONGTHAN_P2_BLUE_ROOT=$iso.Root;$next=Start-Process -FilePath $exe -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $evidence 'GameServer.stdout.log') -RedirectStandardError (Join-Path $evidence 'GameServer.stderr.log')}
finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$oldP0;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1;$env:PHONGTHAN_P2_BLUE_ROOT=$oldP2}
$receipt=@{PreviousPid=$proc.Id;NextPid=$next.Id;ExitCode=$proc.ExitCode;StartedUtc=$next.StartTime.ToUniversalTime().ToString('o');Scope='P2_ISOLATED_ONLY';GatewayReset=$false}
[IO.File]::WriteAllText((Join-Path $evidence 'restart.json'),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
$records=@(Get-Content -Raw (Join-Path $report 'processes.json')|ConvertFrom-Json)
$records=@($records|Where-Object Name -ne 'GameServer.exe')+@(@{Name='GameServer.exe';Pid=$next.Id;Port=16666;Path=$exe;StartUtc=$next.StartTime.ToUniversalTime().ToString('o')})
[IO.File]::WriteAllText((Join-Path $report 'processes.json'),(ConvertTo-Json -InputObject @($records) -Depth 5),[Text.UTF8Encoding]::new($false))
Write-Output 'Wait for a NEW native_world_ready.log timestamp and P2_BLUE_STATUS READY before starting the P2 Client.'
