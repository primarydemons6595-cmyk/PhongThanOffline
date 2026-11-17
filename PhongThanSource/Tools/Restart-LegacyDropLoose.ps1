[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_live'
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$reload=Join-Path $report "field_npcs/reload_$stamp"
if(!(Test-Path -LiteralPath (Join-Path $server 'loose_only.flag'))){throw 'Loose marker required'}
$exe=Join-Path $server 'GameServer.exe'
$world=@(Get-Process -Name GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq $exe})
if($world.Count -ne 1){throw 'Exactly one running Loose GameServer required'}
$deploymentPath=Join-Path $report 'deployment.json'
$deployment=Get-Content -Raw -LiteralPath $deploymentPath | ConvertFrom-Json
if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ne $deployment.Sha256){throw 'Runtime binary differs from deployment receipt'}
New-Item -ItemType Directory -Force $reload | Out-Null
Copy-Item -LiteralPath $deploymentPath -Destination (Join-Path $reload 'deployment.before.json')
$files=@('CoreServer.dll','settings/phongthan/Npcs.txt','settings/phongthan/LegacyDropExperiment.ini','settings/phongthan/LegacyDropQuarantine.txt')
$hashes=@{}
foreach($relative in $files){$hashes[$relative]=(Get-FileHash -LiteralPath (Join-Path $server $relative)).Hash}
Copy-Item -LiteralPath (Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini') -Destination (Join-Path $reload 'LegacyDropExperiment.ini')
$proc=$world[0]
Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public static class LooseReloadExit { [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access,bool inherit,int pid); [DllImport("kernel32.dll", SetLastError=true)] public static extern bool GetExitCodeProcess(IntPtr handle,out uint code); [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr handle); }'
$handle=[LooseReloadExit]::OpenProcess(0x101000,$false,$proc.Id)
if($handle -eq [IntPtr]::Zero){throw 'Cannot retain native stop evidence'}
try{
 $event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
 try{[void]$event.Set()}finally{$event.Dispose()}
 if(!$proc.WaitForExit(45000)){throw 'Save/stop pending; no force kill or duplicate restart'}
 [uint32]$exit=0
 if(![LooseReloadExit]::GetExitCodeProcess($handle,[ref]$exit)){throw 'Cannot verify native exit code'}
 [IO.File]::WriteAllText((Join-Path $reload 'stop.json'),(@{Pid=$proc.Id;ExitCode=$exit;Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json))
 if($exit -ne 0){throw "GameServer exit=$exit; automatic restart withheld"}
}finally{[void][LooseReloadExit]::CloseHandle($handle)}
foreach($relative in $files){if((Get-FileHash -LiteralPath (Join-Path $server $relative)).Hash -ne $hashes[$relative]){throw "File changed during stop: $relative"}}
$next=Start-Process -FilePath $exe -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $reload 'GameServer.stdout.log') -RedirectStandardError (Join-Path $reload 'GameServer.stderr.log')
$receipt=[ordered]@{Scope='LOOSE_REGISTRY_RELOAD_VIA_CLEAN_RESTART';PreviousPid=$proc.Id;NativeExitCode=$exit;GameServerPid=$next.Id;StartedUtc=$next.StartTime.ToUniversalTime().ToString('o');State='WAITING_FOR_FRESH_WORLD_READY';Hashes=$hashes;ProductionGate='CLOSED';Parity='KINGSOFT_PARITY_UNVERIFIED';EvidenceDirectory=$reload}
[IO.File]::WriteAllText((Join-Path $reload 'restart.json'),($receipt|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$deployment.PreviousPid=$proc.Id
$deployment.GameServerPid=$next.Id
$deployment.StopEvidence='NATIVE_EXIT_0'
$deployment.MonitorPid=0
$deployment.MonitorLog=''
$deployment | Add-Member -NotePropertyName RestartedUtc -NotePropertyValue ([DateTime]::UtcNow.ToString('o')) -Force
[IO.File]::WriteAllText($deploymentPath,($deployment|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json -Depth 5
