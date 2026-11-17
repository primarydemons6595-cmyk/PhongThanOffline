$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $source -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$package=Join-Path $PSScriptRoot 'ProjectContent/BoatQuestFix20261102'
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.scope -ne 'LOOSE_ONLY'){throw 'Scope mismatch'}
$running=@(Get-Process GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)})
if($running.Count){throw 'Loose world must be stopped before publication'}
$plans=@()
foreach($f in $manifest.files){$plans+=@{Path=$f.path;Source=(Join-Path $package ('binaries/'+$f.path));Before=$f.before;After=$f.after}}
foreach($p in $plans){
 $p.Target=[IO.Path]::GetFullPath((Join-Path $runtime $p.Path))
 if(!$p.Target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped runtime'}
 if((Get-FileHash -LiteralPath $p.Source).Hash -ine $p.After){throw ('Staged hash changed '+$p.Path)}
 $exists=Test-Path -LiteralPath $p.Target
 $p.Done=$exists -and (Get-FileHash -LiteralPath $p.Target).Hash -ieq $p.After
 if($p.Done){continue}
 if($null -eq $p.Before){if($exists){throw ('Unexpected live file '+$p.Path)}}elseif(!$exists -or (Get-FileHash -LiteralPath $p.Target).Hash -ine $p.Before){throw ('Live hash changed '+$p.Path)}
}
$backup=Join-Path $workspace ('analysis/boat_quest_fix_20261102/runtime-before-'+(Get-Date -Format yyyyMMdd_HHmmss))
foreach($p in $plans){
 if($p.Done){continue}
 if($null -ne $p.Before){$saved=Join-Path $backup $p.Path;New-Item -ItemType Directory -Force (Split-Path $saved) | Out-Null;Copy-Item -LiteralPath $p.Target -Destination $saved;if((Get-FileHash -LiteralPath $saved).Hash -ine $p.Before){throw 'Backup hash mismatch'}}
}
foreach($p in $plans){
 if($p.Done){continue}
 New-Item -ItemType Directory -Force (Split-Path $p.Target) | Out-Null
 $temporary=$p.Target+'.update-'+[guid]::NewGuid().ToString('N')
 try {
  Copy-Item -LiteralPath $p.Source -Destination $temporary
  if((Get-FileHash -LiteralPath $temporary).Hash -ine $p.After){throw 'Temporary hash mismatch'}
  if(Test-Path -LiteralPath $p.Target){$atomic=Join-Path $backup ('atomic/'+$p.Path);New-Item -ItemType Directory -Force (Split-Path $atomic) | Out-Null;[IO.File]::Replace($temporary,$p.Target,$atomic)}else{[IO.File]::Move($temporary,$p.Target)}
 }finally{if(Test-Path -LiteralPath $temporary){Remove-Item -LiteralPath $temporary}}
 if((Get-FileHash -LiteralPath $p.Target).Hash -ine $p.After){throw 'Runtime hash mismatch'}
}
$receipt=@{Scope='LOOSE_ONLY';Result='PASS';Backup=$backup;PublishedAtUtc=[DateTime]::UtcNow.ToString('o');Files=$plans}
$receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $package 'publication.json') -Encoding UTF8
$receipt | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $workspace 'analysis/boat_quest_fix_20261102/publication.json') -Encoding UTF8
Write-Output "PASS: $($plans.Count) Loose files published and verified; backup $backup"
