using Microsoft.Data.Sqlite;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;

// One durable database; private memories are always keyed by bot AND peer.
public sealed class BotBrain {
    readonly string root;
    public string DatabasePath {get;}
    public BotBrain(string botFolder){root=botFolder;DatabasePath=Path.Combine(root,"brain.db");Directory.CreateDirectory(root);using var db=Open();Execute(db,"""
        PRAGMA journal_mode=WAL;
        CREATE TABLE IF NOT EXISTS peers(bot TEXT NOT NULL,peer TEXT NOT NULL,style INTEGER NOT NULL DEFAULT 0,PRIMARY KEY(bot,peer));
        CREATE TABLE IF NOT EXISTS turns(id INTEGER PRIMARY KEY,bot TEXT NOT NULL,peer TEXT NOT NULL,utc INTEGER NOT NULL,text TEXT NOT NULL,reply TEXT NOT NULL,intent INTEGER NOT NULL,initiated INTEGER NOT NULL DEFAULT 0);
        CREATE INDEX IF NOT EXISTS turns_peer ON turns(bot,peer,utc DESC,id DESC);
        CREATE TABLE IF NOT EXISTS dialect(phrase TEXT PRIMARY KEY,normalized TEXT NOT NULL,category TEXT NOT NULL,uses INTEGER NOT NULL,last_utc INTEGER NOT NULL);
        CREATE INDEX IF NOT EXISTS dialect_category ON dialect(category,uses DESC);
        CREATE TABLE IF NOT EXISTS imports(path TEXT PRIMARY KEY);
        CREATE TABLE IF NOT EXISTS relationships(bot TEXT NOT NULL,peer TEXT NOT NULL,hostility INTEGER NOT NULL,trust INTEGER NOT NULL,last_utc INTEGER NOT NULL,last_kind TEXT NOT NULL,PRIMARY KEY(bot,peer));
        CREATE TABLE IF NOT EXISTS experiences(id INTEGER PRIMARY KEY,bot TEXT NOT NULL,peer TEXT NOT NULL,utc INTEGER NOT NULL,kind TEXT NOT NULL,detail TEXT NOT NULL);
        CREATE INDEX IF NOT EXISTS experiences_peer ON experiences(bot,peer,utc DESC);
        CREATE TABLE IF NOT EXISTS vocabulary(word TEXT PRIMARY KEY,uses INTEGER NOT NULL,last_utc INTEGER NOT NULL);
        CREATE VIEW IF NOT EXISTS relationship_labels AS SELECT *,CASE WHEN hostility>=80 THEN 'sworn_enemy' WHEN hostility>=40 THEN 'enemy' WHEN hostility>0 OR trust<0 THEN 'disliked' WHEN trust>=30 THEN 'friend' ELSE 'acquaintance' END AS category FROM relationships;
        PRAGMA user_version=1;
        """);}
    SqliteConnection Open(){var db=new SqliteConnection(new SqliteConnectionStringBuilder{DataSource=DatabasePath,DefaultTimeout=5,Pooling=true}.ToString());db.Open();return db;}
    static SqliteCommand Command(SqliteConnection db,string sql,params (string,object)[] values){var cmd=db.CreateCommand();cmd.CommandText=sql;foreach(var (key,value) in values)cmd.Parameters.AddWithValue(key,value);return cmd;}
    static void Execute(SqliteConnection db,string sql,params (string,object)[] values){using var cmd=Command(db,sql,values);cmd.ExecuteNonQuery();}
    static string Peer(string speaker)=>Convert.ToHexStringLower(SHA256.HashData(Encoding.UTF8.GetBytes(speaker)));
    public static string Clean(string text){text=Regex.Replace(text,@"(?i)\b(password|pass|otp|mật khẩu|mat khau)\s*[:=]?\s*\S+","[đã ẩn]");text=Regex.Replace(text,@"[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}|(?<!\d)(?:\+84|0)\d[\d .-]{7,12}\d(?!\d)","[đã ẩn]");return text[..Math.Min(255,text.Length)];}
    public BotDialogueHistory Read(string bot,string speaker,DateTimeOffset now){using var db=Open();string peer=Peer(speaker);Import(db,bot,peer);return Read(db,bot,peer,now);}
    static BotDialogueHistory Read(SqliteConnection db,string bot,string peer,DateTimeOffset now){
        using var style=Command(db,"SELECT style FROM peers WHERE bot=$b AND peer=$p",("$b",bot),("$p",peer));int address=Convert.ToInt32(style.ExecuteScalar()??0);
        using var query=Command(db,"SELECT utc,text,reply,intent,initiated FROM turns WHERE bot=$b AND peer=$p AND utc BETWEEN $min AND $max ORDER BY utc DESC,id DESC LIMIT 64",("$b",bot),("$p",peer),("$min",now.AddDays(-90).ToUnixTimeSeconds()),("$max",now.ToUnixTimeSeconds()));using var rows=query.ExecuteReader();var turns=new List<BotDialogueTurn>();while(rows.Read())turns.Add(new(rows.GetInt64(0),rows.GetString(1),rows.GetInt32(3)){BotReply=rows.GetString(2),BotInitiated=rows.GetBoolean(4)});turns.Reverse();return new(1,address,turns.ToArray());
    }
    void Import(SqliteConnection db,string bot,string peer){
        string path=Path.Combine(root,"dialogue",bot,peer+".json");if(!File.Exists(path))return;
        using var known=Command(db,"SELECT 1 FROM imports WHERE path=$p",("$p",path));if(known.ExecuteScalar() is not null)return;
        if(new FileInfo(path).Length>131072)throw new InvalidDataException("Legacy conversation is too large: "+path);
        var history=EventControlJson.Read<BotDialogueHistory>(path);if(history.Version!=1||history.Style is <0 or >2)throw new InvalidDataException("Invalid legacy dialogue: "+path);
        using var tx=db.BeginTransaction();Execute(db,"INSERT INTO peers(bot,peer,style) VALUES($b,$p,$s) ON CONFLICT DO NOTHING",("$b",bot),("$p",peer),("$s",history.Style));
        foreach(var t in history.Turns.Where(t=>t.Intent is >=1 and <=5)){Insert(db,bot,peer,t with{Text=Clean(t.Text),BotReply=Clean(t.BotReply)});}
        Execute(db,"INSERT INTO imports VALUES($p)",("$p",path));tx.Commit();
    }
    public int MigrateLegacy(){int count=0;string directory=Path.Combine(root,"dialogue");if(!Directory.Exists(directory))return 0;using var db=Open();foreach(string bot in Directory.EnumerateDirectories(directory)){if(!Regex.IsMatch(Path.GetFileName(bot),@"\A[0-9a-f]{32}\z"))continue;foreach(string path in Directory.EnumerateFiles(bot,"*.json")){string peer=Path.GetFileNameWithoutExtension(path);if(!Regex.IsMatch(peer,@"\A[0-9a-f]{64}\z"))continue;Import(db,Path.GetFileName(bot),peer);++count;}}return count;}
    static void Insert(SqliteConnection db,string bot,string peer,BotDialogueTurn turn)=>Execute(db,"INSERT INTO turns(bot,peer,utc,text,reply,intent,initiated) VALUES($b,$p,$u,$t,$r,$i,$a)",("$b",bot),("$p",peer),("$u",turn.Utc),("$t",turn.Text),("$r",turn.BotReply),("$i",turn.Intent),("$a",turn.BotInitiated));
    public BotDialogueHistory Remember(string bot,string speaker,string text,int intent,DateTimeOffset now,string reply,bool initiated){
        using var db=Open();string peer=Peer(speaker);Import(db,bot,peer);using var tx=db.BeginTransaction();var history=Read(db,bot,peer,now);string clean=Clean(text);int style=history.Style;string normalized=BotDialogueMemory.Normalize(clean);
        if(Regex.IsMatch(normalized,@"\b(tui|ong|bro)\b"))style=1;else if(Regex.IsMatch(normalized,@"\b(anh|em)\b"))style=2;
        Execute(db,"INSERT INTO peers VALUES($b,$p,$s) ON CONFLICT(bot,peer) DO UPDATE SET style=excluded.style",("$b",bot),("$p",peer),("$s",style));
        var last=history.Turns.LastOrDefault();bool duplicate=last is not null&&last.Text==clean&&last.BotInitiated==initiated&&now.ToUnixTimeSeconds()-last.Utc<=10;
        if(duplicate){if(reply.Length>0)Execute(db,"UPDATE turns SET reply=$r WHERE id=(SELECT id FROM turns WHERE bot=$b AND peer=$p ORDER BY utc DESC,id DESC LIMIT 1)",("$r",Clean(reply)),("$b",bot),("$p",peer));}
        else{Insert(db,bot,peer,new(now.ToUnixTimeSeconds(),clean,Math.Clamp(intent,1,5)){BotReply=Clean(reply),BotInitiated=initiated});if(!initiated&&!speaker.StartsWith("b0-",StringComparison.Ordinal))Learn(db,clean,now);}
        tx.Commit();return Read(db,bot,peer,now);
    }
    // Only short generic player expressions enter the shared dialect. No account,
    // quest, inventory or relationship facts are transferable between players.
    public static string? Category(string phrase){string q=BotDialogueMemory.Normalize(phrase);if(q.Length is <2 or >90||Regex.IsMatch(q,@"\d|\[|https?|system|ignore|instruction|prompt|bo qua|mat khau|tai khoan"))return null;
        if(Regex.IsMatch(q,@"\A(chao|hello|hi|alo)( (ong|ban|bro|anh|em|nha|nhe))*[ !?.]*\z"))return "greeting";
        if(Regex.IsMatch(q,@"\A(tam biet|gap lai|bye)( (ong|ban|bro|nha|nhe))*[ !?.]*\z"))return "goodbye";
        if(Regex.IsMatch(q,@"\A(nghi|nghi tay|nghi ti|nghi chut)( (di|ong|ban|bro|nha|nhe|da|ti|chut))*[ !?.]*\z"))return "rest";
        if(Regex.IsMatch(q,@"\A(di dau|di dau day|ong muon di dau|ban muon di dau)[ !?.]*\z"))return "destination";
        return null;
    }
    static void Learn(SqliteConnection db,string phrase,DateTimeOffset now){
        // Frequency vocabulary is separate from reusable complete phrases. Do not
        // concatenate random words into promises or game facts.
        if(!phrase.Contains('[')&&!Regex.IsMatch(phrase,@"(?i)https?://|system|prompt|instruction"))
            foreach(string word in Regex.Matches(phrase,@"\p{L}{2,12}").Select(m=>m.Value).Where(w=>!w.Any(char.IsUpper)).Select(w=>w.ToLowerInvariant()).Distinct().Take(32))
                Execute(db,"INSERT INTO vocabulary VALUES($w,1,$u) ON CONFLICT(word) DO UPDATE SET uses=uses+1,last_utc=excluded.last_utc",("$w",word),("$u",now.ToUnixTimeSeconds()));
        string? category=Category(phrase);if(category is null)return;Execute(db,"INSERT INTO dialect VALUES($t,$n,$c,1,$u) ON CONFLICT(phrase) DO UPDATE SET uses=uses+1,last_utc=excluded.last_utc",("$t",phrase),("$n",BotDialogueMemory.Normalize(phrase)),("$c",category),("$u",now.ToUnixTimeSeconds()));}
    public string[] Phrases(string category){using var db=Open();using var cmd=Command(db,"SELECT phrase FROM dialect WHERE category=$c ORDER BY uses DESC,last_utc DESC LIMIT 6",("$c",category));using var rows=cmd.ExecuteReader();var result=new List<string>();while(rows.Read())result.Add(rows.GetString(0));return result.ToArray();}
    public string DialectContext(string text){string q=BotDialogueMemory.Normalize(text);string category=Category(text)??(Regex.IsMatch(q,@"\b(met|nghi|duoi)\b")?"rest":"greeting");
        using var db=Open();using var cmd=Command(db,"SELECT word FROM vocabulary WHERE uses>=3 ORDER BY uses DESC,last_utc DESC LIMIT 80");using var rows=cmd.ExecuteReader();var words=new List<string>();while(rows.Read())words.Add(rows.GetString(0));
        var queryWords=q.Split(' ',StringSplitOptions.RemoveEmptyEntries).ToHashSet(StringComparer.Ordinal);
        return JsonSerializer.Serialize(new{phrases=Phrases(category),vocabulary=words.OrderByDescending(w=>queryWords.Contains(BotDialogueMemory.Normalize(w))).Take(16)});
    }
    public string? RecallReply(string text){string q=BotDialogueMemory.Normalize(text);string? category=Category(text);if(Regex.IsMatch(q,@"\b(met|duoi|nan)\b"))category="rest";return category is "greeting" or "goodbye" or "rest"?Phrases(category).FirstOrDefault(p=>!BotChatIntent.SameSpeech(p,text)):null;}
    public void Relationship(string bot,string speaker,int hostility,int trust,long utc,string kind){using var db=Open();Execute(db,"INSERT INTO relationships VALUES($b,$p,$h,$t,$u,$k) ON CONFLICT(bot,peer) DO UPDATE SET hostility=excluded.hostility,trust=excluded.trust,last_utc=excluded.last_utc,last_kind=excluded.last_kind",("$b",bot),("$p",Peer(speaker)),("$h",Math.Clamp(hostility,0,100)),("$t",Math.Clamp(trust,-100,100)),("$u",utc),("$k",kind));}
    public string RelationshipContext(string bot,string speaker){using var db=Open();using var cmd=Command(db,"SELECT hostility,trust,last_kind FROM relationships WHERE bot=$b AND peer=$p",("$b",bot),("$p",Peer(speaker)));using var row=cmd.ExecuteReader();return row.Read()?JsonSerializer.Serialize(new{hostility=row.GetInt32(0),trust=row.GetInt32(1),last_event=row.GetString(2)}):"unknown/new acquaintance";}
    public void Experience(string bot,string speaker,string kind,string detail,DateTimeOffset now){using var db=Open();Execute(db,"INSERT INTO experiences(bot,peer,utc,kind,detail) VALUES($b,$p,$u,$k,$d)",("$b",bot),("$p",Peer(speaker)),("$u",now.ToUnixTimeSeconds()),("$k",kind),("$d",Clean(detail)));}
    public void ExperienceOnce(string receipt,string bot,string speaker,string kind,string detail,DateTimeOffset now){
        using var db=Open();using var tx=db.BeginTransaction();using var known=Command(db,"SELECT 1 FROM imports WHERE path=$p",("$p","experience:"+receipt));if(known.ExecuteScalar() is not null)return;
        Execute(db,"INSERT INTO experiences(bot,peer,utc,kind,detail) VALUES($b,$p,$u,$k,$d)",("$b",bot),("$p",Peer(speaker)),("$u",now.ToUnixTimeSeconds()),("$k",kind),("$d",Clean(detail)));
        Execute(db,"INSERT INTO imports VALUES($p)",("$p","experience:"+receipt));tx.Commit();
    }
    public int SyncRelationships(){
        string directory=Path.Combine(root,"memory");if(!Directory.Exists(directory))return 0;int count=0;
        foreach(string path in Directory.EnumerateFiles(directory,"*.ini")){
            string bot=Path.GetFileNameWithoutExtension(path);if(!Regex.IsMatch(bot,@"\A[0-9a-f]{32}\z"))continue;
            string peer="";int hostility=0,trust=0;long utc=0;string kind="";
            void Save(){if(peer.Length>0&&Regex.IsMatch(peer,@"\A[0-9a-f-]{1,160}\z")){Relationship(bot,peer,hostility,trust,utc,kind);++count;}}
            foreach(string raw in File.ReadLines(path)){string line=raw.Trim();if(line.StartsWith('[')&&line.EndsWith(']')){Save();peer=line[1..^1];hostility=trust=0;utc=0;kind="";continue;}int equals=line.IndexOf('=');if(equals<0)continue;string key=line[..equals],value=line[(equals+1)..];if(key=="Hostility")int.TryParse(value,out hostility);else if(key=="Trust")int.TryParse(value,out trust);else if(key=="LastUtc")long.TryParse(value,out utc);else if(key=="LastKind")kind=Clean(value);}Save();
        }return count;
    }
    public object Statistics(){using var db=Open();var counts=new Dictionary<string,long>();foreach(string table in new[]{"peers","turns","dialect","vocabulary","relationships","experiences"}){using var cmd=Command(db,"SELECT COUNT(*) FROM "+table);counts[table]=Convert.ToInt64(cmd.ExecuteScalar());}return new{Database=DatabasePath,Version=1,Counts=counts};}
}
