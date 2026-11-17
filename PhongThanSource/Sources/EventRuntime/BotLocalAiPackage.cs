using System.Text.Json;
namespace PhongThan.Event.Runtime;

public static class BotLocalAiPackage {
    public const string Manifest="BotAIgent/local-ai.package.json";
    public static bool Enabled(string botFolder){
        string path=Path.Combine(botFolder,"local-ai.package.json");
        if(!File.Exists(path))return true;
        try{using var doc=JsonDocument.Parse(File.ReadAllText(path));return doc.RootElement.GetProperty("Included").GetBoolean();}
        catch(Exception e)when(e is IOException or JsonException or InvalidOperationException or KeyNotFoundException){return false;}
    }
}
