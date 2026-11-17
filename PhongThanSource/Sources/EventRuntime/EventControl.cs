using System.Globalization;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.RegularExpressions;

namespace PhongThan.Event.Runtime;

public sealed record EventDefinition
{
    public int Id { get; init; }
    public string Key { get; init; } = "";
    public string Name { get; init; } = "";
    public string Group { get; init; } = "";
    public string Adapter { get; init; } = "";
    public int Map { get; init; }
    public int Mission { get; init; }
    public int Template { get; init; }
    public int BossFlag { get; init; } = -1;
    public string StartScriptHex { get; init; } = "";
    public string SourceEvidence { get; init; } = "";
    public EventRule Defaults { get; init; } = new();
    public override string ToString() => Name;
}

public sealed record EventRule
{
    public int Id { get; init; }
    public bool Enabled { get; init; } = true;
    public string[] DailyTimes { get; init; } = [];
    public int RegistrationSeconds { get; init; } = 300;
    public int DurationSeconds { get; init; } = 1800;
    public int TicketGenre { get; init; } = 3;
    public int TicketDetail { get; init; }
    public int TicketParticular { get; init; }
    public int TicketLevel { get; init; }
    public int TicketSeries { get; init; }
    public int TicketCount { get; init; }
    public string RewardMode { get; init; } = "original";
    public long RewardRevision { get; init; } = 1;
    public string[] NativeDropSlots { get; init; } = ["", "", "", ""];
    public int MinimumLevel { get; init; } = 1;
    public int DailyLimit { get; init; } = 20;
    public int FreeRuns { get; init; } = 1;
    public bool RequireTong { get; init; }
    public int MinimumCredit { get; init; }
    public int RewardGenre { get; init; } = 3;
    public int RewardDetail { get; init; }
    public int RewardParticular { get; init; }
    public int RewardLevel { get; init; }
    public int RewardSeries { get; init; }
    public int RewardCount { get; init; }
    public int RewardExperience { get; init; }
    public int RewardMoney { get; init; }
    public int RewardCredit { get; init; }
}

public sealed record EventConfiguration(long Revision, EventRule[] Events);
public sealed record EventCommand(string Id, int EventId, string Operation, long Revision,
    int TargetPid, long TargetBootUtc, long IssuedUtc, long ExpiresUtc, int Argument = 0);
public sealed record NativeEventState(int Id, int Phase, long ActiveRevision, long Session, string Error);
public sealed record NativeEventStatus(int Version, int Pid, long BootUtc, long UtcSeconds,
    long Revision, bool LeaseValid, NativeEventState[] Events);
public sealed record EventReceipt(string Id, string State, string Message, int Pid, long BootUtc, long UtcSeconds);

public static class EventControlJson
{
    public static readonly JsonSerializerOptions Options = new()
    {
        PropertyNameCaseInsensitive = false,
        WriteIndented = true,
        UnmappedMemberHandling = JsonUnmappedMemberHandling.Disallow
    };
    public static T Read<T>(string path)
    {
        using var stream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite | FileShare.Delete);
        return JsonSerializer.Deserialize<T>(stream, Options) ?? throw new InvalidDataException("JSON trống: " + path);
    }
    public static void Atomic<T>(string path, T value)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        AtomicBytes(path, JsonSerializer.SerializeToUtf8Bytes(value, Options));
    }
    public static void AtomicBytes(string path, byte[] bytes)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        string temporary = path + "." + Guid.NewGuid().ToString("N") + ".tmp";
        try
        {
            using (var file = new FileStream(temporary, FileMode.CreateNew, FileAccess.Write, FileShare.None,
                4096, FileOptions.WriteThrough)) { file.Write(bytes); file.Flush(true); }
            // Native fopen readers share read/write but do not share deletion.
            // Windows can report ERROR_ACCESS_DENIED (5), not just sharing (32).
            // Keep the old snapshot intact and retry only this atomic swap.
            for(int attempt=0;;attempt++)
            {
                try{File.Move(temporary,path,true);break;}
                catch(Exception ex)when(ex is IOException or UnauthorizedAccessException)
                {
                    int code=ex.HResult&65535;
                    bool transient=OperatingSystem.IsWindows()&&code is 5 or 32 or 33&&!Directory.Exists(path);
                    try{if(File.Exists(path)&&(File.GetAttributes(path)&FileAttributes.ReadOnly)!=0)transient=false;}
                    catch(Exception detail)when(detail is IOException or UnauthorizedAccessException){transient=false;}
                    if(!transient||attempt>=7)throw new IOException("Không ghi được tệp '"+path+"' (Windows "+code+"): "+ex.Message,ex);
                    Thread.Sleep(Math.Min(80,10<<attempt));
                }
            }
        }
        finally { if (File.Exists(temporary)) File.Delete(temporary); }
    }
}

