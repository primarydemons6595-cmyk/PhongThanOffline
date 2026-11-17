[CmdletBinding()]
param(
    [string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot),
    [string]$RuntimeRoot
)
$ErrorActionPreference='Stop'
if(-not $RuntimeRoot){$RuntimeRoot=Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging'}

function Assert-True([bool]$Value,[string]$Message){if(-not $Value){throw $Message}}

$core=[IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\Core\Src\KNpc.cpp'))
Assert-True (-not $core.Contains('0x381C646EUL')) 'Fix #1C ID must come from the deterministic registry, not KNpc hardcoding.'
Assert-True (-not $core.Contains('0x5F310810UL')) 'Fix #1C ID must come from the deterministic registry, not KNpc hardcoding.'
Assert-True (-not $core.Contains('FIX1C_UAT_TEMP')) 'Temporary Fix #1C instrumentation remains in production source.'
Assert-True (-not $core.Contains('PhongThanFix1dTrapDiag')) 'Temporary Fix #1D instrumentation remains in production source.'

$registryPath=Join-Path $ProjectRoot 'Deploy\ProjectContent\script\phongthan\mapexit\registry.tsv'
Assert-True (Test-Path -LiteralPath $registryPath -PathType Leaf) 'Missing deterministic map-exit registry.'
$registry=@(Import-Csv -LiteralPath $registryPath -Delimiter "`t")

$pairs=@(
    @{Name='forward';File='chongcheng_camp_to_outskirts.lua';Map=5;X=1808;Y=2943;Id='0x381C646E'},
    @{Name='reverse';File='chongcheng_outskirts_to_camp.lua';Map=2;X=1544;Y=3266;Id='0x5F310810'}
)
foreach($pair in $pairs){
	$binding=@($registry | Where-Object {$_.ScriptIdHex -eq $pair.Id})
	Assert-True ($binding.Count -eq 1) "Missing or duplicate registry binding for $($pair.Id)."
	Assert-True ($binding[0].Classification -eq 'NORMAL_EXIT') "Wrong registry classification for $($pair.Id)."
	$projectFile=Join-Path $ProjectRoot ('Deploy\ProjectContent\script\phongthan\mapexit\'+$pair.File)
	$expectedPath=('\script\phongthan\mapexit\'+$pair.File)
	Assert-True ($binding[0].LoosePath -eq $expectedPath) "Wrong registry path for $($pair.Id)."
    $runtimeFile=Join-Path $RuntimeRoot ('Server\script\phongthan\mapexit\'+$pair.File)
    Assert-True (Test-Path -LiteralPath $projectFile -PathType Leaf) "Missing project $($pair.Name) script."
    $text=[IO.File]::ReadAllText($projectFile)
    Assert-True ($text -match ('NewWorld\s*\(\s*'+$pair.Map+'\s*,\s*'+$pair.X+'\s*,\s*'+$pair.Y+'\s*\)')) "Wrong authentic $($pair.Name) destination."
    if(Test-Path -LiteralPath $runtimeFile -PathType Leaf){
        Assert-True ((Get-FileHash -LiteralPath $projectFile).Hash -eq (Get-FileHash -LiteralPath $runtimeFile).Hash) "Published $($pair.Name) script mismatch."
    }
}

$provenance=[IO.File]::ReadAllText((Join-Path $ProjectRoot 'Deploy\ProjectContent\script\phongthan\mapexit\PROVENANCE.md'))
Assert-True ($provenance.Contains('Client_Original/data/script.pak')) 'Missing original package provenance.'
Assert-True ($provenance.Contains('ADAPTED')) 'Recovery classification is missing.'

[pscustomobject]@{
    Result='PASS'
    Scope='FIX_1C_MAP_EXIT_STATIC_AND_LOOSE_DATA'
    Forward='1002 -> 1005 @ 1808,2943'
    Reverse='1005 -> 1002 @ 1544,3266'
    TemporaryInstrumentationRemoved=$true
}
