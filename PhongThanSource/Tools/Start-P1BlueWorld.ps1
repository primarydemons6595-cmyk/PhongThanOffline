$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$server=$iso.Server
if(!(Test-Path (Join-Path $server 'CharacterStore')) -or (Get-ChildItem (Join-Path $server 'CharacterStore') -File).Count){throw 'Initial isolated CharacterStore must be empty'}
$occupied=@(Get-NetTCPConnection -State Listen -ErrorAction Stop|Where-Object LocalPort -in $iso.Ports)
if($occupied.Count){throw 'Isolated ports occupied; refusing duplicate stack'}
Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll') -Destination (Join-Path $server 'CoreServer.dll')
Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Client/CoreClient.dll') -Destination (Join-Path $iso.Client 'CoreClient.dll')
$processes=@()
foreach($service in @(@{Name='Goddess.exe';Port=15001},@{Name='PhongThanAccountServer.exe';Port=15002},@{Name='PhongThanRelay.exe';Port=15003},@{Name='Bishop.exe';Port=15622},@{Name='GameServer.exe';Port=16666})){
 $oldP0=$env:PHONGTHAN_P0_MAGIC_ROOT;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT
 try{
  if($service.Name -eq 'GameServer.exe'){$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root}
  else{Remove-Item Env:PHONGTHAN_P0_MAGIC_ROOT -ErrorAction SilentlyContinue;Remove-Item Env:PHONGTHAN_P1_BLUE_ROOT -ErrorAction SilentlyContinue}
  $process=Start-Process -FilePath (Join-Path $server $service.Name) -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $report ($service.Name+'.stdout.log')) -RedirectStandardError (Join-Path $report ($service.Name+'.stderr.log'))
 }finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$oldP0;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1}
 $processes+=@{Name=$service.Name;Pid=$process.Id;Port=$service.Port;Path=(Join-Path $server $service.Name)}
 [IO.File]::WriteAllText((Join-Path $report 'processes.json'),($processes|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $deadline=[DateTime]::UtcNow.AddSeconds(25)
 do{
  $process.Refresh();if($process.HasExited){throw "Isolated $($service.Name) exited $($process.ExitCode)"}
  $listen=@(Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue|Where-Object {$_.OwningProcess -eq $process.Id -and $_.LocalPort -eq $service.Port})
  if($listen.Count){break};Start-Sleep -Milliseconds 300
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$listen.Count){throw "Isolated $($service.Name) did not listen on $($service.Port)"}
 Write-Output "$($service.Name) pid=$($process.Id) port=$($service.Port)"
}