public static class EventRules
{
    static readonly Regex TimePattern = new(@"^(?:[01][0-9]|2[0-3]):[0-5][0-9]$", RegexOptions.CultureInvariant);
    static readonly Regex TablePattern = new(@"^[A-Za-z0-9_\-]{1,90}\.ini$", RegexOptions.CultureInvariant);
    public static void Validate(EventDefinition definition, EventRule rule)
    {
        if (rule.Id != definition.Id || rule.Id is < 1 or > 4095) throw new InvalidDataException("Sai mã event.");
        if (rule.DailyTimes.Length > 96 || rule.DailyTimes.Any(t => !TimePattern.IsMatch(t)) ||
            rule.DailyTimes.Distinct(StringComparer.Ordinal).Count() != rule.DailyTimes.Length)
            throw new InvalidDataException("Lịch phải là HH:mm, không trùng, tối đa 96 mốc/ngày.");
        if (rule.RegistrationSeconds is < 1 or > 3600 || rule.DurationSeconds is < 60 or > 86400)
            throw new InvalidDataException("Đăng ký 1–3600 giây; thời lượng 60–86400 giây.");
        if (rule.TicketGenre is < 0 or > 8 || rule.TicketDetail < 0 || rule.TicketParticular < 0 ||
            rule.TicketLevel is < 0 or > 1000 || rule.TicketSeries is < 0 or > 5 || rule.TicketCount is < 0 or > 1000)
            throw new InvalidDataException("Tuple hoặc số lượng vé không hợp lệ.");
        if (rule.NativeDropSlots.Length != 4 || rule.NativeDropSlots.Any(t => t.Length != 0 && !TablePattern.IsMatch(t)))
            throw new InvalidDataException("Cần bốn slot bảng native; tên INI không có thư mục.");
        if (rule.MinimumLevel is < 1 or > 1000 || rule.DailyLimit is < 1 or > 100 || rule.FreeRuns < 0 || rule.FreeRuns > rule.DailyLimit ||
            rule.MinimumCredit is < 0 or > 1000000 || rule.RewardGenre is < 0 or > 8 || rule.RewardDetail < 0 || rule.RewardParticular < 0 ||
            rule.RewardLevel is < 0 or > 1000 || rule.RewardSeries is < 0 or > 5 || rule.RewardCount is < 0 or > 64 ||
            rule.RewardExperience is < 0 or > 1000000000 || rule.RewardMoney is < 0 or > 1000000000 || rule.RewardCredit is < 0 or > 1000000)
            throw new InvalidDataException("Điều kiện hoặc phần thưởng nhiệm vụ ngoài giới hạn cho phép.");
        if (rule.RewardMode is not ("original" or "native_tables")) throw new InvalidDataException("Chế độ thưởng chưa được hỗ trợ.");
        if (rule.RewardMode == "native_tables" && (definition.Adapter is not ("vtt" or "boss") || rule.NativeDropSlots.All(string.IsNullOrEmpty)))
            throw new InvalidDataException("Adapter này chưa hỗ trợ thay bảng rơi native.");
    }
    public static DateTimeOffset? Next(EventRule rule, DateTimeOffset utc)
    {
        if (!rule.Enabled || rule.DailyTimes.Length == 0) return null;
        var local = utc.ToOffset(TimeSpan.FromHours(7));
        return rule.DailyTimes.Select(t =>
        {
            var time = TimeOnly.ParseExact(t, "HH:mm", CultureInfo.InvariantCulture);
            var candidate = new DateTimeOffset(local.Year, local.Month, local.Day, time.Hour, time.Minute, 0, local.Offset);
            return candidate > utc ? candidate : candidate.AddDays(1);
        }).Min();
    }
    public static string ScheduledId(int eventId, long revision, DateTimeOffset occurrence, string operation)
        => Convert.ToHexStringLower(SHA256.HashData(Encoding.ASCII.GetBytes($"{eventId}|{revision}|{occurrence.ToUnixTimeSeconds()}|{operation}")))[..32];
}

