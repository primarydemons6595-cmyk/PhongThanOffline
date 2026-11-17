using System.Text;
using System.Text.Json;

namespace PhongThan.Event.Runtime;

public sealed record BotPriceRule {
    public string Name {get;set;}="";
    public int Genre {get;set;}=-1;
    public int Detail {get;set;}=-1;
    public int Particular {get;set;}=-1;
    public int Level {get;set;}=-1;
    public int Quality {get;set;}=-1;
    public int MinVan {get;set;}=1;
    public int PriceVan {get;set;}=10;
    public int MaxVan {get;set;}=20;
}
public sealed record BotAutonomyProfile {
    public int Version {get;init;}=1;
    public long Revision {get;init;}
    public bool AutoRecover {get;init;}=true;
    public bool AdaptiveTraining {get;init;}=true;
    public bool AutoBag {get;init;}=true;
    public bool BossTeams {get;init;}=true;
    public bool VanTienTran {get;init;}=true;
    public bool ThuongChu {get;init;}=true;
    public bool RockPaperScissors {get;init;}=true;
    public int XuVan {get;init;}=60;
    public int MaxBuyVan {get;init;}=5000;
    public int SpendPercent {get;init;}=25;
    public int EquipmentPowerValue {get;init;}=1000;
    public int NationMode {get;init;}=2;
    public int MaxBotNations {get;init;}=3;
    public int ContributionPercent {get;init;}=5;
    public int CityLand {get;init;}
    public int CityTotem {get;init;}
    public bool SelfDefence {get;init;}=true;
    public bool ProactivePvp {get;init;}=false;
    public bool CallAllies {get;init;}=true;
    public int MaxAllies {get;init;}=2;
    public int PvpMinutes {get;init;}=20;
    public int PvpMinLevel {get;init;}=30;
    public int PvpLevelGap {get;init;}=15;
    public int DominationPercent {get;init;}=25;
    public int[] PvpMaps {get;init;}=[];
    public BotPriceRule[] Prices {get;init;}=[];
}
public sealed record BotAutonomySave(long ExpectedRevision,BotAutonomyProfile Profile);

