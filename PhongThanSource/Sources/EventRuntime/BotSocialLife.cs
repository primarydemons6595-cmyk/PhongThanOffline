using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;
public sealed record BotSocialRequest(string Bot,string Peer,string Incoming,string Scene);
public sealed record BotSocialSettings{public bool Enabled {get;init;}=true;public int GlobalMinimumSeconds {get;init;}=25;public int GlobalMaximumSeconds {get;init;}=55;public int BotMinimumSeconds {get;init;}=150;public int BotMaximumSeconds {get;init;}=420;}
public sealed class BotSocialLife {
    readonly string folder;readonly BotAIgentStore store;readonly BotDialogueMemory memory;readonly Action<string,string> log;
    readonly Dictionary<string,long> due=new();long next,nextPublic;Pending? pending;Reply? reply;
    sealed record Pending(string Id,string Bot,string Peer,char Kind,string Target,string Text,string Incoming,bool First,long Deadline,bool Model);
    sealed record Reply(string Bot,string Peer,string Other,string Text,long Due);
    public BotSocialLife(string root,BotAIgentStore bots,Action<string,string> logger){folder=root;store=bots;memory=new(root);log=logger;Directory.CreateDirectory(Path.Combine(root,"social","in"));Directory.CreateDirectory(Path.Combine(root,"social","out"));}
    static bool Near(int map,int x,int y,BotLive bot)=>map==bot.Map&&Math.Abs(x-bot.X)+Math.Abs(y-bot.Y)<=640;
    static bool Alive(BotLive bot)=>bot.Player>0&&bot.Life>0&&bot.State is not("dead" or "post_death_rest" or "recovering" or "error");
    BotSocialSettings Settings(){string file=Path.Combine(folder,"social-settings.json");if(!File.Exists(file)){var defaults=new BotSocialSettings();EventControlJson.Atomic(file,defaults);return defaults;}var value=EventControlJson.Read<BotSocialSettings>(file);return value with{GlobalMinimumSeconds=Math.Clamp(value.GlobalMinimumSeconds,15,600),GlobalMaximumSeconds=Math.Clamp(value.GlobalMaximumSeconds,15,900),BotMinimumSeconds=Math.Clamp(value.BotMinimumSeconds,60,3600),BotMaximumSeconds=Math.Clamp(value.BotMaximumSeconds,60,7200)};}
    static int Delay(int min,int max)=>RandomNumberGenerator.GetInt32(min,Math.Max(min+1,max+1));
    public async Task Step(DateTimeOffset now,Func<BotSocialRequest,CancellationToken,Task<string?>> generate,CancellationToken stop,BotDashboard? snapshot=null){
        long time=now.ToUnixTimeSeconds();if(pending is null&&reply is null&&time<next)return;var settings=Settings();var dashboard=snapshot??store.Status(now);
        if(!settings.Enabled||!dashboard.Configuration.Running||!dashboard.Fresh||dashboard.Native?.SocialVersion!=1){reply=null;return;}
        var profiles=dashboard.Configuration.Bots.Where(b=>b.Enabled&&!b.Deleted).ToDictionary(b=>b.Id);var live=dashboard.Native.Bots.Where(b=>profiles.ContainsKey(b.Id)&&Alive(b)).ToArray();
        if(pending is { } command){string ack=Path.Combine(folder,"social","out",command.Id+".ack");if(File.Exists(ack)){
            bool accepted=File.ReadAllText(ack).Trim()=="1";File.Delete(ack);pending=null;
            if(accepted){log(command.Bot,"social_spoken target="+command.Kind+" source="+(command.Model?"local_model":"context"));
                if(dashboard.Configuration.RememberChats)memory.Remember(command.Bot,command.Peer,command.Incoming,1,now,command.Text,command.First);
                if(command.Kind=='B'&&command.First){reply=new(command.Target,"b0-"+command.Bot,command.Bot,command.Text,time+Delay(4,9));}
                else if(command.Kind=='B'&&dashboard.Configuration.RememberChats)memory.Remember(command.Target,"b0-"+command.Bot,command.Text,1,now);
            }else log(command.Bot,"social_delivery_skipped_state_or_range_changed");
        }else if(time>command.Deadline){pending=null;log(command.Bot,"social_delivery_timeout");}else return;}
        if(reply is { } response){if(time<response.Due)return;reply=null;var speaker=live.FirstOrDefault(b=>b.Id==response.Bot);var other=live.FirstOrDefault(b=>b.Id==response.Other);if(speaker is null||other is null||!Near(other.Map,other.X,other.Y,speaker))return;
            string? text=dashboard.Configuration.LocalAiEnabled?await generate(new(speaker.Id,response.Peer,response.Text,"reply|"+profiles[other.Id].Name),stop):null;
            bool usedModel=text is not null;
            text??=BotChatIntent.ConversationContext(response.Text,speaker.State);
            Queue(speaker,response.Peer,'B',other.Id,text,response.Text,false,time,usedModel);return;}
        if(time<next)return;
        foreach(var bot in live)if(!due.ContainsKey(bot.Id))due[bot.Id]=time+Delay(20,120);
        if(nextPublic==0)nextPublic=time+Delay(90,180);
        var policy=BotChannelPolicy.Read(folder);if(policy.WorldEnabled&&time>=nextPublic){nextPublic=time+Delay(180,420);var eligible=live.Where(b=>b.Level>=policy.PublicMinLevel&&due[b.Id]<=time).ToArray();if(eligible.Length>0){var bot=eligible[RandomNumberGenerator.GetInt32(eligible.Length)];due[bot.Id]=time+Delay(settings.BotMinimumSeconds,settings.BotMaximumSeconds);next=time+Delay(settings.GlobalMinimumSeconds,settings.GlobalMaximumSeconds);
            string? line=dashboard.Configuration.LocalAiEnabled?await generate(new(bot.Id,"c0-05","","world|mọi người"),stop):null;bool model=line is not null;line??="Có ai đang train không, hôm nay đông vui ghê.";Queue(bot,"c0-05",'W',"-",line,"",true,time,model);return;}}
        foreach(var bot in live.Where(b=>due[b.Id]<=time).OrderBy(_=>RandomNumberGenerator.GetInt32(int.MaxValue))){
            var humans=dashboard.Native.Observers.Where(p=>Regex.IsMatch(p.Key,@"\A[0-9a-f-]{1,160}\z")&&Near(p.Map,p.X,p.Y,bot)&&(!due.TryGetValue("human-"+p.Key,out long until)||until<=time)).ToArray();
            var peers=live.Where(p=>p.Id!=bot.Id&&Near(p.Map,p.X,p.Y,bot)&&due[p.Id]<=time).ToArray();if(humans.Length==0&&peers.Length==0){due[bot.Id]=time+Delay(20,60);continue;}
            bool human=humans.Length>0&&(peers.Length==0||RandomNumberGenerator.GetInt32(100)<60);string peer,target,name;char kind;
            if(human){var p=humans[RandomNumberGenerator.GetInt32(humans.Length)];peer=target=p.Key;name=BotChatEncoding.Decode(p.NameHex);kind='P';due["human-"+peer]=time+180;}
            else{var p=peers[RandomNumberGenerator.GetInt32(peers.Length)];target=p.Id;peer="b0-"+target;name=profiles[target].Name;kind='B';due[p.Id]=time+Delay(settings.BotMinimumSeconds,settings.BotMaximumSeconds);}
            due[bot.Id]=time+Delay(settings.BotMinimumSeconds,settings.BotMaximumSeconds);next=time+Delay(settings.GlobalMinimumSeconds,settings.GlobalMaximumSeconds);
            string? text=dashboard.Configuration.LocalAiEnabled?await generate(new(bot.Id,peer,"","start|"+name),stop):null;
            bool usedModel=text is not null;
            text??=bot.State=="training"?$"{name}, bạn cũng train ở đây à?":$"{name}, bạn đang đi đâu vậy?";
            Queue(bot,peer,kind,target,text,"",true,time,usedModel);return;
        }
        next=time+3;
    }
    void Queue(BotLive bot,string peer,char kind,string target,string text,string incoming,bool first,long now,bool model){
        if(text.Length>160||text.Any(char.IsControl))return;string hex=BotChatEncoding.Encode(text);if(hex.Length>510)return;string id=Convert.ToHexStringLower(RandomNumberGenerator.GetBytes(8));
        EventControlJson.AtomicBytes(Path.Combine(folder,"social","in",id+".say"),Encoding.ASCII.GetBytes($"PT_BOT_SAY\t1\t{now+12}\t{bot.Id}\t{kind}\t{target}\t{bot.Map}\t{hex}\n"));pending=new(id,bot.Id,peer,kind,target,text,incoming,first,now+12,model);
    }
}
