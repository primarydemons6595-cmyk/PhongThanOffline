using PhongThanStudio.Core;
using PhongThan.Event.Runtime;

if (args.Length < 2) { Console.Error.WriteLine("init|scan|base|patch|header <LooseRoot> [Client|Server]"); return 2; }
try
{
    if (args[0] == "stop-loose") { await new ServerControl(args[1]).Stop(Console.WriteLine); return 0; }
    var pipeline = new ReleasePipeline(new Workspace(args[1]));
    if(args[0]=="source-fingerprint") { Console.WriteLine(ReleaseSourceIdentity.Compute(Path.Combine(Directory.GetParent(Path.GetFullPath(args[1]))!.FullName,"PhongThanSource"),Path.Combine(args[1],"ManagerStudio","Wpf","PhongThanStudio.Core")));return 0; }
    if(args[0]=="stage-local-ai"){
        bool include=args.Length>=4&&args[3]=="include";pipeline.SetIncludeLocalAi(include);var staged=LocalAiRelease.Export(Path.Combine(Path.GetFullPath(args[1]),"Server"),Path.GetFullPath(args[2]),include);
        Console.WriteLine($"LOCAL_AI_STAGE {(include?"INCLUDED":"OMITTED")} model={staged.Model} files={staged.Files.Count} bytes={staged.Bytes}");return 0;
    }
    if (args[0] == "init") { pipeline.InitializeKeys(); Console.WriteLine("KEYS_READY"); return 0; }
    if (args[0] == "header") { Console.WriteLine(pipeline.WriteRuntimeHeader()); return 0; }
    var side = Enum.Parse<Side>(args[2]);
    if(args[0]=="runtime-check") { pipeline.ValidateRuntime(side); Console.WriteLine("RUNTIME_SOURCE_CURRENT");return 0; }
    if(args[0]=="bench-patches")
    {
        using var keys=pipeline.OpenKeys();string sourceData=Path.Combine(pipeline.Root,side+" Base","Data"),data=Path.Combine(args[3],"Data");
        var records=ReleaseCatalog.Read(Path.Combine(sourceData,ReleaseCatalog.FileName),side,keys.Signer);byte[] parent=records[^1].Digest;
        string source=Path.Combine(args[3],"_benchmark_input.bin");byte[] name="\\settings\\__patch_benchmark.ini"u8.ToArray();
        for(int i=0;i<100;i++)
        {
            byte[] bytes=System.Text.Encoding.ASCII.GetBytes("patch="+i);File.WriteAllBytes(source,bytes);uint sequence=(uint)records.Count;string file=$"update_{sequence:D10}.pak";
            parent=SecurePak.Write(Path.Combine(data,file),side,sequence,parent,pipeline.ReserveArchiveId(),[new(Legacy.Id(name),name,source,System.Security.Cryptography.SHA256.HashData(bytes),bytes.Length)],keys);
            records.Add(new(sequence,file,parent));
        }
        ReleaseCatalog.Write(Path.Combine(data,ReleaseCatalog.FileName),side,records,keys.Signer);File.Delete(source);Console.WriteLine("PATCH_BENCH_READY count=100");return 0;
    }
    if(args[0] is "catalog" or "bench-list")
    {
        using var keys=pipeline.OpenKeys();string data=Path.Combine(pipeline.Root,side+" Base","Data");
        var records=new List<ReleaseCatalogEntry>();var names=new List<byte[]>();
        foreach(string file in Directory.EnumerateFiles(data,"*.pak")){using var pak=new SecurePak(file,keys.Master,keys.Signer);records.Add(new(pak.Sequence,Path.GetFileName(file),pak.Digest.ToArray()));names.AddRange(pak.Entries.Where(e=>!e.Deleted).Select(e=>e.Name.ToArray()));}
        if(args[0]=="catalog") { string catalog=Path.Combine(data,ReleaseCatalog.FileName);string temporary=catalog+".next";ReleaseCatalog.Write(temporary,side,records.OrderBy(r=>r.Sequence).ToList(),keys.Signer);File.Move(temporary,catalog,true);Console.WriteLine("CATALOG_READY"); }
        else { byte[][] ordered=names.DistinctBy(n=>Convert.ToHexString(n)).OrderBy(n=>Legacy.Id(n)).ToArray();var rng=new Random(20261028);rng.Shuffle(ordered);using var output=File.Create(args[3]);foreach(var name in ordered.Take(2000)){output.Write(name);output.WriteByte(10);}Console.WriteLine("BENCH_LIST_READY"); }
        return 0;
    }
    if(args[0]=="inspect")
    {
        using var keys=pipeline.OpenKeys();using var pak=new SecurePak(Path.Combine(pipeline.Root,side+" Base","Data","release.pak"),keys.Master,keys.Signer);
        var e=pak.Entries.First(e=>Legacy.Encoding(1252).GetString(e.Name).Equals(args[3],StringComparison.OrdinalIgnoreCase));
        Console.WriteLine($"id={e.Id:X8} opaque={Convert.ToHexString(e.OpaqueId)}");
        foreach(var c in e.Chunks){Console.WriteLine($"offset={c.Offset} stored={c.Stored} counter={c.Counter}");byte[] b=pak.ReadChunk(e,c);System.Security.Cryptography.CryptographicOperations.ZeroMemory(b);}
        return 0;
    }
    if (args[0] == "base") Console.WriteLine(pipeline.BuildBase(side, Console.WriteLine));
    else if (args[0] == "patch") Console.WriteLine(pipeline.BuildPatch(side, Console.WriteLine));
    else if (args[0] == "scan")
    {
        var scan = pipeline.Scan(side, Console.WriteLine); foreach (var d in scan.Differences) Console.WriteLine($"{d.Kind}\t{d.Id:X8}\t{d.Path}"); Console.WriteLine($"CHANGES={scan.Differences.Count} NEXT={scan.Sequence}");
    }
    else throw new ArgumentException("Unknown command");
    return 0;
}
catch (Exception e) { Console.Error.WriteLine(e); return 1; }
