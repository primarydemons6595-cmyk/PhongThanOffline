$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$out=Join-Path $root 'Output\Tools'
New-Item -ItemType Directory -Force $out | Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
$exe=Join-Path $out 'PhongThanSummonTests.exe'
& $compiler /nologo /MT /W3 /O2 /DWIN32 "/Fo$out\PhongThanSummonTests.obj" "/Fe$exe" (Join-Path $PSScriptRoot 'Native\PhongThanSummonTests.cpp')
if($LASTEXITCODE -ne 0){throw 'Summon native harness compile failed'}
Push-Location $out
try { & $exe; if($LASTEXITCODE -ne 0){throw 'Summon native harness failed'} } finally {Pop-Location}
$table=Join-Path $root 'Deploy\ProjectContent\settings\summonskill.txt'
if((Get-FileHash -LiteralPath $table).Hash -ne 'C73438CABEF2FD089F44D2D4FFFC9F2C8689FE689AD68B6538D715A539863029'){throw 'Original summon registry changed'}
$rows=@(Import-Csv $table -Delimiter "`t" -Encoding Default | Where-Object {[int]$_.id -ge 450 -and [int]$_.id -le 458})
if($rows.Count -ne 9){throw 'Missing original summon rows'}
$npcs=@(Import-Csv (Join-Path (Split-Path $root -Parent) 'PhongThanRuntime-Staging\Server\settings\phongthan\Npcs.txt') -Delimiter "`t" -Encoding Default)
foreach($row in $rows){
 $npc=$npcs[[int]$row.summonnpcid]
 if($npc.Kind -ne '0' -or !$npc.Skill1 -or !$npc.LevelScript){throw 'Summon NPC template contract missing'}
}
'PASS: exact original table hash, nine ordinary rows and existing PT NPC template contracts.'
