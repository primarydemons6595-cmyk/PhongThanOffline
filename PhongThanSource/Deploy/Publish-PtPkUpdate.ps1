[CmdletBinding()]
param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$runtime=Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging'
$live=@(Get-CimInstance Win32_Process | Where-Object {
 $_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($live){throw 'Save/logout clients and stop staging before publishing PK update'}
$receiptPath=Join-Path $runtime 'NATIVE_DEPLOYMENT.json'
$receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
if($receipt.Schema -ne 1 -or $receipt.Scope -ne 'NATIVE_REBUILD_UAT'){throw 'Invalid native receipt'}
$checked=@()
foreach($pair in @(@{Role='Server';Name='CoreServer.dll'},@{Role='Client';Name='Game.exe'})) {
 $entry=@($receipt.Artifacts | Where-Object {$_.Role -eq $pair.Role -and $_.Name -eq $pair.Name})
 if($entry.Count -ne 1){throw 'Nonunique artifact identity'}
 $source=Join-Path $ProjectRoot "Output\$($pair.Role)\$($pair.Name)"
 $target=Join-Path $runtime "$($pair.Role)\$($pair.Name)"
 $checked+=@{Source=$source;Target=$target;Entry=$entry[0];Hash=(Get-FileHash -LiteralPath $source).Hash}
}
$uiSource=Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiPkConfirmation.ini'
if((Get-FileHash -LiteralPath $uiSource).Hash -ne '331F60AE2498C0CC1C71F66B85E5964FA011F3EB910DAEF8B208C92AA1B69417'){throw 'Dialog provenance hash mismatch'}
$relative='script/phongthan/npc_restore/1002_17.lua'
$script=Join-Path $ProjectRoot ('Deploy\ProjectContent\NpcRestoration\compiled\'+$relative)
$report=Get-Content -LiteralPath (Join-Path $ProjectRoot 'Docs\NPC_RESTORATION_DEPLOYMENT.json') -Raw | ConvertFrom-Json
$identity=@($report.scripts | Where-Object relative -eq $relative)
if($identity.Count -ne 1 -or $identity[0].sha256 -ne (Get-FileHash -LiteralPath $script).Hash){throw 'NPC script identity mismatch'}
$checked+=@{Source=$uiSource;Target=(Join-Path $runtime 'Client\Ui\ui3\UiPkConfirmation.ini');Hash=(Get-FileHash -LiteralPath $uiSource).Hash}
$checked+=@{Source=$script;Target=(Join-Path $runtime ('Server\'+$relative));Hash=$identity[0].sha256}
$backupRoot=Join-Path (Split-Path -Parent $ProjectRoot) 'analysis\repair\pk_ui_feedback_runtime_before'
New-Item -ItemType Directory -Force $backupRoot | Out-Null
foreach($file in $checked) {
 $backup=Join-Path $backupRoot ($file.Target.Substring($runtime.Length+1).Replace('\','_'))
 if((Test-Path -LiteralPath $file.Target) -and !(Test-Path -LiteralPath $backup)){Copy-Item -LiteralPath $file.Target -Destination $backup}
 Copy-Item -LiteralPath $file.Source -Destination $file.Target -Force
 if((Get-FileHash -LiteralPath $file.Target).Hash -ne $file.Hash){throw 'Published hash mismatch'}
 if($file.Entry){$file.Entry.Sha256=$file.Hash}
 [pscustomobject]@{Path=$file.Target;Sha256=$file.Hash}
}
$receipt.CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
[IO.File]::WriteAllText($receiptPath,($receipt | ConvertTo-Json -Depth 12),[Text.UTF8Encoding]::new($false))
'Published two affected binaries, one exact PT dialog resource, one staging-only authored NPC script.'
