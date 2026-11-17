param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot)
$out=Join-Path $ProjectRoot 'Output/Tools/ReservoirTests'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$source=Get-Content -Raw (Join-Path $ProjectRoot 'Sources/Core/Src/KItem.h')
$start=$source.IndexOf('    BOOL IsReservoir() const {')
# Extract the actual implementation through GetMaxStackNum; omit subsequent fields.
$stop=$source.IndexOf('};', $source.IndexOf('GetMaxStackNum() const {return', $start))+2
if($start -lt 0 -or $stop -le $start){throw 'Cannot extract native item methods'}
$methods=$source.Substring($start,$stop-$start)
$fixture=@"
struct TestItem {
    struct Attrib {int nItemGenre,nDetailType,nParticularType,nMaxStack,nStackNum;} m_CommonAttrib;
    TestItem(){memset(&m_CommonAttrib,0,sizeof(m_CommonAttrib));}
    int GetGenre()const{return m_CommonAttrib.nItemGenre;}
    int GetDetailType()const{return m_CommonAttrib.nDetailType;}
    int weight;
    int GetWeight()const{return weight;}
    const char* GetScript()const{return "NONE";}
$methods
};
"@
[IO.File]::WriteAllText((Join-Path $out 'ReservoirItemMethods.inc'),$fixture,[Text.Encoding]::ASCII)
$carrySource=Get-Content -Raw (Join-Path $ProjectRoot 'Sources/Core/Src/CoreShell.cpp')
$carrySource=$carrySource.Replace("`r`n","`n")
$carryStart=$carrySource.IndexOf('int weight=Item[item->nIdx].GetWeight();')
$carryStop=$carrySource.IndexOf("`n                }`n                item=owner",$carryStart)
if($carryStart -lt 0 -or $carryStop -le $carryStart){throw 'Cannot extract production carried-weight calculation'}
[IO.File]::WriteAllText((Join-Path $out 'ReservoirCarryWeight.inc'),$carrySource.Substring($carryStart,$carryStop-$carryStart),[Text.Encoding]::ASCII)
$manifest=Get-Content -Raw (Join-Path $ProjectRoot 'Deploy/ProjectContent/settings/phongthan/ReservoirTooltips.json') | ConvertFrom-Json
$rows=foreach($row in $manifest.rows){"$($row.detail)`t$($row.part)`t$($row.capacity)`t$($row.effect)`t$($row.rate)"}
$tsv=Join-Path $out 'rows.tsv'
[IO.File]::WriteAllLines($tsv,[string[]]$rows,[Text.Encoding]::ASCII)
foreach($dll in 'Engine.dll','LuaLibDll.dll'){Copy-Item (Join-Path $ProjectRoot "Output/Client/$dll") $out -Force}
$vc='C:/Program Files (x86)/Microsoft Visual Studio/VC98'
$oldPath=$env:PATH;$oldLib=$env:LIB
try {
    $env:PATH="$vc/Bin;C:/Program Files (x86)/Microsoft Visual Studio/Common/MSDev98/Bin;$oldPath"
    $env:LIB="$vc/Lib;$oldLib"
    & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc/Include" "/I$out" "/I$ProjectRoot/Sources/Engine/Src" "/Fo$out/test.obj" "/Fe$out/test.exe" "$ProjectRoot/Tests/Native/ReservoirTooltipTests.cpp" "$ProjectRoot/Lib/release/engine.lib" "$ProjectRoot/Lib/LuaLibDll.lib" /link kernel32.lib user32.lib
    if($LASTEXITCODE){throw 'Reservoir test compile failed'}
    & "$out/test.exe" "$ProjectRoot/Deploy/ProjectContent" $tsv
    if($LASTEXITCODE){throw 'Reservoir tooltip tests failed'}
} finally {$env:PATH=$oldPath;$env:LIB=$oldLib}
