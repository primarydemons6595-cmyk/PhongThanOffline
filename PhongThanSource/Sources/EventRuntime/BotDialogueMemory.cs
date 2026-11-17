using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;
public sealed record BotDialogueTurn(long Utc,string Text,int Intent){public string BotReply {get;init;}="";public bool BotInitiated {get;init;}}
public sealed record BotDialogueHistory(int Version,int Style,BotDialogueTurn[] Turns);
public sealed class BotDialogueMemory {
    readonly BotBrain brain;
    public BotDialogueMemory(string botFolder){brain=new(botFolder);}
    static void Validate(string bot,string speaker){if(!Regex.IsMatch(bot,@"\A[0-9a-f]{32}\z")||speaker.Length is <1 or >160||speaker.Any(c=>!char.IsAsciiHexDigit(c)&&c!='-'))throw new InvalidDataException("Danh tính hội thoại không hợp lệ.");}
    public BotDialogueHistory Read(string bot,string speaker,DateTimeOffset now){Validate(bot,speaker);return brain.Read(bot,speaker,now);}
    public BotDialogueHistory Remember(string bot,string speaker,string text,int intent,DateTimeOffset now,string botReply="",bool initiated=false){Validate(bot,speaker);return brain.Remember(bot,speaker,text,intent,now,botReply,initiated);}
    public static string Normalize(string text)=>new string(text.Normalize(NormalizationForm.FormD).Where(c=>CharUnicodeInfo.GetUnicodeCategory(c)!=UnicodeCategory.NonSpacingMark).ToArray()).ToLowerInvariant().Replace('đ','d').Trim();
    public static int LearnedIntent(BotDialogueHistory history,string query){
        string normalized=Normalize(query);var same=history.Turns.LastOrDefault(t=>Normalize(t.Text)==normalized&&t.Intent!=1);return same?.Intent??1;
    }
    public static string Context(BotDialogueHistory history,string query){
        var words=Normalize(query).Split(' ',StringSplitOptions.RemoveEmptyEntries).ToHashSet(StringComparer.Ordinal);
        return System.Text.Json.JsonSerializer.Serialize(history.Turns.OrderByDescending(t=>Normalize(t.Text).Split(' ').Count(words.Contains)).ThenByDescending(t=>t.Utc).Take(6).Select(t=>new{message=t.Text,reply=t.BotReply,intent=t.Intent}));
    }
}
