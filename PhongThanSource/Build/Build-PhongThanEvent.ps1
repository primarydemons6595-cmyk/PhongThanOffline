[CmdletBinding()]
param(
    [ValidateSet('Release','Debug')][string]$Configuration='Release',
    [switch]$NoRebuild
)
$ErrorActionPreference='Stop'
$sourceRoot=[IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$project=Join-Path $sourceRoot 'Sources\EventService\PhongThan.EventService.csproj'
$output=Join-Path $sourceRoot 'Output\Server'
# Build output only. Runtime config, EventService state and access token are
# never deployment artifacts and must not be overwritten by a build.
if(-not $NoRebuild){
    & dotnet clean $project -c $Configuration --nologo
    if($LASTEXITCODE){exit $LASTEXITCODE}
}
& dotnet build $project -c $Configuration --nologo -o $output
if($LASTEXITCODE){exit $LASTEXITCODE}
foreach($name in @('PhongThan.Event.exe','PhongThan.Event.dll','PhongThan.Event.deps.json','PhongThan.Event.runtimeconfig.json','PhongThan.Event.Runtime.dll')){
    if(-not (Test-Path -LiteralPath (Join-Path $output $name))){throw "Missing Event output: $name"}
}
$dependencies=[IO.File]::ReadAllText((Join-Path $output 'PhongThan.Event.deps.json'))
if($dependencies -match 'PhongThanStudio|SecurePak|ReleasePipeline'){throw 'Event output unexpectedly depends on Studio publishing tools.'}
Write-Output 'BUILD Event PASS (output only; runtime unchanged)'
