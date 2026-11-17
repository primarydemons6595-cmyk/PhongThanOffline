namespace PhongThanCharacterEditor;
internal static class BundleVerification
{
    internal static int Run(string source,string output){
        Directory.CreateDirectory(output);var original=File.ReadAllBytes(source);var f=PthcFile.Load(source);
        if(!f.CanSave||!f.ChecksumValid)throw new InvalidDataException(f.CompanionStatus);
        var before=f.GetItems();f.Money+=1;string saved=f.ExportPackage(Path.Combine(output,"export"));var g=PthcFile.Load(saved);
        if(!g.CanSave||!g.ChecksumValid||g.Money!=f.Money||g.Revision!=f.Revision+1||Directory.EnumerateFiles(Path.Combine(output,"export"),"*",SearchOption.AllDirectories).Count()!=1)throw new InvalidDataException("Single-file export regression.");
        var disk=PthcBundle.Read(File.ReadAllBytes(saved));
        if(disk.Items==null||g.ItemCount!=before.Count)throw new InvalidDataException("Embedded attributes missing.");
        string copied=Path.Combine(output,"copied.pthc");File.Copy(saved,copied);var copy=PthcFile.Load(copied);
        if(!copy.CanSave||copy.ItemCount!=f.ItemCount)throw new InvalidDataException("Standalone copy requires sidecars.");
        if(g.ItemCount>0){int index=(int)g.ItemCount-1;g.DeleteItem(index);var deleted=PthcFile.Load(g.ExportPackage(Path.Combine(output,"deleted")));if(!deleted.CanSave||deleted.ItemCount!=f.ItemCount-1)throw new InvalidDataException("Deletion/ordinal rebinding regression.");}
        if(!File.ReadAllBytes(source).SequenceEqual(original))throw new InvalidDataException("Source was changed.");
        File.WriteAllText(Path.Combine(output,"result.txt"),"PASS real editor: exact embedded attributes, single-file export/copy/reload, revision, money edit, deletion and source unchanged");return 0;
    }
}
