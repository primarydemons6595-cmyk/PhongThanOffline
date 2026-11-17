[CmdletBinding()]
param([ValidateSet('MAIN_CATALOG','MONSTER_EQUIPMENT_CATALOG')][string]$LevelRule='MONSTER_EQUIPMENT_CATALOG',
 [ValidateSet('SHADOW','LIVE_TEST')][string]$Mode='LIVE_TEST',[ValidateRange(1,100)][int]$Repetitions=100)
$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_equipment'
$game=@(Get-Process -Name GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
if($game.Count -ne 1){throw 'Exactly one Loose GameServer required'}
$ready=Join-Path $server 'native_world_ready.log'
if((Get-Item $ready).LastWriteTimeUtc -lt $game[0].StartTime.ToUniversalTime()){throw 'Wait for fresh WORLD_READY'}
$config=Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini'
$original=[IO.File]::ReadAllBytes($config)
$nonce='equipment18_'+[Guid]::NewGuid().ToString('N')
try{
 $text=[Text.Encoding]::ASCII.GetString($original)
 $text=[regex]::Replace($text,'(?m)^Mode=[^\r\n]*',"Mode=$Mode")
 $text=[regex]::Replace($text,'(?m)^SelfTestEnabled=[^\r\n]*','SelfTestEnabled=1')
 $text=[regex]::Replace($text,'(?m)^SelfTestEquipment18=[^\r\n]*','SelfTestEquipment18=1')
 $text=[regex]::Replace($text,'(?m)^SelfTestRepetitions=[^\r\n]*',"SelfTestRepetitions=$Repetitions")
 if($text -notmatch '(?m)^SelfTestEquipment18='){$text=$text.Replace('[Main]',"[Main]`nSelfTestEquipment18=1`nSelfTestRepetitions=$Repetitions")}
 foreach($key in @('SelfTestCustomScript','SelfTestGeneratorFailAfter')){$text=[regex]::Replace($text,"(?m)^$key=[^\r\n]*","$key=0")}
 $text=[regex]::Replace($text,'(?ms)(\[NPC_18\]\s*)(.*?)(?=\[NPC_|\z)',{param($m) $m.Groups[1].Value+[regex]::Replace($m.Groups[2].Value,'(?m)^LevelRule=[^\r\n]*',"LevelRule=$LevelRule")})
 [IO.File]::WriteAllText($config,$text,[Text.Encoding]::ASCII)
 [IO.File]::WriteAllText((Join-Path $server 'legacy_drop_selftest.request'),$nonce,[Text.Encoding]::ASCII)
 $deadline=[DateTime]::UtcNow.AddSeconds(55);$answer=''
 do{if(Test-Path (Join-Path $server 'legacy_drop_selftest.result')){$answer=[IO.File]::ReadAllText((Join-Path $server 'legacy_drop_selftest.result'));if($answer.Contains($nonce)){break}};Start-Sleep -Milliseconds 250}while([DateTime]::UtcNow -lt $deadline)
 if(!$answer.Contains($nonce)){throw 'Runtime did not complete requested self-test; NOT PASS'}
 $receipt=[ordered]@{Scope='REAL_GAMESERVER_AUTOMATED_UNOWNED';Nonce=$nonce;Pid=$game[0].Id;LevelRule=$LevelRule;Mode=$Mode;Repetitions=$Repetitions;ExpectedSpawns=(5*$Repetitions);Result=$answer.Trim();PlayerPickup='NOT_TESTED';Utc=[DateTime]::UtcNow.ToString('o')}
 [IO.File]::WriteAllText((Join-Path $report "runtime_${Mode}_${LevelRule}.json"),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false));$receipt|ConvertTo-Json
}finally{[IO.File]::WriteAllBytes($config,$original)}
