[CmdletBinding()]
param([ValidateSet('SHADOW','LIVE_TEST')][string]$Mode='SHADOW',[int]$ReadyWaitSeconds=600,
 [ValidateSet('A','B')][string]$Profile='A',
 [ValidateSet('SUCCESS_CAP','REPEATED_TRIALS')][string]$TreasureRule='SUCCESS_CAP',
 [ValidateSet('MAIN_CATALOG','NPC_DIVISOR_CATALOG')][string]$LevelRule='MAIN_CATALOG',[switch]$CustomDropScript,[switch]$GeneratorFailure)
$ErrorActionPreference='Stop'
$source=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $source
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_live'
$config=Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini'
$game=@(Get-Process -Name GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
if($game.Count -ne 1){throw 'One real Loose GameServer required'}
$ready=Join-Path $server 'native_world_ready.log'
$readyDeadline=[DateTime]::UtcNow.AddSeconds($ReadyWaitSeconds)
$nextNotice=[DateTime]::UtcNow
do {
 $game[0].Refresh();if($game[0].HasExited){throw 'GameServer exited before READY; runtime test NOT PASS'}
 $fresh=(Test-Path -LiteralPath $ready) -and (Get-Item -LiteralPath $ready).LastWriteTimeUtc -ge $game[0].StartTime.ToUniversalTime()
 if($fresh){break}
 if([DateTime]::UtcNow -ge $nextNotice){Write-Output 'Waiting for fresh Loose WORLD_READY before enabling experimental mode...';$nextNotice=[DateTime]::UtcNow.AddSeconds(15)}
 Start-Sleep -Milliseconds 500
}while([DateTime]::UtcNow -lt $readyDeadline)
if(!$fresh){throw 'Cold startup still not READY; test mode was not enabled'}
$original=[IO.File]::ReadAllBytes($config)
$nonce='legacy_'+$Mode+'_'+[Guid]::NewGuid().ToString('N')
$result=Join-Path $server 'legacy_drop_selftest.result'
$request=Join-Path $server 'legacy_drop_selftest.request'
try {
 $text=[Text.Encoding]::ASCII.GetString($original)
 $text=[regex]::Replace($text,'(?m)^Mode=.*$',"Mode=$Mode")
 $text=[regex]::Replace($text,'(?m)^SelfTestEnabled=.*$','SelfTestEnabled=1')
 $text=[regex]::Replace($text,'(?m)^SelfTestCustomScript=[^\r\n]*',('SelfTestCustomScript='+[int]$CustomDropScript.IsPresent))
 $text=[regex]::Replace($text,'(?m)^SelfTestGeneratorFailAfter=[^\r\n]*',('SelfTestGeneratorFailAfter='+[int]$GeneratorFailure.IsPresent))
 $text=[regex]::Replace($text,'(?m)^HitDetails=.*$','HitDetails=1')
 $text=[regex]::Replace($text,'(?m)^Profile=[^\r\n]*',"Profile=$Profile")
 $text=[regex]::Replace($text,'(?m)^TreasureRule=[^\r\n]*',"TreasureRule=$TreasureRule")
 $text=[regex]::Replace($text,'(?m)^LevelRule=[^\r\n]*',"LevelRule=$LevelRule")
 [IO.File]::WriteAllText($config,$text,[Text.Encoding]::ASCII)
 [IO.File]::WriteAllText($request,$nonce,[Text.Encoding]::ASCII)
 $deadline=[DateTime]::UtcNow.AddSeconds(45)
 do{
  if(Test-Path -LiteralPath $result){$answer=[IO.File]::ReadAllText($result);if($answer.Contains($nonce)){break}}
  Start-Sleep -Milliseconds 250
 }while([DateTime]::UtcNow -lt $deadline)
 if(!$answer -or !$answer.Contains($nonce)){throw 'Real GameServer self-test did not acknowledge; runtime test NOT PASS'}
 $receipt=[ordered]@{Mode=$Mode;Profile=$Profile;TreasureRule=$TreasureRule;LevelRule=$LevelRule;CustomDropScript=$CustomDropScript.IsPresent;GeneratorFailure=$GeneratorFailure.IsPresent;Nonce=$nonce;ProcessId=$game[0].Id;Result=$answer.Trim();Pickup='NOT_TESTED';QuestCallbacks='SUPPRESSED_ON_ISOLATED_SELFTEST_NPCS_ONLY';Scope='REAL_GAMESERVER_SPAWN_DEATH_UNOWNED_GROUND'}
 $suffix=if($CustomDropScript){'_CUSTOM_SCRIPT'}elseif($GeneratorFailure){'_GENERATOR_FAILURE'}else{''}
 [IO.File]::WriteAllText((Join-Path $report "runtime_${Mode}_${Profile}_${TreasureRule}_${LevelRule}${suffix}.json"),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $receipt|ConvertTo-Json
}finally{[IO.File]::WriteAllBytes($config,$original)}
