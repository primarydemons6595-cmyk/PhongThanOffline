$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$list=@(Get-Content -Raw (Join-Path $report 'processes.json')|ConvertFrom-Json)
foreach($service in @(@{Name='Bishop.exe';Port=15622},@{Name='GameServer.exe';Port=16666})){
 $exe=Join-Path $iso.Server $service.Name
 if(@(Get-Process -Name ([IO.Path]::GetFileNameWithoutExtension($exe)) -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe}).Count){throw 'Isolated process already running'}
 if(@(Get-NetTCPConnection -State Listen|Where-Object LocalPort -eq $service.Port).Count){throw 'Test port occupied'}
 if($service.Name -eq 'Bishop.exe'){Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/Bishop.exe') -Destination $exe}
 $oldP0=$env:PHONGTHAN_P0_MAGIC_ROOT;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT
 try{
  if($service.Name -eq 'GameServer.exe'){$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root}
  $proc=Start-Process -FilePath $exe -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $report ($service.Name+'.stdout.log')) -RedirectStandardError (Join-Path $report ($service.Name+'.stderr.log'))
 }finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$oldP0;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1}
 $list=@($list|Where-Object Name -ne $service.Name)+@(@{Name=$service.Name;Pid=$proc.Id;Port=$service.Port;Path=$exe})
 [IO.File]::WriteAllText((Join-Path $report 'processes.json'),($list|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $deadline=[DateTime]::UtcNow.AddSeconds(25)
 do{$proc.Refresh();if($proc.HasExited){throw "Isolated $($service.Name) exited"};$listen=@(Get-NetTCPConnection -State Listen|Where-Object {$_.OwningProcess -eq $proc.Id -and $_.LocalPort -eq $service.Port});if($listen.Count){break};Start-Sleep -Milliseconds 300}while([DateTime]::UtcNow -lt $deadline)
 if(!$listen.Count){throw "Isolated port not listening: $($service.Port)"}
 Write-Output "$($service.Name) pid=$($proc.Id) port=$($service.Port)"
}
