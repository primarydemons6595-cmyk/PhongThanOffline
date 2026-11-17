using System.Diagnostics;
using System.Text;
namespace PhongThanCharacterEditor;

public sealed partial class PthcFile
{
    public async Task SaveThroughServerAsync(string serverRoot,CancellationToken cancellation=default){
        if(!CanSave)throw new InvalidOperationException(CompanionStatus);
        serverRoot=Path.GetFullPath(serverRoot);
        if(!Path.GetFullPath(FilePath).StartsWith(Path.Combine(serverRoot,"CharacterStore")+Path.DirectorySeparatorChar,StringComparison.OrdinalIgnoreCase))
            throw new IOException("Chỉ gửi được nhân vật trong CharacterStore của workspace đang mở.");
        var servers=Process.GetProcessesByName("Goddess").Concat(Process.GetProcessesByName("PhongThanDatabaseServer")).ToArray();
        if(!servers.Any(p=>{try{return string.Equals(Path.GetDirectoryName(p.MainModule?.FileName),serverRoot,StringComparison.OrdinalIgnoreCase);}catch{return false;}}))
            throw new IOException("Dịch vụ dữ liệu chưa chạy. Có thể Lưu file offline khi server đã dừng.");
        string queue=Path.Combine(serverRoot,"CharacterAdmin"),id=Guid.NewGuid().ToString("N"),job=Path.Combine(queue,id);Directory.CreateDirectory(job);
        File.WriteAllBytes(Path.Combine(job,"base.pthc"),_original);
        string package=Path.Combine(job,"package"),next=ExportPackage(package);
        File.Copy(next,Path.Combine(job,"next.pthc"),false);
        // next.pthc includes its own exact item attributes.
        File.WriteAllText(Path.Combine(queue,id+".pending"),"PT_STUDIO_CHARACTER_V1",Encoding.ASCII);
        File.Move(Path.Combine(queue,id+".pending"),Path.Combine(queue,id+".ready"));
        string result=Path.Combine(queue,id+".result");var until=DateTime.UtcNow.AddSeconds(15);
        while(!File.Exists(result)){
            if(DateTime.UtcNow>until)throw new IOException("Chưa nhận được kết quả lưu. Không gửi lại; kiểm tra mã lệnh "+id+" rồi nạp lại nhân vật.");
            await Task.Delay(150,cancellation);
        }
        string code=File.ReadAllText(result).Trim();
        if(code=="-1")throw new IOException("Nhân vật đang đăng nhập hoặc còn khóa phiên. Chỉ cần thoát nhân vật này; giữ server chạy rồi nạp lại để lưu.");
        if(code=="-2")throw new IOException("Dữ liệu nhân vật đã thay đổi từ lúc mở. Nạp lại trước khi sửa; chưa ghi đè.");
        if(code!="1")throw new IOException("Dịch vụ dữ liệu từ chối lưu; bản gốc được giữ. Mã lệnh: "+id);
        ReadDisk(File.ReadAllBytes(FilePath));_ordinals.Clear();_ordinals.AddRange(Enumerable.Range(0,(int)ItemCount));_entryOverrides.Clear();LoadCompanion();
        // Acknowledged jobs are temporary transport, not permanent snapshots.
        try{
            string safeJob=Path.GetFullPath(job),prefix=Path.GetFullPath(queue)+Path.DirectorySeparatorChar;
            if(safeJob.StartsWith(prefix,StringComparison.OrdinalIgnoreCase)&&Path.GetFileName(safeJob)==id&&
               (File.GetAttributes(safeJob)&FileAttributes.ReparsePoint)==0&&
               Directory.EnumerateFileSystemEntries(safeJob,"*",SearchOption.AllDirectories).All(p=>(File.GetAttributes(p)&FileAttributes.ReparsePoint)==0)){
                Directory.Delete(safeJob,true);File.Delete(Path.Combine(queue,id+".done"));File.Delete(result);
            }
        }catch(IOException){}catch(UnauthorizedAccessException){}
    }
}
