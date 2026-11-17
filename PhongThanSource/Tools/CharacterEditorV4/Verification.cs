using System.Text;
using System.Text.Json;

namespace PhongThanCharacterEditor;
internal static class Verification
{
    public static int Run(string root){var checks=new List<object>();int failures=0;void Check(bool ok,string name){checks.Add(new{name,ok});if(!ok)failures++;}
        string output=Path.Combine(root,"analysis","character_editor_v4_20261020","verification-"+DateTime.UtcNow.ToString("yyyyMMdd-HHmmss-fffffff"));Directory.CreateDirectory(output);
        string liveComp=Path.Combine(root,"PhongThanRuntime-Loose","Server","NativeDropPersistenceV1"),fixture=Path.Combine(root,"analysis","rps_session_flicker_20261020","release","CharacterStore.saved","role_7175616e6768757933.pthc");
        var source=PthcFile.Load(fixture,liveComp);Check(source.CanSave&&source.ChecksumValid,"Existing canonical + companion accepted");byte[] before=File.ReadAllBytes(fixture);var original=source.GetItems();
        source.Money+=1;var moved=original.First(i=>i.Container!=2);moved.SlotX=0;moved.SlotY=0;source.UpdateItem(moved);
        string package=Path.Combine(output,"money-and-position"),file=source.ExportPackage(package);var read=PthcFile.Load(file);
        Check(read.CanSave&&read.ChecksumValid&&read.Money==source.Money&&read.Revision==source.Revision+1,"Money/position export reloads with bound companion and next revision");
        var sourceBytes=File.ReadAllBytes(fixture);Check(before.SequenceEqual(sourceBytes),"Source character unchanged by export");
        string oldKey=PthcFile.Hash(before[16..]),newKey=PthcFile.Hash(File.ReadAllBytes(file)[16..]);var cOld=File.ReadAllBytes(Path.Combine(liveComp,oldKey+".magic-v1"));var cNew=File.ReadAllBytes(Path.Combine(read.CompanionRoot,newKey+".magic-v1"));
        Check(cOld[144..].SequenceEqual(cNew[144..]),"All magic/base/requirements/serial payload bytes preserved for money/move");
        int removed=3;var priorItems=read.GetItems();read.DeleteItem(removed);string deleted=read.ExportPackage(Path.Combine(output,"deleted"));var afterDelete=PthcFile.Load(deleted);Check(afterDelete.CanSave&&afterDelete.ItemCount==read.ItemCount&&afterDelete.ItemCount==priorItems.Count-1,"Deletion rebinds companion count and ordinals");
        var keyDelete=PthcFile.Hash(File.ReadAllBytes(deleted)[16..]);var cDelete=File.ReadAllBytes(Path.Combine(afterDelete.CompanionRoot,keyDelete+".magic-v1"));bool retained=true;
        for(int i=0;i<afterDelete.ItemCount;i++){int old=i<removed?i:i+1;retained&=cNew.AsSpan(144+old*748+4,744).SequenceEqual(cDelete.AsSpan(144+i*748+4,744));}Check(retained,"Deletion preserves remaining serials and attribute bytes");
        afterDelete.SaveWithBackup();afterDelete.DeleteItem(0);afterDelete.SaveWithBackup();var repeated=PthcFile.Load(deleted);Check(repeated.CanSave&&repeated.ItemCount==priorItems.Count-2,"Repeated save then deletion uses updated ordinal map");
        Check(Directory.GetFiles(Path.GetDirectoryName(deleted)!,"*.before-editor-*.bak").Length==2,"Each direct save makes its own backup");
        string absentCompanion=Path.Combine(output,"missing-companion");Directory.CreateDirectory(absentCompanion);
        var broken=PthcFile.Load(Path.Combine(root,"analysis","character_editor_v4_20261020","Recovered-QuangHuy3-43-items","CharacterStore","role_7175616e6768757933.pthc"),absentCompanion);bool rejected=false;try{broken.ExportPackage(Path.Combine(output,"must-reject"));}catch(InvalidOperationException){rejected=true;}Check(!broken.CanSave&&rejected,"Missing companion blocks save/export");
        var upgraded=new ItemRecord{UpgradeLevel=1073999369};Check(upgraded.UpgradeDisplay==9,"Tagged upgrade renders +9 without destroying rule");
        bool blocked=false;var change=repeated.GetItems()[0];change.UpgradeLevel=99;try{repeated.UpdateItem(change);}catch(InvalidOperationException){blocked=true;}Check(blocked,"Unsafe upgrade writes rejected");
        using var assets=new ClientAssets(Path.Combine(root,"PhongThanRuntime-Loose","Client"));int icons=0;foreach(var item in broken.GetItems())if(assets.Visual(item).Icon!=null)icons++;
        Check(icons==broken.ItemCount,$"Actual client icon resolution {icons}/{broken.ItemCount}");
        var occupancy=new Dictionary<(int,int,int),int>();bool positions=true;foreach(var i in broken.GetItems()){var v=assets.Visual(i);if(i.Container==2){positions&=i.SlotX>=0&&i.SlotX<13;positions&=occupancy.TryAdd((2,i.SlotX,0),i.Index);}else if(i.Container is 3 or 4 or 18 or 19 or >=5 and <=10){int w=i.Container is >=5 and <=10?6:5,h=i.Container is >=5 and <=10?10:7;positions&=i.SlotX>=0&&i.SlotY>=0&&i.SlotX+v.Width<=w&&i.SlotY+v.Height<=h;for(int x=0;x<v.Width;x++)for(int y=0;y<v.Height;y++)positions&=occupancy.TryAdd((i.Container,i.SlotX+x,i.SlotY+y),i.Index);}else if(i.Container is 13 or 16){positions&=i.SlotX>=0&&i.SlotX<9&&i.SlotY==0;positions&=occupancy.TryAdd((i.Container,i.SlotX,i.SlotY),i.Index);}}
        Check(positions,"All 43 saved positions fit native bag/equipment/shortcut/build containers without overlap");
        var background=assets.Image(assets.Value("Main","Image"));Check(background!=null,"Native F4 SPR background decoded");background?.Save(Path.Combine(output,"f4-background.png"));
        bool truncated=false;try{Spr.Decode(new byte[40]);}catch(InvalidDataException){truncated=true;}Check(truncated,"Invalid SPR rejected");
        using var packedAssets=new ClientAssets(Path.Combine(root,"Client_Original"));packedAssets.SetPreferPaks(true);int pakIcons=0;foreach(var item in broken.GetItems())if(packedAssets.Visual(item).Icon!=null)pakIcons++;
        Check(packedAssets.PakCount>0&&packedAssets.PakReads>0&&pakIcons==broken.ItemCount,$"Actual original PACK chain icons {pakIcons}/{broken.ItemCount}, reads={packedAssets.PakReads}");
        var packedBackground=packedAssets.Image(packedAssets.Value("Main","Image"));Check(packedBackground!=null,"Actual PACK F4 background decoded");packedBackground?.Save(Path.Combine(output,"pak-f4-background.png"));
        Check(assets.Image(assets.StoreValue("Main","Image"))!=null&&packedAssets.Image(packedAssets.StoreValue("Main","Image"))!=null,"Native warehouse SPR decoded from Loose and PACK");
        var recoveredPath=Path.Combine(root,"analysis","character_editor_v4_20261020","Recovered-QuangHuy3-43-items","CharacterStore","role_7175616e6768757933.pthc");if(File.Exists(recoveredPath)){var recovered=PthcFile.Load(recoveredPath);Check(recovered.CanSave&&recovered.ChecksumValid&&recovered.ItemCount==43&&recovered.Money==200636014,"Recovered 43-item package is editable and keeps edited money");string recoveredExport=recovered.ExportPackage(Path.Combine(output,"recovered-resave"));var recoveredAgain=PthcFile.Load(recoveredExport);Check(recoveredAgain.CanSave&&recoveredAgain.ItemCount==43,"Recovered package survives editor resave/reload");}
        var unresolved=broken.GetItems().Where(i=>assets.Visual(i).Icon==null).Select(i=>new{i.Index,i.Genre,i.DetailType,i.ParticularType,i.TemplateRow,i.Level}).ToArray();
        var report=new{checks,failures,icons,pakIcons,pakCount=packedAssets.PakCount,pakReads=packedAssets.PakReads,unresolved,packageWarnings=packedAssets.PackageWarnings,pakResourceErrors=packedAssets.Missing,resourceErrors=assets.Missing,sourceDataModified=false,nativeLoginTested=false};File.WriteAllText(Path.Combine(output,"result.json"),JsonSerializer.Serialize(report,new JsonSerializerOptions{WriteIndented=true}));File.WriteAllText(Path.Combine(root,"analysis","character_editor_v4_20261020","latest-verification.txt"),output);return failures==0?0:1;
    }
}
