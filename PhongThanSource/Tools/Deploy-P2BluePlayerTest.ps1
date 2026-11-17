$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $workspace 'analysis/blue_magic_p1_20261018/isolation.json')|ConvertFrom-Json
$root=[IO.Path]::GetFullPath($iso.Root)
if(!$root.StartsWith((Join-Path $workspace 'PhongThanRuntime-Loose\P0-BlueMagic-Isolated\p1_')) -or !(Test-Path -LiteralPath (Join-Path $root 'p0_isolated.flag'))){throw 'Invalid isolated Loose root'}
foreach($name in 'GameServer','Game','Bishop','Goddess','PhongThanAccountServer','PhongThanRelay'){
 if(@(Get-Process $name -ErrorAction SilentlyContinue|Where-Object {$_.Path.StartsWith($root,[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop P1/P2 isolated stack before deployment'}
}
if((Get-FileHash (Join-Path $workspace 'PhongThanSource/Sources/Core/Src/P0ItemMagicPersistence.h')).Hash -ne $iso.P0Sha256){throw 'P0 changed'}
$p1=Get-Content -Raw (Join-Path $workspace 'analysis/blue_magic_p1_20261018/world_manifest.json')|ConvertFrom-Json
foreach($entry in $p1|Where-Object {$_.path -like 'PhongThanSource/*'}){if((Get-FileHash (Join-Path $workspace $entry.path)).Hash.ToLowerInvariant() -ne $entry.sha256){throw "P1 source changed: $($entry.path)"}}
$backup=Join-Path $report 'deployment_backup';if(Test-Path $backup){throw 'Deployment backup exists; refusing overwrite'}
New-Item -ItemType Directory -Path $backup|Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $backup 'CharacterStore') -Recurse
foreach($name in 'CoreServer.dll','GameServer.exe','Bishop.exe'){Copy-Item -LiteralPath (Join-Path $iso.Server $name) -Destination (Join-Path $backup $name)}
Copy-Item -LiteralPath (Join-Path $iso.Client 'CoreClient.dll') -Destination (Join-Path $backup 'CoreClient.dll')
$legacy=Join-Path $iso.Server 'settings/phongthan/LegacyDropExperiment.ini'
$quarantine=Join-Path $iso.Server 'settings/phongthan/LegacyDropQuarantine.txt'
Copy-Item -LiteralPath $legacy,$quarantine -Destination $backup
# Keep P1-approved executable/client/server sync/persistence code. CoreServer
# adds only the P2 gated drop decoration and isolated runtime admission.
foreach($name in 'CoreServer.dll','GameServer.exe','Bishop.exe'){Copy-Item -LiteralPath (Join-Path $workspace ('PhongThanSource/Output/Server/'+$name)) -Destination (Join-Path $iso.Server $name)}
Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Client/CoreClient.dll') -Destination (Join-Path $iso.Client 'CoreClient.dll')
$text=[IO.File]::ReadAllText($legacy)
# Avoid replacement-group ambiguity; only the nonrequested boss is disabled.
$text=[regex]::Replace($text,'(\[NPC_86\]\s*Enabled=)1',{param($match) $match.Groups[1].Value+'0'})
$text=$text -replace '(?m)^SelfTestEnabled=.*$','SelfTestEnabled=0'
[IO.File]::WriteAllText($legacy,$text,[Text.Encoding]::ASCII)
$manifest=Get-Content -LiteralPath (Join-Path $workspace 'analysis/blue_magic_20261018/table_manifest.txt')
$magic=$manifest[0].Split(' ');$limit=$manifest[1].Split(' ')
$config=@"
; P2 player kills only. EXPERIMENTAL, KINGSOFT_PARITY_UNVERIFIED.
; Chance applies AFTER unchanged Legacy Drop selection/level generation.
[BlueMagicExperimental]
Enabled=1
Mode=LIVE_TEST
NpcAllowlist=0,7,11,15,18
LimitTableVersion=0
MagicWeightGroup=0
BlueChancePerMillion=1000000
MagicLineCount=1
DuplicateAttributeRule=UNIQUE_TYPE
BlankElementRule=UNRESTRICTED
AttributeLevelRule=AT_MOST_ITEM_TIER
Seed=20261018
MagicPathHex=$($magic[0])
MagicSha256=$($magic[1])
LimitPathHex=$($limit[0])
LimitSha256=$($limit[1])
"@
$blue=Join-Path $iso.Server 'settings/phongthan/BlueMagicExperimental.ini'
[IO.File]::WriteAllText($blue,$config,[Text.Encoding]::ASCII)
Copy-Item -LiteralPath $blue -Destination (Join-Path $report 'BlueMagicExperimental.ini')
[IO.File]::WriteAllText((Join-Path $root 'p2_player_test.flag'),'P2 MANUAL PLAYER KILLS ONLY; NO AUTOMATED SPAWN/DEATH')
foreach($name in 'p1_world.request','p1_world.running','legacy_drop_selftest.request','legacy_drop_selftest.running'){if(Test-Path (Join-Path $iso.Server $name)){throw 'Unexpected pending harness request; startup withheld'}}
$receipt=@{Root=$root;Server=$iso.Server;Client=$iso.Client;AccountDatabase=$iso.AccountDatabase;Ports=$iso.Ports;Backup=$backup;Utc=[DateTime]::UtcNow.ToString('o');Status='DEPLOYED_NOT_STARTED';OriginalDataTouched=$false;P0Unchanged=$true;P1SourceUnchanged=$true;QuarantineSha256=(Get-FileHash $quarantine).Hash;CoreServerSha256=(Get-FileHash (Join-Path $iso.Server 'CoreServer.dll')).Hash;CoreClientSha256=(Get-FileHash (Join-Path $iso.Client 'CoreClient.dll')).Hash}
[IO.File]::WriteAllText((Join-Path $report 'deployment.json'),($receipt|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json -Depth 5
