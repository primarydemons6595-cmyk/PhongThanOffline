using System.Net;
using System.Net.Sockets;
using System.Text.Json;
using PhongThan.EventService;
using PhongThan.Event.Runtime;

int checks = 0;
void Check(bool condition, string message) { if (!condition) throw new Exception(message);checks++; }
var runtimeAssembly=typeof(EventControlStore).Assembly;
Check(runtimeAssembly.GetName().Name=="PhongThan.Event.Runtime","dedicated runtime assembly");
Check(!runtimeAssembly.GetReferencedAssemblies().Any(a=>a.Name!.Contains("Studio")),"runtime does not reference Studio");
Check(!typeof(EventNetworkConfiguration).Assembly.GetReferencedAssemblies().Any(a=>a.Name!.Contains("Studio")),"Event host does not reference Studio");
Check(!runtimeAssembly.GetTypes().Any(t=>new[]{"SecurePak","SecurePakKeys","ReleasePipeline","ReleaseCatalog","NativeBuild","Workspace","MapEditing"}.Contains(t.Name)),"runtime contains no PAK publisher, keys, build or editor types");
var network = EventNetworkConfiguration.Parse("[Network]\nIP=127.0.0.2\nPort=18980\n");
Check(network.Address.ToString() == "127.0.0.2" && network.Port == 18980, "cfg IP and port parsed");
foreach (string invalid in new[] { "[Network]\nIP=0.0.0.0\nPort=18979", "[Network]\nIP=127.0.0.1\nPort=1", "[Network]\nIP=127.0.0.1\nPort=18979\nPort=18980", "[Network]\nIP=127.0.0.1\nPort=18979\nUnknown=1" })
{
    try { EventNetworkConfiguration.Parse(invalid);throw new Exception("Bad cfg accepted"); }
    catch (InvalidDataException) { checks++; }
}
string root = Path.Combine(Path.GetTempPath(), "PhongThanEventHttp_" + Guid.NewGuid().ToString("N"));
Directory.CreateDirectory(Path.Combine(root, "Client"));Directory.CreateDirectory(Path.Combine(root, "Server"));
EventControlJson.Atomic(Path.Combine(root, "Server", "EventService", "catalog.json"), new[] { new EventDefinition { Id = 1, Key = "vtt_tho", Name = "Test VTT", Adapter = "vtt", Map = 1067, Mission = 1, Defaults = new EventRule { Id = 1 } } });
using var probe = new TcpListener(IPAddress.Loopback, 0);probe.Start();int port = ((IPEndPoint)probe.LocalEndpoint).Port;probe.Stop();
string cfgPath = Path.Combine(AppContext.BaseDirectory, "PhongThan.Event.cfg");
byte[]? oldCfg = File.Exists(cfgPath) ? File.ReadAllBytes(cfgPath) : null;
File.WriteAllText(cfgPath, $"[Network]\nIP=127.0.0.2\nPort={port}\n");
using var cancel = new CancellationTokenSource(TimeSpan.FromSeconds(30));
Task<int>? host = null;
try
{
    host = Task.Run(() => EventServiceHost.RunAsync(["--root", root], cancel.Token, _ => { }));
    for (int i = 0; i < 100 && !File.Exists(Path.Combine(root, "Server", "EventService", "endpoint.json")); i++)
    { if (host.IsCompleted) { await host;throw new Exception("Host exited before listening"); } await Task.Delay(50); }
    var configuredEndpoint = EventControlJson.Read<EventEndpoint>(Path.Combine(root, "Server", "EventService", "endpoint.json"));
    Check(configuredEndpoint.Url == $"http://127.0.0.2:{port}/", "actual HTTP listener honors cfg IP/port without command-line override");
    using var unauthenticated = new HttpClient { BaseAddress = new Uri(configuredEndpoint.Url) };
    using (var reply = await unauthenticated.GetAsync("v1/status")) Check(reply.StatusCode == HttpStatusCode.Unauthorized, "API must reject missing token");
    using var client = new EventServiceClient(root);var dashboard = await client.Status();
    Check(dashboard.ServicePid == Environment.ProcessId && dashboard.Native is null, "No native bridge must not appear connected");
    Check(dashboard.Catalog.Length == 1, "Catalog returned");
    using (var reply = await unauthenticated.GetAsync("v1/bots")) Check(reply.StatusCode == HttpStatusCode.Unauthorized, "Bot API must reject missing token");
    var bots=await client.Bots();Check(!bots.Fresh&&bots.Native is null,"Bot API cannot invent native connectivity");
    var bot=new BotProfile{Name="HttpBot",X=45000,Y=90000};var botSaved=await client.SaveBot(new(bots.Configuration.Revision,bot));
    Check(botSaved.Bots.Single().Id==bot.Id,"Bot creation persisted through authenticated API");
    var botStopped=await client.DeleteBot(new(botSaved.Revision,bot.Id));Check(botStopped.Bots.Single().Deleted,"Bot archive through API preserves identity");
    using (var reply=await unauthenticated.PostAsync("v1/bots/batch",new StringContent("{}")))Check(reply.StatusCode==HttpStatusCode.Unauthorized,"Batch API rejects missing token");
    var created=await client.CreateBots(new(botStopped.Revision,BotCreation.Batch(bot,3,true,botStopped.Bots)));
    Check(created.Bots.Length==4&&created.Revision==botStopped.Revision+1,"Authenticated bulk creation commits all three bots atomically");
    var distributed=BotCreation.ServiceBatch("",13,true,-1,created.Bots);
    Check(distributed.Select(b=>(b.Profession,b.Gender)).Distinct().Count()==6&&distributed.GroupBy(b=>(b.Profession,b.Gender)).All(g=>g.Count() is 2 or 3),"Release form distributes batches evenly across all six profession/sex groups");
    Check(distributed.All(b=>{var spawn=BotCreation.Starter(b.Profession);return b.Map==spawn.Map&&b.X==spawn.X&&b.Y==spawn.Y&&b.Enabled&&b.Activity=="auto";}),"Release creation uses the real starter spawn and automatic activity for every profession");
    var distributedSaved=await client.CreateBots(new(created.Revision,distributed));
    Check(distributedSaved.Bots.Length==17&&!distributedSaved.Running,"Release creation persists new roles without unexpectedly starting the roster");
    var vietnamese=BotCreation.ServiceBatch("Gió Lạnh Về",1,false,5,distributedSaved.Bots).Single();
    Check(vietnamese.Profession==2&&vietnamese.Gender==1&&BotNames.Valid(vietnamese.Name)&&BotNames.WireHex(vietnamese.Name).Length<=48,"Release form accepts accented Vietnamese through the game TCVN3 encoder");
    int activeRemaining=BotLimits.Active-distributedSaved.Bots.Count(b=>!b.Deleted);
    try{BotCreation.ServiceBatch("",activeRemaining+1,true,-1,distributedSaved.Bots);throw new Exception("Exceeded roster capacity");}catch(InvalidDataException){checks++;}
    try{await client.CreateBots(new(created.Revision,[vietnamese]));throw new Exception("Stale form overwrote the roster");}catch(InvalidOperationException){checks++;}
    Check((await client.Bots()).Configuration.Bots.Length==17,"A stale creation request leaves existing bots intact");
    var listed=BotNameList.Parse("\r\n Gió Lạnh Về \r\nHải Bánh Cuốn\nTênThử03\nTênThử04\nTênThử05\nTênThử06\n");
    Check(listed.Length==6&&listed[0]=="Gió Lạnh Về","TXT input trims outer spaces, ignores blank lines and preserves accented names");
    string txt=Path.Combine(root,"bot-names.txt");File.WriteAllText(txt,string.Join("\r\n",listed),new System.Text.UTF8Encoding(true));
    Check(BotNameList.Read(txt).SequenceEqual(listed),"TXT accepts UTF-8 BOM");
    File.WriteAllText(txt,string.Join("\r\n",listed),System.Text.Encoding.Unicode);Check(BotNameList.Read(txt).SequenceEqual(listed),"TXT accepts UTF-16 Unicode BOM");
    foreach(string invalid in new[]{"\n\n","TrùngTên\ntrùngtên","TrùngTên\nTru\u0300ngTe\u0302n","Tên!Sai",string.Join("\n",Enumerable.Range(0,129).Select(i=>"BotTen"+i))}){
        try{BotNameList.Parse(invalid);throw new Exception("Invalid list was accepted");}catch(InvalidDataException){checks++;}
    }
    File.WriteAllBytes(txt,[0xff,0xff,0x00]);try{BotNameList.Read(txt);throw new Exception("Invalid encoding accepted");}catch(InvalidDataException){checks++;}
    var named=BotCreation.ServiceNamedBatch(listed,-1,distributedSaved.Bots);
    Check(named.Select(b=>b.Name).SequenceEqual(listed)&&named.Select(b=>(b.Profession,b.Gender)).Distinct().Count()==6,"Named batch preserves TXT order and balances six groups");
    var namedSaved=await client.CreateBots(new(distributedSaved.Revision,named));Check(namedSaved.Bots.Length==23,"Authenticated API creates the complete TXT batch atomically");
    try{BotCreation.ServiceNamedBatch(listed,0,namedSaved.Bots);throw new Exception("Existing names accepted");}catch(InvalidDataException){checks++;}
    Check((await client.Bots()).Configuration.Bots.Length==23,"A duplicate TXT import does not modify existing bots");
    try { await client.Command(1, "open", Guid.NewGuid());throw new Exception("Open incorrectly succeeded without GameServer"); }
    catch (InvalidOperationException) { checks++; }
    using(var denied=await unauthenticated.GetAsync("v1/bots/autonomy"))Check(denied.StatusCode==HttpStatusCode.Unauthorized,"Autonomy API requires authentication");
    var autonomy=await client.BotAutonomy();
    Check(autonomy.XuVan==60,"Autonomy default is the user-confirmed 60 van per xu");
    var advanced=await client.SaveBotAutonomy(new(autonomy.Revision,autonomy with {MaxAllies=3,PvpMaps=[1005,1006],Prices=[new(){Name="Rare equipment",Genre=0,Quality=4,MinVan=100,PriceVan=150,MaxVan=200}]}));
    var readAdvanced=await client.BotAutonomy();
    Check(readAdvanced.Revision==advanced.Revision&&readAdvanced.MaxAllies==3&&readAdvanced.PvpMaps.SequenceEqual(new[]{1005,1006})&&readAdvanced.Prices.Length==1,"Authenticated autonomy HTTP roundtrip preserves advanced and price rules");
    try {await client.SaveBotAutonomy(new(autonomy.Revision,autonomy));throw new Exception("Stale autonomy edit accepted");}catch(InvalidOperationException){checks++;}
    Check((await client.BotAutonomy()).Revision==advanced.Revision,"Stale autonomy save leaves current configuration unchanged");
    var rule = dashboard.Configuration.Events[0] with { Enabled = false, DailyTimes = ["18:20"] };
    var saved = await client.Save(dashboard.Configuration.Revision, rule);Check(saved.State == "pending_native_ack", "Save must await native ACK");
    Check((await client.Status()).Configuration.Events[0].Enabled == false, "Configuration persisted through HTTP");
    await client.Shutdown();Check(await host.WaitAsync(TimeSpan.FromSeconds(5)) == 0, "API shutdown exits host cleanly");
    using var stopped = JsonDocument.Parse(File.ReadAllBytes(Path.Combine(root, "Server", "EventService", "health.json")));
    Check(stopped.RootElement.GetProperty("State").GetString() == "stopped", "Stop health persisted");
    using var secondCancel = new CancellationTokenSource();
    host = Task.Run(() => EventServiceHost.RunAsync(["--root", root, "--port", port.ToString()], secondCancel.Token, _ => { }));
    await Task.Delay(300);Check(!host.IsCompleted, "Host can restart with same root and port");secondCancel.Cancel();
    Check(await host.WaitAsync(TimeSpan.FromSeconds(5)) == 0, "GUI cancellation path exits cleanly");
    string release=Path.Combine(root,"ReleaseWithoutStudio");Directory.CreateDirectory(Path.Combine(release,"Data"));
    File.WriteAllBytes(Path.Combine(release,"Data","release.catalog"),[0]); // Layout marker only; Event never opens PAK resources.
    EventControlJson.Atomic(Path.Combine(release,"EventService","catalog.json"),dashboard.Catalog);
    using var releaseCancel=new CancellationTokenSource();host=Task.Run(()=>EventServiceHost.RunAsync(["--root",release,"--port",port.ToString()],releaseCancel.Token,_=>{}));
    for(int i=0;i<100&&!File.Exists(Path.Combine(release,"EventService","endpoint.json"));i++){if(host.IsCompleted){await host;throw new Exception("Release host exited");}await Task.Delay(50);}
    using var releaseClient=new EventServiceClient(release);var releaseBots=await releaseClient.Bots();
    var releaseProfiles=BotCreation.ServiceNamedBatch(listed,-1,releaseBots.Configuration.Bots);await releaseClient.CreateBots(new(releaseBots.Configuration.Revision,releaseProfiles));
    Check(File.Exists(Path.Combine(release,"BotAIgent","bots.json"))&&!Directory.Exists(Path.Combine(release,"Server")),"Standalone Release persists TXT-created bots in its own folder without Studio or Loose layout");
    Check((await releaseClient.Bots()).Configuration.Bots.Select(b=>b.Name).SequenceEqual(listed),"Standalone Release returns all imported names through its own authenticated endpoint");
    await releaseClient.Shutdown();Check(await host.WaitAsync(TimeSpan.FromSeconds(5))==0,"Standalone Release service closes normally");
    Console.WriteLine($"PASS EVENT_SERVICE_HTTP checks={checks}; isolated root; no live server modified");
}
finally
{
    cancel.Cancel();if (host is not null) { try { await host.WaitAsync(TimeSpan.FromSeconds(5)); } catch { } }
    if (oldCfg is not null) File.WriteAllBytes(cfgPath, oldCfg);else File.Delete(cfgPath);
    // Keep the temporary fixture for diagnosis; no recursive filesystem cleanup.
}
