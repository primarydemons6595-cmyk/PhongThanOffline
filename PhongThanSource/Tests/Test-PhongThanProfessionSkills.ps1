[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [string]$DataRoot
)

$ErrorActionPreference = 'Stop'
# Windows PowerShell/.NET Framework already provides CP936 and does not expose
# the .NET Core CodePagesEncodingProvider API used by newer PowerShell hosts.
$providerType = [Type]::GetType('System.Text.CodePagesEncodingProvider, System.Text.Encoding.CodePages')
$registerProvider = [Text.Encoding].GetMethod('RegisterProvider')
if ($providerType -and $registerProvider) {
    $registerProvider.Invoke($null, @($providerType.GetProperty('Instance').GetValue($null, $null))) | Out-Null
}
$cp936 = [Text.Encoding]::GetEncoding(936)
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')

function Assert-Match([string]$Text, [string]$Pattern, [string]$Message) {
    if ($Text -notmatch $Pattern) { throw $Message }
}

$registry = Get-Content -Raw -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KPhongThanProfessionSkills.h')
$loader = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\Core\Src\KPlayerDBFuns.cpp'), $cp936)
$skillList = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\Core\Src\KSkillList.cpp'), $cp936)
Assert-Match $registry 'PHONGTHAN_DAOSHI_SKILL_FIRST\s*=\s*3' 'Dao Si skill range is not canonical.'
Assert-Match $registry 'PHONGTHAN_JIASHI_SKILL_FIRST\s*=\s*27' 'Giap Si skill range is not canonical.'
Assert-Match $registry 'PHONGTHAN_YIREN_SKILL_FIRST\s*=\s*43' 'Di Nhan skill range is not canonical.'
Assert-Match $loader 'PhongThanProfessionOwnsSkill' 'Existing-character loader does not reject foreign profession skills.'
Assert-Match $loader 'PHONGTHAN_COMMON_SKILL_FIRST' 'Existing-character loader does not repair common skills.'
Assert-Match $loader 'PhongThanDefaultPhysicalSkill' 'Existing-character loader does not provide native physical attacks.'
if ($loader -match 'for \(int nSkillId = nFirstProfessionSkill') { throw 'Login must not auto-learn profession skills.' }
Assert-Match $skillList 'PhongThanSkillUsesFightUiStyle' 'Client skill panels do not accept VNG skill styles.'
foreach ($style in 'SKILL_SS_PhongThanAttack','SKILL_SS_PhongThanProduce','SKILL_SS_PhongThanAwaken') {
    Assert-Match $skillList ([regex]::Escape($style)) "Client skill panels do not support $style."
}

$checkedRoles = 0
if ($DataRoot) {
    $DataRoot = [IO.Path]::GetFullPath($DataRoot).TrimEnd('\')
    # New-player INIs defer physical defaults to the server loader.
    $expected = @{ 0=@(); 1=@(); 2=@(); 3=@(); 4=@(); 5=@() }
    foreach ($role in 'Client','Server') {
        $settings = Join-Path $DataRoot "$role\settings"
        $skillTable = Join-Path $settings 'Skills.txt'
        $rows = @(Import-Csv -LiteralPath $skillTable -Delimiter "`t" -Encoding Default)
        $ids = @{}
        foreach ($row in $rows) {
            $id = 0
            if ([int]::TryParse([string]$row.SkillId, [ref]$id)) { $ids[$id] = $row }
        }
        foreach ($id in 1..51) {
            if (-not $ids.ContainsKey($id)) { throw "$role Skills.txt is missing id $id." }
        }
        for ($profile = 0; $profile -lt 6; ++$profile) {
            $path = Join-Path $settings ('npc\player\newplayerini{0:D2}.ini' -f $profile)
            $text = [IO.File]::ReadAllText($path, $cp936)
            $block = [regex]::Match($text, '(?ms)^\[FSKILLS\]\r?\n.*?(?=^\[LSKILLS\])').Value
            $actual = @([regex]::Matches($block, '(?m)^S\d+=(\d+)\s*$') | ForEach-Object { [int]$_.Groups[1].Value })
            if (($actual -join ',') -ne (@($expected[$profile]) -join ',')) {
                throw "$role profile $profile has the wrong profession skill ids."
            }
            if ($block -notmatch "(?m)^COUNT=$(@($expected[$profile]).Count)\s*$") {
                throw "$role profile $profile has the wrong skill count."
            }
        }
        ++$checkedRoles
    }
}

[pscustomobject]@{
    Result = 'PASS'
    Registry = 'VNG Skills.txt ids 1..51'
    Profiles = 6
    RuntimeRolesChecked = $checkedRoles
}
