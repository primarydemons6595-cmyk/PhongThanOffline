[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RuntimeRoot,[switch]$Apply)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'AccountRegistration.ps1')
$cn=[Data.SqlClient.SqlConnection]::new((Get-PhongThanAccountConnectionString $RuntimeRoot))
try {
 $cn.Open();$tx=$cn.BeginTransaction()
 try {
  $read=$cn.CreateCommand();$read.Transaction=$tx;$read.CommandText="SELECT iid,cServerName FROM dbo.ServerList WITH (UPDLOCK,HOLDLOCK) WHERE RTRIM(cServerName)=@old OR RTRIM(cServerName)=@new"
  [void]$read.Parameters.AddWithValue('@old','anhviet');[void]$read.Parameters.AddWithValue('@new','FengShen')
  $table=[Data.DataTable]::new();$adapter=[Data.SqlClient.SqlDataAdapter]::new($read);[void]$adapter.Fill($table);$adapter.Dispose()
  $table | Select-Object iid,cServerName
  $old=@($table.Rows | Where-Object {([string]$_.cServerName).Trim()-eq'anhviet'})
  $new=@($table.Rows | Where-Object {([string]$_.cServerName).Trim()-eq'FengShen'})
  if($old.Count-and$new.Count){throw 'Both gateway names exist; resolve the intended service IDs before migrating.'}
  if(!$Apply){$tx.Rollback();Write-Output 'Preview only. Apply together with the rebuilt Bishop; existing gateway sessions keep the old name until restart.';return}
  $cmd=$cn.CreateCommand();$cmd.Transaction=$tx;$cmd.CommandText='UPDATE dbo.ServerList SET cServerName=@new WHERE RTRIM(cServerName)=@old'
  [void]$cmd.Parameters.AddWithValue('@old','anhviet');[void]$cmd.Parameters.AddWithValue('@new','FengShen')
  $affected=$cmd.ExecuteNonQuery();if($affected-ne$old.Count){throw 'Gateway row count changed'}
  $tx.Commit();Write-Output "Renamed $affected gateway registrations. IDs, credentials and service configuration preserved."
 }catch{try{$tx.Rollback()}catch{};throw}finally{$tx.Dispose()}
}finally{$cn.Dispose()}
