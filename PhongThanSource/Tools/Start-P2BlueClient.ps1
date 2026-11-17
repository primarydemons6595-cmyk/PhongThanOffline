param([switch]$TestAutoLogin)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
$exe=Join-Path $iso.Client 'Game.exe'
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe}).Count){throw 'P2 isolated Client already running'}
$oldAuto=$env:PHONGTHAN_AUTO_LOGIN;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT
try{
 $env:PHONGTHAN_AUTO_LOGIN=$(if($TestAutoLogin){'1'}else{'0'});$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root
 $proc=Start-Process -FilePath $exe -WorkingDirectory $iso.Client -WindowStyle Normal -PassThru
 [IO.File]::WriteAllText((Join-Path $report 'client_process.json'),(@{Pid=$proc.Id;Path=$exe;AutoLogin=[bool]$TestAutoLogin;StartedUtc=$proc.StartTime.ToUniversalTime().ToString('o');Scope='P2_MANUAL_PLAYER_TEST'}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 $proc|Select-Object Id,Path
}finally{$env:PHONGTHAN_AUTO_LOGIN=$oldAuto;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1}