public sealed class EventControlStore
{
    readonly object sync = new();
    public string Root { get; }
    public string ServerRoot => EventRuntimeLayout.Server(Root);
    public string ServiceRoot => Path.Combine(ServerRoot, "EventService");
    public string BridgeRoot => ServiceRoot;
    public EventDefinition[] Catalog { get; }
    public EventControlStore(string root)
    {
        Root = Path.GetFullPath(root);
        if (!EventRuntimeLayout.Valid(Root))
            throw new InvalidDataException("Chọn đúng thư mục runtime có Client và Server.");
        Catalog = EventControlJson.Read<EventDefinition[]>(Path.Combine(ServiceRoot, "catalog.json"));
        if (Catalog.Length is < 1 or > 256 || Catalog.Select(d => d.Id).Distinct().Count() != Catalog.Length)
            throw new InvalidDataException("Danh mục event rỗng/trùng/quá lớn.");
        foreach (var item in Catalog) EventRules.Validate(item, item.Defaults);
        Directory.CreateDirectory(Path.Combine(ServiceRoot, "commands"));
        Directory.CreateDirectory(Path.Combine(BridgeRoot, "pending"));
        Directory.CreateDirectory(Path.Combine(BridgeRoot, "acks"));
        if (!File.Exists(ConfigPath)) EventControlJson.Atomic(ConfigPath, new EventConfiguration(1, Catalog.Select(d => d.Defaults).ToArray()));
        _ = Configuration();
    }
    string ConfigPath => Path.Combine(ServiceRoot, "events.json");
    public EventConfiguration Configuration()
    {
        lock (sync)
        {
            var config = EventControlJson.Read<EventConfiguration>(ConfigPath);
            if (config.Revision is < 1 or > 1000000000 || config.Events.Length != Catalog.Length ||
                config.Events.Select(e => e.Id).Distinct().Count() != config.Events.Length) throw new InvalidDataException("Cấu hình event không đầy đủ.");
            foreach (var rule in config.Events) EventRules.Validate(Catalog.Single(d => d.Id == rule.Id), rule);
            return config;
        }
    }
    public EventConfiguration Save(long expectedRevision, EventRule rule)
    {
        lock (sync)
        {
            var current = Configuration();
            if (current.Revision != expectedRevision) throw new InvalidOperationException("Cấu hình đã đổi; tải lại trước khi lưu.");
            EventRules.Validate(Catalog.Single(d => d.Id == rule.Id), rule);
            ValidateDropBindings(rule);
            rule = rule with { RewardRevision = checked(current.Revision + 1) };
            FreezeDropTables(rule);
            var next = new EventConfiguration(checked(current.Revision + 1), current.Events.Select(e => e.Id == rule.Id ? rule : e).ToArray());
            EventControlJson.Atomic(Path.Combine(ServiceRoot, "revisions", current.Revision.ToString(CultureInfo.InvariantCulture) + ".json"), current);
            EventControlJson.Atomic(ConfigPath, next);
            return next;
        }
    }
    void ValidateDropBindings(EventRule rule)
    {
        if (rule.RewardMode != "native_tables") return;
        foreach (string name in rule.NativeDropSlots.Where(t => t.Length > 0))
        {
            var path = DropSource(name);
            if (!File.Exists(path)) throw new InvalidDataException("Không có bảng native: " + name);
            if (new FileInfo(path).Length is < 8 or > 2097152) throw new InvalidDataException("Kích thước bảng native không hợp lệ.");
            _ = EventNativeDrop.Validate(Encoding.Latin1.GetString(File.ReadAllBytes(path)));
        }
    }
    string DropSource(string name)
    {
        if (name.Length < 5 || !System.Text.RegularExpressions.Regex.IsMatch(name, @"^[A-Za-z0-9_\-]{1,90}\.ini$", System.Text.RegularExpressions.RegexOptions.CultureInvariant)) throw new InvalidDataException("Tên bảng không hợp lệ.");
        return name.StartsWith("pt_event_custom_", StringComparison.Ordinal)
            ? Path.Combine(ServiceRoot, "drop_drafts", name)
            : Path.Combine(ServerRoot, "settings", "item", "droprate", name);
    }
    public string[] DropTables() => (Directory.Exists(Path.Combine(ServerRoot,"settings","item","droprate")) ? Directory.EnumerateFiles(Path.Combine(ServerRoot,"settings","item","droprate"), "*.ini") : [])
        .Concat(Directory.Exists(Path.Combine(ServiceRoot, "drop_drafts")) ? Directory.EnumerateFiles(Path.Combine(ServiceRoot, "drop_drafts"), "*.ini") : [])
        .Select(Path.GetFileName).OfType<string>().Order(StringComparer.Ordinal).ToArray();
    public EventDropText DropTable(string name)
    {
        byte[] bytes = File.ReadAllBytes(DropSource(name));
        if (bytes.Length > 2097152) throw new InvalidDataException("Bảng native quá lớn.");
        return new(name, Encoding.Latin1.GetString(bytes), Convert.ToHexStringLower(SHA256.HashData(bytes)));
    }
    public EventConfiguration SaveDropDraft(EventDropDraftRequest request)
    {
        lock (sync)
        {
            if (request.Slot is < 0 or > 3) throw new InvalidDataException("Slot phải nằm trong 0–3.");
            var config = Configuration();if (config.Revision != request.ExpectedRevision) throw new InvalidOperationException("Cấu hình đã đổi; tải lại.");
            var rule = config.Events.Single(e => e.Id == request.EventId);
            byte[] bytes = EventNativeDrop.Validate(request.Text);
            string name = $"pt_event_custom_{request.EventId}_{Guid.NewGuid():N}.ini";
            string[] slots = (string[])rule.NativeDropSlots.Clone();slots[request.Slot] = name;
            var next = rule with { RewardMode = "native_tables", NativeDropSlots = slots };
            EventRules.Validate(Catalog.Single(d => d.Id == next.Id), next);
            EventControlJson.AtomicBytes(Path.Combine(ServiceRoot, "drop_drafts", name), bytes);
            return Save(config.Revision, next);
        }
    }
    public static string PinnedDropName(EventRule rule, int slot) => $"pt_event_{rule.Id}_r{rule.RewardRevision}_s{slot}.ini";
    void FreezeDropTables(EventRule rule)
    {
        if (rule.RewardMode != "native_tables") return;
        for (int slot = 0; slot < 4; slot++)
        {
            string source = rule.NativeDropSlots[slot];
            if (source.Length == 0) continue;
            byte[] bytes = EventNativeDrop.Validate(Encoding.Latin1.GetString(File.ReadAllBytes(DropSource(source))));
            string target = Path.Combine(BridgeRoot, "drop", PinnedDropName(rule, slot));
            if (File.Exists(target) && !File.ReadAllBytes(target).AsSpan().SequenceEqual(bytes)) throw new InvalidDataException("Snapshot thưởng đã tồn tại với nội dung khác.");
            if (!File.Exists(target)) EventControlJson.AtomicBytes(target, bytes);
        }
    }
    public NativeEventStatus? Native(DateTimeOffset now)
    {
        try
        {
            var status = EventControlJson.Read<NativeEventStatus>(Path.Combine(BridgeRoot, "status.json"));
            if (status.Version != 1 || status.Pid <= 0 || status.UtcSeconds > now.ToUnixTimeSeconds() + 5 || now.ToUnixTimeSeconds() - status.UtcSeconds > 15) return null;
            return status;
        }
        catch (Exception ex) when (ex is IOException or JsonException or InvalidDataException) { return null; }
    }
    public void PublishLease(DateTimeOffset now)
    {
        lock (sync)
        {
            var config = Configuration();
            var text = new StringBuilder($"PT_EVENT_SNAPSHOT\t2\t{config.Revision}\t{now.ToUnixTimeSeconds() + 15}\t{config.Events.Length}\n");
            foreach (var e in config.Events.OrderBy(e => e.Id))
            {
                // The native bridge reads a bounded ASCII snapshot, not JSON or the Studio assemblies.
                text.AppendJoin('\t', e.Id, e.Enabled ? 1 : 0, e.RegistrationSeconds, e.DurationSeconds,
                    e.TicketGenre, e.TicketDetail, e.TicketParticular, e.TicketLevel, e.TicketSeries, e.TicketCount,
                    e.RewardMode == "original" ? 0 : 1);
                for (int slot = 0; slot < 4; slot++) text.Append('\t').Append(e.NativeDropSlots[slot].Length == 0 ? "-" : e.RewardMode == "native_tables" ? PinnedDropName(e, slot) : e.NativeDropSlots[slot]);
                text.Append('\t').AppendJoin('\t', e.MinimumLevel, e.DailyLimit, e.FreeRuns, e.RewardGenre, e.RewardDetail, e.RewardParticular,
                    e.RewardLevel, e.RewardSeries, e.RewardCount, e.RewardExperience, e.RewardMoney, e.RewardCredit, e.RequireTong ? 1 : 0, e.MinimumCredit, e.DailyTimes.Length > 0 ? 1 : 0);
                text.Append('\n');
            }
            EventControlJson.AtomicBytes(Path.Combine(BridgeRoot, "snapshot.tsv"), Encoding.ASCII.GetBytes(text.ToString()));
        }
    }
    public EventCommand Queue(int eventId, string operation, Guid requestId, DateTimeOffset now, int argument = 0)
    {
        lock (sync)
        {
            if (operation is not ("open" or "close" or "warning")) throw new InvalidDataException("Lệnh event không hợp lệ.");
            if (requestId == Guid.Empty) throw new InvalidDataException("Cần mã yêu cầu riêng.");
            _ = Catalog.Single(d => d.Id == eventId);
            if (argument is < 0 or > 86400) throw new InvalidDataException("Tham số lệnh không hợp lệ.");
            string id = requestId.ToString("N"), journal = Path.Combine(ServiceRoot, "commands", id + ".json");
            if (File.Exists(journal))
            {
                var existing = EventControlJson.Read<EventCommand>(journal);
                if (existing.EventId != eventId || existing.Operation != operation || existing.Argument != argument)
                    throw new InvalidOperationException("Mã yêu cầu đã dùng cho lệnh khác.");
                var observed = Native(now);
                if (existing.ExpiresUtc >= now.ToUnixTimeSeconds() && observed is not null && observed.LeaseValid && observed.Pid == existing.TargetPid && observed.BootUtc == existing.TargetBootUtc && observed.Revision == existing.Revision &&
                    !File.Exists(Path.Combine(BridgeRoot, "acks", id + ".json")) && !File.Exists(Path.Combine(BridgeRoot, "pending", id + ".cmd")))
                    PublishCommand(existing);
                return existing;
            }
            var native = Native(now) ?? throw new InvalidOperationException("GameServer chưa có bridge đang sẵn sàng; lệnh chưa được gửi.");
            var config = Configuration();
            if (native.Revision != config.Revision || !native.LeaseValid) throw new InvalidOperationException("Đợi GameServer xác nhận revision cấu hình trước khi điều khiển.");
            if (!native.Events.Any(e => e.Id == eventId && e.Phase >= 0)) throw new InvalidOperationException("Adapter event chưa sẵn sàng trên GameServer.");
            if (operation == "open" && !config.Events.Single(e => e.Id == eventId).Enabled) throw new InvalidOperationException("Bật nhận lượt mới và lưu trước khi mở event.");
            var command = new EventCommand(id, eventId, operation, config.Revision, native.Pid, native.BootUtc,
                now.ToUnixTimeSeconds(), now.ToUnixTimeSeconds() + 60, argument);
            EventControlJson.Atomic(journal, command);
            PublishCommand(command);
            return command;
        }
    }
    void PublishCommand(EventCommand command)
    {
        string line = $"PT_EVENT_COMMAND\t1\t{command.Id}\t{command.TargetPid}\t{command.TargetBootUtc}\t{command.IssuedUtc}\t{command.ExpiresUtc}\t{command.EventId}\t{command.Operation}\t{command.Revision}\t{command.Argument}\n";
        EventControlJson.AtomicBytes(Path.Combine(BridgeRoot, "pending", command.Id + ".cmd"), Encoding.ASCII.GetBytes(line));
    }
    public EventReceipt Receipt(string id, DateTimeOffset now)
    {
        if (!Guid.TryParseExact(id, "N", out _)) throw new InvalidDataException("Mã lệnh không hợp lệ.");
        string ack = Path.Combine(BridgeRoot, "acks", id + ".json");
        if (File.Exists(ack))
        {
            var receipt = EventControlJson.Read<EventReceipt>(ack);
            if (receipt.State == "executing" && now.ToUnixTimeSeconds() - receipt.UtcSeconds > 60)
                return receipt with { State = "unknown", Message = "GameServer chưa ghi kết quả cuối. Không tự phát lại lệnh này." };
            return receipt;
        }
        var command = EventControlJson.Read<EventCommand>(Path.Combine(ServiceRoot, "commands", id + ".json"));
        return new(id, command.ExpiresUtc < now.ToUnixTimeSeconds() ? "expired" : "queued",
            "Chưa có xác nhận thực thi từ GameServer.", command.TargetPid, command.TargetBootUtc, now.ToUnixTimeSeconds());
    }
}
