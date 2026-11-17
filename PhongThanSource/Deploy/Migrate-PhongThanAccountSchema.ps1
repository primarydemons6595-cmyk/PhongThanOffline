[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RuntimeRoot,[string]$DatabaseName='account',[Parameter(Mandatory=$true)][string]$EvidenceRoot,[switch]$Apply)
$ErrorActionPreference='Stop'
. (Join-Path $PSScriptRoot 'AccountRegistration.ps1')
if($DatabaseName -cnotmatch '\A(account|pt_schema_test_[a-f0-9]{12})\z'){throw 'Only account or an isolated schema-test database is supported.'}
$EvidenceRoot=[IO.Path]::GetFullPath($EvidenceRoot);[void][IO.Directory]::CreateDirectory($EvidenceRoot)
$builder=[Data.SqlClient.SqlConnectionStringBuilder]::new((Get-PhongThanAccountConnectionString $RuntimeRoot));$builder['Initial Catalog']=$DatabaseName
$cn=[Data.SqlClient.SqlConnection]::new($builder.ConnectionString)
function Table([string]$sql){$cmd=$cn.CreateCommand();$cmd.CommandText=$sql;$a=[Data.SqlClient.SqlDataAdapter]::new($cmd);$t=[Data.DataTable]::new();[void]$a.Fill($t);$a.Dispose();return ,$t}
function Quote([string]$name){return '['+$name.Replace(']',']]')+']'}
try{
 $cn.Open()
 if($cn.Database -cne $DatabaseName){throw 'Wrong database.'}
 $keep=@{
  Account_Info=@('iid','cAccName','cPassWord','cSecPassword','dRegDate','iClientID','dLoginDate','dLogoutDate','iLock','nMac')
  Account_Habitus=@('iid','cAccName','iFlag','iLeftSecond','nExtPoint','dBeginDate','iLeftMonth','dEndDate')
  ServerList=@('iid','cServerName','cPassword','cIP','iPort','cMemo','dwGamers')
 }
 $allowedLegacy=@('Account_Info2','Account_Habitus2','activates','admins','article_categories','article_tags','articles','banners','categories','dtproperties','hwids','images','migrations','payments','settings','sysdiagrams','tags','user_forgot_passwords')
 $tables=Table 'SELECT s.name AS SchemaName,t.name FROM sys.tables t JOIN sys.schemas s ON s.schema_id=t.schema_id'
 foreach($r in $tables.Rows){if($r.SchemaName -ne 'dbo' -or ($r.name -notin $keep.Keys -and $r.name -notin $allowedLegacy)){throw "Unreviewed table: $($r.SchemaName).$($r.name). No changes made."}}
 foreach($name in $keep.Keys){if($name -notin @($tables.Rows|ForEach-Object name)){throw "Required table missing: $name"}}
 $drops=@($tables.Rows|Where-Object name -notin $keep.Keys|ForEach-Object name)
 $columns=Table 'SELECT t.name AS TableName,c.name,dc.name AS DefaultName FROM sys.tables t JOIN sys.columns c ON c.object_id=t.object_id LEFT JOIN sys.default_constraints dc ON dc.object_id=c.default_object_id'
 $dropColumns=@($columns.Rows|Where-Object {$_.TableName -in $keep.Keys -and $_.name -notin $keep[$_.TableName]})
 $modules=Table "SELECT s.name AS SchemaName,o.name,o.type, m.definition FROM sys.objects o JOIN sys.schemas s ON s.schema_id=o.schema_id LEFT JOIN sys.sql_modules m ON m.object_id=o.object_id WHERE s.name='dbo' AND o.type IN ('P','V','FN','IF','TF','TR')"
 $dropModules=@($modules.Rows|Where-Object {$_.name -like 'dt_*' -or $_.name -in @('sp_alterdiagram','sp_creatediagram','sp_dropdiagram','sp_helpdiagramdefinition','sp_helpdiagrams','sp_renamediagram','sp_upgraddiagrams','fn_diagramobjects')})
 foreach($m in $modules.Rows){if($m.name -ne 'View_AccountMoney' -and $m.name -notin @($dropModules|ForEach-Object name)){throw "Unreviewed SQL module: $($m.name)"}}
 $statements=[Collections.Generic.List[string]]::new()
 $databaseTriggers=Table 'SELECT name FROM sys.triggers WHERE parent_class=0'
 foreach($trigger in $databaseTriggers.Rows){if($trigger.name -ne 'drop_table'){throw "Unreviewed database trigger: $($trigger.name)"};$statements.Add('DROP TRIGGER [drop_table] ON DATABASE;')}
 # All cross-table constraints in the discarded web schema go first.
 $foreign=Table 'SELECT fk.name,OBJECT_SCHEMA_NAME(fk.parent_object_id) AS SchemaName,OBJECT_NAME(fk.parent_object_id) AS TableName,OBJECT_NAME(fk.referenced_object_id) AS TargetTable FROM sys.foreign_keys fk'
 foreach($f in $foreign.Rows){if($f.TableName -in $drops -or $f.TargetTable -in $drops){$statements.Add('ALTER TABLE '+(Quote $f.SchemaName)+'.'+(Quote $f.TableName)+' DROP CONSTRAINT '+(Quote $f.name)+';')}}
 foreach($m in $dropModules){$kind=([string]$m.type).Trim();$type=if($kind -eq 'P'){'PROCEDURE'}elseif($kind -eq 'V'){'VIEW'}elseif($kind -eq 'TR'){'TRIGGER'}else{'FUNCTION'};$statements.Add("DROP $type dbo."+(Quote $m.name)+';')}
 foreach($name in $drops){$statements.Add('DROP TABLE dbo.'+(Quote $name)+';')}
 foreach($col in $dropColumns){if($col.DefaultName -isnot [DBNull]){$statements.Add('ALTER TABLE dbo.'+(Quote $col.TableName)+' DROP CONSTRAINT '+(Quote $col.DefaultName)+';')};$statements.Add('ALTER TABLE dbo.'+(Quote $col.TableName)+' DROP COLUMN '+(Quote $col.name)+';')}
 $sql="SET XACT_ABORT ON;`n"+($statements -join "`n")
 [IO.File]::WriteAllText((Join-Path $EvidenceRoot 'migration.sql'),$sql,[Text.UTF8Encoding]::new($false))
 @{Database=$DatabaseName;KeptTables=@($keep.Keys);KeptViews=@('View_AccountMoney');DropTables=$drops;DropModules=@($dropModules|ForEach-Object name);DropColumns=@($dropColumns|ForEach-Object { $_.TableName+'.'+$_.name });Applied=$false}|ConvertTo-Json -Depth 6|Set-Content -LiteralPath (Join-Path $EvidenceRoot 'plan.json') -Encoding UTF8
 Write-Output "Plan: keep 3 tables + View_AccountMoney; remove $($drops.Count) tables, $($dropColumns.Count) columns, $($dropModules.Count) SQL tools."
 if(!$Apply){return}
 $backup=Join-Path $EvidenceRoot ($DatabaseName+'_before.bak')
 if(Test-Path -LiteralPath $backup){throw 'Backup path already exists; select a fresh evidence directory.'}
 $cmd=$cn.CreateCommand();$cmd.CommandTimeout=120;$cmd.Parameters.Add('@path',[Data.SqlDbType]::NVarChar,4000).Value=$backup
 $cmd.CommandText='BACKUP DATABASE '+(Quote $DatabaseName)+' TO DISK=@path WITH COPY_ONLY, CHECKSUM, INIT;';[void]$cmd.ExecuteNonQuery()
 $cmd.CommandText='RESTORE VERIFYONLY FROM DISK=@path WITH CHECKSUM;';[void]$cmd.ExecuteNonQuery();$cmd.Dispose()
 $tx=$cn.BeginTransaction([Data.IsolationLevel]::Serializable)
 try{
  $verify=$cn.CreateCommand();$verify.Transaction=$tx;$verify.CommandText='SELECT (SELECT COUNT_BIG(*) FROM dbo.Account_Info),(SELECT COUNT_BIG(*) FROM dbo.Account_Habitus),(SELECT COUNT_BIG(*) FROM dbo.ServerList), (SELECT CHECKSUM_AGG(BINARY_CHECKSUM(iid,cAccName,cPassWord,cSecPassword,dRegDate,iLock)) FROM dbo.Account_Info), (SELECT CHECKSUM_AGG(BINARY_CHECKSUM(iid,cAccName,iFlag,iLeftSecond,nExtPoint,dBeginDate,iLeftMonth,dEndDate)) FROM dbo.Account_Habitus), (SELECT CHECKSUM_AGG(BINARY_CHECKSUM(iid,cServerName,cPassword,cIP,iPort,cMemo,dwGamers)) FROM dbo.ServerList)'
  $reader=$verify.ExecuteReader();[void]$reader.Read();$before=@();for($i=0;$i -lt 6;$i++){$before+= [string]$reader[$i]};$reader.Close()
  $cmd=$cn.CreateCommand();$cmd.Transaction=$tx;$cmd.CommandTimeout=60;$cmd.CommandText=$sql;[void]$cmd.ExecuteNonQuery()
  $cmd.CommandText='SELECT TOP (0) cAccName,cPassWord,cSecPassword,iClientID,nMac,iLock,dRegDate,dLoginDate,dLogoutDate FROM dbo.Account_Info; SELECT TOP (0) iClientID,dLoginDate,iLeftSecond,nExtPoint,dEndDate FROM dbo.View_AccountMoney; SELECT TOP (0) cServerName,cPassword,cIP,iPort,iid,cMemo,dwGamers FROM dbo.ServerList;';[void]$cmd.ExecuteNonQuery()
  $reader=$verify.ExecuteReader();[void]$reader.Read();$after=@();for($i=0;$i -lt 6;$i++){$after+= [string]$reader[$i]};$reader.Close()
  if(($before -join '|') -cne ($after -join '|')){throw 'Required account/deposit/server data changed; rollback.'}
  $tx.Commit()
 }catch{if($tx.Connection){$tx.Rollback()};throw}finally{$tx.Dispose()}
 $check=$cn.CreateCommand();$check.CommandText='DBCC CHECKDB WITH NO_INFOMSGS;';$check.CommandTimeout=120;[void]$check.ExecuteNonQuery()
 @{Database=$DatabaseName;Applied=$true;Backup=$backup;TablesRemoved=$drops;ColumnsRemoved=@($dropColumns|ForEach-Object {$_.TableName+'.'+$_.name});Utc=[DateTimeOffset]::UtcNow}|ConvertTo-Json -Depth 5|Set-Content -LiteralPath (Join-Path $EvidenceRoot 'applied.json') -Encoding UTF8
 Write-Output 'Migration committed; backup verified; DBCC CHECKDB passed.'
}finally{$cn.Dispose()}
