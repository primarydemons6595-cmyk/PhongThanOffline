$ErrorActionPreference='Stop'
$taskSource=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../../..'))
if(!(Test-Path -LiteralPath (Join-Path $taskSource 'Tests/LegacyDropNativeTest/prepare-item-units.mjs'))){
 $taskParent=[IO.DirectoryInfo]$PSScriptRoot
 while($taskParent){$taskCandidate=Join-Path $taskParent.FullName 'PT/PhongThanSource';if(Test-Path -LiteralPath (Join-Path $taskCandidate 'Tests/LegacyDropNativeTest/prepare-item-units.mjs')){$taskSource=$taskCandidate;break};$taskParent=$taskParent.Parent}
 if(!(Test-Path -LiteralPath (Join-Path $taskSource 'Tests/LegacyDropNativeTest/prepare-item-units.mjs'))){throw 'Không tìm thấy engine source PhongThanSource trong workspace PT.'}
}
$taskWorkspace=Split-Path -Parent $taskSource
$taskVc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$taskOutput=Join-Path $PSScriptRoot 'build'
New-Item -ItemType Directory -Force $taskOutput | Out-Null
$taskOldPath=$env:PATH;$taskOldLib=$env:LIB
Push-Location $taskSource
try {
 $env:PATH="$taskVc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$env:PATH";$env:LIB="$taskVc\Lib;$env:LIB"
 & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
 if($LASTEXITCODE){throw 'Native units preparation failed'}
 $taskObjects=@()
 foreach($taskUnit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp',"$PSScriptRoot/UpgradeNative.cpp")){
  $taskObj=Join-Path $taskOutput ([IO.Path]::GetFileNameWithoutExtension($taskUnit)+'.obj')
  & "$taskVc/Bin/cl.exe" /nologo /W3 /GX /Gy /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h "/I$taskVc/Include" /IHeaders /ISources/Core/Src /ISources/Engine/Src /ISources/Engine/Include "/Fo$taskObj" /c $taskUnit
  if($LASTEXITCODE){throw "Native compile failed: $taskUnit"};$taskObjects+=$taskObj
 }
 & "$taskVc/Bin/link.exe" /nologo /OPT:REF "/OUT:$PSScriptRoot/UpgradeNative.exe" @taskObjects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Native link failed'}
 foreach($taskDll in @('Engine.dll','LuaLibDll.dll')){Copy-Item -LiteralPath "$taskWorkspace/PhongThanRuntime-Loose/Server/$taskDll" -Destination (Join-Path $PSScriptRoot $taskDll) -Force}
}finally{Pop-Location;$env:PATH=$taskOldPath;$env:LIB=$taskOldLib}
