$ErrorActionPreference='Stop'
$server=[IO.Path]::GetFullPath($PSScriptRoot)
function Processes([string]$name){$target=Join-Path $server ($name+'.exe');@(Get-Process -Name $name -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.Equals($target,[StringComparison]::OrdinalIgnoreCase)})}
foreach($world in Processes 'GameServer'){
 $null=$world.Handle
 $stop=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($world.Id)")
 $null=$stop.Set();$stop.Dispose()
 if(!$world.WaitForExit(60000)){throw 'World has not saved/stopped. Supporting services were kept running.'}
 if($world.ExitCode -ne 0){throw "World save failed: exit $($world.ExitCode). Supporting services were kept running."}
 Write-Output 'World saved and stopped.'
}
foreach($name in 'PhongThan.Event','Bishop','PhongThanRelay','PhongThanAccountServer','Goddess'){
 foreach($p in Processes $name){
  if($name -eq 'PhongThan.Event'){$null=$p.CloseMainWindow();if($p.WaitForExit(15000)){continue}}
  $p.Kill();$p.WaitForExit()
 }
}
Write-Output 'Server stopped.'
