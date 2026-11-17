[CmdletBinding()]
param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot),[string]$RuntimeRoot)
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
if(-not $RuntimeRoot){$RuntimeRoot=Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging'}
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
if($RuntimeRoot -ine (Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging')){throw 'Publish target must be the named staging runtime.'}
$running=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RuntimeRoot+'\',[StringComparison]::OrdinalIgnoreCase)})
if($running){throw "Runtime is running: $($running.Name -join ', ')"}
$manifest=Get-Content (Join-Path $PSScriptRoot 'RUNTIME_CONTENT_MANIFEST.json') -Raw | ConvertFrom-Json
$artifacts=[Collections.Generic.List[object]]::new()
foreach($role in 'Client','Server'){
    $target=Join-Path $RuntimeRoot $role
    if(-not(Test-Path -LiteralPath (Join-Path $target 'settings'))){throw "Missing existing VNG content: $target"}
    $names=@($manifest.Roles.$role.ArtifactFiles)
    if($role -eq 'Server'){$names=@($names | Where-Object {$_ -notin @('S3Relay.exe','PhongThanRelayServer.exe')})+@('PhongThanRelay.exe')}
    foreach($name in $names){
        $source=Join-Path $ProjectRoot "Output\$role\$name"
        if(-not(Test-Path -LiteralPath $source)){throw "Missing build: $source"}
        $destination=Join-Path $target $name
        Copy-Item -LiteralPath $source -Destination $destination -Force
        $hash=(Get-FileHash -LiteralPath $source).Hash
        if((Get-FileHash -LiteralPath $destination).Hash -ne $hash){throw "Binary mismatch: $destination"}
        $artifacts.Add([pscustomobject]@{Role=$role;Name=$name;Sha256=$hash})
    }
}
& (Join-Path $PSScriptRoot 'Publish-VngGameplaySettings.ps1') `
    -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot | Out-Null
& (Join-Path $PSScriptRoot 'Publish-UiInputContent.ps1') `
    -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot | Out-Null
& (Join-Path $ProjectRoot 'Tests\Test-VngGameplaySettings.ps1') `
    -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot | Out-Null
& (Join-Path $ProjectRoot 'Tests\Test-EscDialogAvailability.ps1') `
    -ProjectRoot $ProjectRoot | Out-Null
$configs=[Collections.Generic.List[object]]::new()
foreach($consumableRole in 'Client','Server') {
    foreach($consumableName in 'ConsumableEffects.txt','ConsumableEffects.provenance.json') {
        $consumableTarget=Join-Path $RuntimeRoot "$consumableRole\settings\phongthan\$consumableName"
        New-Item -ItemType Directory -Force (Split-Path -Parent $consumableTarget) | Out-Null
        Copy-Item -LiteralPath (Join-Path $PSScriptRoot "ProjectContent\settings\phongthan\$consumableName") -Destination $consumableTarget -Force
    }
}
foreach($role in 'Client','Server') {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'ProjectContent\settings\item\PhongThanHorseVisual.txt') -Destination (Join-Path $RuntimeRoot "$role\settings\item\PhongThanHorseVisual.txt") -Force
}
$gateScriptSource=Join-Path $PSScriptRoot 'ProjectContent\script\phongthan\gate\supertrap.lua'
$gateScriptTarget=Join-Path $RuntimeRoot 'Server\script\phongthan\gate\supertrap.lua'
if(-not(Test-Path -LiteralPath $gateScriptSource -PathType Leaf)){throw "Missing recovered gate script: $gateScriptSource"}
New-Item -ItemType Directory -Path (Split-Path -Parent $gateScriptTarget) -Force | Out-Null
Copy-Item -LiteralPath $gateScriptSource -Destination $gateScriptTarget -Force
if((Get-FileHash -LiteralPath $gateScriptSource).Hash -ne (Get-FileHash -LiteralPath $gateScriptTarget).Hash){throw "Gate script mismatch: $gateScriptTarget"}
$mapExitSource=Join-Path $PSScriptRoot 'ProjectContent\script\phongthan\mapexit'
$mapExitTarget=Join-Path $RuntimeRoot 'Server\script\phongthan\mapexit'
if(-not(Test-Path -LiteralPath (Join-Path $mapExitSource 'registry.tsv') -PathType Leaf)){throw "Missing map-exit registry: $mapExitSource"}
if(Test-Path -LiteralPath $mapExitTarget){Remove-Item -LiteralPath $mapExitTarget -Recurse -Force}
New-Item -ItemType Directory -Path $mapExitTarget -Force | Out-Null
Copy-Item -Path (Join-Path $mapExitSource '*') -Destination $mapExitTarget -Recurse -Force
foreach($source in Get-ChildItem -LiteralPath $mapExitSource -File -Recurse) {
    $relative=$source.FullName.Substring($mapExitSource.Length).TrimStart('\')
    $target=Join-Path $mapExitTarget $relative
    if(-not(Test-Path -LiteralPath $target -PathType Leaf) -or
       (Get-FileHash -LiteralPath $source.FullName).Hash -ne (Get-FileHash -LiteralPath $target).Hash){
        throw "Map-exit artifact mismatch: $target"
    }
}
$hotfixItemScriptSource=Join-Path $PSScriptRoot 'ProjectContent\script\item'
$hotfixItemScriptTarget=Join-Path $RuntimeRoot 'Server\script\item'
if(Test-Path -LiteralPath $hotfixItemScriptSource -PathType Container){
    foreach($source in Get-ChildItem -LiteralPath $hotfixItemScriptSource -File -Recurse){
        $relative=$source.FullName.Substring($hotfixItemScriptSource.Length).TrimStart('\')
        $target=Join-Path $hotfixItemScriptTarget $relative
        New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
        Copy-Item -LiteralPath $source.FullName -Destination $target -Force
        if((Get-FileHash -LiteralPath $source.FullName).Hash -ne (Get-FileHash -LiteralPath $target).Hash){
            throw "Item-script artifact mismatch: $target"
        }
    }
}
foreach($name in 'ServerCfg.ini','PhongThanRelay.ini'){
    $source=Join-Path $PSScriptRoot "Config\$name"
    Copy-Item -LiteralPath $source -Destination (Join-Path $RuntimeRoot "Server\$name") -Force
    $configs.Add([pscustomobject]@{Name=$name;Sha256=(Get-FileHash -LiteralPath $source).Hash})
}
$pakChain=& (Join-Path $PSScriptRoot 'Sync-VngPakChain.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
& (Join-Path $PSScriptRoot 'Apply-MapOverrides.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot | Out-Null
& (Join-Path $ProjectRoot 'Tests\Test-GameplayDataRegistry.ps1') `
    -ProjectRoot $ProjectRoot -DataRoot $RuntimeRoot | Out-Null
& (Join-Path $PSScriptRoot 'Publish-StarterBagContent.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
foreach($role in 'Client','Server') {
    & (Join-Path $PSScriptRoot 'Publish-NpcRestoration.ps1') -ProjectRoot $ProjectRoot -RoleRoot (Join-Path $RuntimeRoot $role)
}
$removed=[Collections.Generic.List[string]]::new()
foreach($name in 'S3Relay.exe','S3RelayServer.exe','PhongThanRelayServer.exe','Sword3PaySys.exe','S3DBInterface.dll'){
    $path=[IO.Path]::GetFullPath((Join-Path $RuntimeRoot "Server\$name"))
    if(-not $path.StartsWith($RuntimeRoot+'\Server\',[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe obsolete artifact path'}
    if(Test-Path -LiteralPath $path -PathType Leaf){Remove-Item -LiteralPath $path -Force;$removed.Add($name)}
}
$state=Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-State\Server\CharacterStore'
New-Item -ItemType Directory -Path $state -Force | Out-Null
$link=Join-Path $RuntimeRoot 'Server\CharacterStore'
if(Test-Path -LiteralPath $link){
    $linkItem=Get-Item -LiteralPath $link -Force
    if(($linkItem.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0){
        $targets=@($linkItem.Target | ForEach-Object {[IO.Path]::GetFullPath($_).TrimEnd('\')})
        if([IO.Path]::GetFullPath($state).TrimEnd('\') -notin $targets){throw 'CharacterStore junction points to the wrong persistent state.'}
    }else{
        $runtimeServer=[IO.Path]::GetFullPath((Join-Path $RuntimeRoot 'Server')).TrimEnd('\')
        $resolvedLink=[IO.Path]::GetFullPath($link).TrimEnd('\')
        $resolvedState=[IO.Path]::GetFullPath($state).TrimEnd('\')
        if(-not $resolvedLink.StartsWith($runtimeServer+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe CharacterStore migration source.'}
        if(-not $resolvedState.StartsWith([IO.Path]::GetFullPath((Split-Path -Parent $ProjectRoot)).TrimEnd('\')+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Unsafe CharacterStore migration target.'}
        foreach($file in Get-ChildItem -LiteralPath $link -Recurse -File){
            $relative=$file.FullName.Substring($resolvedLink.Length).TrimStart('\')
            $destination=Join-Path $state $relative
            $destinationParent=Split-Path -Parent $destination
            New-Item -ItemType Directory -Path $destinationParent -Force | Out-Null
            if(Test-Path -LiteralPath $destination){
                if((Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $destination).Hash){throw "CharacterStore migration conflict: $relative"}
            }else{
                Copy-Item -LiteralPath $file.FullName -Destination $destination
                if((Get-FileHash -LiteralPath $file.FullName).Hash -ne (Get-FileHash -LiteralPath $destination).Hash){throw "CharacterStore migration hash mismatch: $relative"}
            }
        }
        $backup=$link+'.pre-junction-backup-'+[DateTime]::UtcNow.ToString('yyyyMMddHHmmss')
        if(Test-Path -LiteralPath $backup){throw 'CharacterStore migration backup already exists.'}
        Move-Item -LiteralPath $link -Destination $backup
        New-Item -ItemType Junction -Path $link -Target $state | Out-Null
    }
}else{New-Item -ItemType Junction -Path $link -Target $state | Out-Null}
$receipt=[pscustomobject]@{Schema=1;Scope='NATIVE_REBUILD_UAT';CreatedAtUtc=[DateTime]::UtcNow.ToString('o');Artifacts=@($artifacts | ForEach-Object { $_ });Configs=@($configs | ForEach-Object { $_ });PakChain=$pakChain;RemovedObsoleteArtifacts=@($removed | ForEach-Object { $_ });CharacterStore=$state;ContentPolicy='Game client loose-first with PAK fallback; server remains PAK-first; exact recovered PT assets are canonical loose content';FullProjectAcceptance=$false}
[IO.File]::WriteAllText((Join-Path $RuntimeRoot 'NATIVE_DEPLOYMENT.json'),($receipt | ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))
& (Join-Path $ProjectRoot 'Tests\Test-NativeRuntime.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
"Removed obsolete runtime binaries: $($removed -join ', ')"
