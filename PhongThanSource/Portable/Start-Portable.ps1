[CmdletBinding()]
param([ValidateSet('Studio','Server','Client','Status','BuildNative','BuildStudio')][string]$Action='Studio',
 [string]$RepoRoot,[string[]]$Targets)
$ErrorActionPreference='Stop'
if(!$RepoRoot){$RepoRoot=Join-Path $PSScriptRoot '../..'}
$RepoRoot=[IO.Path]::GetFullPath($RepoRoot).TrimEnd('\')
$local=Join-Path $RepoRoot 'PhongThanSource/Portable/Local'
$settingsPath=Join-Path $local 'settings.json'
if(!(Test-Path $settingsPath)){throw 'Run SETUP.cmd once before launching.'}
$settings=Get-Content $settingsPath -Raw -Encoding UTF8|ConvertFrom-Json
if($settings.RepoRoot -ine $RepoRoot){throw 'Repository moved. Run SETUP.cmd again to prepare the new location.'}
& $settings.SqlLocalDb start $settings.InstanceName|Out-Host
if($LASTEXITCODE){throw 'Cannot start project LocalDB instance'}
$info=(& $settings.SqlLocalDb info $settings.InstanceName)-join "`n"
$pipe=[regex]::Match($info,'np:\\\\.\\pipe\\[^\r\n]+').Value.Trim();if(!$pipe){throw 'Missing LocalDB pipe'}
$runtime=Join-Path $RepoRoot 'PhongThanRuntime-Loose'
$ini=Join-Path $runtime 'Server/DataBase.ini'
$text=[IO.File]::ReadAllText($ini)
$updated=[regex]::Replace($text,'(?im)^Server=.*$',('Server='+$pipe+';Trusted_Connection=Yes'))
if($updated -cne $text){[IO.File]::WriteAllText($ini,$updated,[Text.Encoding]::ASCII)}
$vc=Join-Path $local 'Toolchain/VC6'
$env:PHONGTHAN_MSDEV=Join-Path $vc 'Common/MSDev98/Bin/msdev-portable.cmd'
$env:MSDevDir=Join-Path $vc 'Common/MSDev98'
$env:PATH=(Join-Path $env:ProgramFiles 'dotnet')+';'+(Join-Path $vc 'VC98/Bin')+';'+(Join-Path $vc 'Common/MSDev98/Bin')+';'+(Join-Path $vc 'Common/Tools')+';'+$env:PATH
$env:INCLUDE=(Join-Path $vc 'VC98/Include')+';'+(Join-Path $vc 'VC98/MFC/Include')+';'+(Join-Path $vc 'VC98/ATL/Include')
$env:LIB=(Join-Path $vc 'VC98/Lib')+';'+(Join-Path $vc 'VC98/MFC/Lib')
$env:NUGET_PACKAGES=Join-Path $local 'NuGetPackages'
switch($Action){
 Studio {$exe=Join-Path $runtime 'ManagerStudio/Release/PhongThanStudio.exe';Start-Process -FilePath $exe -WorkingDirectory ([IO.Path]::GetDirectoryName($exe))}
 Server {& (Join-Path $RepoRoot 'PhongThanSource/Deploy/Start-LooseRuntime.ps1') -Action Server
  if(Test-Path (Join-Path $runtime 'Server/PhongThan.Event.exe')){Start-Process -FilePath (Join-Path $runtime 'Server/PhongThan.Event.exe') -WorkingDirectory (Join-Path $runtime 'Server') -WindowStyle Hidden}}
 Client {& (Join-Path $RepoRoot 'PhongThanSource/Deploy/Start-LooseRuntime.ps1') -Action Client}
 Status {& (Join-Path $RepoRoot 'PhongThanSource/Deploy/Start-LooseRuntime.ps1') -Action Status}
 BuildNative {$argsMap=@{MsDevPath=$env:PHONGTHAN_MSDEV;Configuration='Release'};if($Targets){$argsMap.Targets=$Targets};& (Join-Path $RepoRoot 'PhongThanSource/Build/Build-PhongThan.ps1') @argsMap}
 BuildStudio {& dotnet build (Join-Path $runtime 'ManagerStudio/Wpf/PhongThanStudio.App/PhongThanStudio.App.csproj') -c Release;if($LASTEXITCODE){throw 'Studio build failed'}}
}
