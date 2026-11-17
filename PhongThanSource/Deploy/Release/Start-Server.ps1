$ErrorActionPreference='Stop'
$server=[IO.Path]::GetFullPath($PSScriptRoot)
if(!(Test-Path -LiteralPath (Join-Path $server 'Data\release.catalog'))){throw 'Missing signed release catalog.'}
& (Join-Path $server 'Remove-LegacyStudio.ps1')
$units=@(@{Name='Goddess.exe';Port=5001},@{Name='PhongThanAccountServer.exe';Port=5002},@{Name='PhongThanRelay.exe';Port=5003},@{Name='Bishop.exe';Port=5622},@{Name='GameServer.exe';Port=6666})
foreach($unit in $units){
 $target=Join-Path $server $unit.Name
 $existing=@(Get-Process -Name ([IO.Path]::GetFileNameWithoutExtension($unit.Name)) -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.Equals($target,[StringComparison]::OrdinalIgnoreCase)})
 if($existing.Count){Write-Output "$($unit.Name) already running";continue}
 if(@(Get-NetTCPConnection -State Listen -LocalPort $unit.Port -ErrorAction SilentlyContinue).Count){throw "Port $($unit.Port) is busy. Stop the other deployment first."}
 $p=Start-Process -FilePath $target -WorkingDirectory $server -WindowStyle Hidden -PassThru
 Write-Output "Started $($unit.Name) PID $($p.Id)"
 if($unit.Name -eq 'GameServer.exe'){continue}
 $ready=$false
 for($i=0;$i -lt 120;$i++){if($p.HasExited){throw "$($unit.Name) exited"};if(@(Get-NetTCPConnection -State Listen -LocalPort $unit.Port -ErrorAction SilentlyContinue).Count){$ready=$true;break};Start-Sleep -Milliseconds 250}
 if(!$ready){throw "$($unit.Name) is not listening yet. Inspect before starting again."}
}
$event=Join-Path $server 'PhongThan.Event.exe'
if(Test-Path -LiteralPath $event){
 $running=@(Get-Process -Name 'PhongThan.Event' -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.Equals($event,[StringComparison]::OrdinalIgnoreCase)})
 if(!$running.Count){Start-Process -FilePath $event -ArgumentList @('--root',('"'+$server+'"')) -WorkingDirectory $server -WindowStyle Hidden | Out-Null}
}
Write-Output 'Server started. World initialization can take several minutes.'
