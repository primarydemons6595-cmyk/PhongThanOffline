param([switch]$RollbackTest)
$ErrorActionPreference='Stop'
[Console]::InputEncoding=[Text.UTF8Encoding]::new($false)
[Console]::OutputEncoding=[Text.UTF8Encoding]::new($false)
. (Join-Path $PSScriptRoot 'AccountRegistration.ps1')
function Fail([string]$message){throw $message}
function Assert-NoClientSessions([string]$runtime){
    # This repair only releases stale login reservations. Never release an
    # account while a gateway/world client connection may still own it.
    $server=[IO.Path]::GetFullPath((Join-Path $runtime 'Server'))
    $services=@(Get-Process GameServer,Bishop -ErrorAction SilentlyContinue|Where-Object {$_.Path -and $_.Path.StartsWith($server+'\',[StringComparison]::OrdinalIgnoreCase)})
    if(!$services.Count){Fail 'Không xác minh được dịch vụ đăng nhập đang chạy.'}
    foreach($service in $services){
        $port=if($service.ProcessName -eq 'Bishop'){5622}else{6666}
        if(@(Get-NetTCPConnection -OwningProcess $service.Id -ErrorAction Stop|Where-Object {$_.LocalPort -eq $port -and $_.State -eq 'Established'}).Count){Fail 'Còn phiên kết nối vào game hoặc đăng nhập. Hãy thoát các Client Loose trước khi Fix kẹt acc; không cần tắt server.'}
    }
}
function Invoke-StudioAccountAdmin($request){
    $runtime=[IO.Path]::GetFullPath([string]$request.Root)
    if(!(Test-Path -LiteralPath (Join-Path $runtime 'Server/DataBase.ini'))){Fail 'Workspace không có cấu hình tài khoản.'}
    $op=[string]$request.Operation;$name=[string]$request.Account
    if($op -notin @('List','Register','Password','Fix','DeleteAccount')){Fail 'Thao tác tài khoản không hợp lệ.'}
    if($op -ne 'List'){Assert-PhongThanAccountName $name}
    if($op -in @('Register','Password')){
        if([string]$request.Proof -cnotmatch '\A[0-9A-F]{32}\z'){Fail 'Mã mật khẩu không hợp lệ.'}
    }
    $cn=[Data.SqlClient.SqlConnection]::new((Get-PhongThanAccountConnectionString $runtime));$tx=$null
    try{
        $cn.Open();$cmd=$cn.CreateCommand();$cmd.CommandTimeout=8
        if($op -eq 'List'){
            $cmd.CommandText='SELECT cAccName,iClientID,iLock,dLoginDate,dLogoutDate FROM dbo.Account_Info ORDER BY cAccName'
            $reader=$cmd.ExecuteReader();$rows=@();while($reader.Read()){$rows+=@{Account=$reader.GetString(0).Trim();InUse=([int]$reader[1] -ne 0);Frozen=([int]$reader[2] -ne 0);Login=([string]$reader[3]);Logout=([string]$reader[4])}};$reader.Close()
            return @{Success=$true;Message='Đã tải danh sách tài khoản.';Rows=$rows}
        }
        if($op -eq 'DeleteAccount'){
            # The Studio first archives role files. Never release a live session here.
            foreach($processName in @('GameServer','Goddess','Bishop','AccountServer','PhongThanAccountServer','PhongThanRelay','RelayServer','Game')){
                foreach($p in @(Get-Process -Name $processName -ErrorAction SilentlyContinue)){
                    if(!$p.Path -or $p.Path.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){Fail 'Còn dịch vụ game/client của workspace; không xóa tài khoản.'}
                }
            }
        }
        $tx=$cn.BeginTransaction([Data.IsolationLevel]::Serializable);$cmd.Transaction=$tx
        $cmd.Parameters.Add('@name',[Data.SqlDbType]::NVarChar,16).Value=$name
        if($op -eq 'Register'){
            Add-PhongThanAccountRows $cn $tx $name ([string]$request.Proof)
        }elseif($op -eq 'DeleteAccount'){
            $cmd.CommandText="SELECT name FROM sys.tables WHERE schema_id=SCHEMA_ID('dbo') AND name IN ('Account_Info','Account_Habitus','Account_Info2','Account_Habitus2','payments','user_forgot_passwords')"
            $reader=$cmd.ExecuteReader();$existing=@();while($reader.Read()){$existing+= $reader.GetString(0)};$reader.Close()
            $tables=@('Account_Info','Account_Habitus','Account_Info2','Account_Habitus2','payments','user_forgot_passwords')|Where-Object {$_ -in $existing}
            if('Account_Info' -notin $tables -or 'Account_Habitus' -notin $tables){Fail 'Schema tài khoản thiếu bảng bắt buộc.'}
            $cmd.CommandText=($tables|ForEach-Object {"SELECT * FROM dbo.[$_] WITH (UPDLOCK,HOLDLOCK) WHERE cAccName=@name"}) -join '; '
            $adapter=[Data.SqlClient.SqlDataAdapter]::new($cmd);$data=[Data.DataSet]::new();[void]$adapter.Fill($data);$adapter.Dispose()
            if($data.Tables[0].Rows.Count -ne 1){Fail 'Tài khoản không tồn tại.'}
            # Full SQL preimage, encrypted for the current Windows operator only.
            if(!$RollbackTest){
                Add-Type -AssemblyName System.Security
                $backup=Join-Path $runtime ('ManagerStudio/PhongThanStudio/Backups/AccountDelete_'+[Guid]::NewGuid().ToString('N'))
                [void][IO.Directory]::CreateDirectory($backup)
                $memory=[IO.MemoryStream]::new();$data.WriteXml($memory,[Data.XmlWriteMode]::WriteSchema);$payload=$memory.ToArray();$memory.Dispose()
                $encrypted=[Security.Cryptography.ProtectedData]::Protect($payload,$null,[Security.Cryptography.DataProtectionScope]::CurrentUser)
                [IO.File]::WriteAllBytes((Join-Path $backup 'account.dpapi'),$encrypted);[Array]::Clear($payload,0,$payload.Length)
            }
            $cmd.CommandText=((@('user_forgot_passwords','payments','Account_Habitus2','Account_Info2','Account_Habitus','Account_Info')|Where-Object {$_ -in $tables}|ForEach-Object {"DELETE FROM dbo.[$_] WHERE cAccName=@name"}) -join '; ')+'; SELECT @@ROWCOUNT;'
            if([int]$cmd.ExecuteScalar() -ne 1){Fail 'Tài khoản đã thay đổi; đã hoàn tác.'}
        }else{
            $cmd.CommandText='SELECT cAccName,cPassWord,cSecPassword,iClientID,nMac,dLoginDate,dLogoutDate FROM dbo.Account_Info WITH (UPDLOCK,HOLDLOCK) WHERE cAccName=@name'
            $reader=$cmd.ExecuteReader();$row=[Data.DataTable]::new();$row.Load($reader)
            if($row.Rows.Count -ne 1){Fail 'Tài khoản không tồn tại.'}
            if($op -eq 'Password' -and [int]$row.Rows[0].iClientID -ne 0){Fail 'Tài khoản còn được ghi nhận đang sử dụng. Hãy đăng xuất hoặc Fix kẹt acc trước khi đổi mật khẩu.'}
            if($op -eq 'Fix'){Assert-NoClientSessions $runtime}
            # Password hashes are credentials too: encrypt rollback material,
            # never write it to UI/history/plain-text backup or command lines.
            if(!$RollbackTest){
                Add-Type -AssemblyName System.Security
                $backup=Join-Path $runtime ('ManagerStudio/PhongThanStudio/Backups/AccountAdmin_'+(Get-Date -Format 'yyyyMMdd_HHmmss_fff'))
                [void][IO.Directory]::CreateDirectory($backup)
                $payload=[Text.Encoding]::UTF8.GetBytes(($row.Rows[0]|Select-Object cAccName,cPassWord,cSecPassword,iClientID,nMac,dLoginDate,dLogoutDate|ConvertTo-Json -Compress))
                $encrypted=[Security.Cryptography.ProtectedData]::Protect($payload,$null,[Security.Cryptography.DataProtectionScope]::CurrentUser)
                [IO.File]::WriteAllBytes((Join-Path $backup 'account.dpapi'),$encrypted);[Array]::Clear($payload,0,$payload.Length)
            }
            if($op -eq 'Password'){
                $cmd.CommandText='UPDATE dbo.Account_Info SET cPassWord=@proof WHERE cAccName=@name AND iClientID=0'
                $cmd.Parameters.Add('@proof',[Data.SqlDbType]::NVarChar,32).Value=[string]$request.Proof
            }else{
                # Frozen state, passwords, characters and secondary password stay intact.
                $cmd.CommandText='UPDATE dbo.Account_Info SET iClientID=0,nMac=0,dLogoutDate=GETDATE() WHERE cAccName=@name AND iClientID=@previous'
                $cmd.Parameters.Add('@previous',[Data.SqlDbType]::Int).Value=[int]$row.Rows[0].iClientID
            }
            if($cmd.ExecuteNonQuery() -ne 1){Fail 'Trạng thái tài khoản đã đổi; không ghi đè. Hãy tải lại.'}
        }
        if($RollbackTest){$tx.Rollback()}else{$tx.Commit()}
        $message=switch($op){Register{'Đã đăng ký tài khoản.'}Password{'Đã đổi mật khẩu đăng nhập. Mật khẩu cấp hai giữ nguyên.'}Fix{'Đã xóa trạng thái đăng nhập còn treo (AUTH_RESULT=2). Dữ liệu nhân vật giữ nguyên.'}DeleteAccount{'Đã xóa tài khoản và dữ liệu thanh toán. Bản sao SQL được mã hóa trong Backups.'}}
        return @{Success=$true;Message=$message;Account=$name;RollbackTest=[bool]$RollbackTest}
    }finally{if($tx){if($tx.Connection){$tx.Rollback()};$tx.Dispose()};$cn.Dispose()}
}
if($MyInvocation.InvocationName -ne '.'){
    try{$request=([Console]::ReadLine()|ConvertFrom-Json);Invoke-StudioAccountAdmin $request|ConvertTo-Json -Depth 5 -Compress}
    catch{@{Success=$false;Message=('Thao tác không hoàn tất: '+$_.Exception.Message)}|ConvertTo-Json -Compress;exit 1}
}
