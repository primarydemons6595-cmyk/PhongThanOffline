# Start the existing Main stack without deployment, rollback or data resets.
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace ('analysis/native_drop_final_20261018/start_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))
if(!(Test-Path (Join-Path $server 'loose_only.flag'))){throw 'Main Loose marker required'}
$manifest=Get-Content -Raw (Join-Path $workspace 'analysis/native_drop_final_20261018/binary_manifest.json')|ConvertFrom-Json
foreach($entry in $manifest){if((Get-FileHash (Join-Path $workspace $entry.path)).Hash -ine $entry.sha256){throw "Binary differs: $($entry.path)"}}
foreach($name in @('Goddess.exe','PhongThanRelay.exe','PhongThanAccountServer.exe')){
 if(@(Get-CimInstance Win32_Process|Where-Object ExecutablePath -IEQ (Join-Path $server $name)).Count -ne 1){throw "Exactly one Main service required: $name"}
}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
if($world.Count){throw 'Main world already running; inspect instead of starting a duplicate'}
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($bishop.Count -gt 1){throw 'Multiple Main gateways'}
foreach($file in @('native_drop_persistence_v1.required','NativeDropPersistenceV1/rollout-v1.approved','NativeDropPersistenceV1/serial-reservation.v1')){
 if(!(Test-Path (Join-Path $server $file))){throw "Missing persistence admission: $file"}
}
$bytes=[IO.File]::ReadAllBytes((Join-Path $server 'NativeDropPersistenceV1/serial-reservation.v1'))
if($bytes.Length -ne 16){throw 'Invalid serial reservation length'}
$words=New-Object uint32[] 4;[Buffer]::BlockCopy($bytes,0,$words,0,16)
$check=$words[0] -bxor $words[1] -bxor $words[2] -bxor [uint32]::Parse('a55a5aa5',[Globalization.NumberStyles]::HexNumber)
if($words[0] -ne 0x3153444e -or $words[1] -ne 1 -or $words[3] -ne $check){throw 'Invalid serial reservation checksum/version'}
New-Item -ItemType Directory -Path $out|Out-Null
# Back up the currently stopped store as-is; never copy a backup into Main.
Copy-Item -LiteralPath (Join-Path $server 'CharacterStore') -Destination (Join-Path $out 'CharacterStore.backup') -Recurse
Copy-Item -LiteralPath (Join-Path $server 'NativeDropPersistenceV1') -Destination (Join-Path $out 'NativeDropPersistenceV1.backup') -Recurse
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 if(!$bishop.Count){
  $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out 'Bishop.stdout.log') -RedirectStandardError (Join-Path $out 'Bishop.stderr.log')
  Start-Sleep -Seconds 2
 }else{$nextBishop=$bishop[0]}
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out 'GameServer.stdout.log') -RedirectStandardError (Join-Path $out 'GameServer.stderr.log')
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$receipt=@{GameServerPid=$next.Id;BishopPid=$nextBishop.Id;StartedUtc=[DateTime]::UtcNow.ToString('o');State='WAIT_WORLD_READY';DataRestored=$false;SerialReservationBefore=$words[2];CompanionVersion=1;EvidenceDirectory=$out;BinarySha256=(Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash}
$json=$receipt|ConvertTo-Json
[IO.File]::WriteAllText((Join-Path $out 'start.json'),$json,[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText((Join-Path $workspace 'analysis/native_drop_final_20261018/latest_start.json'),$json,[Text.UTF8Encoding]::new($false))
$json
