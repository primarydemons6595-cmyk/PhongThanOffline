$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
if(!(Test-Path (Join-Path $iso.Root 'p2_player_test.flag'))){throw 'P2 marker required'}
if(@(Get-NetTCPConnection -State Listen|Where-Object LocalPort -in $iso.Ports).Count){throw 'Test ports occupied'}
$processes=@()
foreach($service in @(@{Name='Goddess.exe';Port=15001},@{Name='PhongThanAccountServer.exe';Port=15002},@{Name='PhongThanRelay.exe';Port=15003},@{Name='Bishop.exe';Port=15622},@{Name='GameServer.exe';Port=16666})){
 $oldP0=$env:PHONGTHAN_P0_MAGIC_ROOT;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT;$oldP2=$env:PHONGTHAN_P2_BLUE_ROOT
 try{
  if($service.Name -eq 'GameServer.exe'){$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root;$env:PHONGTHAN_P2_BLUE_ROOT=$iso.Root}
  else{$env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null}
  $proc=Start-Process -FilePath (Join-Path $iso.Server $service.Name) -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $report ($service.Name+'.stdout.log')) -RedirectStandardError (Join-Path $report ($service.Name+'.stderr.log'))
 }finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$oldP0;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1;$env:PHONGTHAN_P2_BLUE_ROOT=$oldP2}
 $processes+=@{Name=$service.Name;Pid=$proc.Id;Port=$service.Port;Path=$proc.Path;StartUtc=$proc.StartTime.ToUniversalTime().ToString('o')}
 [IO.File]::WriteAllText((Join-Path $report 'processes.json'),($processes|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $deadline=[DateTime]::UtcNow.AddSeconds(20)
 do{$proc.Refresh();if($proc.HasExited){throw "Isolated service exited: $($service.Name)"};$listen=@(Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue|Where-Object {$_.OwningProcess -eq $proc.Id -and $_.LocalPort -eq $service.Port});if($listen.Count){break};Start-Sleep -Milliseconds 300}while([DateTime]::UtcNow -lt $deadline)
 if(!$listen.Count -and $service.Name -ne 'GameServer.exe'){throw "Port not ready: $($service.Name)"}
 Write-Output "$($service.Name) pid=$($proc.Id)"
}
