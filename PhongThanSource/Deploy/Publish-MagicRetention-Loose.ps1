[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $project -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose\Server'
if(!(Test-Path (Join-Path $runtime 'loose_only.flag'))){throw 'Loose marker missing'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.Name -in 'GameServer.exe','Goddess.exe' -and $_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop Loose world and Goddess safely before publishing'}
$backup=Join-Path $workspace ('analysis\magic_retention_20261031\binary_backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
New-Item -ItemType Directory -Force $backup|Out-Null
$files=@()
foreach($name in 'CoreServer.dll','Goddess.exe'){
 $source=Join-Path $project ('Output\Server\'+$name);$target=Join-Path $runtime $name
 $before=(Get-FileHash -LiteralPath $target).Hash;$expected=(Get-FileHash -LiteralPath $source).Hash
 Copy-Item -LiteralPath $target -Destination (Join-Path $backup $name)
 $temp=$target+'.update-'+[guid]::NewGuid().ToString('N')
 try{
  Copy-Item -LiteralPath $source -Destination $temp
  if((Get-FileHash -LiteralPath $temp).Hash -ne $expected){throw 'Temporary hash mismatch'}
  [IO.File]::Replace($temp,$target,(Join-Path $backup ($name+'.atomic')))
 }finally{if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp}}
 if((Get-FileHash -LiteralPath $target).Hash -ne $expected){throw 'Published hash mismatch'}
 $files+=@{Name=$name;Before=$before;After=$expected}
}
$receipt=@{Scope='LOOSE_ONLY';Backup=$backup;Files=$files;Result='PASS'}
$receipt|ConvertTo-Json -Depth 4|Set-Content (Join-Path $workspace 'analysis\magic_retention_20261031\publication.json') -Encoding UTF8
$receipt|ConvertTo-Json -Depth 4
