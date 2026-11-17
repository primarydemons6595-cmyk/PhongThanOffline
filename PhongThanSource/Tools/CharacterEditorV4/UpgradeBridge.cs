using System.Diagnostics;
using System.Text;

namespace PhongThanCharacterEditor;

public sealed record UpgradeRule(int Id,string Name)
{
    public override string ToString()=>$"{Id} · {Name}";
}

public static class UpgradeBridge
{
    public static IReadOnlyList<UpgradeRule> Rules(ItemRecord item,string serverRoot){
        if(item.Genre!=0)return Array.Empty<UpgradeRule>();
        using var resources=new ClientAssets(serverRoot);
        var bytes=resources.Read("settings/item/001/zhuang_bei_he_cheng_gui_ze_biao.txt");
        if(bytes==null)return Array.Empty<UpgradeRule>();
        var names=new Dictionary<int,string>();
        string? tablePart=item.DetailType switch{0 or 1=>"wu_qi",2=>"kui_jia",7=>"tou_kui",5=>"xie_zi",6=>"yao_dai",9=>"pi_feng",_=>null};
        if(tablePart!=null)foreach(string profession in new[]{"jia_shi","shu_shi","yi_ren"}){var table=resources.Read("settings/item/001/"+profession+"_"+tablePart+"_sheng_ji_shu_zhi_biao.txt");if(table==null)continue;foreach(var line in Encoding.Latin1.GetString(table).Split('\n').Skip(1)){var row=line.TrimEnd('\r').Split('\t');if(row.Length>17&&int.TryParse(row[0],out int id))names.TryAdd(id,ClientAssets.Tcvn(row[17].TrimStart('#')).Trim());}}
        var rules=new Dictionary<int,UpgradeRule>();
        foreach(var line in Encoding.Latin1.GetString(bytes).Split('\n').Skip(1)){
            var row=line.TrimEnd('\r').Split('\t');if(row.Length<19)continue;
            if(!int.TryParse(row[1],out int id)||id<=0||row[14]!="1"||row[15]!="1"||!string.IsNullOrWhiteSpace(row[17]))continue;
            var input=row[3].Split('(')[0].Split('.');var output=row[13].Split('(')[0].Split('.');
            if(input.Length!=4||output.Length!=4||!int.TryParse(input[0],out int genre)||!int.TryParse(input[1],out int detail)||!int.TryParse(input[2],out int part)||input[3]!="1"||output[0]!=input[0]||output[1]!=input[1]||output[2]!=input[2]||output[3]!="1")continue;
            if(genre!=0||detail!=item.DetailType||part!=item.ParticularType)continue;
            string name=names.GetValueOrDefault(id,ClientAssets.Tcvn(row[18].TrimStart('#')).Trim());if(name.Length>55)name=name[..55]+"…";
            rules.TryAdd(id,new(id,string.IsNullOrEmpty(name)?"Quy tắc trong bảng gốc":name));
        }
        return rules.Values.OrderBy(r=>r.Id).ToArray();
    }
    public static async Task<byte[]> RunAsync(byte[] input,string serverRoot,int level,int rule,CancellationToken cancellation=default){
        if(input.Length!=1016||level<0||level>12||rule<0||rule>65535||(level>0&&rule==0))throw new ArgumentException("Mức cường hóa/rule không hợp lệ (0–12).");
        string helper=Path.Combine(AppContext.BaseDirectory,"Native","UpgradeNative.exe");
        if(!File.Exists(helper))throw new FileNotFoundException("Thiếu bộ tính cường hóa native. Cần giữ thư mục Native cạnh editor.",helper);
        string root=Path.GetFullPath(Path.Combine(Path.GetTempPath(),"PhongThanEditorUpgrade"));string job=Path.GetFullPath(Path.Combine(root,Guid.NewGuid().ToString("N")));
        if(!job.StartsWith(root+Path.DirectorySeparatorChar,StringComparison.OrdinalIgnoreCase))throw new IOException("Invalid job path.");
        Directory.CreateDirectory(job);string source=Path.Combine(job,"input.bin"),target=Path.Combine(job,"output.bin");File.WriteAllBytes(source,input);
        try{
            var info=new ProcessStartInfo(helper){UseShellExecute=false,CreateNoWindow=true,RedirectStandardOutput=true,RedirectStandardError=true,WorkingDirectory=job};
            foreach(string arg in new[]{Path.GetFullPath(serverRoot),source,level.ToString(),rule.ToString(),target})info.ArgumentList.Add(arg);
            using var process=Process.Start(info)??throw new IOException("Không mở được bộ tính native.");
            var stdout=process.StandardOutput.ReadToEndAsync(cancellation);var stderr=process.StandardError.ReadToEndAsync(cancellation);
            using var timeout=CancellationTokenSource.CreateLinkedTokenSource(cancellation);timeout.CancelAfter(TimeSpan.FromSeconds(60));
            try{await process.WaitForExitAsync(timeout.Token);}catch(OperationCanceledException){if(!process.HasExited)process.Kill(true);await process.WaitForExitAsync(CancellationToken.None);throw new IOException("Bộ tính native quá thời gian; vật phẩm chưa bị thay đổi.");}
            string log=await stdout+await stderr;
            if(process.ExitCode!=0||!File.Exists(target)){string error=log.Split('\n').LastOrDefault(l=>l.StartsWith("UPGRADE_ERROR"))??log.Split('\n').LastOrDefault(l=>!string.IsNullOrWhiteSpace(l))??"Native không trả kết quả.";throw new InvalidDataException(error.Trim());}
            var result=File.ReadAllBytes(target);if(result.Length!=1016)throw new InvalidDataException("Kết quả cường hóa sai kích thước.");return result;
        }finally{if(Directory.Exists(job))Directory.Delete(job,true);}
    }
}
