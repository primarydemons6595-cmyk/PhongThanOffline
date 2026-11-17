using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;
public sealed class BotWorldKnowledge {
    readonly Dictionary<int,string> maps=new();
    readonly List<string> npcs=new();
    public BotWorldKnowledge(string folder){string file=Path.GetFullPath(Path.Combine(folder,"..","settings","MapList.ini"));if(!File.Exists(file))return;
        string text=BotChatEncoding.Decode(Convert.ToHexString(File.ReadAllBytes(file)));foreach(string line in text.Split('\n')){var match=Regex.Match(line.Trim(),@"\A(\d+)_name=\$?(.+)\z");if(match.Success&&int.TryParse(match.Groups[1].Value,out int index))maps[1000+index]=match.Groups[2].Value.Trim();}
        string names=Path.GetFullPath(Path.Combine(folder,"..","settings","phongthan","NpcDisplayNames.txt"));if(File.Exists(names))foreach(string row in BotChatEncoding.Decode(Convert.ToHexString(File.ReadAllBytes(names))).Split('\n').Skip(1)){string[] fields=row.Split('\t');if(fields.Length>=2){string name=BotDialogueMemory.Normalize(fields[1].Trim().TrimStart('#','$'));if(name.Length>=3)npcs.Add(name);}}
    }
    public string Map(int id)=>maps.TryGetValue(id,out string? name)?name:"khu vực chưa rõ tên";
    public bool MentionsMap(string text){string q=BotDialogueMemory.Normalize(text);return maps.Values.Any(name=>q.Contains(BotDialogueMemory.Normalize(name),StringComparison.Ordinal));}
    public bool MentionsNpc(string text){string q=BotDialogueMemory.Normalize(text);return npcs.Any(name=>Regex.IsMatch(q,@"(?:\A|\s)"+Regex.Escape(name)+@"(?:\z|\s|[.!?])"));}
}
