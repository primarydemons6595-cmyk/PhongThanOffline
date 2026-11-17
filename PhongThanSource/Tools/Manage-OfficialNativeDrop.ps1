param([ValidateSet('Deploy','On','Off','Rollback')][string]$Action='Off')
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace 'analysis/native_drop_official_20261018'
$settings=Join-Path $server 'settings/phongthan'
$latch=Join-Path $server 'native_drop_official_v1.required'
function Write-Atomic([string]$path,[byte[]]$bytes){
 [IO.File]::WriteAllBytes(($path+'.official.tmp'),$bytes)
 if(Test-Path -LiteralPath $path){[IO.File]::Replace(($path+'.official.tmp'),$path,($path+'.before-official'))}
 else{[IO.File]::Move(($path+'.official.tmp'),$path)}
}
if($Action -in @('On','Off')){
 if(!(Test-Path $latch)){throw 'Official release not deployed'}
 $mode=if($Action -eq 'On'){'OFFICIAL'}else{'OFF'};$enabled=if($Action -eq 'On'){'1'}else{'0'}
 foreach($pair in @(@('NativeDrop.ini','(?m)^Mode=[A-Z_]+\r?$',"Mode=$mode"),@('NativeDropBlue.ini','(?m)^Enabled=[01]\r?$',"Enabled=$enabled"))){
  $path=Join-Path $settings $pair[0];$text=[IO.File]::ReadAllText($path)
  Write-Atomic $path ([Text.Encoding]::ASCII.GetBytes([regex]::Replace($text,$pair[1],$pair[2])))
 }
 $recordPath=Join-Path $out 'deployment.json'
 if(Test-Path $recordPath){
  $record=Get-Content -Raw $recordPath|ConvertFrom-Json
  if($record.ActiveConfigHashes){foreach($name in @('NativeDrop.ini','NativeDropBlue.ini')){$record.ActiveConfigHashes.$name=(Get-FileHash (Join-Path $settings $name)).Hash}}
  [IO.File]::WriteAllText($recordPath,($record|ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))
 }
 Write-Output "Native mode=$mode. NpcDropPolicy stays disabled; persistence v1 stays active."
 exit
}
$receipt=Join-Path $out 'deployment.json'
if($Action -eq 'Deploy'){
 if(Test-Path $receipt){throw 'Already deployed'}
 if((Get-Content -Raw (Join-Path $out 'native_preflight.log')) -notmatch 'checks=16 failures=0'){throw 'Official native checks required'}
 $scope=Get-Content -Raw (Join-Path $out 'rollout_scope.json')|ConvertFrom-Json
 if((Get-FileHash (Join-Path $settings 'LegacyDropQuarantine.txt')).Hash -ine $scope.before_sha256){throw 'Quarantine changed after preflight'}
 $expected=(Get-FileHash (Join-Path $out 'backup/CoreServer.dll')).Hash
}else{
 $deployed=Get-Content -Raw $receipt|ConvertFrom-Json;$expected=$deployed.CoreServerSha256
 if($deployed.ActiveConfigHashes){foreach($property in $deployed.ActiveConfigHashes.PSObject.Properties){
  if((Get-FileHash (Join-Path $settings $property.Name)).Hash -ine $property.Value){throw "Configuration changed after rollout; review before rollback: $($property.Name)"}
 }}
}
if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ine $expected){throw 'Main binary changed; no overwrite'}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($world.Count -ne 1 -or $bishop.Count -ne 1){throw 'Exactly one Main world and Bishop required'}
$proc=$world[0];[void]$proc.Handle
$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native save/stop not confirmed; no forced kill'}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$saved=Join-Path $out ('saved_'+$stamp);New-Item -ItemType Directory -Path $saved|Out-Null
Copy-Item -LiteralPath (Join-Path $server 'CharacterStore') -Destination (Join-Path $saved 'CharacterStore') -Recurse
Copy-Item -LiteralPath (Join-Path $server 'NativeDropPersistenceV1') -Destination (Join-Path $saved 'NativeDropPersistenceV1') -Recurse
[IO.File]::WriteAllText((Join-Path $saved 'clean_stop.json'),(@{Pid=$proc.Id;ExitCode=$proc.ExitCode;Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json))
if($Action -eq 'Deploy'){
 Write-Atomic (Join-Path $server 'CoreServer.dll') ([IO.File]::ReadAllBytes((Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll')))
 foreach($name in @('NativeDrop.ini','NativeDropBlue.ini','NativeDropQuarantine.txt')){Write-Atomic (Join-Path $settings $name) ([IO.File]::ReadAllBytes((Join-Path $out $name)))}
 Write-Atomic (Join-Path $settings 'NpcDropPolicy.ini') ([Text.Encoding]::ASCII.GetBytes("; Retired by Loose Native Drop v1. No runtime fallback.`r`n[Main]`r`nVersion=1`r`nEnabled=0`r`n"))
 [IO.File]::WriteAllText($latch,"LOOSE_NATIVE_DROP_V1`n",[Text.Encoding]::ASCII)
}else{
 Write-Atomic (Join-Path $server 'CoreServer.dll') ([IO.File]::ReadAllBytes((Join-Path $out 'backup/CoreServer.dll')))
 foreach($name in @('LegacyDropExperiment.ini','LegacyDropQuarantine.txt','BlueMagicExperimental.ini','NpcDropPolicy.ini')){Write-Atomic (Join-Path $settings $name) ([IO.File]::ReadAllBytes((Join-Path $out ('backup/'+$name))))}
 Move-Item -LiteralPath $latch -Destination (Join-Path $saved 'official_latch.removed')
}
# Bishop is replaced only after the world has committed and exited.
Stop-Process -Id $bishop[0].Id
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $saved 'Bishop.stdout.log') -RedirectStandardError (Join-Path $saved 'Bishop.stderr.log')
 Start-Sleep -Seconds 2
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $saved 'GameServer.stdout.log') -RedirectStandardError (Join-Path $saved 'GameServer.stderr.log')
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$result=@{Action=$Action;GameServerPid=$next.Id;BishopPid=$nextBishop.Id;PreviousPid=$proc.Id;Utc=[DateTime]::UtcNow.ToString('o');CleanExitCode=0;State='WAIT_WORLD_READY';CoreServerSha256=(Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash;CompanionVersion=1;CharacterDataRestored=$false;CounterReset=$false;EvidenceDirectory=$saved;NormalEngine='LOOSE_NATIVE_DROP_V1';OldPolicy='NpcDropPolicy.ini DISABLED'}
$name=if($Action -eq 'Deploy'){'deployment.json'}else{'rollback.json'}
[IO.File]::WriteAllText((Join-Path $out $name),($result|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$result|ConvertTo-Json
