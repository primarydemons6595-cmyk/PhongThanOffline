$ErrorActionPreference='Stop'
$source=Split-Path -Parent $PSScriptRoot
$runtime=Join-Path (Split-Path -Parent $source) 'PhongThanRuntime-Loose'
$live=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -in @('Game.exe','GameServer.exe','Bishop.exe','Goddess.exe','PhongThanAccountServer.exe','PhongThanRelay.exe')})
if($live){throw 'Loose runtime must be closed before publish'}
$studio=Join-Path $runtime 'ManagerStudio/Release/PhongThanStudio.exe'
if(!(Test-Path -LiteralPath $studio)){throw 'Publish Studio first'}
$stamp=Join-Path $runtime ('completion/NpcDropPublish_' + (Get-Date -Format yyyyMMdd_HHmmss))
New-Item -ItemType Directory -Path $stamp | Out-Null
$writes=@()
foreach($pair in @(@('Client','CoreClient.dll'),@('Server','CoreServer.dll'))){
    $from=Join-Path $source "Output/$($pair[0])/$($pair[1])"; $to=Join-Path $runtime "$($pair[0])/$($pair[1])"
    if(!(Test-Path -LiteralPath $from)){throw "Missing $from"}
    $backup=Join-Path $stamp "$($pair[0])_$($pair[1])"
    Copy-Item -LiteralPath $to -Destination $backup
    $writes += [pscustomobject]@{From=$from;To=$to;Backup=$backup;Before=(Get-FileHash $to).Hash;After=(Get-FileHash $from).Hash}
}
try{
    foreach($w in $writes){Copy-Item -LiteralPath $w.From -Destination $w.To -Force; if((Get-FileHash $w.To).Hash -ne $w.After){throw 'Binary publish hash mismatch'}}
    $p=Start-Process -FilePath $studio -ArgumentList @('--install-drop-policy',('"'+$runtime+'"')) -WindowStyle Hidden -PassThru -Wait
    if($p.ExitCode -ne 0){throw 'Policy install failed; inspect studio_start_error.log'}
    $writes | ConvertTo-Json | Set-Content -Encoding UTF8 (Join-Path $stamp 'published.json')
    Write-Output "NPC_DROP_PUBLISH_PASS backup=$stamp"
}catch{
    $failure=$_
    foreach($w in $writes){try{Copy-Item -LiteralPath $w.Backup -Destination $w.To -Force}catch{Write-Warning "Rollback failed: $($w.To). Backup retained: $stamp"}}
    throw $failure
}
