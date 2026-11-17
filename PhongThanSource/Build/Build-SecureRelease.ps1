[CmdletBinding()]
param([string]$MsDevPath,[switch]$NoRebuild,[switch]$IncludeLocalAi)
$ErrorActionPreference = 'Stop'
$sourceRoot = [IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$header = Join-Path $sourceRoot 'Sources\Engine\Src\PtReleaseKeys.generated.h'
$text = [IO.File]::ReadAllText($header)
if ($text -notmatch '#define PT_SECURE_KEYS_READY 1') { throw 'Initialize Release keys in Studio first.' }
$match = [regex]::Match($text, 'ptReleaseMaster\[\]\s*=\s*\{([^}]+)\}')
if (-not $match.Success) { throw 'Runtime key header invalid.' }
[byte[]]$dataKey = @([regex]::Matches($match.Groups[1].Value, '0x([0-9A-F]{2})') | ForEach-Object { [Convert]::ToByte($_.Groups[1].Value,16) })
if ($dataKey.Length -ne 32) { throw 'Runtime data key invalid.' }
$sha = [Security.Cryptography.SHA256]::Create()
$publicMatch=[regex]::Match($text,'ptReleasePublic\[\]\s*=\s*\{([^}]+)\}')
[byte[]]$publicKey=@([regex]::Matches($publicMatch.Groups[1].Value,'0x([0-9A-F]{2})') | ForEach-Object { [Convert]::ToByte($_.Groups[1].Value,16) })
if($publicKey.Length -ne 72){throw 'Runtime public verification key invalid.'}
[byte[]]$identity=New-Object byte[] 96
[Buffer]::BlockCopy($dataKey,0,$identity,0,32);[Buffer]::BlockCopy($publicKey,8,$identity,32,64)
$fingerprint = ([BitConverter]::ToString($sha.ComputeHash($identity))).Replace('-','')
[Array]::Clear($identity,0,$identity.Length)
[Array]::Clear($dataKey,0,$dataKey.Length)
$oldCl = $env:CL
try {
    $looseRoot=Join-Path (Split-Path -Parent $sourceRoot) 'PhongThanRuntime-Loose'
    $fingerprintProject=Join-Path $sourceRoot 'Tools\SecureRelease\PhongThan.SecureRelease.csproj'
    $fingerprintOutput=@(& dotnet run --project $fingerprintProject -c Release -- source-fingerprint $looseRoot)
    if($LASTEXITCODE){throw 'Cannot fingerprint Release sources.'}
    $sourceFingerprint=$fingerprintOutput[-1].Trim()
    if($sourceFingerprint -notmatch '^[0-9A-F]{64}$'){throw 'Invalid source fingerprint.'}
    # Build output is isolated from deployed Loose. Rebuild avoids mixing PCH/OBJ profiles.
    $env:CL = "$oldCl /DPT_SECURE_RELEASE_PROFILE"
    $parameters = @{ Configuration='Release'; RuntimeProfile='SecureRelease'; NoRebuild=$NoRebuild }
    if ($MsDevPath) { $parameters.MsDevPath=$MsDevPath }
    & (Join-Path $PSScriptRoot 'Build-PhongThan.ps1') @parameters
    if (-not $?) { throw 'Secure runtime native build failed.' }
    & (Join-Path $PSScriptRoot 'Build-PhongThanEvent.ps1') -Configuration Release -NoRebuild
    if (-not $?) { throw 'Release Event build failed.' }
    $aiChoice=if($IncludeLocalAi){'include'}else{'omit'}
    & dotnet run --no-build --project $fingerprintProject -c Release -- stage-local-ai $looseRoot (Join-Path $sourceRoot 'Output\SecureRelease\Server') $aiChoice
    if($LASTEXITCODE){throw 'AI local package staging failed.'}
    $finalFingerprint=@(& dotnet run --no-build --project $fingerprintProject -c Release -- source-fingerprint $looseRoot)
    if($LASTEXITCODE -or $finalFingerprint[-1].Trim() -ne $sourceFingerprint){throw 'Source changed during Release build. Build again before publishing.'}
    foreach ($role in 'Client','Server') {
        $inputRoot = Join-Path $sourceRoot "Output\$role"
        $outputRoot = Join-Path $sourceRoot "Output\SecureRelease\$role"
        New-Item -ItemType Directory -Path $outputRoot -Force | Out-Null
        $files = @{}
        foreach ($file in Get-ChildItem -LiteralPath $inputRoot -File) {
            if ($file.Name -like 'PhongThanStudio*') { continue }
            if ($file.Extension -notin '.dll','.exe' -and $file.Name -notin 'PhongThan.Event.deps.json','PhongThan.Event.runtimeconfig.json','PhongThan.Event.cfg') { continue }
            $destination = Join-Path $outputRoot $file.Name
            Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
            $files[$file.Name] = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
        }
        if($role-eq'Server'){
            foreach($file in @(Get-ChildItem -LiteralPath (Join-Path $inputRoot 'runtimes') -Recurse -File | Where-Object {$_.FullName -match '\\win-(x64|x86|arm64)\\native\\e_sqlite3.dll$'})){
                $relative=$file.FullName.Substring($inputRoot.Length+1);$destination=Join-Path $outputRoot $relative
                [void][IO.Directory]::CreateDirectory((Split-Path $destination));Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
                $files[$relative]=(Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
            }
            $packagePath=Join-Path $outputRoot 'BotAIgent/local-ai.package.json'
            $package=Get-Content -Raw -Encoding UTF8 -LiteralPath $packagePath|ConvertFrom-Json
            foreach($property in $package.Files.PSObject.Properties){$files[$property.Name]=[string]$property.Value}
            $files['BotAIgent/local-ai.package.json']=(Get-FileHash -LiteralPath $packagePath -Algorithm SHA256).Hash
        }
        foreach($old in @(Get-ChildItem -LiteralPath $outputRoot -File -Filter 'PhongThanStudio*')) { Remove-Item -LiteralPath $old.FullName }
        @{ KeyFingerprint=$fingerprint; SourceFingerprint=$sourceFingerprint; Files=$files;IncludeLocalAi=($role-eq'Server'-and$IncludeLocalAi.IsPresent) } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $outputRoot 'runtime-build.json') -Encoding UTF8
    }
    Write-Output 'SECURE_RUNTIME_BUILD_PASS; deployed Loose unchanged'
}
finally { $env:CL=$oldCl; $sha.Dispose() }
