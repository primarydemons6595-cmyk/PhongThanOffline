[CmdletBinding()]
param([string]$ProjectRoot)
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$workspace=Split-Path -Parent $ProjectRoot
$runtime=Join-Path $workspace 'PhongThanRuntime-Staging'
$running=@(Get-CimInstance Win32_Process | Where-Object {
 $_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($running){throw 'Save/logout clients and safely stop staging before publishing consumables'}
& python (Join-Path $ProjectRoot 'Tools\check_consumable_content.py')
if($LASTEXITCODE -ne 0){throw 'Consumable content gate failed'}
$receiptPath=Join-Path $runtime 'NATIVE_DEPLOYMENT.json'
$receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
if($receipt.Schema -ne 1 -or $receipt.Scope -ne 'NATIVE_REBUILD_UAT'){throw 'Invalid native receipt'}
$store=Get-Item -LiteralPath (Join-Path $runtime 'Server\CharacterStore')
$expected=Join-Path $workspace 'PhongThanRuntime-State\Server\CharacterStore'
if($store.LinkType -ne 'Junction' -or @($store.Target) -notcontains $expected){throw 'CharacterStore must point to existing persistent state'}
$plans=@()
foreach($pair in @(@{Role='Server';Name='CoreServer.dll'},@{Role='Client';Name='CoreClient.dll'})){
 $entry=@($receipt.Artifacts | Where-Object {$_.Role -eq $pair.Role -and $_.Name -eq $pair.Name})
 if($entry.Count -ne 1){throw 'Nonunique native artifact'}
 $source=Join-Path $ProjectRoot "Output\$($pair.Role)\$($pair.Name)"
 $log=Join-Path $ProjectRoot "Build\Logs\Core$($pair.Role)-Release.log"
 if(!(Select-String -LiteralPath $log -Pattern '0 error\(s\)' -Quiet)){throw 'Build log is not PASS'}
 $plans+=@{Source=$source;Target=(Join-Path $runtime "$($pair.Role)\$($pair.Name)");Entry=$entry[0];Hash=(Get-FileHash -LiteralPath $source).Hash}
}
foreach($role in 'Server','Client'){
 foreach($name in 'ConsumableEffects.txt','ConsumableEffects.provenance.json'){
  $source=Join-Path $ProjectRoot "Deploy\ProjectContent\settings\phongthan\$name"
  $plans+=@{Source=$source;Target=(Join-Path $runtime "$role\settings\phongthan\$name");Hash=(Get-FileHash -LiteralPath $source).Hash}
 }
}
$backup=Join-Path $workspace ('analysis\repair\consumable_runtime_before_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))
New-Item -ItemType Directory -Path $backup | Out-Null
Copy-Item -LiteralPath $receiptPath -Destination (Join-Path $backup 'NATIVE_DEPLOYMENT.json')
foreach($file in $plans){
 if(Test-Path -LiteralPath $file.Target){Copy-Item -LiteralPath $file.Target -Destination (Join-Path $backup ($file.Target.Substring($runtime.Length+1).Replace('\','_')))}
 New-Item -ItemType Directory -Force (Split-Path -Parent $file.Target) | Out-Null
 Copy-Item -LiteralPath $file.Source -Destination $file.Target -Force
 if((Get-FileHash -LiteralPath $file.Target).Hash -ne $file.Hash){throw 'Publish readback mismatch'}
 if($file.Entry){$file.Entry.Sha256=$file.Hash}
 [pscustomobject]@{Path=$file.Target;Sha256=$file.Hash}
}
New-Item -ItemType Directory -Force (Join-Path $store.FullName 'ConsumableBuffs') | Out-Null
$receipt.CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
[IO.File]::WriteAllText($receiptPath,($receipt | ConvertTo-Json -Depth 12),[Text.UTF8Encoding]::new($false))
'PASS: two affected core DLLs + canonical descriptors/provenance; existing character data unchanged.'
