param([Parameter(Mandatory=$true)][int]$ClientProcessId,[Parameter(Mandatory=$true)][string]$UpdateFolder)
$ErrorActionPreference='Stop'
$plan=Get-Content -LiteralPath (Join-Path $UpdateFolder 'plan.json') -Raw | ConvertFrom-Json
$result=[ordered]@{status='WAITING_FOR_CLIENT_EXIT';process=$ClientProcessId;target=$plan.target}
$journal=Join-Path $UpdateFolder 'result.json'
$result | ConvertTo-Json | Set-Content -LiteralPath $journal -Encoding UTF8
try {
    # A quick user relaunch can map the DLL before the first wait returns.
    # Follow all matching loose clients and retry sharing violations safely.
    if((Get-FileHash -LiteralPath $plan.source).Hash -ine $plan.after_sha256){throw 'Pending binary changed'}
    if((Get-FileHash -LiteralPath $plan.target).Hash -ine $plan.before_sha256){throw 'Installed binary changed'}
    $backup=Join-Path $UpdateFolder 'CoreClient.before.dll'
    if(-not (Test-Path -LiteralPath $backup)){Copy-Item -LiteralPath $plan.target -Destination $backup}
    $applied=$false
    while(-not $applied) {
        $active=@(Get-Process -Name Game -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq $plan.client_exe})
        if($active.Count) { $active[0].WaitForExit(); continue }
        if((Get-FileHash -LiteralPath $plan.target).Hash -ine $plan.before_sha256){throw 'Installed binary changed during wait'}
        try { Copy-Item -LiteralPath $plan.source -Destination $plan.target -Force; $applied=$true }
        catch [IO.IOException] { Start-Sleep -Milliseconds 100 }
    }
    if((Get-FileHash -LiteralPath $plan.target).Hash -ine $plan.after_sha256){throw 'Published hash mismatch'}
    $result.status='APPLIED';$result.sha256=$plan.after_sha256
} catch {$result.status='FAILED';$result.error=$_.Exception.Message}
$result | ConvertTo-Json | Set-Content -LiteralPath $journal -Encoding UTF8
