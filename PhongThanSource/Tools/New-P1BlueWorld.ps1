$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$run='p1_'+[Guid]::NewGuid().ToString('N').Substring(0,12)
$root=Join-Path $workspace ('PhongThanRuntime-Loose/P0-BlueMagic-Isolated/'+$run)
New-Item -ItemType Directory -Path $report,$root,(Join-Path $report 'before') -Force|Out-Null
$p0=Join-Path $workspace 'PhongThanSource/Sources/Core/Src/P0ItemMagicPersistence.h'
Copy-Item -LiteralPath $p0 -Destination (Join-Path $report 'before/P0ItemMagicPersistence.h')
foreach($name in 'KItemList.cpp','KProtocolProcess.cpp','KPlayerDBFuns.cpp') {Copy-Item -LiteralPath (Join-Path $workspace "PhongThanSource/Sources/Core/Src/$name") -Destination (Join-Path $report "before/$name")}
[IO.File]::WriteAllText((Join-Path $root 'p0_isolated.flag'),'P1 ISOLATED LOOSE WORLD; NO LIVE CHARACTER DATA')
$receipt=[ordered]@{Root=$root;Server=(Join-Path $root 'Server');Client=(Join-Path $root 'Client');AccountDatabase=('PT_P1_Blue_'+$run);P0Sha256=(Get-FileHash $p0).Hash;Ports=@(15001,15002,15003,15622,15632,16666);CreatedUtc=[DateTime]::UtcNow.ToString('o')}
[IO.File]::WriteAllText((Join-Path $report 'isolation.json'),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
foreach($role in 'Server','Client') {
 $source=Join-Path $workspace "PhongThanRuntime-Loose/$role";$dest=Join-Path $root $role
 New-Item -ItemType Directory -Path $dest -Force|Out-Null
 Get-ChildItem -LiteralPath $source -File | Where-Object {$_.Extension -in '.exe','.dll','.ini','.cfg','.flag','.bin','.pak'} | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $dest $_.Name)}
 $dirs=if($role -eq 'Server'){@('maps','script','datascript','settings')}else{@('Font','maps','music','script','settings','sound','spr','Ui')}
 foreach($dir in $dirs){
  & robocopy (Join-Path $source $dir) (Join-Path $dest $dir) /E /XJ /R:0 /W:0 /NFL /NDL /NJH /NJS /NP >> (Join-Path $report 'copy.log')
  if($LASTEXITCODE -ge 8){throw "Isolated copy failed $role/$dir"}
 }
}
New-Item -ItemType Directory -Path (Join-Path $root 'Server/CharacterStore') -Force|Out-Null
foreach($file in 'Server/Goddess.cfg','Server/Bishop.cfg','Server/ServerCfg.ini','Server/PhongThanRelay.ini','Client/config.ini') {
 $target=Join-Path $root $file;$bytes=[IO.File]::ReadAllBytes($target);$text=[Text.Encoding]::GetEncoding(28591).GetString($bytes)
 foreach($port in 5001,5002,5003,5622,5632,6666){$text=[regex]::Replace($text,"(?m)(?<![0-9])$port(?![0-9])",[string]($port+10000))}
 [IO.File]::WriteAllBytes($target,[Text.Encoding]::GetEncoding(28591).GetBytes($text))
}
[IO.File]::WriteAllText((Join-Path $root 'Server/acc_setup.ini'),"[Local]`r`nport=15002`r`nmaxconnect=10`r`nprecision=1`r`n",[Text.Encoding]::ASCII)
$db=Join-Path $root 'Server/DataBase.ini';$text=[IO.File]::ReadAllText($db);$text=[regex]::Replace($text,'(?im)^DataBase=.*$',('DataBase='+$receipt.AccountDatabase));[IO.File]::WriteAllText($db,$text,[Text.Encoding]::ASCII)
foreach($file in 'CoreServer.dll'){Copy-Item -LiteralPath (Join-Path $workspace "PhongThanSource/Output/Server/$file") -Destination (Join-Path $root "Server/$file")}
foreach($file in 'CoreClient.dll'){Copy-Item -LiteralPath (Join-Path $workspace "PhongThanSource/Output/Client/$file") -Destination (Join-Path $root "Client/$file")}
$receipt|ConvertTo-Json
