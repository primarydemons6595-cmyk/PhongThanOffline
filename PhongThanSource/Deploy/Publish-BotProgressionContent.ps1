[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\');$workspace=Split-Path -Parent $ProjectRoot
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -ne 'Server'){return}
if($RoleRoot -ine (Join-Path $workspace 'PhongThanRuntime-Loose\Server')){throw 'Only named Loose Server supported'}
if(Get-CimInstance Win32_Process|Where-Object {$_.Name -eq 'GameServer.exe' -and $_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}){throw 'Save and stop target world first'}
$base=Join-Path $ProjectRoot 'Deploy/ProjectContent/BotProgression20261109'
$manifest=Get-Content -LiteralPath (Join-Path $base 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.profile -ne 'LooseOnly'){throw 'Unexpected content profile'}
foreach($entry in $manifest.files){
 if($entry.path -notmatch '^Server/(script/[^.]+\.lua|settings/phongthan/BotShopRegistry\.tsv)$' -or $entry.path.Contains('..')){throw 'Unexpected content path'}
 $source=Join-Path $base ('compiled/'+$entry.path);$target=Join-Path $RoleRoot $entry.path.Substring(7)
 if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Package hash mismatch'}
 if(Test-Path -LiteralPath $target){$current=(Get-FileHash -LiteralPath $target).Hash;if($current -ine $entry.before_sha256 -and $current -ine $entry.sha256 -and $current -inotIn @($entry.accepted_before_sha256)){Write-Warning "Preserved later changes: $($entry.path)";continue}}
 New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force|Out-Null
 Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target).Hash -ine $entry.sha256){throw 'Content readback mismatch'}
}
# Add the shared contract binding, retaining every other named asset.
$index=Join-Path $RoleRoot 'loose_index.bin';$inputStream=[IO.MemoryStream]::new([IO.File]::ReadAllBytes($index));$reader=[IO.BinaryReader]::new($inputStream)
$bindings=[Collections.Generic.SortedDictionary[uint32,byte[]]]::new()
try{if([Text.Encoding]::ASCII.GetString($reader.ReadBytes(8)) -ne 'PTLOOSE1'){throw 'Invalid loose index'};$count=$reader.ReadUInt32();for($i=0;$i -lt $count;$i++){$id=$reader.ReadUInt32();$length=$reader.ReadUInt16();$raw=$reader.ReadBytes($length);if($raw.Length -ne $length -or $bindings.ContainsKey($id)){throw 'Invalid binding'};$bindings.Add($id,$raw)};if($inputStream.Position -ne $inputStream.Length){throw 'Trailing index bytes'}}finally{$reader.Dispose();$inputStream.Dispose()}
$key=[Convert]::ToUInt32([string]$manifest.contractId,16);$name=[Text.Encoding]::ASCII.GetBytes('\script\phongthan\bots\npc_goals.lua')
if($bindings.ContainsKey($key)){if([Convert]::ToBase64String($bindings[$key]) -ne [Convert]::ToBase64String($name)){throw 'Contract ID collision'}}else{$bindings.Add($key,$name)}
$output=[IO.MemoryStream]::new();$writer=[IO.BinaryWriter]::new($output)
try{$writer.Write([Text.Encoding]::ASCII.GetBytes('PTLOOSE1'));$writer.Write([uint32]$bindings.Count);foreach($pair in $bindings.GetEnumerator()){$writer.Write([uint32]$pair.Key);$writer.Write([uint16]$pair.Value.Length);$writer.Write([byte[]]$pair.Value)};$writer.Flush();$data=$output.ToArray()}finally{$writer.Dispose();$output.Dispose()}
[IO.File]::WriteAllBytes($index,$data)
