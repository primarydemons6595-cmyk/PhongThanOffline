[CmdletBinding()]
param(
    # Thu muc PhongThanRuntime-Loose (ban runtime dang chay duoc cua ban online)
    [string]$RuntimeRoot = (Join-Path (Split-Path -Parent $PSScriptRoot) '..\PhongThanRuntime-Loose'),
    # Thu muc game offline se duoc tao
    [string]$OutputDir = (Join-Path (Split-Path -Parent $PSScriptRoot) '..\PhongThanOffline-Play'),
    # Thu muc client nguon: mac dinh la Client ban Loose trong RuntimeRoot.
    # Ban Pak: chi vao thu muc client Pak (kieu Client_Original, co data\*.pak
    # va package.ini liet ke pak o thu muc goc) - ServerData van lay ban Loose.
    [string]$ClientRoot = ''
)

$ErrorActionPreference = 'Stop'

$RuntimeRoot = [IO.Path]::GetFullPath($RuntimeRoot)
$OutputDir = [IO.Path]::GetFullPath($OutputDir)
$clientSrc = if ($ClientRoot) { [IO.Path]::GetFullPath($ClientRoot) } else { Join-Path $RuntimeRoot 'Client' }
$serverSrc = Join-Path $RuntimeRoot 'Server'

if (-not (Test-Path -LiteralPath (Join-Path $clientSrc 'Game.exe'))) {
    throw "Khong thay Game.exe trong $clientSrc. Hay build target GameClient truoc (Build\Build-PhongThan.ps1 -Targets Common,CoreServer,GameClient)."
}
if (-not (Test-Path -LiteralPath (Join-Path $serverSrc 'ServerCfg.ini'))) {
    throw "Khong thay du lieu server trong $serverSrc."
}

Write-Host "Nguon runtime : $RuntimeRoot"
Write-Host "Thu muc game  : $OutputDir"

# 1) Client -> thu muc game
robocopy $clientSrc $OutputDir /E /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -ge 8) { throw "Robocopy Client that bai ($LASTEXITCODE)" }

# 2) Server -> ServerData (du lieu cho server nhung trong client)
$serverData = Join-Path $OutputDir 'ServerData'
robocopy $serverSrc $serverData /E /NFL /NDL /NJH /NJS | Out-Null
if ($LASTEXITCODE -ge 8) { throw "Robocopy Server that bai ($LASTEXITCODE)" }

# 3) Bat MapSleep: map khong co nguoi se ngu, chi map dang choi moi chay
$mapSleep = Join-Path $serverData 'MapSleep.ini'
if (Test-Path -LiteralPath $mapSleep) {
    $text = [IO.File]::ReadAllText($mapSleep)
    $text = $text -replace '(?m)^Enabled\s*=\s*0', 'Enabled=1'
    [IO.File]::WriteAllText($mapSleep, $text)
}

# 3b) ServerData bat buoc co loose_only.flag: server nhung ghim che do du lieu
# theo co nay (doc file roi trong ServerData), ke ca khi nua client la ban
# phat hanh bao mat doc catalog. Thieu co nay server se tra du lieu sai cho.
$looseFlag = Join-Path $serverData 'loose_only.flag'
if (-not (Test-Path -LiteralPath $looseFlag)) { Set-Content -LiteralPath $looseFlag -Value 'LOOSE_ONLY' -Encoding ASCII }

# 4) Thu muc chua nhan vat (moi nhan vat 1 file .pthc trong CharacterStore\<tai khoan>\)
$store = Join-Path $serverData 'CharacterStore'
if (-not (Test-Path -LiteralPath $store)) { New-Item -ItemType Directory -Path $store | Out-Null }

# 5) Bat che do offline trong config.ini cua client
$configPath = Join-Path $OutputDir 'config.ini'
$config = if (Test-Path -LiteralPath $configPath) { [IO.File]::ReadAllText($configPath) } else { '' }
if ($config -match '(?ms)\[Offline\].*?Enabled\s*=\s*\d+') {
    $config = $config -replace '(?ms)(\[Offline\].*?Enabled\s*=\s*)\d+', '${1}1'
} elseif ($config -match '(?m)^\[Offline\]') {
    $config = $config -replace '(?m)^\[Offline\]', "[Offline]`r`nEnabled=1"
} else {
    $config = $config.TrimEnd() + "`r`n`r`n[Offline]`r`nEnabled=1`r`n"
}
[IO.File]::WriteAllText($configPath, $config)

# 6) Launcher
$bat = "@echo off`r`ncd /d %~dp0`r`nstart `"`" Game.exe`r`n"
[IO.File]::WriteAllText((Join-Path $OutputDir 'Choi-PhongThan-Offline.bat'), $bat)

Write-Host ''
Write-Host 'XONG. Mo file Choi-PhongThan-Offline.bat trong thu muc game de choi.'
Write-Host "Nhan vat luu tai: $store"
