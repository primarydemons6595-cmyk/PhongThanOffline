$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$resolved=[IO.Path]::GetFullPath($iso.Root)
if(!$resolved.StartsWith((Join-Path $workspace 'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_'),[StringComparison]::OrdinalIgnoreCase)){throw 'Invalid isolated root'}
$store=Join-Path $iso.Server 'CharacterStore'
if(@(Get-ChildItem -LiteralPath $store -File).Count){throw 'P1 character data exists: require clean save/shutdown, no process termination'}
$results=@()
foreach($role in @(@{Name='Game';Root=$iso.Client},@{Name='GameServer';Root=$iso.Server},@{Name='Bishop';Root=$iso.Server},@{Name='PhongThanRelay';Root=$iso.Server},@{Name='PhongThanAccountServer';Root=$iso.Server},@{Name='Goddess';Root=$iso.Server})){
 $expected=Join-Path $role.Root ($role.Name+'.exe')
 foreach($proc in @(Get-Process -Name $role.Name -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $expected})){
  if($proc.Id -eq 49460 -or !$proc.Path.StartsWith($resolved+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Protected process: refusing stop'}
  $method='TERMINATE_ISOLATED_EMPTY_CHARACTERSTORE'
  if($role.Name -eq 'GameServer'){
   try{$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)");try{[void]$event.Set();$method='NATIVE_STOP_EVENT'}finally{$event.Dispose()}}catch [Threading.WaitHandleCannotBeOpenedException]{}
   if($method -eq 'NATIVE_STOP_EVENT' -and !$proc.WaitForExit(30000)){throw 'P1 clean stop still pending; no forced stop'}
  }
  if($method -ne 'NATIVE_STOP_EVENT'){Stop-Process -Id $proc.Id -ErrorAction Stop}
  $results+=@{Name=$role.Name;Pid=$proc.Id;Path=$expected;Method=$method}
 }
}
[IO.File]::WriteAllText((Join-Path $report 'stop.json'),(@{Utc=[DateTime]::UtcNow.ToString('o');CharacterStoreFiles=0;Stopped=$results;LiveGameServerPid=49460}|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
'Stopped isolated P1 processes only; empty character data retained'
