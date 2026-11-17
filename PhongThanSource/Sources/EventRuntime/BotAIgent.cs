using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;

public static class BotLimits {public const int Active=500,History=4096;}

public sealed record BotProfile {
    public string Id {get;init;}=Guid.NewGuid().ToString("N");
    public string Name {get;init;}="";
    public bool Enabled {get;init;}
    public bool Deleted {get;init;}
    public int Profession {get;init;}
    public int Gender {get;init;}
    public int Map {get;init;}=1002;
    public int X {get;init;}
    public int Y {get;init;}
    public string Activity {get;init;}="idle";
    public string Personality {get;init;}="practical";
}
public sealed record BotConfiguration {
    public int Version {get;init;}=1;
    public long Revision {get;init;}
    public bool Running {get;init;}
    public bool LocalAiEnabled {get;init;}
    public bool RememberChats {get;init;}=true;
    public bool SellAdsEnabled {get;init;}=true;
    public bool BuyAdsEnabled {get;init;}=true;
    public BotProfile[] Bots {get;init;}=[];
}
[System.Text.Json.Serialization.JsonUnmappedMemberHandling(System.Text.Json.Serialization.JsonUnmappedMemberHandling.Skip)]
public sealed record BotLive(string Id,int Player,int Entity,int Map,int X,int Y,int Life,int MaxLife,int Level,string State,string Error){public int Team {get;init;}=-1;public int TargetMap {get;init;}public int TargetLevel {get;init;}public uint TargetEntity {get;init;}public bool Sleeping {get;init;}public uint MobKills {get;init;}public int Command {get;init;}public int CommandOwner {get;init;}public int PendingCommand {get;init;}}
public sealed record BotSocialObserver(string Key,string NameHex,int Map,int X,int Y);
[System.Text.Json.Serialization.JsonUnmappedMemberHandling(System.Text.Json.Serialization.JsonUnmappedMemberHandling.Skip)]
public sealed record BotNativeStatus(int Version,int Pid,long UtcSeconds,long Revision,BotLive[] Bots){public int SocialVersion {get;init;}public int CommandProtocol {get;init;}public BotSocialObserver[] Observers {get;init;}=[];}
public sealed record BotDashboard(BotConfiguration Configuration,BotNativeStatus? Native,bool Fresh,string[] Logs){public BotScheduleEntry[] Schedule {get;init;}=[];}
public sealed record BotSaveRequest(long ExpectedRevision,BotProfile Profile);
public sealed record BotControlRequest(long ExpectedRevision,bool Running,bool LocalAiEnabled){public bool? RememberChats {get;init;}public bool? SellAdsEnabled {get;init;}public bool? BuyAdsEnabled {get;init;}}
public sealed record BotDeleteRequest(long ExpectedRevision,string Id);
public static class BotConsole {
    public static string State(string value)=>value switch{"npc_goal_travel"=>"Đi đến NPC mục tiêu","npc_goal_dialog"=>"Tương tác NPC mục tiêu","bank_materials_travel"=>"Đi lấy nguyên liệu trong rương","saving_recipe_fee"=>"Tích tiền ghép đồ","saving_for_recipe"=>"Tìm nguyên liệu ghép đồ","offline"=>"Offline","idle"=>"Đang nghỉ","training"=>"Đang đánh quái","combat_blocked"=>"Lệnh đánh bị từ chối","shopping"=>"Mua / bán tại NPC","shopping_travel"=>"Đi đến thương nhân","post_death_rest"=>"Nghỉ sau hồi sinh","quest_travel"=>"Đi gặp NPC nhiệm vụ","quest_dialog"=>"Nhận / trả nhiệm vụ","recovering"=>"Ngồi hồi phục","retreating"=>"Rút về nơi an toàn","party_follow"=>"Đi cùng người chơi / tổ đội","companion_wait"=>"Đứng chờ người chơi","awaiting_gift"=>"Chờ nhận quà giao dịch","purchase_blocked"=>"Chưa mua được món được nhờ","busy_transaction"=>"Đang giao dịch","travel_training"=>"Đi đến bãi train","seeking_training"=>"Tìm bãi vừa cấp","path_blocked"=>"Đang tìm đường khác","awaiting_target"=>"Chờ xác định đúng mục tiêu","pk_blocked"=>"Luật PK / tổ đội đang chặn","returning_to_target"=>"Hồi sinh rồi quay lại mục tiêu","using_travel_item"=>"Dùng phù đang sở hữu","boss_travel"=>"Đi tới boss","sale_negotiating"=>"Bàn giá món đang bán","trade_meeting_travel"=>"Đi tới chỗ hẹn","trade_meeting_wait"=>"Đã tới, chờ giao dịch","awaiting_rps"=>"Chờ mời oẳn tù tì","vtt_entry_blocked"=>"Chưa vào được Vạn Tiên Trận","vtt_waiting_start"=>"Chờ Vạn Tiên Trận bắt đầu","vtt_seeking_objective"=>"Tìm mục tiêu trong trận","vtt_finished"=>"Trận đã kết thúc","event_travel"=>"Đi tới NPC sự kiện","event_dialog"=>"Tương tác NPC sự kiện","dead"=>"Chờ hồi sinh","error"=>"Lỗi",_=>value};
    public static string Format(BotProfile[] profiles,string line){
        foreach(var bot in profiles)line=line.Replace("["+bot.Id+"]","["+bot.Name+" / "+bot.Id[..8]+"]",StringComparison.Ordinal);
        return line.Replace("online_server_player","Đã vào game bằng nhân vật server",StringComparison.Ordinal)
            .Replace("offline_saved","Đã lưu nhân vật và offline",StringComparison.Ordinal)
            .Replace("training_started","Bắt đầu train",StringComparison.Ordinal)
            .Replace("pickup_accepted","Server đã chấp nhận nhặt đồ",StringComparison.Ordinal)
            .Replace("whisper_replied_from_authoritative_state","Đã trả lời tin nhắn theo trạng thái thật",StringComparison.Ordinal)
            .Replace("remembered_killer","Đã ghi nhớ người chơi đánh chết bot",StringComparison.Ordinal);
    }
}

