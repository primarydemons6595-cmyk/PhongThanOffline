[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$ProjectRoot,
      [Parameter(Mandatory=$true)][string]$RuntimeRoot)
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
if($RuntimeRoot -ine (Join-Path (Split-Path $ProjectRoot -Parent) 'PhongThanRuntime-Staging')){throw 'Wrong staging root'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RuntimeRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save/logout and safely stop staging before publish'}
$store=Get-Item -LiteralPath (Join-Path $RuntimeRoot 'Server\CharacterStore')
if($store.LinkType -ne 'Junction' -or @($store.Target) -notcontains (Join-Path (Split-Path $ProjectRoot -Parent) 'PhongThanRuntime-State\Server\CharacterStore')){throw 'Persistent CharacterStore junction mismatch'}
$receiptPath=Join-Path $RuntimeRoot 'NATIVE_DEPLOYMENT.json'
$receipt=Get-Content $receiptPath -Raw | ConvertFrom-Json
if($receipt.Schema -ne 1 -or $receipt.Scope -ne 'NATIVE_REBUILD_UAT'){throw 'Invalid receipt'}
$plans=@()
foreach($role in 'Client','Server'){
 $name="Core$role.dll"
 $entry=@($receipt.Artifacts | Where-Object {$_.Role -eq $role -and $_.Name -eq $name})
 if($entry.Count -ne 1){throw 'Nonunique core artifact'}
 if(!(Select-String -Path (Join-Path $ProjectRoot "Build\Logs\Core$role-Release.log") -Pattern '0 error\(s\)' -Quiet)){throw 'Core build not PASS'}
 $plans+=@{Source=(Join-Path $ProjectRoot "Output\$role\$name");Target=(Join-Path $RuntimeRoot "$role\$name");Entry=$entry[0]}
 foreach($file in 'summonskill.txt','summonskill.provenance.json'){
  $plans+=@{Source=(Join-Path $ProjectRoot "Deploy\ProjectContent\settings\$file");Target=(Join-Path $RuntimeRoot "$role\settings\$file")}
 }
}
$table=Join-Path $ProjectRoot 'Deploy\ProjectContent\settings\summonskill.txt'
if((Get-FileHash $table).Hash -ne 'C73438CABEF2FD089F44D2D4FFFC9F2C8689FE689AD68B6538D715A539863029'){throw 'Original registry hash mismatch'}
foreach($plan in $plans){$plan.Hash=(Get-FileHash -LiteralPath $plan.Source).Hash}
$backup=Join-Path (Split-Path $ProjectRoot -Parent) ('analysis\repair\summon_runtime_before_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))
New-Item -ItemType Directory $backup | Out-Null
Copy-Item -LiteralPath $receiptPath -Destination (Join-Path $backup 'NATIVE_DEPLOYMENT.json')
foreach($plan in $plans){
 if(Test-Path -LiteralPath $plan.Target){Copy-Item -LiteralPath $plan.Target -Destination (Join-Path $backup ($plan.Target.Substring($RuntimeRoot.Length+1).Replace('\','_')))}
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ne $plan.Hash){throw 'Publish hash mismatch'}
 if($plan.Entry){$plan.Entry.Sha256=$plan.Hash}
 [pscustomobject]@{Path=$plan.Target;Sha256=$plan.Hash}
}
$receipt.CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
[IO.File]::WriteAllText($receiptPath,($receipt|ConvertTo-Json -Depth 12),[Text.UTF8Encoding]::new($false))
& (Join-Path $ProjectRoot 'Tests\Test-NativeRuntime.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
'PASS: only two core DLLs and original summon registry/provenance. No Game.exe/server service binary or character data changed.'
