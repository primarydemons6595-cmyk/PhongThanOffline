[CmdletBinding()]
param(
 [string]$RepoRoot,
 [string]$InstanceName,
 [switch]$InstallPrerequisites,
 [switch]$CheckOnly,
 [switch]$SkipModel
)
$ErrorActionPreference='Stop'
if(!$RepoRoot){$RepoRoot=Join-Path $PSScriptRoot '../..'}
$RepoRoot=[IO.Path]::GetFullPath($RepoRoot).TrimEnd('\')
$local=Join-Path $RepoRoot 'PhongThanSource/Portable/Local'
function Read-Json($p){Get-Content -LiteralPath $p -Raw -Encoding UTF8|ConvertFrom-Json}
function Verify-File($path,$hash){
 if(!(Test-Path -LiteralPath $path -PathType Leaf)){throw "Missing $path. Run git lfs pull first."}
 if((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ine $hash){throw "Checksum mismatch: $path. Run git lfs pull; do not use incomplete files."}
}
function Find-LocalDb {
 $c=Get-Command SqlLocalDB.exe -ErrorAction SilentlyContinue
 if($c){return $c.Source}
 $base=Join-Path $env:ProgramFiles 'Microsoft SQL Server'
 if(Test-Path $base){return (Get-ChildItem $base -Filter SqlLocalDB.exe -Recurse -File|Sort-Object FullName -Descending|Select-Object -First 1).FullName}
}
function Install-Package($path,$argsText){
 $signature=Get-AuthenticodeSignature -LiteralPath $path
 if($signature.Status -ne 'Valid' -or $signature.SignerCertificate.Subject -notmatch 'O=Microsoft Corporation'){throw "Untrusted installer: $path"}
 $exe=$path
 if([IO.Path]::GetExtension($path) -ieq '.msi'){$exe="$env:WINDIR/System32/msiexec.exe";$argsText='/i "'+$path+'" '+$argsText}
 $options=@{FilePath=$exe;ArgumentList=$argsText;Wait=$true;PassThru=$true;WindowStyle='Hidden'}
 if(!([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)){$options.Verb='RunAs'}
 $p=Start-Process @options
 if($p.ExitCode -notin @(0,3010,1638)){throw "Installer failed: $path, code $($p.ExitCode)"}
 if($p.ExitCode -eq 3010){Write-Warning 'A prerequisite requests a restart. Restart Windows if setup cannot continue.'}
}
function Expand-Safe($zip,$destination,[switch]$MissingOnly){
 Add-Type -AssemblyName System.IO.Compression.FileSystem
 $base=[IO.Path]::GetFullPath($destination).TrimEnd('\')+'\'
 [void][IO.Directory]::CreateDirectory($base)
 $archive=[IO.Compression.ZipFile]::OpenRead($zip)
 try{foreach($e in $archive.Entries){
  $path=[IO.Path]::GetFullPath((Join-Path $base $e.FullName))
  if(!$path.StartsWith($base,[StringComparison]::OrdinalIgnoreCase)){throw 'Archive path leaves destination'}
  if(!$e.Name){[void][IO.Directory]::CreateDirectory($path);continue}
  if(Test-Path -LiteralPath $path){if($MissingOnly){continue}else{throw "Existing archive destination: $path"}}
  [void][IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($path))
  [IO.Compression.ZipFileExtensions]::ExtractToFile($e,$path,$false)
 }}finally{$archive.Dispose()}
}
$payload=Read-Json (Join-Path $PSScriptRoot 'payload-manifest.json')
$prereqs=Read-Json (Join-Path $PSScriptRoot 'Prerequisites/manifest.json')
foreach($a in $payload.archives){Verify-File (Join-Path $PSScriptRoot $a.file) $a.sha256}
foreach($p in $prereqs){Verify-File (Join-Path $PSScriptRoot ('Prerequisites/'+$p.file)) $p.sha256}
$dbManifest=Read-Json (Join-Path $PSScriptRoot 'Backups/account-manifest.json')
$bak=Join-Path $PSScriptRoot 'Backups/account.bak';Verify-File $bak $dbManifest.BackupSha256
$model=Read-Json (Join-Path $PSScriptRoot 'Models/manifest.json')
if(!$SkipModel){foreach($p in $model.parts){Verify-File (Join-Path $PSScriptRoot ('Models/'+$p.file)) $p.sha256}}
foreach($side in @('Client','Server')){if(!(Test-Path (Join-Path $RepoRoot "PhongThanRuntime-Loose/$side/loose_only.flag"))){throw "Wrong/incomplete repository: missing $side marker"}}
foreach($p in @(Get-ChildItem (Join-Path $RepoRoot 'Client_Original/data') -Filter '*.pak' -File)){
 $stream=[IO.File]::OpenRead($p.FullName)
 try{$bytes=New-Object byte[] 80;$n=$stream.Read($bytes,0,$bytes.Length);if([Text.Encoding]::ASCII.GetString($bytes,0,$n).StartsWith('version https://git-lfs.github.com/spec/v1')){throw "LFS pointer remains: $($p.Name). Run git lfs pull."}}finally{$stream.Dispose()}
}
Write-Output 'PAYLOAD_CHECKSUMS_OK'
if($CheckOnly){return}
$running=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RepoRoot+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -in @('Game.exe','GameServer.exe','Goddess.exe','Bishop.exe','PhongThanAccountServer.exe','PhongThanRelay.exe','PhongThan.Event.exe')})
if($running){throw 'Close this repository client/server through Studio before setup. No running service was stopped.'}
if($InstallPrerequisites){
 $runtimeText=if(Get-Command dotnet -ErrorAction SilentlyContinue){(& dotnet --list-runtimes)-join "`n"}else{''}
 $sdkText=if(Get-Command dotnet -ErrorAction SilentlyContinue){(& dotnet --list-sdks)-join "`n"}else{''}
 $installOrder=$prereqs|Sort-Object @{Expression={if($_.file -like 'vc_redist*'){0}elseif($_.file -eq 'SqlLocalDB.msi'){2}else{1}}}
 foreach($p in $installOrder){
  $needed=$false
  if($p.file -eq 'SqlLocalDB.msi'){$needed=!(Find-LocalDb)}
  elseif($p.file -like 'windowsdesktop*'){$major=if($p.file -like '*-8.*'){'8'}else{'10'};$needed=$runtimeText -notmatch ('Microsoft.WindowsDesktop.App '+$major+'\.')}
  elseif($p.file -like 'dotnet-sdk*'){$needed=$sdkText -notmatch '(?m)^10\.'}
  elseif($p.file -like 'vc_redist*'){$needed=$true}
  if($needed){Write-Output "Installing $($p.file)";$argsText='/install /quiet /norestart';if($p.file -like '*.msi'){$argsText='/qn /norestart IACCEPTSQLLOCALDBLICENSETERMS=YES'};Install-Package (Join-Path $PSScriptRoot ('Prerequisites/'+$p.file)) $argsText}
 }
 $env:PATH=(Join-Path $env:ProgramFiles 'dotnet')+';'+$env:PATH
}
$sql=Find-LocalDb;if(!$sql){throw 'SQL LocalDB is missing. Run SETUP.cmd to install bundled prerequisites.'}
$desktop=if(Get-Command dotnet -ErrorAction SilentlyContinue){(& dotnet --list-runtimes)-join "`n"}else{''}
if($desktop -notmatch 'Microsoft.WindowsDesktop.App 10\.'){throw '.NET Desktop Runtime 10 is missing. Run SETUP.cmd.'}
[void][IO.Directory]::CreateDirectory($local)
if(!$InstanceName){$hasher=[Security.Cryptography.SHA256]::Create();try{$key=[BitConverter]::ToString($hasher.ComputeHash([Text.Encoding]::UTF8.GetBytes($RepoRoot.ToLowerInvariant()))).Replace('-','').Substring(0,12)}finally{$hasher.Dispose()};$InstanceName='PhongThanPT_'+$key}
if($InstanceName -cnotmatch '\APhongThan[A-Za-z0-9_]{1,50}\z'){throw 'Use a dedicated instance name beginning with PhongThan.'}
$instances=@(& $sql info);if($LASTEXITCODE){throw 'Cannot list LocalDB instances'}
if($InstanceName -notin $instances){& $sql create $InstanceName|Out-Host;if($LASTEXITCODE){throw 'Cannot create dedicated LocalDB instance'}}
& $sql start $InstanceName|Out-Host;if($LASTEXITCODE){throw 'Cannot start dedicated LocalDB instance'}
$info=(& $sql info $InstanceName)-join "`n";$pipe=[regex]::Match($info,'np:\\\\.\\pipe\\[^\r\n]+').Value.Trim()
if(!$pipe){throw 'LocalDB did not return its named pipe'}
$cn=[Data.SqlClient.SqlConnection]::new("Server=$pipe;Integrated Security=True;Initial Catalog=master;Connect Timeout=30")
$restored=$false
try{
 $cn.Open();$cmd=$cn.CreateCommand();$cmd.CommandTimeout=180
 $cmd.CommandText="SELECT DB_ID(N'account')";$existing=$cmd.ExecuteScalar()
 if($existing -is [DBNull]){
  $escaped=$bak.Replace("'","''");$cmd.CommandText="RESTORE VERIFYONLY FROM DISK=N'$escaped' WITH CHECKSUM";[void]$cmd.ExecuteNonQuery()
  $cmd.CommandText="RESTORE FILELISTONLY FROM DISK=N'$escaped'";$adapter=[Data.SqlClient.SqlDataAdapter]::new($cmd);$files=[Data.DataTable]::new();[void]$adapter.Fill($files);$adapter.Dispose()
  $sqlFiles=Join-Path $local 'Sql';[void][IO.Directory]::CreateDirectory($sqlFiles);$moves=@();$i=0
  foreach($f in $files.Rows){$i++;if($f.Type -notin @('D','L')){throw 'Unsupported database file type'};$ext=if($f.Type -eq 'L'){'.ldf'}else{'.mdf'};$dest=Join-Path $sqlFiles ('account_'+$i+$ext);if(Test-Path $dest){throw 'Existing SQL data file preserved; use a fresh destination'};$moves+="MOVE N'$(([string]$f.LogicalName).Replace("'","''"))' TO N'$($dest.Replace("'","''"))'"}
  $cmd.CommandText="RESTORE DATABASE [account] FROM DISK=N'$escaped' WITH CHECKSUM, RECOVERY, "+($moves -join ', ');[void]$cmd.ExecuteNonQuery()
  $restored=$true
 }
 $cmd.CommandText='DBCC CHECKDB([account]) WITH NO_INFOMSGS';[void]$cmd.ExecuteNonQuery()
 $cn.ChangeDatabase('account');$counts=@{}
 foreach($p in $dbManifest.Tables.PSObject.Properties){
  if($p.Name -notin @('Account_Info','Account_Habitus','ServerList')){throw 'Unexpected backup table manifest'}
  $cmd.CommandText="SELECT COUNT_BIG(*),CHECKSUM_AGG(BINARY_CHECKSUM(*)) FROM dbo.[$($p.Name)]";$r=$cmd.ExecuteReader();[void]$r.Read();$count=[long]$r[0];$sum=[int]$r[1];$r.Close();$counts[$p.Name]=$count
  if($restored -and ($count -ne $p.Value.Rows -or $sum -ne $p.Value.Checksum)){throw "Restored table does not match snapshot: $($p.Name)"}
 }
 Write-Output "ACCOUNT_DATABASE_OK accounts=$($counts.Account_Info) restored=$restored"
}finally{$cn.Dispose()}
$stateMarker=Join-Path $local 'state-restored.json'
if(!(Test-Path $stateMarker)){
 $store=Join-Path $RepoRoot 'PhongThanRuntime-Loose/Server/CharacterStore'
 if((Test-Path $store) -and @(Get-ChildItem $store -Recurse -File).Count){Write-Output 'Existing player state preserved; snapshot was not applied.'}
 else{Expand-Safe (Join-Path $PSScriptRoot 'State/runtime-state.zip') $RepoRoot -MissingOnly;Write-Output 'PLAYER_STATE_RESTORED'}
 @{Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content $stateMarker -Encoding UTF8
}
$toolchain=Join-Path $local 'Toolchain'
if(!(Test-Path (Join-Path $toolchain 'VC6/Common/MSDev98/Bin/msdev-portable.cmd'))){Expand-Safe (Join-Path $PSScriptRoot 'Toolchain/vc6.zip') $toolchain -MissingOnly}
if(!(Test-Path (Join-Path $local 'NuGetPackages'))){Expand-Safe (Join-Path $PSScriptRoot 'Toolchain/nuget-packages.zip') $local -MissingOnly}
if(!(Test-Path (Join-Path $local 'development-restored.marker'))){Expand-Safe (Join-Path $PSScriptRoot 'Toolchain/development-tools.zip') $RepoRoot -MissingOnly;Set-Content (Join-Path $local 'development-restored.marker') 'done'}
if(!$SkipModel){
 $target=Join-Path $RepoRoot $model.target
 if(Test-Path $target){Verify-File $target $model.sha256}
 else{
  [void][IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target));$temp=$target+'.portable-download';$output=[IO.File]::Create($temp)
  try{foreach($p in $model.parts){$inputFile=[IO.File]::OpenRead((Join-Path $PSScriptRoot ('Models/'+$p.file)));try{$inputFile.CopyTo($output)}finally{$inputFile.Dispose()}}}finally{$output.Dispose()}
  Verify-File $temp $model.sha256;Move-Item -LiteralPath $temp -Destination $target
 }
 Write-Output 'LOCAL_AI_MODEL_OK'
}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'DataBase.ini.template'))
$dbIni=Join-Path $RepoRoot 'PhongThanRuntime-Loose/Server/DataBase.ini'
if((Test-Path $dbIni) -and !(Test-Path (Join-Path $local 'DataBase.before-portable.ini'))){Copy-Item -LiteralPath $dbIni -Destination (Join-Path $local 'DataBase.before-portable.ini')}
[IO.File]::WriteAllText($dbIni,$template.Replace('__PORTABLE_SQL_PIPE__',$pipe),[Text.Encoding]::ASCII)
@{RepoRoot=$RepoRoot;InstanceName=$InstanceName;SqlLocalDb=$sql;Accounts=$counts.Account_Info;CreatedUtc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json|Set-Content (Join-Path $local 'settings.json') -Encoding UTF8
$code='$ProgressPreference="SilentlyContinue";$c=New-Object -ComObject ADODB.Connection;try{$c.Open("driver={SQL Server};Server='+$pipe+';Trusted_Connection=Yes;Database=account;");$r=$c.Execute("SELECT COUNT(*) FROM dbo.Account_Info");Write-Output ("PORTABLE_ODBC32_OK accounts="+$r.Fields.Item(0).Value);$r.Close()}finally{if($c.State){$c.Close()}}'
$encoded=[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($code))
& "$env:WINDIR/SysWOW64/WindowsPowerShell/v1.0/powershell.exe" -NoProfile -ExecutionPolicy Bypass -EncodedCommand $encoded
if($LASTEXITCODE){throw '32-bit SQL driver connection failed'}
Write-Output 'PORTABLE_SETUP_OK. Open START-STUDIO.cmd.'
