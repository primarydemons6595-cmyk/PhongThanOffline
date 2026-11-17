[CmdletBinding()]
param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot),
      [Parameter(Mandatory=$true)][string]$RoleRoot)
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
if((Split-Path -Leaf $RoleRoot) -eq 'Client'){return}
$workspace=Split-Path -Parent $ProjectRoot
$expected=Join-Path $workspace 'PhongThanRuntime-Staging\Server'
if($RoleRoot -ine $expected){throw 'Yenson population publisher accepts staging Server only'}
$running=@(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($running){throw 'Save and stop staging servers before publishing Yenson population'}
$sourceRoot=Join-Path $ProjectRoot 'Deploy\ProjectContent\YenSonPopulation'
$manifestPath=Join-Path $sourceRoot 'manifest.json'
if(!(Test-Path -LiteralPath $manifestPath)){return}
$manifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.map -ne 1007 -or
   $manifest.classification -ne 'AUTHORED_RECONSTRUCTION' -or
   $manifest.population -ne $manifest.rows.Count -or $manifest.population -gt 543){throw 'Invalid Yenson manifest'}
if((Get-FileHash -LiteralPath (Join-Path $RoleRoot 'settings\phongthan\Npcs.txt')).Hash -ine $manifest.template_table_sha256){throw 'Yenson template table changed'}
$latin=[Text.Encoding]::GetEncoding(28591)
$worlds=$latin.GetString([IO.File]::ReadAllBytes((Join-Path $RoleRoot 'settings\WorldSet.ini')))
$match=[regex]::Match($worlds,'(?m)^1007=([^\r\n]+)')
if(!$match.Success){throw 'Missing Yenson map identity'}
$mapName=$match.Groups[1].Value
$plans=@()
foreach($entry in $manifest.rows){
    $m=[regex]::Match($entry.relative,'^settings/phongthan/npc_regions/1007/v_(\d{3})/(\d{3})_npc_s\.dat$')
    if(!$m.Success -or $entry.template -notin @(0,7)){throw 'Unsafe Yenson row'}
    $source=Join-Path $sourceRoot $entry.relative
    if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Yenson source hash mismatch'}
    $ry=$m.Groups[1].Value; $rx=$m.Groups[2].Value
    $base=Join-Path $RoleRoot ("maps\"+$mapName+"_S\v_$ry\${rx}_region_s.dat")
    if((Get-FileHash -LiteralPath $base).Hash -ine $entry.server_sha256){throw 'Yenson server base changed; revalidate population'}
    $client=Join-Path (Split-Path -Parent $RoleRoot) ("Client\maps\"+$mapName+"\v_$ry\${rx}_region_c.dat")
    if((Get-FileHash -LiteralPath $client).Hash -ine $entry.client_sha256){throw 'Yenson client obstacle evidence changed'}
    $target=Join-Path $RoleRoot $entry.relative
    $combined=$target.Replace('_npc_s.dat','_region_s.dat')
    if(Test-Path -LiteralPath $combined){throw 'Existing NPC replacement would hide Yenson population'}
    if((Test-Path -LiteralPath $target) -and (Get-FileHash -LiteralPath $target).Hash -ine $entry.sha256){throw 'Refuse overwrite of unrelated NPC data'}
    $plans+=@{Source=$source;Target=$target;Hash=$entry.sha256}
}
foreach($plan in $plans){
    New-Item -ItemType Directory -Force (Split-Path -Parent $plan.Target) | Out-Null
    Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
    if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.Hash){throw 'Yenson publish readback mismatch'}
}
[pscustomobject]@{Result='PASS';Map=1007;Population=$plans.Count;Classification='AUTHORED_RECONSTRUCTION';OriginalMapsModified=$false;BinaryChanged=$false}
