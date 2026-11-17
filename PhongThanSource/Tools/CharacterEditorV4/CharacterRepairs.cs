using System.Buffers.Binary;
using System.Text;
using System.Text.Json;

namespace PhongThanCharacterEditor;

public sealed record LearnedSkill(int Index,int Id,int Level,int Experience);
public sealed record PotentialRepair(int[] Before,int[] After,int Consumed,int Added,int[] Vitals);

public sealed partial class PthcFile
{
    public IReadOnlyList<LearnedSkill> LearnedSkills()=>Enumerable.Range(0,checked((int)FightSkillCount))
        .Select(i=>{int o=FileHeaderSize+StateHeaderSize+i*12;return new LearnedSkill(i,BitConverter.ToInt32(_bytes,o),BitConverter.ToInt32(_bytes,o+4),BitConverter.ToInt32(_bytes,o+8));}).ToArray();

    public void SetSkillLevel(int index,int level,int maximum){
        if(!CanSave)throw new InvalidOperationException(CompanionStatus);
        if(index<0||index>=FightSkillCount||maximum<1||maximum>63||level<1||level>maximum)
            throw new InvalidDataException("Cấp kỹ năng vượt giới hạn hợp lệ.");
        int o=FileHeaderSize+StateHeaderSize+index*12;
        BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(o+4,4),level);
        // Training progress belongs to the old level and must not be carried into the new one.
        BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(o+8,4),0);
    }

    public PotentialRepair PreviewPotential(string serverRoot,int level){
        if(Profession>3||level<1||level>200)throw new InvalidDataException("Phái hoặc cấp độ không có trong bảng tăng trưởng gốc.");
        if(_bytes[FileHeaderSize+88]!=0)throw new InvalidDataException("Nhân vật đã chuyển sinh: cần đối chiếu lịch sử chuyển sinh trước khi bù, tránh cộng sai điểm.");
        using var stream=typeof(PthcFile).Assembly.GetManifestResourceStream("PhongThanCharacterEditor.LevelGrowth.json")!;
        var tables=JsonSerializer.Deserialize<Dictionary<string,int[][]>>(stream)!;
        int conversion=Profession==3?TaskValue(4894):0;
        if(Profession==3&&(conversion<1||conversion>200||conversion>level))
            throw new InvalidDataException("Vũ Sĩ chưa có mốc chuyển từ Giáp Sĩ hợp lệ. Có thể chỉnh/lưu thủ công; không tự suy đoán chỉ số để bù tiềm năng.");
        int originalProfession=Profession==3?0:Profession;
        var initial=ReadIniNumbers(Path.Combine(serverRoot,"settings","npc","player",$"newplayerini{originalProfession*2+Gender:00}.ini"));
        string[] keys={"ipower","iagility","iouter","iinside"};
        int[] target=keys.Select(k=>initial.TryGetValue(k,out int v)?v:throw new InvalidDataException("Thiếu thuộc tính khởi tạo gốc.")).ToArray();
        for(int n=1;n<level;n++){
            int profession=Profession==3&&n<conversion?0:Profession;
            var growth=tables[$"ptGrowth0_{profession}"];
            for(int a=0;a<4;a++)target[a]=checked(target[a]+growth[n][a]);
        }
        int[] before=Enumerable.Range(0,4).Select(a=>ReadS32(224+a*4)).ToArray();
        if(before.Any(v=>v<0)||RemainingAttributePoints<0)throw new InvalidDataException("Thuộc tính hoặc điểm tiềm năng đang âm; cần kiểm tra dữ liệu.");
        int[] after=before.Zip(target,Math.Max).ToArray();
        int needed=after.Zip(before,(a,b)=>a-b).Sum(),consume=Math.Min(needed,RemainingAttributePoints);
        var lines=File.ReadAllLines(Path.Combine(serverRoot,"settings","npc","player","level_add.txt"),Encoding.Latin1);
        var header=lines[0].Split('\t');var values=lines[Profession+1].Split('\t');
        int Rate(string key){int i=Array.IndexOf(header,key);if(i<0||i>=values.Length||!int.TryParse(values[i],out int v)||v<0)throw new InvalidDataException("Thiếu hệ số sinh lực/nội lực gốc.");return v;}
        int dv=after[2]-before[2],de=after[3]-before[3];
        int[] vitals={checked(ReadS32(244)+dv*Rate("LifePerVitality")),checked(ReadS32(248)+dv*Rate("StaminaPerVitality")),checked(ReadS32(252)+de*Rate("ManaPerEnergy"))};
        return new(before,after,consume,needed-consume,vitals);
    }
    public void SupplementPotential(string serverRoot,int level){
        if(!CanSave)throw new InvalidOperationException(CompanionStatus);
        var plan=PreviewPotential(serverRoot,level);
        for(int a=0;a<4;a++)WriteS32(224+a*4,plan.After[a]);
        for(int a=0;a<3;a++)WriteS32(244+a*4,plan.Vitals[a]);
        RemainingAttributePoints-=plan.Consumed;
    }
    private static Dictionary<string,int> ReadIniNumbers(string file){
        var result=new Dictionary<string,int>(StringComparer.OrdinalIgnoreCase);
        foreach(string raw in File.ReadAllLines(file,Encoding.Latin1)){var p=raw.Split('=',2);if(p.Length==2&&int.TryParse(p[1].Trim(),out int v))result[p[0].Trim()]=v;}return result;
    }

    // Recover only an exact, content-bound snapshot. Never regenerate equipment properties or serials.
    public string RepairCompanion(IEnumerable<string> searchRoots){
        if(CanSave)return "Companion đã hợp lệ, không cần sửa.";
        if(!_bytes.AsSpan().SequenceEqual(_original))throw new InvalidOperationException("Nạp lại nhân vật trước khi khôi phục Companion.");
        string key=Hash(_bytes[16..]);
        var roots=searchRoots.Prepend(CompanionRoot).Where(Directory.Exists).Select(Path.GetFullPath).Distinct(StringComparer.OrdinalIgnoreCase).ToArray();
        foreach(string root in roots){
            foreach(string file in Directory.EnumerateFiles(root,key+".magic-v1",new EnumerationOptions{RecurseSubdirectories=true,IgnoreInaccessible=true,AttributesToSkip=FileAttributes.ReparsePoint})){
                byte[] c;try{c=File.ReadAllBytes(file);ValidateRecoveredCompanion(c,key);}catch(IOException){continue;}catch(InvalidDataException){continue;}
                _companion=c;CompanionStatus="Companion đã phục hồi · giữ nguyên serial và thuộc tính. Cần Lưu để áp dụng.";
                return "Đã tìm thấy bản Companion khớp hoàn toàn. Nhấn Lưu trực tiếp hoặc Xuất bộ lưu để áp dụng.";
            }
        }
        // A backup may differ in revision, skills or currency but must contain exactly
        // the same role/account and complete item records in the same order. Conflicting
        // serial/property payloads are ambiguous and must never be selected arbitrarily.
        byte[]? recovered=null;
        foreach(string root in roots)foreach(string file in Directory.EnumerateFiles(root,"role_*.pthc*",new EnumerationOptions{RecurseSubdirectories=true,IgnoreInaccessible=true,AttributesToSkip=FileAttributes.ReparsePoint})){
            try{
                if(new FileInfo(file).Length>FileHeaderSize+65536)continue;
                var candidate=Load(file);
                if(!candidate.CanSave||candidate.RoleName!=RoleName||candidate.AccountName!=AccountName||candidate.ItemCount!=ItemCount)continue;
                bool same=true;for(int i=0;i<ItemCount;i++)same&=candidate.ItemBytes(i).SequenceEqual(ItemBytes(i));
                if(!same)continue;
                byte[] payload=candidate._companion!;
                if(recovered!=null&&!recovered.AsSpan(144).SequenceEqual(payload.AsSpan(144)))throw new InvalidOperationException("Có nhiều bản sao lưu cùng đồ nhưng khác serial/thuộc tính. Cần chọn thư mục chứa đúng một bản sao lưu đã xác minh.");
                recovered=(byte[])payload.Clone();
            }catch(IOException){continue;}catch(InvalidDataException){continue;}catch(ArgumentException){continue;}
        }
        if(recovered!=null){
            Encoding.ASCII.GetBytes(key).CopyTo(recovered,16);ValidateRecoveredCompanion(recovered,key);_companion=recovered;
            CompanionStatus="Companion phục hồi từ bộ đồ khớp hoàn toàn · cần Lưu để áp dụng.";
            return "Đã phục hồi từ bản sao lưu cùng nhân vật và toàn bộ vật phẩm khớp. Giữ nguyên serial/thuộc tính. Nhấn Lưu để áp dụng.";
        }
        throw new InvalidDataException("Không tìm được Companion khớp dữ liệu nhân vật. Hãy chọn thư mục bản sao lưu có Companion; không thể tự tạo lại thuộc tính hoặc serial đã mất.");
    }
    private void ValidateRecoveredCompanion(byte[] c,string key){
        if(c.Length!=144+748*(long)ItemCount||BitConverter.ToUInt32(c,0)!=0x474d3050||(BitConverter.ToUInt32(c,4)!=1&&BitConverter.ToUInt32(c,4)!=2)||BitConverter.ToUInt32(c,8)!=748||BitConverter.ToUInt32(c,12)!=ItemCount||Encoding.ASCII.GetString(c,16,64)!=key||Encoding.ASCII.GetString(c,80,64)!=Hash(c[144..]))throw new InvalidDataException("Companion không khớp.");
        var serials=new HashSet<uint>();
        for(int i=0;i<ItemCount;i++){
            int o=144+i*748;uint serial=BitConverter.ToUInt32(c,o+4);
            if(BitConverter.ToUInt32(c,o)!=(uint)i||serial==0||serial>=0xfffffffe||!serials.Add(serial))throw new InvalidDataException("Serial Companion không hợp lệ.");
            for(int a=0;a<20;a++){int type=BitConverter.ToInt32(c,o+12+a*16);if(type<0||type>=(BitConverter.ToUInt32(c,4)==1?357:387))throw new InvalidDataException("Thuộc tính Companion không hợp lệ.");}
        }
    }
}