// All mutations are called by Event Service's authenticated, serialized request loop.
public sealed class BotAIgentStore {
    readonly object gate=new();
    readonly string folder,config;
    public BotAIgentStore(string root) {folder=Path.Combine(EventRuntimeLayout.Server(root),"BotAIgent");Directory.CreateDirectory(folder);config=Path.Combine(folder,"bots.json");}
    public BotConfiguration Configuration() {lock(gate)return File.Exists(config)?EventControlJson.Read<BotConfiguration>(config):new();}
    static void Validate(BotConfiguration c) {
        if(c.Version!=1||(c.Bots.Length>BotLimits.History||c.Bots.Count(b=>!b.Deleted)>BotLimits.Active)||c.Bots.Select(b=>b.Id).Distinct().Count()!=c.Bots.Length||c.Bots.Select(b=>b.Name.ToLowerInvariant()).Distinct().Count()!=c.Bots.Length)throw new InvalidDataException("Tối đa 500 bot chưa xóa và 4096 hồ sơ gồm lịch sử; tên và ID không được trùng.");
        foreach(var b in c.Bots)if(!Regex.IsMatch(b.Id,@"\A[0-9a-f]{32}\z")||!BotNames.Valid(b.Name)||b.Profession is <0 or >3||b.Gender is <0 or >1||b.Map is <1 or >65535||b.X is <1 or >2000000||b.Y is <1 or >2000000||b.Activity is not("idle" or "train" or "auto")||b.Personality is not("practical" or "friendly" or "grudge"))throw new InvalidDataException("Bot: tên 3–24 byte TCVN3, môn phái 0–3, giới tính 0–1, map và tọa độ MPS hợp lệ.");
    }
    BotConfiguration Save(long expected,Func<BotConfiguration,BotConfiguration> change) {lock(gate) {
        var old=Configuration();if(old.Revision!=expected)throw new InvalidOperationException("Cấu hình bot đã đổi. Hãy tải lại.");
        var next=change(old) with {Revision=checked(old.Revision+1)};Validate(next);EventControlJson.Atomic(config,next);Publish(DateTimeOffset.UtcNow);return next;
    }}
    public BotConfiguration Upsert(BotSaveRequest r)=>Save(r.ExpectedRevision,c=> {
        var old=c.Bots.SingleOrDefault(b=>b.Id==r.Profile.Id);
        if(old is not null&&(old.Name!=r.Profile.Name||old.Profession!=r.Profile.Profession||old.Gender!=r.Profile.Gender))throw new InvalidOperationException("Danh tính/môn phái bot đã tạo không thể đổi; tạo bot mới.");
        if(old?.Deleted==true)throw new InvalidOperationException("Bot đã được lưu trữ.");
        return c with{Bots=c.Bots.Where(b=>b.Id!=r.Profile.Id).Append(r.Profile with{Deleted=false}).ToArray()};
    });
    public BotConfiguration Control(BotControlRequest r)=>Save(r.ExpectedRevision,c=>{
        if(r.LocalAiEnabled&&new BotAiSettingsStore(folder).Profile().Provider=="local"&&!BotLocalChat.Installed(folder))throw new InvalidOperationException("Chưa cài model AI local. Chọn Online hoặc dùng hội thoại quy tắc.");
        return c with{Running=r.Running,LocalAiEnabled=r.LocalAiEnabled,RememberChats=r.RememberChats??c.RememberChats,SellAdsEnabled=r.SellAdsEnabled??c.SellAdsEnabled,BuyAdsEnabled=r.BuyAdsEnabled??c.BuyAdsEnabled};
    });
    public BotConfiguration CreateBatch(BotBatchRequest r)=>Save(r.ExpectedRevision,c=> {
        if(r.Profiles is null||r.Profiles.Length<1||r.Profiles.Length>BotLimits.Active)throw new InvalidDataException("Số lượng bot phải từ 1 đến 500.");
        if(r.Profiles.Any(b=>b is null||b.Deleted||c.Bots.Any(old=>old.Id==b.Id)))throw new InvalidDataException("Chỉ được thêm hồ sơ bot mới.");
        return c with{Bots=c.Bots.Concat(r.Profiles).ToArray()};
    });
    public BotConfiguration Delete(BotDeleteRequest r)=>Save(r.ExpectedRevision,c=> {
        if(!c.Bots.Any(b=>b.Id==r.Id))throw new InvalidDataException("Không tìm thấy bot.");
        return c with{Bots=c.Bots.Select(b=>b.Id==r.Id?b with{Enabled=false,Deleted=true}:b).ToArray()};
    });
    public void Publish(DateTimeOffset now) {lock(gate) {
        var c=Configuration();Validate(c);var text=new StringBuilder($"PT_BOTAIGENT\t2\t{c.Revision}\t{now.ToUnixTimeSeconds()+10}\n");
        var schedule=new BotSchedule(folder).Advance(c,now);
        foreach(var b in c.Bots.Where(b=>!b.Deleted))text.AppendJoin('\t',b.Id,BotNames.WireHex(b.Name),c.Running&&b.Enabled&&!b.Deleted?1:0,b.Profession,b.Gender,b.Map,b.X,b.Y,b.Activity=="train"||(b.Activity=="auto"&&schedule.Any(e=>e.Id==b.Id&&e.Work=="train"))?1:0,b.Activity=="auto"?1:0).Append('\n');
        EventControlJson.AtomicBytes(Path.Combine(folder,"control.tsv"),Encoding.ASCII.GetBytes(text.ToString()));
        EventControlJson.AtomicBytes(Path.Combine(folder,"market-policy.ini"),Encoding.ASCII.GetBytes($"[Market]\nSellAds={(c.SellAdsEnabled?1:0)}\nBuyAds={(c.BuyAdsEnabled?1:0)}\n"));
    }}
    public BotDashboard Status(DateTimeOffset now) {lock(gate) {
        BotNativeStatus? native=null;string path=Path.Combine(folder,"status.json");
        try{if(File.Exists(path))native=EventControlJson.Read<BotNativeStatus>(path);}catch(Exception e) when(e is IOException or System.Text.Json.JsonException){ }
        bool fresh=native is not null&&native.Version==1&&now.ToUnixTimeSeconds()-native.UtcSeconds is >=0 and <=5;
        if(fresh)try{using var process=System.Diagnostics.Process.GetProcessById(native!.Pid);fresh=process.ProcessName=="GameServer"&&string.Equals(process.MainModule?.FileName,Path.Combine(Path.GetDirectoryName(folder)!,"GameServer.exe"),StringComparison.OrdinalIgnoreCase);}catch(Exception e) when(e is ArgumentException or InvalidOperationException or System.ComponentModel.Win32Exception){fresh=false;}
        var logLines=new List<string>();
        foreach(string name in new[]{"activity.log","chat.log"})try{string log=Path.Combine(folder,name);if(File.Exists(log)){using var stream=new FileStream(log,FileMode.Open,FileAccess.Read,FileShare.ReadWrite|FileShare.Delete);if(stream.Length>65536)stream.Seek(-65536,SeekOrigin.End);using var reader=new StreamReader(stream,Encoding.UTF8);logLines.AddRange(reader.ReadToEnd().Split('\n',StringSplitOptions.RemoveEmptyEntries).TakeLast(200));}}catch(IOException){}
        string[] lines=logLines.Order(StringComparer.Ordinal).TakeLast(300).ToArray();
        var configuration=Configuration();return new(configuration,native,fresh,lines){Schedule=new BotSchedule(folder).Advance(configuration,now)};
    }}
}

