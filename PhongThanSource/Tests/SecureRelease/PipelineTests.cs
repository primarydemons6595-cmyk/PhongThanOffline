using System.Security.Cryptography;
using System.Text.Json;
using PhongThanStudio.Core;
using PhongThan.Event.Runtime;

static class PipelineTests
{
    public static void Run(string fixture, Action<bool,string> check)
    {
        string project = Path.Combine(fixture, "pipeline"), loose = Path.Combine(project, "PhongThanRuntime-Loose");
        foreach (Side side in Enum.GetValues<Side>())
        {
            string role = Path.Combine(loose, side.ToString()); Directory.CreateDirectory(Path.Combine(role, "settings"));
            File.WriteAllText(Path.Combine(role, "loose_only.flag"), "test");
            File.WriteAllBytes(Path.Combine(role, "loose_index.bin"), new LooseIndex().Serialize());
            File.WriteAllText(Path.Combine(role, "settings", "a.ini"), "first");
            File.WriteAllText(Path.Combine(role, "settings", "c.ini"), "remove me");
        }
        File.WriteAllText(Path.Combine(loose,"Client","serverlist.ini"), "[Server]\nHost=127.0.0.1\n");
        File.WriteAllText(Path.Combine(loose,"Client","config.ini"), "[Client]\nFPS=1\n");
        File.WriteAllText(Path.Combine(loose,"Client","settings","raw-\u009B.ini"), "C1 byte name");
        var workspace = new Workspace(loose); var pipeline = new ReleasePipeline(workspace); pipeline.InitializeKeys();
        using var keys = pipeline.OpenKeys();
        foreach (Side side in Enum.GetValues<Side>())
        {
            string runtime = Path.Combine(project,"PhongThanSource","Output","SecureRelease",side.ToString()); Directory.CreateDirectory(runtime);
            string[] names = side == Side.Client ? ["Engine.dll","CoreClient.dll","Game.exe"] : ["Engine.dll","CoreServer.dll","GameServer.exe","Bishop.exe","Goddess.exe","PhongThanAccountServer.exe","PhongThanRelay.exe"];
            var hashes = new Dictionary<string,string>();
            foreach (string name in names) { byte[] binary = "fixture-only-not-executable"u8.ToArray(); File.WriteAllBytes(Path.Combine(runtime,name),binary); hashes[name.ToLowerInvariant()] = Convert.ToHexString(SHA256.HashData(binary)); }
            File.WriteAllText(Path.Combine(runtime,"runtime-build.json"),JsonSerializer.Serialize(new ReleasePipeline.RuntimeCertificate(keys.RuntimeFingerprint,hashes,ReleaseSourceIdentity.Compute(Path.Combine(project,"PhongThanSource"),Path.Combine(loose,"ManagerStudio","Wpf","PhongThanStudio.Core")))));
            pipeline.BuildBase(side);
            check(pipeline.Scan(side).Differences.Count == 0,"base scan clean " + side);
        }
        string client = workspace.RoleRoot(Side.Client);
        File.WriteAllText(Path.Combine(client,"settings","a.ini"),"changed");
        File.WriteAllText(Path.Combine(client,"settings","b.ini"),"added");
        File.Delete(Path.Combine(client,"settings","c.ini"));
        var scan = pipeline.Scan(Side.Client);
        check(scan.Differences.Select(d=>d.Kind).Order().SequenceEqual(new[]{"ADD","CHANGE","DELETE"}),"delta detects add, edit, delete");
        string patch = pipeline.BuildPatch(Side.Client);
        check(Path.GetFileName(patch)=="Client 1.01" && File.Exists(Path.Combine(patch,"Data","update_0000000002.pak")),"copy-paste patch layout");
        check(pipeline.Scan(Side.Client).Differences.Count==0,"effective chain baseline advances after patch");
        check(pipeline.BuildPatch(Side.Client)=="NO_CHANGES","no empty version on no-op patch");
        File.AppendAllText(Path.Combine(client,"config.ini"),"Window=1\n");
        string second = pipeline.BuildPatch(Side.Client);
        check(File.Exists(Path.Combine(second,"config.ini")),"operational file delta copied");
        check(!Directory.Exists(Path.Combine(second,"CharacterStore"))&&!Directory.Exists(Path.Combine(second,"NativeDropPersistenceV1")),"patch excludes live persistence");
        // Remove a required predecessor: a later patch cannot silently mount alone.
        string firstPak=Path.Combine(patch,"Data","update_0000000002.pak"); File.Move(firstPak,firstPak+".held");
        bool rejected=false;try{pipeline.Scan(Side.Client);}catch(InvalidDataException){rejected=true;}
        check(rejected,"missing patch predecessor rejected"); File.Move(firstPak+".held",firstPak);
        check(File.Exists(workspace.FilePath(Side.Client,"loose_only.flag")),"dev marker preserved");
        string sourceRoot=Path.Combine(project,"PhongThanSource"),coreRoot=Path.Combine(loose,"ManagerStudio","Wpf","PhongThanStudio.Core");
        string eventSource=Path.Combine(sourceRoot,"Sources","EventService","Program.cs");Directory.CreateDirectory(Path.GetDirectoryName(eventSource)!);File.WriteAllText(eventSource,"// new Event source");
        rejected=false;try{pipeline.BuildPatch(Side.Server);}catch(InvalidOperationException e){rejected=e.Message.Contains("Build runtime Release");}
        check(rejected&&!Directory.Exists(pipeline.Update(Side.Server,1)),"stale Event source blocks patch instead of silently omitting Event");
        File.Delete(eventSource);Directory.CreateDirectory(coreRoot);string coreSource=Path.Combine(coreRoot,"EventControl.cs");File.WriteAllText(coreSource,"// changed shared service logic");
        rejected=false;try{pipeline.ValidateRuntime(Side.Server);}catch(InvalidOperationException){rejected=true;}
        check(rejected,"changed Studio.Core invalidates certified Event runtime");File.Delete(coreSource);
        string serverRuntime=Path.Combine(sourceRoot,"Output","SecureRelease","Server");
        var newHashes=new Dictionary<string,string>();foreach(string file in Directory.EnumerateFiles(serverRuntime,"*.exe").Concat(Directory.EnumerateFiles(serverRuntime,"*.dll")))newHashes[Path.GetFileName(file)]=Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(file)));
        foreach(string name in new[]{"PhongThan.Event.dll","PhongThan.Event.Runtime.dll","PhongThan.Event.deps.json","PhongThan.Event.runtimeconfig.json","PhongThan.Event.exe"}){
            byte[] bytes=System.Text.Encoding.UTF8.GetBytes("updated fixture "+name);File.WriteAllBytes(Path.Combine(serverRuntime,name),bytes);newHashes[name]=Convert.ToHexString(SHA256.HashData(bytes));
        }
        string marker=Path.Combine(serverRuntime,"runtime-build.json");
        File.WriteAllText(marker,JsonSerializer.Serialize(new ReleasePipeline.RuntimeCertificate(keys.RuntimeFingerprint,newHashes)));
        rejected=false;try{pipeline.ValidateRuntime(Side.Server);}catch(InvalidOperationException){rejected=true;}
        check(rejected,"legacy certificate without source identity rejected");
        File.WriteAllText(marker,JsonSerializer.Serialize(new ReleasePipeline.RuntimeCertificate(keys.RuntimeFingerprint,newHashes,ReleaseSourceIdentity.Compute(sourceRoot,coreRoot))));
        var previewLog=new List<string>();int preview=pipeline.PreviewRuntimeChanges(Side.Server,previewLog.Add);
        check(preview==5&&previewLog.Contains("ADD runtime: PhongThan.Event.dll")&&!Directory.Exists(pipeline.Update(Side.Server,1)),"runtime preview detects Event additions without publishing");
        var runtimeLog=new List<string>();string eventPatch=pipeline.BuildPatch(Side.Server,runtimeLog.Add);
        check(newHashes.Keys.Where(n=>n.StartsWith("PhongThan.Event.")||n=="PhongThan.Event.Runtime.dll").All(n=>File.Exists(Path.Combine(eventPatch,n))),"rebuilt Event runtime and dependency files included in patch");
        check(runtimeLog.Any(l=>l=="ADD runtime: PhongThan.Event.dll"),"patch logs individual Event runtime file");
        check(!File.Exists(Path.Combine(eventPatch,"PhongThanStudio.Core.dll"))&&File.Exists(Path.Combine(eventPatch,"PhongThan.Event.Runtime.dll")),"release patch uses independent runtime without Studio DLL");
        newHashes["PhongThanStudio.Core.dll"]="obsolete";
        File.WriteAllText(marker,JsonSerializer.Serialize(new ReleasePipeline.RuntimeCertificate(keys.RuntimeFingerprint,newHashes,ReleaseSourceIdentity.Compute(sourceRoot,coreRoot))));
        rejected=false;try{pipeline.ValidateRuntime(Side.Server);}catch(InvalidOperationException e){rejected=e.Message.Contains("còn thư viện Studio");}
        check(rejected,"certificate containing Studio dependency is rejected");
        newHashes.Remove("PhongThanStudio.Core.dll");
        string ai=Path.Combine(workspace.RoleRoot(Side.Server),"BotAIgent","LocalAI");Directory.CreateDirectory(Path.Combine(ai,"llama"));File.WriteAllText(Path.Combine(ai,"Qwen3-0.6B-Q8_0.gguf"),"GGUF fixture");File.WriteAllText(Path.Combine(ai,"llama","llama-server.exe"),"fixture server");File.WriteAllText(Path.Combine(ai,"llama","llama.dll"),"fixture DLL");
        var package=LocalAiRelease.Export(workspace.RoleRoot(Side.Server),serverRuntime,true);foreach(var file in package.Files)newHashes[file.Key]=file.Value;newHashes[LocalAiRelease.Manifest]=Convert.ToHexString(SHA256.HashData(File.ReadAllBytes(Path.Combine(serverRuntime,LocalAiRelease.Manifest))));
        File.WriteAllText(marker,JsonSerializer.Serialize(new ReleasePipeline.RuntimeCertificate(keys.RuntimeFingerprint,newHashes,ReleaseSourceIdentity.Compute(sourceRoot,coreRoot),true)));pipeline.SetIncludeLocalAi(true);
        string aiPatch=pipeline.BuildPatch(Side.Server);check(package.Files.Keys.All(file=>File.Exists(Path.Combine(aiPatch,file))),"server patch includes certified model, executable and nested AI dependency files");
        pipeline.SetIncludeLocalAi(false);rejected=false;try{pipeline.ValidateRuntime(Side.Server);}catch(InvalidOperationException){rejected=true;}check(rejected,"changing include-AI checkbox requires a matching runtime build");
    }
}
