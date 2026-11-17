param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$server=Join-Path $ProjectRoot 'Output\Server'
$binary=& (Join-Path $PSScriptRoot 'Test-LuaApiBinaryRegistry.ps1') -ProjectRoot $ProjectRoot -IncludeAll
$forms=@{
    EventServiceOwns='number';EventServiceAccepting='number';EventServiceValue='number,number';EventEnterVanTien='number,number,number';EventEscortAction='number,number';EventEscortInfo='number'
    GetNextPlayer='number,number,number';HaveHandItem='number,number,number,number';DelHandItem='number,number,number,number';LockCamp='number'
    GetGoodsName='number';GetGoodsWeight='number';GetGoodsNum='number,number';GetGoodsPurchasePrice='number,number';GetGoodsSalePrice='number,number'
    ChangeGoodsNum='number,number,number';PlayerSellGoods='number,number,number';InputDialog='string,number,string,string';Roulette='number';ConsumeRouletteResult='';SetNpcDeathScript='number,string'
}
$runId=[guid]::NewGuid().ToString('N')
$fixture=Join-Path $server "event-api-$runId.tsv"
$runner=Join-Path $server "EventApiSmoke-$runId.exe"
try {
    $lines=foreach($name in $forms.Keys | Sort-Object){
        $matches=@($binary.AllRegistrations | Where-Object Name -ceq $name)
        if($matches.Count -ne 1){throw "Missing/duplicate event API $name"}
        "$name`t$($matches[0].FunctionRva)`t$($forms[$name])"
    }
    [IO.File]::WriteAllLines($fixture,$lines,(New-Object Text.UTF8Encoding($false)))
    Copy-Item -LiteralPath (Join-Path $ProjectRoot 'Output\Tools\LuaApiBinarySmoke.exe') -Destination $runner
    $output=@(& $runner $server $fixture 2>&1);$exit=$LASTEXITCODE
    if($exit -ne 0 -or @($output | Where-Object {$_ -like 'FAIL*' -or $_ -like 'ERROR*'}).Count){throw "Event ABI smoke failed: $($output -join '; ')"}
    if(@($output | Where-Object {$_ -like 'PASS*'}).Count -ne $forms.Count){throw 'Expected all native event APIs to pass'}
    [pscustomobject]@{Status='PASS';NewEventApis=$forms.Count;CoreServerSha256=$binary.BinarySha256;Scope='Native DLL registrations and invalid-context ABI; connected Lua behavior tested separately'}
} finally {Remove-Item -LiteralPath $fixture,$runner -Force -ErrorAction SilentlyContinue}