public sealed record BotMemoryEvent(string EventId,DateTimeOffset Utc,string PlayerId,string Kind,string Detail,int TrustDelta,int HostilityDelta);
public sealed record BotRelationship(int Trust,int Hostility,BotMemoryEvent[] Events);
public static class BotMemory {
    public static BotRelationship Apply(BotRelationship previous,BotMemoryEvent e) {
        if(previous.Events.Any(x=>x.EventId==e.EventId))return previous;
        return new(Math.Clamp(previous.Trust+e.TrustDelta,-100,100),Math.Clamp(previous.Hostility+e.HostilityDelta,0,100),previous.Events.Append(e).TakeLast(64).ToArray());
    }
    public static string HelpReply(BotRelationship relation,string activity,int offeredValue,int minimumFee) {
        if(activity!="idle")return "Mình đang bận, chưa đi giúp được.";
        if(relation.Hostility>=60&&offeredValue<minimumFee)return "Lần trước còn phá mình, giờ lại nhờ giúp à?";
        if(offeredValue>=minimumFee&&minimumFee>0)return "Mức thù lao đó được. Mình chỉ nhận việc sau khi giao dịch hoàn tất.";
        if(relation.Trust<0)return "Mình chưa tin bạn đủ để nhận việc này.";
        return "Bạn cần giúp ở đâu? Cho mình biết mục tiêu và tổ đội trước.";
    }
}
