$ErrorActionPreference='Stop'
$server=[IO.Path]::GetFullPath($PSScriptRoot)
$dependency=Join-Path $server 'PhongThan.Event.deps.json'
if(!(Test-Path -LiteralPath $dependency)){throw 'Missing Event dependency manifest.'}
$text=[IO.File]::ReadAllText($dependency)
if($text -match 'PhongThanStudio' -or $text -notmatch 'PhongThan.Event.Runtime'){throw 'Install the independent Event runtime before removing legacy Studio files.'}
$obsolete=@('PhongThanStudio.Core.dll','PhongThanStudio.Core.pdb','PhongThanStudio.Core.deps.json')
$existing=@($obsolete | ForEach-Object { Join-Path $server $_ } | Where-Object { Test-Path -LiteralPath $_ })
if(!$existing.Count){return}
$event=Join-Path $server 'PhongThan.Event.exe'
if(@(Get-Process -Name 'PhongThan.Event' -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.Equals($event,[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop Event before removing its legacy Studio dependency.'}
foreach($file in $existing){Remove-Item -LiteralPath $file;Write-Output ('Removed obsolete dependency: '+[IO.Path]::GetFileName($file))}