// A single atomic file contains both the lossless editable profile and its
// bounded native projection. A failed save cannot publish half a price table.
public sealed class BotAutonomyStore {
    readonly object gate=new();
    readonly string path;
    public BotAutonomyStore(string root){path=Path.Combine(EventRuntimeLayout.Server(root),"BotAIgent","autonomy.ini");}
    public BotAutonomyProfile Read(){lock(gate){
        if(!File.Exists(path))return new();
        var line=File.ReadLines(path,Encoding.ASCII).FirstOrDefault(s=>s.StartsWith(";Profile=",StringComparison.Ordinal))??throw new InvalidDataException("Thiếu dữ liệu cấu hình bot.");
        var profile=JsonSerializer.Deserialize<BotAutonomyProfile>(Convert.FromBase64String(line[9..]))??throw new InvalidDataException("Cấu hình bot rỗng.");
        Validate(profile);return profile;
    }}
    public BotAutonomyProfile Save(BotAutonomySave request){lock(gate){
        var previous=Read();if(previous.Revision!=request.ExpectedRevision)throw new InvalidOperationException("Cấu hình bot đã thay đổi; tải lại trước khi lưu.");
        Validate(request.Profile);var next=request.Profile with{Revision=checked(previous.Revision+1)};
        var text=new StringBuilder("; Bot autonomy / local reference prices, not official market quotes.\n;Profile=");
        text.AppendLine(Convert.ToBase64String(JsonSerializer.SerializeToUtf8Bytes(next))).AppendLine("[Policy]");
        void Value(string name,object value)=>text.Append(name).Append('=').Append(value is bool b?b?"1":"0":Convert.ToString(value,System.Globalization.CultureInfo.InvariantCulture)).Append('\n');
        Value("Version",1);Value("Revision",next.Revision);Value("AutoRecover",next.AutoRecover);Value("AdaptiveTraining",next.AdaptiveTraining);Value("AutoBag",next.AutoBag);
        Value("BossTeams",next.BossTeams);Value("VanTienTran",next.VanTienTran);Value("ThuongChu",next.ThuongChu);Value("RockPaperScissors",next.RockPaperScissors);
        Value("XuVan",next.XuVan);Value("MaxBuyVan",next.MaxBuyVan);Value("SpendPercent",next.SpendPercent);Value("EquipmentPowerValue",next.EquipmentPowerValue);Value("PriceCount",next.Prices.Length);
        Value("NationMode",next.NationMode);Value("MaxBotNations",next.MaxBotNations);Value("ContributionPercent",next.ContributionPercent);Value("CityLand",next.CityLand);Value("CityTotem",next.CityTotem);
        Value("SelfDefence",next.SelfDefence);Value("ProactivePvp",next.ProactivePvp);Value("CallAllies",next.CallAllies);Value("MaxAllies",next.MaxAllies);Value("PvpMinutes",next.PvpMinutes);Value("PvpMinLevel",next.PvpMinLevel);Value("PvpLevelGap",next.PvpLevelGap);Value("DominationPercent",next.DominationPercent);
        Value("PvpMapCount",next.PvpMaps.Length);for(int i=0;i<next.PvpMaps.Length;i++)Value("PvpMap"+i,next.PvpMaps[i]);
        for(int i=0;i<next.Prices.Length;i++){
            var p=next.Prices[i];text.Append("[Price").Append(i).AppendLine("]");
            Value("Genre",p.Genre);Value("Detail",p.Detail);Value("Particular",p.Particular);Value("Level",p.Level);Value("Quality",p.Quality);
            Value("MinVan",p.MinVan);Value("PriceVan",p.PriceVan);Value("MaxVan",p.MaxVan);
        }
        EventControlJson.AtomicBytes(path,Encoding.ASCII.GetBytes(text.ToString()));return next;
    }}
    public static void Validate(BotAutonomyProfile p){
        if(p.Version!=1||p.Revision<0||p.XuVan is <1 or >50000||p.MaxBuyVan is <1 or >50000||p.SpendPercent is <1 or >50||p.EquipmentPowerValue is <1 or >100000||p.Prices is null||p.Prices.Length>128)
            throw new InvalidDataException("Tỷ giá, ngân sách hoặc bảng giá bot không hợp lệ (tối đa 128 dòng).");
        if(p.NationMode is <0 or >2||p.MaxBotNations is <1 or >25||p.ContributionPercent is <0 or >25||p.CityLand is <0 or >4||p.CityTotem is <0 or >2||p.MaxAllies is <0 or >5||p.PvpMinutes is <1 or >1440||p.PvpMinLevel is <12 or >200||p.PvpLevelGap is <0 or >50||p.DominationPercent is <0 or >100||p.PvpMaps is null||p.PvpMaps.Length>128||p.PvpMaps.Any(m=>m<1001||m>9999)||p.PvpMaps.Distinct().Count()!=p.PvpMaps.Length)
            throw new InvalidDataException("Thiết lập bang hội, lãnh địa hoặc PvP không hợp lệ. Tối đa 5 người hỗ trợ; map phải là mã riêng biệt.");
        var keys=new HashSet<string>();
        foreach(var r in p.Prices){
            if(r is null||r.Name is null||r.Name.Length>80||r.Name.Any(char.IsControl)||r.Genre is <-1 or >20||r.Detail is <-1 or >100000||r.Particular is <-1 or >100000||r.Level is <-1 or >1000||r.Quality is <-1 or >20||r.MinVan<1||r.MaxVan>50000||r.MinVan>r.PriceVan||r.PriceVan>r.MaxVan)
                throw new InvalidDataException("Mỗi giá cần: 1 ≤ thấp nhất ≤ tham chiếu ≤ cao nhất ≤ 50.000 vạn. Mã -1 áp dụng cho mọi giá trị.");
            if(!keys.Add($"{r.Genre}/{r.Detail}/{r.Particular}/{r.Level}/{r.Quality}"))throw new InvalidDataException("Trùng nhóm vật phẩm trong bảng giá.");
            if(r.Genre==4&&(r.Detail is 46 or 47))throw new InvalidDataException("Xu và khối xu dùng tỷ giá chung, không thêm vào bảng giá đồ.");
        }
    }
}
