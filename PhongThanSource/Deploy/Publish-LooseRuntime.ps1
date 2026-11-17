[CmdletBinding()]
param([string]$ProjectRoot,[string]$RuntimeRoot)
$ErrorActionPreference='Stop'
if (-not $ProjectRoot) { $ProjectRoot=Split-Path -Parent $PSScriptRoot }
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$workspace=Split-Path -Parent $ProjectRoot
$expected=Join-Path $workspace 'PhongThanRuntime-Loose'
if (-not $RuntimeRoot) { $RuntimeRoot=$expected }
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
if ($RuntimeRoot -ine $expected) { throw 'Publisher accepts only PhongThanRuntime-Loose' }
$staging=Join-Path $workspace 'PhongThanRuntime-Staging'
$running=@(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.StartsWith($RuntimeRoot+'\',[StringComparison]::OrdinalIgnoreCase)
})
if ($running) { throw 'Close the loose runtime before publishing' }
$manifest=Get-Content (Join-Path $PSScriptRoot 'RUNTIME_CONTENT_MANIFEST.json') -Raw | ConvertFrom-Json
$published=New-Object Collections.Generic.List[object]
foreach ($role in 'Client','Server') {
    $targetRoot=Join-Path $RuntimeRoot $role
    foreach ($required in 'loose_only.flag','loose_index.bin') {
        if (!(Test-Path -LiteralPath (Join-Path $targetRoot $required) -PathType Leaf)) { throw "Missing $role/$required" }
    }
    $pakFiles=@(Get-ChildItem -LiteralPath $targetRoot -File -Recurse -Filter '*.pak')
    if ($pakFiles.Count) { throw "Unexpected PAK in loose tree $role; no files were deleted" }
    $artifacts=@($manifest.Roles.$role.ArtifactFiles)
    if ($role -eq 'Server') {
        $artifacts=@($artifacts | Where-Object { $_ -notin 'S3Relay.exe','PhongThanRelayServer.exe' }) + 'PhongThanRelay.exe'
    }
    foreach ($name in $artifacts) {
        $source=Join-Path $ProjectRoot "Output\$role\$name"
        $target=Join-Path $targetRoot $name
        if (!(Test-Path -LiteralPath $source -PathType Leaf)) { throw "Missing build output $role/$name" }
        Copy-Item -LiteralPath $source -Destination $target -Force
        $hash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
        if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $hash) { throw "Artifact hash mismatch $role/$name" }
        $published.Add([pscustomobject]@{Role=$role;Name=$name;Sha256=$hash})
    }
    # Runtime configuration/support DLLs only. Never copy staging PAK/content,
    # logs, cached sessions or overwrite existing loose data/preferences.
    $support=@($manifest.Roles.$role.ContentFiles | Where-Object { $_ -ine 'package.ini' })
    if ($role -eq 'Server') { $support+=@('DataBase.ini','PhongThanRelay.ini') }
    foreach ($name in $support) {
        $source=Join-Path $staging "$role\$name"
        $target=Join-Path $targetRoot $name
        if (!(Test-Path -LiteralPath $target) -and (Test-Path -LiteralPath $source -PathType Leaf)) {
            Copy-Item -LiteralPath $source -Destination $target
            if ((Get-FileHash $source).Hash -ne (Get-FileHash $target).Hash) { throw "Support hash mismatch $role/$name" }
        }
    }
    # Kept for old callers, but there is no package chain to mount.
    [IO.File]::WriteAllText((Join-Path $targetRoot 'package.ini'),"[Package]`r`nMode=LOOSE_ONLY`r`n",[Text.Encoding]::ASCII)
}
foreach($role in 'Client','Server') {
    & (Join-Path $PSScriptRoot 'Publish-GiapSiQuestContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
    & (Join-Path $PSScriptRoot 'Publish-LifeSkillContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
    & (Join-Path $PSScriptRoot 'Publish-RealmUiKhulaiContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
    & (Join-Path $PSScriptRoot 'Publish-BotAutonomyContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
    & (Join-Path $PSScriptRoot 'Publish-ThanKyPresentationContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
    & (Join-Path $PSScriptRoot 'Publish-BotProgressionContent.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role) | Out-Null
}
$saveTarget=Join-Path $RuntimeRoot 'Server\CharacterStore'
if (!(Test-Path -LiteralPath $saveTarget)) {
    $liveWorld=@(Get-CimInstance Win32_Process | Where-Object { $_.Name -eq 'GameServer.exe' })
    if ($liveWorld) { throw 'Cannot snapshot saves while GameServer is running; binaries published, snapshot pending' }
    $saveSource=Join-Path $workspace 'PhongThanRuntime-State\Server\CharacterStore'
    if (!(Test-Path -LiteralPath $saveSource -PathType Container)) { throw 'Persistent save source missing' }
    Copy-Item -LiteralPath $saveSource -Destination $saveTarget -Recurse
}
if ((Get-Item -LiteralPath $saveTarget).Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw 'Loose CharacterStore must be an independent snapshot, not a junction'
}
foreach ($name in @('database','databasetong','databasetongmember','dbfriend','TongDB','TongMemberDB','datascript')) {
    $source=Join-Path $staging "Server\$name"
    $target=Join-Path $RuntimeRoot "Server\$name"
    if (!(Test-Path -LiteralPath $target) -and (Test-Path -LiteralPath $source -PathType Container)) {
        Copy-Item -LiteralPath $source -Destination $target -Recurse
    }
}
$receipt=[pscustomobject]@{Mode='NAMED_LOOSE_ONLY';PakFallback=$false;PublishedAtUtc=[DateTime]::UtcNow.ToString('o');
    Artifacts=$published.ToArray();CharacterStore='INDEPENDENT_SNAPSHOT';AccountDatabase='EXISTING_LOCAL_ACCOUNT_DB';GameVerified=$false}
[IO.File]::WriteAllText((Join-Path $RuntimeRoot 'LOOSE_DEPLOYMENT.json'),($receipt | ConvertTo-Json -Depth 6),(New-Object Text.UTF8Encoding($false)))
[pscustomobject]@{Status='PASS';Artifacts=$published.Count;RuntimeRoot=$RuntimeRoot;PakFiles=0;CharacterStore='INDEPENDENT_SNAPSHOT'}
