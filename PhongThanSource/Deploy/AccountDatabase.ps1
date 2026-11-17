function Resolve-SqlLocalDbExecutable {
    $candidates = New-Object System.Collections.Generic.List[string]
    $command = Get-Command SqlLocalDB.exe -ErrorAction SilentlyContinue
    if ($command) { $candidates.Add($command.Source) }
    foreach ($root in @($env:ProgramFiles, ${env:ProgramFiles(x86)})) {
        if (-not $root) { continue }
        $sqlRoot = Join-Path $root 'Microsoft SQL Server'
        if (-not (Test-Path -LiteralPath $sqlRoot -PathType Container)) { continue }
        foreach ($item in Get-ChildItem -LiteralPath $sqlRoot -Filter SqlLocalDB.exe -Recurse -File -ErrorAction SilentlyContinue |
            Sort-Object FullName -Descending) {
            $candidates.Add($item.FullName)
        }
    }
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return [IO.Path]::GetFullPath($candidate)
        }
    }
    throw 'Khong tim thay SqlLocalDB.exe.'
}

function Start-AccountLocalDb([string]$InstanceName) {
    $sqlLocalDb = Resolve-SqlLocalDbExecutable
    $startOutput = @(& $sqlLocalDb start $InstanceName 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Khong khoi dong duoc LocalDB '$InstanceName': $($startOutput -join ' ')"
    }
    $infoOutput = @(& $sqlLocalDb info $InstanceName 2>&1)
    if ($LASTEXITCODE -ne 0) {
        throw "Khong doc duoc LocalDB '$InstanceName': $($infoOutput -join ' ')"
    }
    $match = [regex]::Match(($infoOutput -join "`n"), 'np:\\\\.\\pipe\\[^\r\n]+')
    if (-not $match.Success) {
        throw "LocalDB '$InstanceName' khong tra ve instance pipe dang chay."
    }
    return $match.Value.Trim()
}

function Set-AccountDatabaseIni([string]$Path, [string]$PipeName) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "Thieu DataBase.ini: $Path"
    }
    $text = [IO.File]::ReadAllText($Path)
    $serverValue = "$PipeName;Trusted_Connection=Yes"
    if ($text -notmatch '(?im)^Server=.*$') {
        throw "DataBase.ini khong co khoa Server: $Path"
    }
    $text = [regex]::Replace($text, '(?im)^Server=.*$', "Server=$serverValue", 1)
    [IO.File]::WriteAllText($Path, $text, [Text.Encoding]::ASCII)
    return $serverValue
}

function Test-AccountDatabase32 {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Server,

        [Parameter(Mandatory=$true)]
        [string]$DatabaseName,

        [string]$User = 'sa',

        [string]$Password = '123456'
    )

    $ps32 = "$env:WINDIR\SysWOW64\WindowsPowerShell\v1.0\powershell.exe"

    if (!(Test-Path -LiteralPath $ps32)) {
        throw "Khong tim thay PowerShell 32-bit: $ps32"
    }

    $connectionString =
        "Provider=SQLOLEDB;Data Source=$Server;Initial Catalog=$DatabaseName;User ID=$User;Password=$Password;"

    $encoded = [Convert]::ToBase64String(
        [Text.Encoding]::Unicode.GetBytes($connectionString)
    )

    $code = @"
`$cs = [Text.Encoding]::Unicode.GetString(
    [Convert]::FromBase64String('$encoded')
)

`$cn = New-Object -ComObject ADODB.Connection

try {
    `$cn.ConnectionTimeout = 10
    `$cn.Open(`$cs)

    `$rs = `$cn.Execute('SELECT DB_NAME()')

    if (!`$rs.EOF) {
        Write-Output `$rs.Fields.Item(0).Value
    }

    `$rs.Close()
}
finally {
    if (`$cn.State -ne 0) {
        `$cn.Close()
    }
}
"@

    $result = & $ps32 -NoProfile -ExecutionPolicy Bypass -Command $code

    if ($LASTEXITCODE -ne 0) {
        throw "Khong ket noi duoc SQL Server database '$DatabaseName'."
    }

    $actual = "$result".Trim()

    if ($actual -ne $DatabaseName) {
        throw "SQL Server tra ve database '$actual', mong doi '$DatabaseName'."
    }

    Write-Output "Database OK: $actual"
}