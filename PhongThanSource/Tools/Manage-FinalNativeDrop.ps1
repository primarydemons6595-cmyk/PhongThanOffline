param([ValidateSet('Deploy','Off','On','RollbackBinary')][string]$Action='Off')
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
if(Test-Path (Join-Path $workspace 'PhongThanRuntime-Loose/Server/native_drop_official_v1.required')){
 if($Action -eq 'On' -or $Action -eq 'Off'){
  & (Join-Path $PSScriptRoot 'Manage-OfficialNativeDrop.ps1') -Action $Action
  exit
 }
 throw 'Loose Native Drop v1 is official. Use Manage-OfficialNativeDrop.ps1; legacy deployment/downgrade is retired.'
}
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace 'analysis/native_drop_final_20261018'
$backup=Join-Path $out 'backup'
$magic=Join-Path $server 'NativeDropPersistenceV1'
$experiment=Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini'
$blue=Join-Path $server 'settings/phongthan/BlueMagicExperimental.ini'
function Replace-Bytes([string]$Path,[byte[]]$Data){$temp=$Path+'.final.tmp';[IO.File]::WriteAllBytes($temp,$Data);[IO.File]::Replace($temp,$Path,$Path+'.pre-final')}
function Text([string]$Path){[Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes($Path))}
function Write-Legacy([string]$Path,[string]$Value){Replace-Bytes $Path ([Text.Encoding]::GetEncoding(28591).GetBytes($Value))}
if($Action -eq 'Off' -or $Action -eq 'On'){
 if(!(Test-Path (Join-Path $out 'deployment.json'))){throw 'Deploy first'}
 $enabled=if($Action -eq 'On'){'1'}else{'0'};$mode=if($Action -eq 'On'){'LIVE_TEST'}else{'OFF'}
 Write-Legacy $blue ([regex]::Replace((Text $blue),'(?m)^Enabled=[01]\s*$',('Enabled='+$enabled)))
 Write-Legacy $experiment ([regex]::Replace((Text $experiment),'(?m)^Mode=[A-Z_]+\s*$',('Mode='+$mode)))
 Write-Output "Drop mode=$mode, blue=$enabled; companion v1 remains active. No restart or character rollback."
 exit
}
if($Action -eq 'Deploy'){
 if(Test-Path (Join-Path $out 'deployment.json')){throw 'Already deployed'}
 if(Test-Path $magic){throw 'Existing main companion root: inspect instead of reinitializing'}
 if((Get-Content -Raw (Join-Path $out 'native_validation.log')) -notmatch 'native_plans_ready=12 blocked=0'){throw 'Native profile validation required'}
 if((Get-Content -Raw (Join-Path $out 'native_validation.log')) -notmatch 'CATEGORY_RESULT checks=24 failures=0'){throw 'Native category validation required'}
 if((Get-Content -Raw (Join-Path $out 'policy_tests.log')) -notmatch 'FINAL_POLICY_RESULT checks=15 failed=0'){throw 'Boundary tests required'}
 if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ne (Get-FileHash (Join-Path $backup 'Server/CoreServer.dll')).Hash){throw 'Main binary changed since backup'}
}else{
 if(!(Test-Path (Join-Path $out 'deployment.json'))){throw 'No deployment'}
 if(@(Get-ChildItem -LiteralPath $magic -Filter '*.managed-v1' -File -ErrorAction SilentlyContinue).Count){throw 'Companion-managed roles exist: old binary cannot preserve new magic. Use -Action Off; retain v1-capable binary and companions.'}
 $deployed=Get-Content -Raw (Join-Path $out 'deployment.json')|ConvertFrom-Json
 if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ne $deployed.CoreServerSha256){throw 'Server binary changed after rollout; refuse downgrade'}
 $client=Join-Path $workspace 'PhongThanRuntime-Loose/Client'
 if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $client 'Game.exe')).Count){throw 'Save/logout and close Main Client before binary rollback; use Off for immediate hot rollback'}
 if(Test-Path (Join-Path $out 'client_deployment.json')){
  $clientReceipt=Get-Content -Raw (Join-Path $out 'client_deployment.json')|ConvertFrom-Json
  if((Get-FileHash (Join-Path $client 'CoreClient.dll')).Hash -ne $clientReceipt.CoreClientSha256){throw 'Client binary changed after rollout'}
 }
}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
if($world.Count -eq 1){
 $proc=$world[0];[void]$proc.Handle
 $event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
 try{[void]$event.Set()}finally{$event.Dispose()}
 if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native world save/stop not confirmed; no forced stop'}
}elseif($world.Count -eq 0 -and $Action -eq 'Deploy' -and (Test-Path (Join-Path $out 'clean_stop.json'))){
 $stopped=Get-Content -Raw (Join-Path $out 'clean_stop.json')|ConvertFrom-Json
 if($stopped.ExitCode -ne 0){throw 'No verified clean stop'}
 $proc=[pscustomobject]@{Id=$stopped.Pid;ExitCode=0}
}else{throw 'Exactly one main GameServer or verified saved/stopped deployment required'}
[IO.File]::WriteAllText((Join-Path $out 'clean_stop.json'),(@{Pid=$proc.Id;ExitCode=$proc.ExitCode;Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json))
Copy-Item -LiteralPath (Join-Path $server 'CharacterStore') -Destination (Join-Path $backup ('CharacterStore.saved_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))) -Recurse
if($Action -eq 'Deploy'){
 $reuseCopy=$false
 if((Test-Path (Join-Path $out 'saved_copy.json')) -and (Test-Path (Join-Path $out 'saved_copy_write.log')) -and (Test-Path (Join-Path $out 'saved_copy_read.log'))){
  $prior=Get-Content -Raw (Join-Path $out 'saved_copy.json')|ConvertFrom-Json
  $testedStamp=[IO.Path]::GetFileName($prior.Root).Replace('final_saved_copy_','')
  $lastBackup=Get-Item -LiteralPath (Join-Path $backup ('CharacterStore.saved_'+$testedStamp)) -ErrorAction Stop
  $reuseCopy=$prior.Status -eq 'COPY_COMPATIBILITY_PASS' -and (Get-Content -Raw (Join-Path $out 'saved_copy_write.log')) -match 'checks=23 failures=0' -and (Get-Content -Raw (Join-Path $out 'saved_copy_read.log')) -match 'checks=47 failures=0'
  foreach($file in Get-ChildItem -LiteralPath (Join-Path $server 'CharacterStore') -File){$previous=Join-Path $lastBackup.FullName $file.Name;if(!(Test-Path $previous) -or (Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $previous).Hash){$reuseCopy=$false}}
 }
 if(!$reuseCopy){
 $copy=Join-Path $workspace ('PhongThanRuntime-Loose/P0-BlueMagic-Isolated/final_saved_copy_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))
 New-Item -ItemType Directory -Path $copy|Out-Null
 [IO.File]::WriteAllText((Join-Path $copy 'p0_isolated.flag'),'COPY COMPATIBILITY ONLY; NO WORLD')
 Copy-Item -LiteralPath (Join-Path $server 'CharacterStore') -Destination (Join-Path $copy 'CharacterStore') -Recurse
 $env:PATH="$server;$env:PATH"
 foreach($operation in 'write','read'){
  & (Join-Path $out 'FinalMainStoreCopyTests.exe') $operation $server $copy > (Join-Path $out ('saved_copy_'+$operation+'.log'))
  if($LASTEXITCODE){throw "Saved CharacterStore copy test failed: $operation. Main data untouched, world stopped; do not grant rollout."}
 }
 $receipt=@{Root=$copy;Status='COPY_COMPATIBILITY_PASS';WorldOrClientStarted=$false;MainCharacterDataModified=$false;CompanionVersion=1}
 [IO.File]::WriteAllText((Join-Path $out 'saved_copy.json'),($receipt|ConvertTo-Json))
 }else{Write-Output 'Reusing successful stopped-store copy tests; all source store files still match the saved backup.'}
 $source=Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll'
 Replace-Bytes (Join-Path $server 'CoreServer.dll') ([IO.File]::ReadAllBytes($source))
 if((Get-FileHash $source).Hash -ne (Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash){throw 'Binary publish hash mismatch'}
 $settings=Text $experiment
 $settings=[regex]::Replace($settings,'(?m)^LevelRule=MONSTER_EQUIPMENT_CATALOG\r?$',"LevelRule=MONSTER_EQUIPMENT_CATALOG`r`nEquipmentMaxRequirementAhead=10")
 if(([regex]::Matches($settings,'EquipmentMaxRequirementAhead=10')).Count -ne 6){throw 'Exactly six approved NPC policies required'}
 Write-Legacy $experiment $settings
 Write-Legacy $blue ([regex]::Replace((Text $blue),'(?m)^Enabled=0\r?$','Enabled=1'))
 New-Item -ItemType Directory -Path $magic|Out-Null
 [IO.File]::WriteAllText((Join-Path $server 'native_drop_persistence_v1.required'),"PERSISTENCE_V1_REQUIRED`n",[Text.Encoding]::ASCII)
 $serial=[uint32[]]@(0x3153444e,1,100,0)
 $serial[3]=$serial[0] -bxor $serial[1] -bxor $serial[2] -bxor [uint32]::Parse('a55a5aa5',[Globalization.NumberStyles]::HexNumber)
 $serialBytes=New-Object byte[] 16;[Buffer]::BlockCopy($serial,0,$serialBytes,0,16)
 [IO.File]::WriteAllBytes((Join-Path $magic 'serial-reservation.v1'),$serialBytes)
 [IO.File]::WriteAllText((Join-Path $magic 'rollout-v1.approved'),"NATIVE_DROP_MAIN_LOOSE_V1`nCompanionVersion=1`nCOPY_COMPATIBILITY_TESTS=PASS`n",[Text.Encoding]::ASCII)
}else{
 Replace-Bytes (Join-Path $server 'CoreServer.dll') ([IO.File]::ReadAllBytes((Join-Path $backup 'Server/CoreServer.dll')))
 if(Test-Path (Join-Path $backup 'Client/CoreClient.dll')){Replace-Bytes (Join-Path $client 'CoreClient.dll') ([IO.File]::ReadAllBytes((Join-Path $backup 'Client/CoreClient.dll')))}
 foreach($name in 'LegacyDropExperiment.ini','BlueMagicExperimental.ini'){Replace-Bytes (Join-Path $server ('settings/phongthan/'+$name)) ([IO.File]::ReadAllBytes((Join-Path $backup ('Server/settings/phongthan/'+$name))))}
 # Empty rollout data only; preserve the approval and index as rollback evidence.
 if(Test-Path $magic){
  $resolvedMagic=(Resolve-Path -LiteralPath $magic).Path
  $destination=[IO.Path]::GetFullPath((Join-Path $out ('UnusedCompanionRoot_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))))
  if($resolvedMagic -ine (Join-Path $server 'NativeDropPersistenceV1') -or !$destination.StartsWith($out+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Rollback move target outside explicit workspace paths'}
  Move-Item -LiteralPath $resolvedMagic -Destination $destination
  $latch=Join-Path $server 'native_drop_persistence_v1.required'
  if(Test-Path $latch){Move-Item -LiteralPath $latch -Destination (Join-Path $out ('UnusedPersistenceLatch_'+(Get-Date -Format 'yyyyMMdd_HHmmss')))}
 }
}
$bishops=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($bishops.Count -ne 1){throw 'Exactly one main Bishop required'}
[void]$bishops[0].Handle
Stop-Process -Id $bishops[0].Id
if(!$bishops[0].WaitForExit(10000)){throw 'Bishop still running after clean world stop'}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 $bishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.Bishop.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.Bishop.stderr.log")
 Start-Sleep -Seconds 2
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.GameServer.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.GameServer.stderr.log")
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$receipt=@{Action=$Action;GameServerPid=$next.Id;BishopPid=$bishop.Id;PreviousGameServerPid=$proc.Id;CleanExitCode=$proc.ExitCode;CoreServerSha256=(Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash;Utc=[DateTime]::UtcNow.ToString('o');CompanionVersion=1;BlueChancePerMillion=100000;AwaitingPlayerGameplay=$true;Status='WAIT_WORLD_READY';MainCharacterDataRestored=$false}
$name=if($Action -eq 'Deploy'){'deployment.json'}else{'rollback.json'}
[IO.File]::WriteAllText((Join-Path $out $name),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
