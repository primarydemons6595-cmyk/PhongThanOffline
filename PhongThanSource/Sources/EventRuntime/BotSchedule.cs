using System.Security.Cryptography;

namespace PhongThan.Event.Runtime;

public sealed record BotScheduleEntry(string Id,string Work,long UntilUtc,int Session);
public sealed record BotScheduleState(int Version,BotScheduleEntry[] Entries);
public sealed class BotSchedule {
    readonly string path;readonly object gate=new();
    public BotSchedule(string folder){path=Path.Combine(folder,"schedule.json");}
    public BotScheduleEntry[] Advance(BotConfiguration configuration,DateTimeOffset now) {lock(gate){
        var old=File.Exists(path)?EventControlJson.Read<BotScheduleState>(path):new(1,[]);
        if(old.Version!=1)throw new InvalidDataException("Phiên bản lịch bot không hợp lệ.");
        long utc=now.ToUnixTimeSeconds();bool changed=false;
        var entries=new List<BotScheduleEntry>();
        foreach(var bot in configuration.Bots.Where(b=>!b.Deleted&&b.Activity=="auto")) {
            var entry=old.Entries.SingleOrDefault(e=>e.Id==bot.Id);
            if(entry is null){entry=new(bot.Id,"rest",utc+RandomNumberGenerator.GetInt32(5,121),0);changed=true;}
            // Preserve the chosen deadline on restart; never replay missed sessions.
            if(configuration.Running&&bot.Enabled&&entry.UntilUtc<=utc){
                bool missed=utc-entry.UntilUtc>60,rest=missed||entry.Work=="train";
                entry=new(bot.Id,rest?"rest":"train",utc+(missed?RandomNumberGenerator.GetInt32(5,121):rest?RandomNumberGenerator.GetInt32(25,181):RandomNumberGenerator.GetInt32(180,901)),checked(entry.Session+1));changed=true;
            }
            entries.Add(entry);
        }
        if(entries.Count!=old.Entries.Length)changed=true;
        if(changed)EventControlJson.Atomic(path,new BotScheduleState(1,entries.ToArray()));
        return entries.ToArray();
    }}
}
