[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected Client or Server'}
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/RealmMovementKhulai20261106'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.classification -ne 'REALM_UI_HANDBOOK_KHULAI_MISSILE_20261106'){throw 'Unexpected content package'}
$plans=@()
foreach($entry in $manifest.files){
 if($entry.side -ne $role){continue}
 if($entry.relative -notmatch '^(settings/Missles\.txt|ui/realm_skills\.ini|spr/skill/bomb/earthbomb-01\.spr|script/phongthan/giapsi/item_(34|142|181|9201)\.lua|script/phongthan/test/event_handbook\.lua)$'){throw 'Unexpected content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$entry.relative);$target=Join-Path $RoleRoot $entry.relative
 if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Source content hash mismatch'}
 if(Test-Path -LiteralPath $target){$current=(Get-FileHash -LiteralPath $target).Hash;if($current -ine $entry.sha256 -and $current -ine $entry.before_sha256){continue}}
 $plans+=@{Source=$source;Target=$target;Entry=$entry}
}
# Publishing new resource bindings needs a stopped target consumer. Other
# support processes do not read these files and are not shut down here.
$consumer=if($role -eq 'Client'){'Game.exe'}else{'GameServer.exe'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.Name -eq $consumer -and $_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Close the target game/world before publishing content'}
foreach($plan in $plans){
 New-Item -ItemType Directory -Force -Path (Split-Path -Parent $plan.Target)|Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.Entry.sha256){throw 'Content readback mismatch'}
}
$indexFile=Join-Path $RoleRoot 'loose_index.bin'
if(Test-Path -LiteralPath $indexFile){
 $stream=[IO.MemoryStream]::new([IO.File]::ReadAllBytes($indexFile));$reader=[IO.BinaryReader]::new($stream)
 try{
  if([Text.Encoding]::ASCII.GetString($reader.ReadBytes(8)) -ne 'PTLOOSE1'){throw 'Invalid index magic'}
  $count=$reader.ReadUInt32();$bindings=[Collections.Generic.SortedDictionary[uint32,byte[]]]::new()
  for($i=0;$i -lt $count;$i++){$id=$reader.ReadUInt32();$length=$reader.ReadUInt16();$raw=$reader.ReadBytes($length);if($raw.Length -ne $length -or $bindings.ContainsKey($id)){throw 'Invalid index entry'};$bindings.Add($id,$raw)}
  if($stream.Position -ne $stream.Length){throw 'Trailing index bytes'}
 }finally{$reader.Dispose();$stream.Dispose()}
 foreach($plan in $plans){
  $name='\'+$plan.Entry.relative.Replace('/','\');$raw=[Text.Encoding]::ASCII.GetBytes($name)
  $key=[Convert]::ToUInt32([string]$plan.Entry.id,16)
  if(!$bindings.ContainsKey($key)){$bindings.Add($key,$raw)}
 }
 $next=[IO.MemoryStream]::new();$writer=[IO.BinaryWriter]::new($next)
 try{$writer.Write([Text.Encoding]::ASCII.GetBytes('PTLOOSE1'));$writer.Write([uint32]$bindings.Count);foreach($pair in $bindings.GetEnumerator()){$writer.Write([uint32]$pair.Key);$writer.Write([uint16]$pair.Value.Length);$writer.Write([byte[]]$pair.Value)};$writer.Flush();$data=$next.ToArray()}finally{$writer.Dispose();$next.Dispose()}
 [IO.File]::WriteAllBytes($indexFile,$data)
}
$namedManifest=Join-Path (Split-Path -Parent $RoleRoot) ('completion/'+$role+'_named_effective_manifest.csv')
if(Test-Path -LiteralPath $namedManifest){
 $rows=[Collections.Generic.List[object]]::new();@(Import-Csv -LiteralPath $namedManifest -Encoding UTF8)|ForEach-Object {$rows.Add($_)}
 foreach($plan in $plans){
  $relative=$plan.Entry.relative.Replace('/','\');$matches=@($rows|Where-Object {$_.path.TrimStart('\').Replace('/','\') -ieq $relative})
  if(!$matches.Count){$empty=[ordered]@{};foreach($field in $rows[0].PSObject.Properties.Name){$empty[$field]=''};$row=[pscustomobject]$empty;$row.path=$relative;$rows.Add($row);$matches=@($row)}
  foreach($row in $matches){$row.sha256=$plan.Entry.sha256;$row.bytes=[string]$plan.Entry.bytes;$row.status='RESOLVED';$row.origin='REALM_UI_KHULAI_20261106'}
 }
 $rows|Export-Csv -LiteralPath $namedManifest -NoTypeInformation -Encoding UTF8
}
[pscustomobject]@{Result='PASS';Role=$role;Files=$plans.Count;PreservedExternalEdits=$true;OriginalPakModified=$false}
