$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$role=Join-Path $iso.Server 'CharacterStore/role_7465737464726f7072617465.pthc'
$before=[IO.File]::ReadAllBytes($role);$stamp=Get-Date -Format 'yyyyMMdd_HHmmss';$backup=Join-Path $report ('save_failure_'+$stamp);New-Item -ItemType Directory -Path $backup|Out-Null;[IO.File]::WriteAllBytes((Join-Path $backup 'before.pthc'),$before)
$lock=[IO.File]::Open($role,[IO.FileMode]::Open,[IO.FileAccess]::Read,[IO.FileShare]::Read)
try{
 & (Join-Path $PSScriptRoot 'Invoke-P1BlueWorld.ps1') -Command save
 Start-Sleep -Seconds 4
 $after=[IO.File]::ReadAllBytes($role)
 $exact=[Convert]::ToBase64String($before) -ceq [Convert]::ToBase64String($after)
 if(!$exact){throw 'Committed canonical state changed under save fault'}
 [IO.File]::WriteAllBytes((Join-Path $backup 'during_fault.pthc'),$after)
 [IO.File]::WriteAllText((Join-Path $backup 'result.json'),(@{Scope='REAL_GAMESERVER_GODDESS';Fault='Canonical file denies delete/replace';PriorStateByteExact=$exact;Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
}finally{$lock.Dispose()}
'P1 canonical save fault injected; prior committed bytes unchanged; verify Goddess saved=0 and recovery separately'
