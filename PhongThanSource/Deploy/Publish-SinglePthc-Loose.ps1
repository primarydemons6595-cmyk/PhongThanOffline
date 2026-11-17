[CmdletBinding()]param()
$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent;$workspace=Split-Path $source -Parent;$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$packages=@('ItemQuestFollowup20261031','SinglePthc20261101');$plans=@()
$running=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -in 'Game.exe','GameServer.exe','Goddess.exe','PhongThanStudio.exe','PhongThanCharacterEditor.exe'})
if($running.Count){throw 'Stop Loose client/world/database/editor before publishing'}
foreach($name in $packages){
 $package=Join-Path $PSScriptRoot ('ProjectContent/'+$name);$m=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
 if($m.scope -ne 'LOOSE_ONLY' -or (Test-Path -LiteralPath (Join-Path $package 'publication.json'))){throw 'Scope mismatch or package already published'}
 foreach($file in $m.files){$plans+=@{Path=$file.path;Source=(Join-Path $package ('compiled/'+$file.path));Before=$file.before;After=$file.after}}
}
foreach($p in $plans){
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $p.Path));if(!$target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped Loose'}
 if((Get-FileHash -LiteralPath $p.Source).Hash -ine $p.After){throw ('Staged hash mismatch '+$p.Path)}
 $p.Done=(Test-Path -LiteralPath $target) -and (Get-FileHash -LiteralPath $target).Hash -ieq $p.After
 if(!$p.Done){if($null -eq $p.Before){if(Test-Path -LiteralPath $target){throw ('Unexpected existing file '+$p.Path)}}elseif(!(Test-Path -LiteralPath $target) -or (Get-FileHash -LiteralPath $target).Hash -ine $p.Before){throw ('Live file changed '+$p.Path)}}
 $p.Target=$target
}
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$prior=@(Get-ChildItem -LiteralPath (Join-Path $workspace 'analysis/single_pthc_20261101') -Filter 'runtime-before-*.zip'|Sort-Object Name)
if($prior.Count){$backup=$prior[0].FullName}else{
 $backup=Join-Path $workspace ('analysis/single_pthc_20261101/runtime-before-'+(Get-Date -Format yyyyMMdd_HHmmss)+'.zip')
 $zip=[IO.Compression.ZipFile]::Open($backup,[IO.Compression.ZipArchiveMode]::Create)
 try{foreach($p in $plans){if($null -ne $p.Before){[void][IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip,$p.Target,($p.Path -replace '\\','/'),[IO.Compression.CompressionLevel]::Optimal)}}}finally{$zip.Dispose()}
}
$zip=[IO.Compression.ZipFile]::OpenRead($backup)
try{foreach($p in $plans){if($null -eq $p.Before){continue};$entry=$zip.GetEntry(($p.Path -replace '\\','/'));if(!$entry){throw 'Missing backup entry'};$stream=$entry.Open();$sha=[Security.Cryptography.SHA256]::Create();try{$hash=[BitConverter]::ToString($sha.ComputeHash($stream)).Replace('-','');if($hash -ine $p.Before){throw 'Backup checksum mismatch'}}finally{$sha.Dispose();$stream.Dispose()}}}finally{$zip.Dispose()}
Add-Type -TypeDefinition 'using System;using System.Runtime.InteropServices;public static class PtPublishMove{[DllImport("kernel32.dll",CharSet=CharSet.Unicode,SetLastError=true)]public static extern bool MoveFileEx(string source,string target,uint flags);}'
foreach($p in $plans){
 if($p.Done){continue}
 New-Item -ItemType Directory -Force (Split-Path $p.Target)|Out-Null
 $temp=$p.Target+'.update';try{Copy-Item -LiteralPath $p.Source -Destination $temp -Force;if((Get-FileHash -LiteralPath $temp).Hash -ine $p.After){throw 'Temporary file hash mismatch'}
 if(![PtPublishMove]::MoveFileEx($temp,$p.Target,9)){throw ('Atomic publication failed '+$p.Path)}
 }finally{if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp}}
 if((Get-FileHash -LiteralPath $p.Target).Hash -ine $p.After){throw 'Published hash mismatch'}
}
$receipt=@{Scope='LOOSE_ONLY';Result='PASS';Backup=$backup;Files=@($plans|ForEach-Object {[pscustomobject]@{Path=$_.Path;Before=$_.Before;After=$_.After}})}
foreach($name in $packages){$receipt|ConvertTo-Json -Depth 5|Set-Content (Join-Path $PSScriptRoot ('ProjectContent/'+$name+'/publication.json')) -Encoding UTF8}
$receipt|ConvertTo-Json -Depth 5|Set-Content (Join-Path $workspace 'analysis/single_pthc_20261101/publication.json') -Encoding UTF8
Write-Output "PASS: $($plans.Count) Loose files published; backup $backup"
