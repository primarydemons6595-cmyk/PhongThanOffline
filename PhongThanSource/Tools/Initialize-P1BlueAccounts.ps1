$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$isolation=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$db=$isolation.AccountDatabase
if($db -cnotmatch '^PT_P1_Blue_p1_[a-f0-9]{12}$'){throw 'Invalid isolated database name'}
$connection=[Data.SqlClient.SqlConnection]::new('Server=127.0.0.1;Database=master;User ID=sa;Password=123456;Connect Timeout=5')
try {
 $connection.Open();$command=$connection.CreateCommand();$command.CommandText="SELECT DB_ID('$db')";$exists=$command.ExecuteScalar()
 if($exists -is [DBNull]){
  $command.CommandText="CREATE DATABASE [$db];";[void]$command.ExecuteNonQuery()
  $command.CommandText="SELECT TOP(0) * INTO [$db].dbo.Account_Info FROM account.dbo.Account_Info; SELECT TOP(0) * INTO [$db].dbo.Account_Habitus FROM account.dbo.Account_Habitus; SELECT * INTO [$db].dbo.ServerList FROM account.dbo.ServerList;";[void]$command.ExecuteNonQuery()
 }
 $connection.ChangeDatabase('account');$command=$connection.CreateCommand();$command.CommandText="SELECT OBJECT_DEFINITION(OBJECT_ID('dbo.View_AccountMoney'))";$view=$command.ExecuteScalar()
 if(!$view -or $view -is [DBNull]){throw 'Source deposit-view definition unavailable'}
 $connection.ChangeDatabase($db);$command=$connection.CreateCommand();$command.CommandText=$view;[void]$command.ExecuteNonQuery()
 . (Join-Path $workspace 'PhongThanSource/Deploy/AccountRegistration.ps1')
 $transaction=$connection.BeginTransaction();try {Add-PhongThanAccountRows $connection $transaction 'p1blueonly' 'E10ADC3949BA59ABBE56E057F20F883E';$transaction.Commit()}catch{$transaction.Rollback();throw}finally{$transaction.Dispose()}
 $command=$connection.CreateCommand();$command.CommandText='SELECT COUNT(*) FROM dbo.Account_Info';$count=$command.ExecuteScalar()
 if($count -ne 1){throw 'Isolated account count is not exactly one'}
 [IO.File]::WriteAllText((Join-Path $report 'account_isolation.json'),(@{Database=$db;Account='p1blueonly';AccountRows=$count;PlayerAccountRowsCopied=0;SourceAccountWrites=0;CreatedUtc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
 'P1 isolated account created; original account database not written'
}finally{$connection.Dispose()}
