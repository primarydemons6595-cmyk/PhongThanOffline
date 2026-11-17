$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent;$workspace=Split-Path $project -Parent;$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
foreach($name in @('GameServer','PhongThan.Event')){
 if(@(Get-Process -Name $name -ErrorAction SilentlyContinue | Where-Object {$_.Path -and $_.Path.StartsWith($server+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop the target world and Event service first'}
}
Push-Location $workspace
try{& python (Join-Path $project 'Tools/publish_doctors_thuongchu_20261102.py');if($LASTEXITCODE){throw 'Content publication failed'}}finally{Pop-Location}
