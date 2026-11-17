from pathlib import Path
p=Path(__file__).resolve().parents[2]/'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.Core/CustomEvents.cs'
d=p.read_text('utf8');start=d.index('        // Generate everything before the commit marker;');end=d.index('    static void EnsureOwned',start)
code='''        var writes=new Dictionary<string,byte[]>(StringComparer.OrdinalIgnoreCase);
        foreach(var (relative,bytes) in files)writes[workspace.FilePath(Side.Server,relative)]=bytes;
        const string tooltipPath="settings/phongthan/custom_event_items.tsv";byte[] tooltipBytes=CustomEventCompiler.Tooltips(doc);
        index.WithPath(tooltipPath);writes[indexPath]=index.Serialize();
        var clientIndex=new LooseIndex(workspace.FilePath(Side.Client,"loose_index.bin"));clientIndex.WithPath(tooltipPath);writes[workspace.FilePath(Side.Client,"loose_index.bin")]=clientIndex.Serialize();
        foreach(var role in Enum.GetValues<Side>())writes[workspace.FilePath(role,tooltipPath)]=tooltipBytes;
        writes[DocumentPath]=JsonSerializer.SerializeToUtf8Bytes(doc,Json);
        var ownership=new CustomEventOwnership((saved??previous!).Id,files.Select(f=>new CustomEventPublishedFile(f.Key,Legacy.Hash(f.Value))).ToArray());
        writes[ownerPath]=JsonSerializer.SerializeToUtf8Bytes(ownership,Json);
        string overlayPath=Path.Combine(workspace.Root,"completion","studio_effective_overrides.json");
        var overlays=File.Exists(overlayPath)?JsonSerializer.Deserialize<List<StudioOverride>>(File.ReadAllBytes(overlayPath))??[]:[];
        foreach(var f in owned)overlays.RemoveAll(o=>o.Side=="Server"&&o.Path.Replace('\\\\','/').Equals(f.Path,StringComparison.OrdinalIgnoreCase));
        foreach(var (relative,bytes) in files)overlays.Add(new("Server",relative,Legacy.Hash(bytes),[Legacy.Id(Encoding.ASCII.GetBytes("\\\\"+relative.Replace('/','\\\\')))]));
        foreach(var role in Enum.GetValues<Side>()){overlays.RemoveAll(o=>o.Side==role.ToString()&&o.Path.Replace('\\\\','/')==tooltipPath);overlays.Add(new(role.ToString(),tooltipPath,Legacy.Hash(tooltipBytes),[Legacy.Id(Encoding.ASCII.GetBytes("\\\\"+tooltipPath.Replace('/','\\\\')))]));}
        writes[overlayPath]=JsonSerializer.SerializeToUtf8Bytes(overlays,Json);
        string releasePath=Path.Combine(workspace.Root,"StudioData","CustomEvents","release-manifest.json");
        writes[releasePath]=JsonSerializer.SerializeToUtf8Bytes(doc.Events.SelectMany(e=>CustomEventCompiler.Scripts(e)).Select(f=>new CustomEventPublishedFile("Server/"+f.Key,Legacy.Hash(f.Value))).ToArray(),Json);
        string snapshotPath=Path.Combine(Root,"current.tsv");byte[] snapshot=CustomEventCompiler.Snapshot(doc);
        var deletions=owned.Where(o=>!files.ContainsKey(o.Path)).Select(o=>workspace.FilePath(Side.Server,o.Path)).ToList();if(saved is null){writes.Remove(ownerPath);deletions.Add(ownerPath);}
        var preimages=writes.Keys.Append(snapshotPath).Concat(deletions).Distinct(StringComparer.OrdinalIgnoreCase).ToDictionary(p=>p,p=>File.Exists(p)?File.ReadAllBytes(p):null,StringComparer.OrdinalIgnoreCase);
        try{
            foreach(var (path,bytes) in writes)EventControlJson.AtomicBytes(path,bytes);
            foreach(string path in deletions)if(File.Exists(path))File.Delete(path);
            EventControlJson.AtomicBytes(snapshotPath,snapshot);
        }catch{
            foreach(var (path,bytes) in preimages){if(bytes is null){if(File.Exists(path))File.Delete(path);}else EventControlJson.AtomicBytes(path,bytes);}throw;
        }
    }
'''
d=d[:start]+code+d[end:]
start=d.index('    void WriteReleasePackage(');d=d[:start]+'}\n'
p.write_text(d,'utf8');print('Custom event publication now rolls back all file/index/metadata changes on failure; records source ownership and client tooltip overlay')
