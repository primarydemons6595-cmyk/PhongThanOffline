using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;

public sealed record BotBatchRequest(long ExpectedRevision, BotProfile[] Profiles);
public sealed record BotSpawn(int Map,int X,int Y,string Place);
public static class BotCreation {
    // -1 distributes new roles across the six profession/sex combinations.
    public static BotProfile[] ServiceBatch(string name,int count,bool random,int group,IEnumerable<BotProfile> existing) {
        if(group is < -1 or >5)throw new InvalidDataException("Nhóm bot không hợp lệ.");
        var template=new BotProfile{Name=name,Enabled=true,Activity="auto",Personality="practical"};
        var profiles=Batch(template,count,random,existing);
        return profiles.Select((b,i)=>{
            int selected=group<0?i%6:group,profession=selected/2,gender=selected%2;
            var spawn=Starter(profession);
            return b with{Profession=profession,Gender=gender,Map=spawn.Map,X=spawn.X,Y=spawn.Y};
        }).ToArray();
    }
    public static BotProfile[] ServiceNamedBatch(string[] names,int group,IEnumerable<BotProfile> existing) {
        if(group is < -1 or >5)throw new InvalidDataException("Nhóm bot không hợp lệ.");
        var all=existing.ToList();
        if(names.Length<1||names.Length>BotLimits.Active-all.Count(b=>!b.Deleted))throw new InvalidDataException($"Chỉ còn chỗ cho {Math.Max(0,BotLimits.Active-all.Count(b=>!b.Deleted))} bot; danh sách có {names.Length} tên.");
        var result=new List<BotProfile>();
        for(int i=0;i<names.Length;i++){
            int selected=group<0?i%6:group;
            try{
                var bot=ServiceBatch(names[i],1,false,selected,all).Single();result.Add(bot);all.Add(bot);
            }catch(InvalidDataException ex){throw new InvalidDataException($"Tên thứ {i+1} '{names[i]}': {ex.Message}",ex);}
        }
        return result.ToArray();
    }
    // Mirrors Headers/PhongThanStarterSpawn.h, applied to fresh roles by Goddess.
    public static BotSpawn Starter(string root,int profession)=>Starter(profession);
    public static BotSpawn Starter(int profession) {
        if(profession is <0 or >2)throw new InvalidDataException("Chỉ hỗ trợ ba môn phái đang được tạo trong game.");
        return new(1002+profession,new[]{201,217,196}[profession]*256+128,new[]{199,201,203}[profession]*512+256,
            new[]{"Sùng Thành Doanh","Ngọc Hư Cung","Xi Vưu Mộ"}[profession]);
    }
    public static string RandomName(IEnumerable<string> existing) {
        var used=new HashSet<string>(existing,StringComparer.OrdinalIgnoreCase);
        string[] first=["Thanh","Thien","Minh","Lam","Bach","Hac","Long","Van","Phong","Ngoc","Tieu","Hai"];
        string[] last=["Van","Phong","Kiem","Long","Vu","Son","Linh","Nguyet","Hoa","Bao","Anh","Tam"];
        for(int i=0;i<1000;i++) {
            string name=first[RandomNumberGenerator.GetInt32(first.Length)]+last[RandomNumberGenerator.GetInt32(last.Length)]+RandomNumberGenerator.GetInt32(1000,10000);
            if(!used.Contains(name))return name;
        }
        throw new InvalidOperationException("Không tạo được tên mới; hãy thử lại.");
    }
    public static BotProfile[] Batch(BotProfile template,int count,bool random,IEnumerable<BotProfile> existing) {
        var old=existing.ToArray();if(count<1||count>BotLimits.Active-old.Count(b=>!b.Deleted))throw new InvalidDataException($"Số lượng phải từ 1 đến {Math.Max(0,BotLimits.Active-old.Count(b=>!b.Deleted))} (tối đa 500 bot chưa xóa; bot đã xóa không chiếm chỗ).");
        if(count>1&&!random)throw new InvalidDataException("Tạo nhiều bot cần bật Tên ngẫu nhiên.");
        var names=old.Select(b=>b.Name.Normalize(NormalizationForm.FormC)).ToList();var result=new List<BotProfile>();
        for(int i=0;i<count;i++) {
            string name=random?RandomName(names):template.Name.Normalize(NormalizationForm.FormC);
            if(!BotNames.Valid(name)||names.Contains(name,StringComparer.OrdinalIgnoreCase))throw new InvalidDataException("Tên không hợp lệ hoặc đã tồn tại.");
            names.Add(name);result.Add(template with{Id=Guid.NewGuid().ToString("N"),Name=name,Deleted=false});
        }
        return result.ToArray();
    }
}
