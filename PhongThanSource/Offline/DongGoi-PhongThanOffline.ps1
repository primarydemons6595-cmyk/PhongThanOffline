[CmdletBinding()]
param(
    # Thu muc PhongThanRuntime-Loose (ban runtime dang chay duoc cua ban online)
    [string]$RuntimeRoot = (Join-Path (Split-Path -Parent $PSScriptRoot) '..\PhongThanRuntime-Loose'),
    # Thu muc game offline se duoc tao
    [string]$OutputDir = (Join-Path (Split-Path -Parent $PSScriptRoot) '..\PhongThanOffline-Play'),
    # Thu muc client nguon: mac dinh la Client ban Loose trong RuntimeRoot.
    # Ban Pak: chi vao thu muc client Pak (kieu Client_Original, co data\*.pak
    # va package.ini liet ke pak o thu muc goc) - ServerData van lay ban Loose.
    [string]$ClientRoot = '',
    # Thu muc du lieu server nguon: mac dinh la Server ban Loose trong
    # RuntimeRoot. Ban phat hanh bao mat: chi vao Server Base (co
    # Data\release.catalog va cac pak server da ky) - ServerData se doc
    # catalog phia server, khong con file roi.
    [string]$ServerRoot = ''
)

$ErrorActionPreference = 'Stop'

$RuntimeRoot = [IO.Path]::GetFullPath($RuntimeRoot)
$OutputDir = [IO.Path]::GetFullPath($OutputDir)
$clientSrc = if ($ClientRoot) { [IO.Path]::GetFullPath($ClientRoot) } else { Join-Path $RuntimeRoot 'Client' }
$serverSrc = if ($ServerRoot) { [IO.Path]::GetFullPath($ServerRoot) } else { Join-Path $RuntimeRoot 'Server' }

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

# 3b) Co loose_only.flag quyet dinh che do du lieu phia server:
# - ServerData la Server Base (co Data\release.catalog): KHONG duoc co co
#   nay - co no Engine se tat che do catalog phia server va doc sai cho.
# - ServerData Loose: bat buoc co co nay de server doc file roi.
if (Test-Path -LiteralPath (Join-Path $serverData 'Data\release.catalog')) {
    Remove-Item -LiteralPath (Join-Path $serverData 'loose_only.flag') -Force -ErrorAction SilentlyContinue
} else {
    $looseFlag = Join-Path $serverData 'loose_only.flag'
    if (-not (Test-Path -LiteralPath $looseFlag)) { Set-Content -LiteralPath $looseFlag -Value 'LOOSE_ONLY' -Encoding ASCII }
}

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
