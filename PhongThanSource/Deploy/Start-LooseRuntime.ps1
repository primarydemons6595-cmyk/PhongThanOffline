[CmdletBinding()]
param([ValidateSet('Server','Client','Status','StopServer')][string]$Action='Status')
$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$runtime=Join-Path (Split-Path -Parent $project) 'PhongThanRuntime-Loose'
$server=Join-Path $runtime 'Server'
$client=Join-Path $runtime 'Client'
$contentManifest=Get-Content -LiteralPath (Join-Path $PSScriptRoot 'RUNTIME_CONTENT_MANIFEST.json') -Raw | ConvertFrom-Json
$expectedMapCount=[int]$contentManifest.DataRegistries.Gameplay.Map.ExpectedActiveMapCount
if ($expectedMapCount -le 0) { throw 'Expected active map count missing from content manifest' }
function Processes([string]$root) {
    @(Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and $_.ExecutablePath.StartsWith($root+'\',[StringComparison]::OrdinalIgnoreCase)
    })
}
function Ready {
    $bishop=@(Processes $server | Where-Object Name -eq 'Bishop.exe')
    $world=@(Processes $server | Where-Object Name -eq 'GameServer.exe')
    if ($bishop.Count -ne 1 -or $world.Count -ne 1) { return $false }
    $log=Join-Path $server 'native_world_ready.log'
    if (!(Test-Path -LiteralPath $log)) { return $false }
    if ((Get-Item -LiteralPath $log).LastWriteTime -lt $world[0].CreationDate) { return $false }
    $pattern='^WORLD_READY bishop_pid='+$bishop[0].ProcessId+' service=1001 maps='+$expectedMapCount+'$'
    return [bool](@(Get-Content -LiteralPath $log -Tail 20) -match $pattern)
}
if (!(Test-Path -LiteralPath (Join-Path $runtime 'LOOSE_DEPLOYMENT.json'))) { throw 'Publish loose runtime first' }
foreach ($roleRoot in $server,$client) {
    if (!(Test-Path -LiteralPath (Join-Path $roleRoot 'loose_only.flag'))) { throw 'Loose-only marker missing' }
}
if ($Action -eq 'Status') {
    Processes $runtime | Select-Object Name,ProcessId,ExecutablePath
    Write-Output "LOOSE_WORLD_READY_${expectedMapCount}=$(Ready)"
    return
}
if ($Action -eq 'StopServer') {
    foreach ($world in @(Processes $server | Where-Object Name -eq 'GameServer.exe')) {
        $process=Get-Process -Id $world.ProcessId -ErrorAction Stop
        # Keep the process handle before signaling a fast, already-empty server.
        # Get-Process obtained after exit can lose the exit-code handle.
        [void]$process.Handle
        $event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($world.ProcessId)")
        try { [void]$event.Set() } finally { $event.Dispose() }
        if (!$process.WaitForExit(45000)) { throw 'Save/stop pending; support services retained. Do not force-kill.' }
        if ($process.ExitCode -ne 0) { throw 'GameServer stop not confirmed clean; support services retained' }
    }
    foreach ($name in 'Bishop.exe','PhongThanRelay.exe','PhongThanAccountServer.exe','Goddess.exe') {
        foreach ($process in @(Processes $server | Where-Object Name -eq $name)) { Stop-Process -Id $process.ProcessId }
    }
    Write-Output 'LOOSE_SERVER_STOPPED_SAFELY'
    return
}
if ($Action -eq 'Client') {
    if (!(Ready)) { throw "Loose server is not READY/$expectedMapCount; do not connect this client to staging" }
    if (@(Processes $client | Where-Object Name -eq 'Game.exe').Count) { throw 'Loose client already running' }
    $env:__COMPAT_LAYER='HIGHDPIAWARE'
    Start-Process -FilePath (Join-Path $client 'Game.exe') -WorkingDirectory $client -PassThru |
        Select-Object Id,ProcessName
    return
}
if (@(Processes $server | Where-Object Name -in 'Goddess.exe','PhongThanAccountServer.exe','PhongThanRelay.exe','Bishop.exe','GameServer.exe').Count) { throw 'Loose server is already running/partially running. Use Status, not another start.' }
$ports=@(5001,5002,5003,5622,5632,6666)
$occupied=@(Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue | Where-Object LocalPort -in $ports)
if ($occupied.Count) { throw 'Native service ports are in use. Stop the existing stack safely before starting loose.' }
if ((Get-Item (Join-Path $server 'CharacterStore')).Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw 'Loose save storage must not be a shared junction'
}
$services=@(
    @{Name='Goddess.exe';Ports=@(5001)},@{Name='PhongThanAccountServer.exe';Ports=@(5002)},
    @{Name='PhongThanRelay.exe';Ports=@(5003)},@{Name='Bishop.exe';Ports=@(5622,5632)},
    @{Name='GameServer.exe';Ports=@(6666)})
$diagnostics=Join-Path $server 'settings\phongthan\Diagnostics.ini'
$traceLogs=(Test-Path -LiteralPath $diagnostics) -and [bool](@(Get-Content -LiteralPath $diagnostics) -match '^\s*Enabled\s*=\s*1\s*$')
foreach ($service in $services) {
    $launch=@{FilePath=(Join-Path $server $service.Name);WorkingDirectory=$server;WindowStyle='Hidden';PassThru=$true}
    if($traceLogs){$launch.RedirectStandardOutput=Join-Path $server ($service.Name+'.stdout.log');$launch.RedirectStandardError=Join-Path $server ($service.Name+'.stderr.log')}
    $process=Start-Process @launch
    $deadline=[DateTime]::UtcNow.AddSeconds(30)
    do {
        $process.Refresh()
        if ($process.HasExited) { throw "$($service.Name) exited; inspect loose logs. Remaining services retained." }
        $owned=@(Get-NetTCPConnection -State Listen -ErrorAction SilentlyContinue | Where-Object OwningProcess -eq $process.Id |
            Select-Object -ExpandProperty LocalPort)
        $missing=@($service.Ports | Where-Object { $_ -notin $owned })
        if (!$missing.Count) { break }
        Start-Sleep -Milliseconds 250
    } while ([DateTime]::UtcNow -lt $deadline)
    if ($missing.Count) { throw "$($service.Name) is not listening yet; inspect logs, do not start another stack" }
    Write-Output "$($service.Name) PID=$($process.Id) LISTENING"
}
$deadline=[DateTime]::UtcNow.AddSeconds(45)
while (!(Ready) -and [DateTime]::UtcNow -lt $deadline) { Start-Sleep -Milliseconds 500 }
if (Ready) { Write-Output "LOOSE_READY_FOR_LOGIN maps=$expectedMapCount" }
else { Write-Warning 'Loose world still loading/not ready. Services retained. Use Status and inspect missing-file logs; do not restart blindly.' }
