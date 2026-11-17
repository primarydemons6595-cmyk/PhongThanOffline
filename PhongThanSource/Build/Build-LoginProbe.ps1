$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$visualStudioRoots=@()
if($env:PHONGTHAN_MSDEV){
    $msdevBin=Split-Path -Parent ([IO.Path]::GetFullPath($env:PHONGTHAN_MSDEV))
    $visualStudioRoots+=[IO.Path]::GetFullPath((Join-Path $msdevBin '..\..\..'))
}
$visualStudioRoots+=@(
    'C:\Program Files (x86)\Microsoft Visual Studio',
    'C:\Program Files\Microsoft Visual Studio',
    'D:\VisualStudio6\VisualStudio6portable-langman.congdongcviet\Microsoft Visual Studio',
    'D:\Microsoft Visual Studio'
)
$vsRoot=$visualStudioRoots |
    Where-Object { Test-Path -LiteralPath (Join-Path $_ 'VC98\Bin\CL.EXE') } |
    Select-Object -First 1
if(-not $vsRoot){throw 'Khong tim thay VC6 CL.EXE. Dat PHONGTHAN_MSDEV den MSDEV.COM cua bo VC6.'}
$vc=Join-Path $vsRoot 'VC98'
$env:PATH="$vc\Bin;$(Split-Path -Parent $vc)\Common\MSDev98\Bin;$env:PATH"
$env:LIB="$vc\Lib;$env:LIB"
& "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc\Include" "/I$root\Headers" "/I$root\Sources\Core\Src" "/I$root\Sources\Engine\Src" `
    "/Fo$root\Output\Tools\PhongThanLoginProbe.obj" "/Fe$root\Output\Tools\PhongThanLoginProbe.exe" `
    "$root\Tests\Native\PhongThanLoginProbe.cpp" /link kernel32.lib ole32.lib uuid.lib
if($LASTEXITCODE){throw 'Login probe compile failed'}
& "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc\Include" "/I$root\Headers" "/I$root\Sources\Core\Src" /c `
    "/Fo$root\Output\Tools\PositionNpcUat.obj" "$root\Tests\Native\PositionNpcUat.cpp"
if($LASTEXITCODE){throw 'Position UAT source compile failed'}
& "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc\Include" "/I$root\Headers" "/I$root\Sources\Core\Src" /c `
    "/Fo$root\Output\Tools\PhongThanCharacterStore.obj" "$root\Sources\MultiServer\Goddess\PhongThanCharacterStore.cpp"
if($LASTEXITCODE){throw 'CharacterStore UAT source compile failed'}
& "$vc\Bin\link.exe" /nologo "/OUT:$root\Output\Tools\PositionNpcUat.exe" `
    "$root\Output\Tools\PositionNpcUat.obj" "$root\Output\Tools\PhongThanCharacterStore.obj" kernel32.lib
if($LASTEXITCODE){throw 'Position UAT compile failed'}
