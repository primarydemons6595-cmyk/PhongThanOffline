using System.IO;
using System.Net;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using PhongThan.Event.Runtime;

namespace PhongThan.EventService;

internal static class EventServiceHost
{
internal static async Task<int> RunAsync(string[] args, CancellationToken cancellation, Action<string> log)
{
if (args.Length < 2 || args[0] != "--root")
{
    log("PhongThan.Event --root <runtime> [--port 18979]");
    return 2;
}
string root = Path.GetFullPath(args[1]);
var network = EventNetworkConfiguration.Load(Path.Combine(AppContext.BaseDirectory, "PhongThan.Event.cfg"));
int port = args.Length == 4 && args[2] == "--port" ? int.Parse(args[3], System.Globalization.CultureInfo.InvariantCulture) : network.Port;
if (port is < 1024 or > 65535) throw new ArgumentOutOfRangeException(nameof(port));
string mutexName = "Local\\PhongThan.Events." + Convert.ToHexString(SHA256.HashData(Encoding.UTF8.GetBytes(root.ToUpperInvariant())))[..24];
using var singleton = new Mutex(true, mutexName, out bool created);
if (!created) { log("Event service đang chạy cho runtime này."); return 3; }
var store = new EventControlStore(root);
var bots = new BotAIgentStore(root);
var botAutonomy=new BotAutonomyStore(root);
var botAiSettings=new BotAiSettingsStore(Path.Combine(EventRuntimeLayout.Server(root),"BotAIgent"));
using var runtimeLock = new FileStream(Path.Combine(store.ServiceRoot, "service.lock"), FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.None);
string tokenPath = Path.Combine(store.ServiceRoot, "access.token");
if (!File.Exists(tokenPath)) EventControlJson.AtomicBytes(tokenPath, Encoding.ASCII.GetBytes(Convert.ToHexStringLower(RandomNumberGenerator.GetBytes(32))));
byte[] expectedToken = Encoding.ASCII.GetBytes(File.ReadAllText(tokenPath).Trim());
if (expectedToken.Length != 64) throw new InvalidDataException("Token service không hợp lệ.");
using var stop = CancellationTokenSource.CreateLinkedTokenSource(cancellation);
using var botChat=new BotLocalChat(root,bots);
var chatWorker=Task.Run(()=>BotWorkerSupervisor.Run(botChat.Run,error=>{
    log("Bot chat worker recovered after "+error.GetType().Name);
    EventControlJson.Atomic(Path.Combine(store.ServiceRoot,"bot-chat-worker-fault.json"),new{Utc=DateTimeOffset.UtcNow,Type=error.GetType().FullName,Stack=error.StackTrace});
},stop.Token),stop.Token);
using var listener = new HttpListener();
string endpoint = $"http://{network.Address}:{port}/";
listener.Prefixes.Add(endpoint);listener.Start();
EventControlJson.Atomic(Path.Combine(store.ServiceRoot, "endpoint.json"), new { Version = 1, Pid = Environment.ProcessId, Url = endpoint, Root = root, StartedUtc = DateTimeOffset.UtcNow });
var scheduler = Task.Run(async () =>
{
    long lastMinute = DateTimeOffset.UtcNow.ToUnixTimeSeconds() / 60;
    while (!stop.IsCancellationRequested)
    {
        try
        {
            var now = DateTimeOffset.UtcNow;store.PublishLease(now);bots.Publish(now);
            long minute = now.ToUnixTimeSeconds() / 60;
            if (minute != lastMinute)
            {
                lastMinute = minute;
                var native = store.Native(now);var config = store.Configuration();
                if (native is not null && native.LeaseValid && native.Revision == config.Revision)
                {
                    var local = now.ToOffset(TimeSpan.FromHours(7));
                    foreach (var rule in config.Events.Where(e => e.Enabled))
                    foreach (string text in rule.DailyTimes)
                    {
                        var time = TimeOnly.ParseExact(text, "HH:mm", System.Globalization.CultureInfo.InvariantCulture);
                        var occurrence = new DateTimeOffset(local.Year, local.Month, local.Day, time.Hour, time.Minute, 0, local.Offset);
                        if (occurrence.ToUnixTimeSeconds() / 60 == minute)
                        {
                            var id = Guid.ParseExact(EventRules.ScheduledId(rule.Id, config.Revision, occurrence, "open"), "N");
                            store.Queue(rule.Id, "open", id, now);
                        }
                        foreach (int lead in new[] { 10, 5, 1 })
                        {
                            var nextOccurrence = occurrence < now.AddMinutes(-1) ? occurrence.AddDays(1) : occurrence;
                            if ((nextOccurrence.ToUnixTimeSeconds() - minute * 60) / 60 != lead) continue;
                            var id = Guid.ParseExact(EventRules.ScheduledId(rule.Id, config.Revision, nextOccurrence, "warning" + lead), "N");
                            store.Queue(rule.Id, "warning", id, now, lead * 60);
                        }
                    }
                }
            }
            EventControlJson.Atomic(Path.Combine(store.ServiceRoot, "health.json"), new { Version = 1, Pid = Environment.ProcessId, UtcSeconds = now.ToUnixTimeSeconds(), State = "running", Error = "" });
        }
        catch (Exception ex)
        {
            EventControlJson.Atomic(Path.Combine(store.ServiceRoot, "health.json"), new { Version = 1, Pid = Environment.ProcessId, UtcSeconds = DateTimeOffset.UtcNow.ToUnixTimeSeconds(), State = "error", Error = ex.Message });
            log(ex.Message);
        }
        await Task.Delay(1000, stop.Token).ConfigureAwait(false);
    }
}, stop.Token);
log("Event Phong Than listening on " + endpoint);
try
{
    while (!stop.IsCancellationRequested)
    {
        var pending = listener.GetContextAsync();
        var context = await pending.WaitAsync(stop.Token);
        // Serialized requests bound file writes and make CAS behavior deterministic.
        try
        {
            string? supplied = context.Request.Headers["X-PhongThan-Event-Token"];
            if (context.Request.RemoteEndPoint is null || !IPAddress.IsLoopback(context.Request.RemoteEndPoint.Address) ||
                !string.IsNullOrEmpty(context.Request.Headers["Origin"]) || supplied is null || supplied.Length != expectedToken.Length ||
                !CryptographicOperations.FixedTimeEquals(Encoding.ASCII.GetBytes(supplied), expectedToken))
            { await Reply(context, 401, new { Error = "Unauthorized" });continue; }
            string path = context.Request.Url!.AbsolutePath;
            int limit = path == "/v1/drop-draft" ? 4194304 : 65536;
            if (context.Request.ContentLength64 > limit) { await Reply(context, 413, new { Error = "Request too large" });continue; }
            string method = context.Request.HttpMethod;
            var now = DateTimeOffset.UtcNow;
            if (method == "GET" && path == "/v1/bots")
                await Reply(context, 200, bots.Status(now));
            else if(method=="GET"&&path=="/v1/bots/autonomy")
                await Reply(context,200,botAutonomy.Read());
            else if(method=="PUT"&&path=="/v1/bots/autonomy")
                await Reply(context,200,botAutonomy.Save(await Read<BotAutonomySave>(context.Request,stop.Token,limit)));
            else if(method=="GET"&&path=="/v1/bots/brain")
                await Reply(context,200,new BotBrain(Path.Combine(EventRuntimeLayout.Server(root),"BotAIgent")).Statistics());
            else if(method=="GET"&&path=="/v1/bots/ai")
                await Reply(context,200,botAiSettings.Status());
            else if(method=="PUT"&&path=="/v1/bots/ai")
                await Reply(context,200,botAiSettings.Save(await Read<BotAiSaveRequest>(context.Request,stop.Token,limit)));
            else if(method=="POST"&&path=="/v1/bots/ai/test")
            {
                var result=await botChat.Preview(await Read<BotAiTestRequest>(context.Request,stop.Token,limit),stop.Token);
                await Reply(context,200,result.Select(row=>new {row.Provider,row.Model,Reply=row.Command>0?$"Nhận diện yêu cầu ({row.Command}): {row.Argument}":row.Reply,row.Intent,row.Milliseconds,row.Error}));
            }
            else if(method=="POST"&&path=="/v1/bots/ai/test/local")
                await Reply(context,200,await botChat.Preview(await Read<BotAiTestRequest>(context.Request,stop.Token,limit),stop.Token,includeOnline:false));
            else if (method == "PUT" && path == "/v1/bots")
                await Reply(context, 200, bots.Upsert(await Read<BotSaveRequest>(context.Request, stop.Token, limit)));
            else if (method == "POST" && path == "/v1/bots/batch")
                await Reply(context, 200, bots.CreateBatch(await Read<BotBatchRequest>(context.Request, stop.Token, limit)));
            else if (method == "POST" && path == "/v1/bots/control")
                await Reply(context, 200, bots.Control(await Read<BotControlRequest>(context.Request, stop.Token, limit)));
            else if (method == "POST" && path == "/v1/bots/delete")
                await Reply(context, 200, bots.Delete(await Read<BotDeleteRequest>(context.Request, stop.Token, limit)));
            else if (method == "GET" && path == "/v1/status")
                await Reply(context, 200, new { ServicePid = Environment.ProcessId, UtcSeconds = now.ToUnixTimeSeconds(), Catalog = store.Catalog, Configuration = store.Configuration(), Native = store.Native(now) });
            else if (method == "PUT" && path == "/v1/event")
            {
                var request = await Read<SaveRequest>(context.Request, stop.Token, limit);
                var config = store.Save(request.ExpectedRevision, request.Rule);store.PublishLease(now);
                await Reply(context, 200, new { Configuration = config, State = "pending_native_ack" });
            }
            else if (method == "POST" && path == "/v1/command")
            {
                var request = await Read<CommandRequest>(context.Request, stop.Token, limit);
                var command = store.Queue(request.EventId, request.Operation, request.RequestId, now);
                await Reply(context, 202, new { Command = command, Receipt = store.Receipt(command.Id, now) });
            }
            else if (method == "GET" && path.StartsWith("/v1/receipt/", StringComparison.Ordinal))
                await Reply(context, 200, store.Receipt(path[12..], now));
            else if (method == "GET" && path == "/v1/drop-tables") await Reply(context, 200, store.DropTables());
            else if (method == "GET" && path == "/v1/drop-table") await Reply(context, 200, store.DropTable(context.Request.QueryString["name"] ?? ""));
            else if (method == "POST" && path == "/v1/drop-draft")
            {
                var request = await Read<EventDropDraftRequest>(context.Request, stop.Token, limit);
                var config = store.SaveDropDraft(request);store.PublishLease(now);
                await Reply(context, 200, new { Configuration = config, State = "pending_native_ack" });
            }
            else if (method == "POST" && path == "/v1/shutdown")
            { await Reply(context, 200, new { State = "stopping", ActiveMissionsContinueOnGameServer = true });stop.Cancel(); }
            else await Reply(context, 404, new { Error = "Not found" });
        }
        catch (Exception ex) when (ex is InvalidOperationException or InvalidDataException or ArgumentException or IOException or JsonException)
        { await Reply(context, ex is InvalidOperationException ? 409 : 400, new { Error = ex.Message }); }
        catch (OperationCanceledException) when (!stop.IsCancellationRequested) { await Reply(context, 408, new { Error = "Request timeout" }); }
        finally { context.Response.Close(); }
    }
}
catch (OperationCanceledException) when (stop.IsCancellationRequested) { }
finally
{
    stop.Cancel();listener.Stop();
    try { await scheduler; } catch (OperationCanceledException) { }
    try { await chatWorker; } catch (OperationCanceledException) { }
    EventControlJson.Atomic(Path.Combine(store.ServiceRoot, "health.json"), new { Version = 1, Pid = Environment.ProcessId, UtcSeconds = DateTimeOffset.UtcNow.ToUnixTimeSeconds(), State = "stopped", Error = "" });
}
return 0;
}

static async Task<T> Read<T>(HttpListenerRequest request, CancellationToken stop, int limit)
{
    using var timeout = CancellationTokenSource.CreateLinkedTokenSource(stop);timeout.CancelAfter(TimeSpan.FromSeconds(5));
    using var memory = new MemoryStream();var buffer = new byte[4096];int read;
    while ((read = await request.InputStream.ReadAsync(buffer, timeout.Token)) > 0)
    {
        if (memory.Length + read > limit) throw new InvalidDataException("Request too large");
        memory.Write(buffer, 0, read);
    }
    return JsonSerializer.Deserialize<T>(memory.ToArray(), EventControlJson.Options) ?? throw new InvalidDataException("Request trống.");
}
static async Task Reply(HttpListenerContext context, int status, object value)
{
    byte[] bytes = JsonSerializer.SerializeToUtf8Bytes(value, EventControlJson.Options);
    context.Response.StatusCode = status;context.Response.ContentType = "application/json; charset=utf-8";
    context.Response.ContentLength64 = bytes.Length;context.Response.Headers["Cache-Control"] = "no-store";
    await context.Response.OutputStream.WriteAsync(bytes);
}
sealed record SaveRequest(long ExpectedRevision, EventRule Rule);
sealed record CommandRequest(int EventId, string Operation, Guid RequestId);

}
