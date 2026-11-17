param([switch]$Rollback)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace 'analysis/native_drop_final_20261018/structural_repair'
$plan=Get-Content -Raw (Join-Path $out 'repair_plan.json') | ConvertFrom-Json
if(@(Get-Process GameServer -ErrorAction SilentlyContinue | Where-Object Path -IEQ (Join-Path $server 'GameServer.exe')).Count){throw 'Main world must be stopped; no forced stop'}
$qpath=Join-Path $server 'settings/phongthan/LegacyDropQuarantine.txt'
$qbackup=Join-Path $out 'backup/LegacyDropQuarantine.txt'
$receiptPath=Join-Path $out 'applied.json'
if(!$Rollback -and (Test-Path $receiptPath)){throw 'Repair already applied; inspect receipt'}
if($Rollback){
 $receipt=Get-Content -Raw $receiptPath | ConvertFrom-Json
 if((Get-FileHash $qpath).Hash -ine $receipt.quarantine_after_sha256){throw 'Quarantine changed after repair; refuse rollback'}
}else{
 foreach($entry in $plan.files){
  $log=Get-Content -Raw (Join-Path $out ($entry.file+'.native.log'))
  $expected="TABLE_REPAIR_RESULT count=$($entry.new_count) generated=$($entry.new_count) failed=0 spawn_death=0"
  if(!$log.Contains($expected)){throw "Native generation not verified: $($entry.file)"}
 }
 if(Test-Path $qbackup){if((Get-FileHash $qbackup).Hash -ne (Get-FileHash $qpath).Hash){throw 'Existing quarantine backup differs'}}else{Copy-Item -LiteralPath $qpath -Destination $qbackup}
}
# Verify every source and target before the first mutation.
foreach($entry in $plan.files){
 $target=Join-Path $server ('settings/item/droprate/'+$entry.file)
 $expected=if($Rollback){$entry.after_sha256}else{$entry.before_sha256}
 $folder=if($Rollback){'backup'}else{'candidates'}
 $source=Join-Path $out ($folder+'/'+$entry.file)
 $sourceHash=if($Rollback){$entry.before_sha256}else{$entry.after_sha256}
 if((Get-FileHash $target).Hash -ine $expected -or (Get-FileHash $source).Hash -ine $sourceHash){throw "Data changed; refuse overwrite: $($entry.file)"}
}
$beforeQ=[IO.File]::ReadAllBytes($qpath)
$beforeQHash=(Get-FileHash $qpath).Hash
if($Rollback){$afterQ=[IO.File]::ReadAllBytes($qbackup)}else{
 $text=[Text.Encoding]::ASCII.GetString($beforeQ)
 foreach($entry in $plan.files){
  $record='T \settings\item\droprate\'+$entry.file
  $pattern='(?m)^'+[regex]::Escape($record)+'\r?\n'
  if([regex]::Matches($text,$pattern).Count -ne 1){throw "Expected one T quarantine: $($entry.file)"}
  $text=[regex]::Replace($text,$pattern,'')
 }
 $afterQ=[Text.Encoding]::ASCII.GetBytes($text)
}
$done=@()
try{
 foreach($entry in $plan.files){
  $target=Join-Path $server ('settings/item/droprate/'+$entry.file)
  $folder=if($Rollback){'backup'}else{'candidates'}
  Copy-Item -LiteralPath (Join-Path $out ($folder+'/'+$entry.file)) -Destination ($target+'.structural.tmp')
  [IO.File]::Replace(($target+'.structural.tmp'),$target,($target+'.structural.previous'))
  $done+=,$entry
 }
 [IO.File]::WriteAllBytes(($qpath+'.structural.tmp'),$afterQ)
 [IO.File]::Replace(($qpath+'.structural.tmp'),$qpath,($qpath+'.structural.previous'))
}catch{
 foreach($entry in $done){
  $folder=if($Rollback){'candidates'}else{'backup'}
  Copy-Item -LiteralPath (Join-Path $out ($folder+'/'+$entry.file)) -Destination (Join-Path $server ('settings/item/droprate/'+$entry.file))
 }
 [IO.File]::WriteAllBytes($qpath,$beforeQ)
 throw
}
$result=@{Rollback=[bool]$Rollback;Utc=[DateTime]::UtcNow.ToString('o');quarantine_before_sha256=$beforeQHash;quarantine_after_sha256=(Get-FileHash $qpath).Hash;ReleasedTables=@($plan.files.file);NpcQuarantineChanged=$false;BinaryChanged=$false;CharacterDataChanged=$false;Policy=$plan.policy;KingsoftParity='UNVERIFIED';GeneratedRows=116;NativeFailures=0}
$file=if($Rollback){'rollback.json'}else{'applied.json'}
[IO.File]::WriteAllText((Join-Path $out $file),($result|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$result|ConvertTo-Json -Depth 5
