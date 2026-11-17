$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$exe=Join-Path $iso.Client 'Game.exe'
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe}).Count){throw 'P1 Client already running'}
Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Client/CoreClient.dll') -Destination (Join-Path $iso.Client 'CoreClient.dll')
$oldAuto=$env:PHONGTHAN_AUTO_LOGIN;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT
try{
 $env:PHONGTHAN_AUTO_LOGIN='1';$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root
 $proc=Start-Process -FilePath $exe -WorkingDirectory $iso.Client -WindowStyle Hidden -PassThru
 [IO.File]::WriteAllText((Join-Path $report 'client_process.json'),(@{Pid=$proc.Id;Path=$exe;Account='p1blueonly';StartedUtc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $proc|Select-Object Id,Path
}finally{$env:PHONGTHAN_AUTO_LOGIN=$oldAuto;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1}
