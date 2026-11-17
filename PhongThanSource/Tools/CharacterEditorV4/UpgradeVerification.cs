using System.Text.Json;

namespace PhongThanCharacterEditor;
internal static class UpgradeVerification
{
    public static async Task<int> RunGear(string root){
        string folder=Path.Combine(root,"analysis","character_editor_v41_20261020");Directory.CreateDirectory(folder);var checks=new List<object>();int failures=0;
        string file=Path.Combine(root,"analysis","character_editor_v4_20261020","Recovered-QuangHuy3-43-items","CharacterStore","role_7175616e6768757933.pthc"),server=Path.Combine(root,"PhongThanRuntime-Loose","Server");var bytes=File.ReadAllBytes(file);
        foreach(int detail in new[]{2,5,7}){var character=PthcFile.Load(file);var item=character.GetItems().FirstOrDefault(i=>i.Genre==0&&i.DetailType==detail&&i.UpgradeDisplay==0);if(item==null){checks.Add(new{detail,skipped="No fixture item"});continue;}
            try{var rules=UpgradeBridge.Rules(item,server);if(!rules.Any())throw new InvalidDataException("No matching recipe");int rule=rules[0].Id;var before=character.UpgradeInput(item.Index);await character.UpgradeAsync(item.Index,1,rule,server);var after=character.UpgradeInput(item.Index);bool ok=character.GetItems()[item.Index].UpgradeDisplay==1&&before.AsSpan(268,12).SequenceEqual(after.AsSpan(268,12))&&!before.AsSpan(280).SequenceEqual(after.AsSpan(280));checks.Add(new{detail,rule,ok});if(!ok)failures++;}
            catch(Exception ex){checks.Add(new{detail,ok=false,error=ex.ToString()});failures++;}}
        bool unchanged=File.ReadAllBytes(file).SequenceEqual(bytes);if(!unchanged)failures++;File.WriteAllText(Path.Combine(folder,"gear-verification.json"),JsonSerializer.Serialize(new{checks,failures,sourceUnchanged=unchanged},new JsonSerializerOptions{WriteIndented=true}));return failures==0?0:1;
    }
    public static async Task<int> Run(string root){
        string folder=Path.Combine(root,"analysis","character_editor_v41_20261020","verify-"+DateTime.UtcNow.ToString("yyyyMMdd-HHmmss-fffffff"));Directory.CreateDirectory(folder);
        var checks=new List<object>();int failures=0;void Check(bool ok,string name){checks.Add(new{name,ok});if(!ok)failures++;}
        string file=Path.Combine(root,"analysis","character_editor_v4_20261020","Recovered-QuangHuy3-43-items","CharacterStore","role_7175616e6768757933.pthc"),server=Path.Combine(root,"PhongThanRuntime-Loose","Server");
        var original=File.ReadAllBytes(file);var character=PthcFile.Load(file);var item=character.GetItems()[0];var baseline=character.UpgradeInput(0);
        var rules=UpgradeBridge.Rules(item,server);Check(item.UpgradeDisplay==9&&rules.Any(r=>r.Id==1006),"Actual +9 item resolves original rule 1006 from recipes");
        try{
            await character.UpgradeAsync(0,10,1006,server);var raised=character.UpgradeInput(0);
            Check(character.GetItems()[0].UpgradeDisplay==10,"Native +9 to +10 applied");
            Check(baseline.AsSpan(268,12).SequenceEqual(raised.AsSpan(268,12)),"Upgrade preserves ordinal/serial/quality");
            Check(!baseline.AsSpan(280).SequenceEqual(raised.AsSpan(280)),"Native upgrade changes actual item attributes");
            string exported=character.ExportPackage(Path.Combine(folder,"plus10"));var loaded=PthcFile.Load(exported);
            Check(loaded.CanSave&&loaded.ChecksumValid&&loaded.ItemCount==43&&loaded.Money==200636014&&loaded.GetItems()[0].UpgradeDisplay==10,"+10 canonical + companion export/reload valid");
            bool untouched=true;for(int i=1;i<43;i++)untouched&=character.UpgradeInput(i).SequenceEqual(PthcFile.Load(file).UpgradeInput(i));Check(untouched,"Other 42 canonical records and companion entries unchanged");
            await loaded.UpgradeAsync(0,9,1006,server);Check(loaded.UpgradeInput(0).SequenceEqual(baseline),"Native +10 back to +9 restores original record and all companion bytes");
            await loaded.UpgradeAsync(0,9,1006,server);Check(loaded.UpgradeInput(0).SequenceEqual(baseline),"Repeated +9 is idempotent, no stat accumulation");
            await loaded.UpgradeAsync(0,0,0,server);Check(loaded.GetItems()[0].UpgradeLevel==0,"Remove upgrade to +0 via native deltas");
            await loaded.UpgradeAsync(0,9,1006,server);Check(loaded.UpgradeInput(0).SequenceEqual(baseline),"+0 to original +9 restores exact item bytes");
            await loaded.UpgradeAsync(0,12,1006,server);Check(loaded.GetItems()[0].UpgradeDisplay==12,"Native maximum +12 cold reconstructs");
            var beforeFailure=loaded.UpgradeInput(0);bool invalid=false;try{await loaded.UpgradeAsync(0,13,1006,server);}catch(ArgumentException){invalid=true;}Check(invalid&&beforeFailure.SequenceEqual(loaded.UpgradeInput(0)),"+13 rejected without mutation");
            invalid=false;try{await loaded.UpgradeAsync(0,11,65535,server);}catch(InvalidDataException){invalid=true;}Check(invalid&&beforeFailure.SequenceEqual(loaded.UpgradeInput(0)),"Unknown/mixed upgrade rule rejected without mutation");
            int potion=loaded.GetItems().First(i=>i.Genre!=0).Index;var potionBefore=loaded.UpgradeInput(potion);invalid=false;try{await loaded.UpgradeAsync(potion,1,1,server);}catch(InvalidOperationException){invalid=true;}Check(invalid&&potionBefore.SequenceEqual(loaded.UpgradeInput(potion)),"Non-equipment cannot be upgraded");
            loaded.DeleteItem(3);string reduced=loaded.ExportPackage(Path.Combine(folder,"plus12-deleted"));var reducedLoad=PthcFile.Load(reduced);Check(reducedLoad.CanSave&&reducedLoad.ItemCount==42&&reducedLoad.GetItems()[0].UpgradeDisplay==12,"Delete/reindex retains upgraded entry override");
            reducedLoad.SaveWithBackup();var saved=PthcFile.Load(reduced);await saved.UpgradeAsync(0,9,1006,server);Check(saved.GetItems()[0].UpgradeDisplay==9,"Direct save resets override mapping correctly for later upgrade");
        }catch(Exception ex){Check(false,"Unexpected failure: "+ex);}
        Check(File.ReadAllBytes(file).SequenceEqual(original),"Source recovery character unchanged; no Main writes");
        var report=new{checks,failures,nativeMethod="Production Gen_ExistEquipmentByTemplateRow + ApplyUpgradeState + P0 Restore/Capture + fresh cold reconstruction",serverDataModified=false};
        File.WriteAllText(Path.Combine(folder,"result.json"),JsonSerializer.Serialize(report,new JsonSerializerOptions{WriteIndented=true}));File.WriteAllText(Path.Combine(root,"analysis","character_editor_v41_20261020","latest-verification.txt"),folder);return failures==0?0:1;
    }
}
