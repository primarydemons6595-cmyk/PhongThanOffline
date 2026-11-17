param([Parameter(Mandatory=$true)][ValidateSet('ground','resync','trace','badtype','save','level4')][string]$Command)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$exe=Join-Path $iso.Server 'GameServer.exe'
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe})
if($world.Count -ne 1){throw 'Exactly one isolated P1 GameServer required'}
$nonce='p1_'+[Guid]::NewGuid().ToString('N')
$request=Join-Path $iso.Server 'p1_world.request';$result=Join-Path $iso.Server 'p1_world.result'
if(Test-Path -LiteralPath $request){throw 'A previous P1 request is still pending'}
[IO.File]::WriteAllText($request,"$Command $nonce",[Text.Encoding]::ASCII)
$deadline=[DateTime]::UtcNow.AddSeconds(15)
do{
 if(Test-Path -LiteralPath $result){$answer=[IO.File]::ReadAllText($result);if($answer.Contains($nonce)){break}}
 Start-Sleep -Milliseconds 200
}while([DateTime]::UtcNow -lt $deadline)
if(!$answer -or !$answer.Contains($nonce)){throw 'P1 request not acknowledged in real GameServer'}
$receipt=@{Command=$Command;Nonce=$nonce;GameServerPid=$world[0].Id;Result=$answer.Trim();Utc=[DateTime]::UtcNow.ToString('o');SaveAcknowledgement=($false)}
[IO.File]::WriteAllText((Join-Path $report ($nonce+'.json')),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
