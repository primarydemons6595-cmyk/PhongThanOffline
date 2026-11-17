using System.Diagnostics;
using System.Net;
using System.Net.Http.Json;
using System.Text.Json;

namespace PhongThan.Event.Runtime;

public sealed record EventDashboard(int ServicePid, long UtcSeconds, EventDefinition[] Catalog, EventConfiguration Configuration, NativeEventStatus? Native);
public sealed record EventEndpoint(int Version, int Pid, string Url, string Root, DateTimeOffset StartedUtc);
public sealed record EventSaveResponse(EventConfiguration Configuration, string State);
public sealed record EventQueueResponse(EventCommand Command, EventReceipt Receipt);

public sealed class EventServiceClient : IDisposable
{
    readonly HttpClient http;
    public EventServiceClient(string root)
    {
        var endpoint = EventControlJson.Read<EventEndpoint>(Path.Combine(EventRuntimeLayout.Server(root), "EventService", "endpoint.json"));
        if (endpoint.Version != 1 || !Path.GetFullPath(endpoint.Root).TrimEnd('\\', '/').Equals(Path.GetFullPath(root).TrimEnd('\\', '/'), StringComparison.OrdinalIgnoreCase) ||
            !Uri.TryCreate(endpoint.Url, UriKind.Absolute, out var uri) || uri.Scheme != "http" || !IPAddress.TryParse(uri.Host, out var address) || !IPAddress.IsLoopback(address) || uri.AbsolutePath != "/" || uri.UserInfo.Length != 0)
            throw new InvalidDataException("Endpoint service không hợp lệ cho runtime này.");
        http = new HttpClient { BaseAddress = uri, Timeout = TimeSpan.FromSeconds(120) };
        string token = File.ReadAllText(Path.Combine(EventRuntimeLayout.Server(root), "EventService", "access.token")).Trim();
        if (token.Length != 64 || token.Any(c => !char.IsAsciiHexDigit(c))) throw new InvalidDataException("Token service không hợp lệ.");
        http.DefaultRequestHeaders.Add("X-PhongThan-Event-Token", token);
    }
    public static void Start(string root)
    {
        string exe = Path.Combine(EventRuntimeLayout.Server(root), "PhongThan.Event.exe");
        if (!File.Exists(exe)) throw new FileNotFoundException("Chưa triển khai Event Phong Thần cho runtime này.", exe);
        var start = new ProcessStartInfo(exe) { UseShellExecute = false, WorkingDirectory = Path.GetDirectoryName(exe)! };
        start.ArgumentList.Add("--root");start.ArgumentList.Add(Path.GetFullPath(root));
        _ = Process.Start(start) ?? throw new IOException("Không khởi động được service.");
    }
    async Task<T> Send<T>(HttpMethod method, string path, object? body = null)
    {
        using var request = new HttpRequestMessage(method, path);
        if (body is not null) request.Content = JsonContent.Create(body, options: EventControlJson.Options);
        using var response = await http.SendAsync(request).ConfigureAwait(false);
        byte[] bytes = await response.Content.ReadAsByteArrayAsync().ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            using var json = JsonDocument.Parse(bytes);
            throw new InvalidOperationException(json.RootElement.TryGetProperty("Error", out var error) ? error.GetString() : response.ReasonPhrase);
        }
        return JsonSerializer.Deserialize<T>(bytes, EventControlJson.Options) ?? throw new InvalidDataException("Service trả dữ liệu trống.");
    }
    public Task<EventDashboard> Status() => Send<EventDashboard>(HttpMethod.Get, "v1/status");
    public Task<BotDashboard> Bots() => Send<BotDashboard>(HttpMethod.Get,"v1/bots");
    public Task<BotAutonomyProfile> BotAutonomy()=>Send<BotAutonomyProfile>(HttpMethod.Get,"v1/bots/autonomy");
    public Task<BotAutonomyProfile> SaveBotAutonomy(BotAutonomySave request)=>Send<BotAutonomyProfile>(HttpMethod.Put,"v1/bots/autonomy",request);
    public Task<BotAiStatus> BotAi()=>Send<BotAiStatus>(HttpMethod.Get,"v1/bots/ai");
    public Task<BotAiStatus> SaveBotAi(BotAiSaveRequest request)=>Send<BotAiStatus>(HttpMethod.Put,"v1/bots/ai",request);
    public Task<BotAiTestResult[]> TestBotAi(BotAiTestRequest request)=>Send<BotAiTestResult[]>(HttpMethod.Post,"v1/bots/ai/test",request);
    public Task<BotConfiguration> SaveBot(BotSaveRequest request)=>Send<BotConfiguration>(HttpMethod.Put,"v1/bots",request);
    public Task<BotConfiguration> CreateBots(BotBatchRequest request)=>Send<BotConfiguration>(HttpMethod.Post,"v1/bots/batch",request);
    public Task<BotConfiguration> ControlBots(BotControlRequest request)=>Send<BotConfiguration>(HttpMethod.Post,"v1/bots/control",request);
    public Task<BotConfiguration> DeleteBot(BotDeleteRequest request)=>Send<BotConfiguration>(HttpMethod.Post,"v1/bots/delete",request);
    public Task<EventSaveResponse> Save(long revision, EventRule rule) => Send<EventSaveResponse>(HttpMethod.Put, "v1/event", new { ExpectedRevision = revision, Rule = rule });
    public Task<EventQueueResponse> Command(int id, string operation, Guid requestId) => Send<EventQueueResponse>(HttpMethod.Post, "v1/command", new { EventId = id, Operation = operation, RequestId = requestId });
    public Task<EventReceipt> Receipt(string id) => Send<EventReceipt>(HttpMethod.Get, "v1/receipt/" + Uri.EscapeDataString(id));
    public Task<string[]> DropTables() => Send<string[]>(HttpMethod.Get, "v1/drop-tables");
    public Task<EventDropText> DropTable(string name) => Send<EventDropText>(HttpMethod.Get, "v1/drop-table?name=" + Uri.EscapeDataString(name));
    public Task<EventSaveResponse> SaveDrop(EventDropDraftRequest request) => Send<EventSaveResponse>(HttpMethod.Post, "v1/drop-draft", request);
    public async Task Shutdown() { using var document = await Send<JsonDocument>(HttpMethod.Post, "v1/shutdown"); }
    public void Dispose() => http.Dispose();
}
