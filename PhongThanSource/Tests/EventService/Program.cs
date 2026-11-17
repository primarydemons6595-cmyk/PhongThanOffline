using System.Text;
using PhongThanStudio.Core;
using PhongThan.Event.Runtime;

int checks = 0;
void Check(bool valid, string name) { if (!valid) throw new Exception("FAIL " + name);checks++; }
void Reject(Action action, string name)
{
    try { action(); } catch (Exception e) when (e is InvalidDataException or InvalidOperationException or ArgumentException) { checks++;return; }
    throw new Exception("FAIL accepted " + name);
}
string root = Path.Combine(Path.GetTempPath(), "PhongThanEventTests_" + Guid.NewGuid().ToString("N"));
Directory.CreateDirectory(Path.Combine(root, "Server"));Directory.CreateDirectory(Path.Combine(root, "Client"));
DateTimeOffset now = DateTimeOffset.Parse("2026-10-19T07:00:00Z", System.Globalization.CultureInfo.InvariantCulture);
var rule = new EventRule { Id = 1, DailyTimes = ["02:20", "10:20", "13:20", "18:20", "21:20", "23:20"], TicketDetail = 62, TicketCount = 1 };
var definition = new EventDefinition { Id = 1, Key = "vtt_tho", Name = "Vạn Tiên Trận Thổ", Adapter = "vtt", Map = 1067, Mission = 1, Defaults = rule };
try
{
    string diagnosticRoot=Path.Combine(root,"Server"),diagnosticConfig=Path.Combine(diagnosticRoot,"settings","phongthan","Diagnostics.ini");
    Check(!RuntimeDiagnostics.Enabled(diagnosticRoot),"No implicit file logging without Loose marker/config");
    File.WriteAllText(Path.Combine(diagnosticRoot,"loose_only.flag"),"LOOSE_ONLY");Directory.CreateDirectory(Path.GetDirectoryName(diagnosticConfig)!);
    File.WriteAllText(diagnosticConfig,"[Other]\nEnabled=1\n[Diagnostics]\nEnabled=0\n");Check(!RuntimeDiagnostics.Enabled(diagnosticRoot),"Quiet section overrides unrelated Enabled keys");
    File.WriteAllText(diagnosticConfig,"[Diagnostics]\nEnabled=1\n");Check(RuntimeDiagnostics.Enabled(diagnosticRoot),"Explicit diagnostics switch enables traces");
    File.WriteAllText(diagnosticConfig,"[Diagnostics]\nEnabled=0\n");
    Check(EventManagementSections.Contains(EventManagementSection.VanTien, definition), "VTT has its own Studio module");
    Check(!EventManagementSections.Contains(EventManagementSection.Boss, definition), "VTT is excluded from Boss module");
    foreach (var section in Enum.GetValues<EventManagementSection>())
    {
        var item = definition with { Adapter = section switch { EventManagementSection.Boss => "boss", EventManagementSection.VanTien => "vtt", EventManagementSection.VanLuong => "van_luong", EventManagementSection.VanTieu => "van_tieu", EventManagementSection.ThuongChu => "thuong_chu", _ => "npc" }, Key = "test" };
        Check(Enum.GetValues<EventManagementSection>().Count(s => EventManagementSections.Contains(s, item)) == 1 && EventManagementSections.Contains(section, item), "Studio section routing " + section);
    }
    EventControlJson.Atomic(Path.Combine(root, "Server", "EventService", "catalog.json"), new[] { definition });
    var store = new EventControlStore(root);
    Check(store.Configuration().Revision == 1, "default revision");
    Check(EventRules.Next(rule, now)!.Value.ToString("HH:mm") == "18:20", "UTC+7 next slot");
    Check(EventRules.Next(rule, DateTimeOffset.Parse("2026-10-19T16:21:00Z"))!.Value.Day == 20, "midnight next day");
    Check(EventRules.Next(rule with { Enabled = false }, now) is null, "disabled has no next slot");
    Reject(() => EventRules.Validate(definition, rule with { DailyTimes = ["25:20"] }), "invalid hour");
    Reject(() => EventRules.Validate(definition, rule with { DailyTimes = ["02:20", "02:20"] }), "duplicate slot");
    Reject(() => EventRules.Validate(definition, rule with { TicketCount = -1 }), "negative ticket");
    Reject(() => EventRules.Validate(definition, rule with { RegistrationSeconds = 0 }), "zero registration");
    Reject(() => EventRules.Validate(definition, rule with { NativeDropSlots = ["../bad.ini", "", "", ""] }), "path traversal");
    Reject(() => EventRules.Validate(definition, rule with { NativeDropSlots = ["C:\\bad.ini", "", "", ""] }), "absolute path");
    Reject(() => EventRules.Validate(definition, rule with { RewardMode = "fake_reward" }), "unrecognized reward mode");
    Reject(() => store.Queue(1, "open", Guid.NewGuid(), now), "no native bridge is not queued");
    void Native(long revision, long time, int pid = 100, long boot = 1000, int phase = 0)
        => EventControlJson.Atomic(Path.Combine(store.BridgeRoot, "status.json"), new NativeEventStatus(1, pid, boot, time, revision, true, [new(1, phase, 0, 0, "")]));
    Native(1, now.ToUnixTimeSeconds() - 16);
    Check(store.Native(now) is null, "stale status rejected");
    Native(1, now.ToUnixTimeSeconds() + 6);
    Check(store.Native(now) is null, "future status rejected");
    Native(1, now.ToUnixTimeSeconds());
    store.PublishLease(now);
    string snapshot = File.ReadAllText(Path.Combine(store.BridgeRoot, "snapshot.tsv"));
    Check(snapshot.StartsWith("PT_EVENT_SNAPSHOT\t2\t1\t"), "bounded native snapshot header");
    Check(snapshot.All(c => c <= 127), "snapshot ASCII");
    var command = store.Queue(1, "open", Guid.NewGuid(), now);
    Check(command.TargetPid == 100 && command.TargetBootUtc == 1000, "command tied to exact native boot");
    Check(store.Receipt(command.Id, now).State == "queued", "queued not reported applied");
    string pendingPath = Path.Combine(store.BridgeRoot, "pending", command.Id + ".cmd");File.Delete(pendingPath);
    _ = store.Queue(1, "open", Guid.ParseExact(command.Id, "N"), now);
    Check(File.Exists(pendingPath), "retry recovers interrupted journal-to-pending publish on same boot");
    Check(snapshot.Split('\n', StringSplitOptions.RemoveEmptyEntries)[1].Split('\t').Length == 30, "native v2 includes pinned quest policy fields");
    Reject(() => EventRules.Validate(definition, rule with { DailyLimit = 0 }), "invalid daily quest limit");
    Reject(() => EventRules.Validate(definition, rule with { FreeRuns = 21 }), "free quest runs cannot exceed limit");
    Reject(() => EventRules.Validate(definition, rule with { RewardCount = 65 }), "quest payout transaction capacity");
    Check(store.Queue(1, "open", Guid.ParseExact(command.Id, "N"), now) == command, "retry idempotency");
    Reject(() => store.Queue(1, "close", Guid.ParseExact(command.Id, "N"), now), "id reuse different operation");
    Check(store.Receipt(command.Id, now.AddSeconds(61)).State == "expired", "expiry without ack");
    EventControlJson.Atomic(Path.Combine(store.BridgeRoot, "acks", command.Id + ".json"), new EventReceipt(command.Id, "applied", "confirmed", 100, 1000, now.ToUnixTimeSeconds()));
    Check(store.Receipt(command.Id, now).State == "applied", "only native acknowledgement is applied");
    var disabled = store.Save(1, rule with { Enabled = false });
    Check(disabled.Revision == 2 && !disabled.Events[0].Enabled, "save revision CAS");
    Reject(() => store.Save(1, rule), "stale edit rejected");
    Reject(() => store.Queue(1, "open", Guid.NewGuid(), now), "unacknowledged new revision blocked");
    Native(2, now.ToUnixTimeSeconds());
    Reject(() => store.Queue(1, "open", Guid.NewGuid(), now), "disabled opening rejected");
    Check(store.Queue(1, "close", Guid.NewGuid(), now).Operation == "close", "disabled event can still close existing round");
    Native(2, now.ToUnixTimeSeconds(), phase: -1);
    Reject(() => store.Queue(1, "close", Guid.NewGuid(), now), "missing adapter rejected");
    string id = EventRules.ScheduledId(1, 2, now, "open");
    Check(id.Length == 32 && id == EventRules.ScheduledId(1, 2, now, "open"), "schedule ID stable across service restart");
    Check(id != EventRules.ScheduledId(1, 2, now.AddDays(1), "open"), "next day distinct ID");
    string tableRoot = Path.Combine(root, "Server", "settings", "item", "droprate");Directory.CreateDirectory(tableRoot);
    byte[] original = Encoding.ASCII.GetBytes("[Main]\nCount=1\nRandRange=1000000\n[1]\nGenre=3\nDetail=62\nParticular=0\nRandRate=1000\n");
    File.WriteAllBytes(Path.Combine(tableRoot, "tongtian1.ini"), original);
    var custom = store.Save(2, rule with { RewardMode = "native_tables", NativeDropSlots = ["tongtian1.ini", "", "", ""] });
    string pinned = Path.Combine(store.BridgeRoot, "drop", EventControlStore.PinnedDropName(custom.Events[0], 0));
    Check(File.ReadAllBytes(pinned).AsSpan().SequenceEqual(original), "immutable native table snapshot");
    File.WriteAllText(Path.Combine(tableRoot, "tongtian1.ini"), "modified later");
    Check(File.ReadAllBytes(pinned).AsSpan().SequenceEqual(original), "source edit cannot change active pinned profile");
    store.PublishLease(now);
    Check(File.ReadAllText(Path.Combine(store.BridgeRoot, "snapshot.tsv")).Contains("pt_event_1_r3_s0.ini"), "export points at pinned profile");
    var restarted = new EventControlStore(root);
    Check(restarted.Configuration().Revision == 3, "service restart preserves revision");
    Check(restarted.Receipt(command.Id, now).State == "applied", "service restart preserves native receipt");
    Native(3, now.ToUnixTimeSeconds(), 200, 2000);
    var newCommand = restarted.Queue(1, "open", Guid.NewGuid(), now);
    Check(newCommand.TargetPid == 200 && newCommand.TargetBootUtc == 2000, "new command uses new boot");
    Check(restarted.Queue(1, "open", Guid.ParseExact(command.Id, "N"), now).TargetPid == 100, "retry old ID never retargets new world");
    Console.WriteLine($"PASS EVENT_SERVICE_CONTRACTS checks={checks}; no live server data modified");
}
finally
{
    string resolved = Path.GetFullPath(root);
    if (!resolved.StartsWith(Path.GetFullPath(Path.GetTempPath()), StringComparison.OrdinalIgnoreCase) || !Path.GetFileName(resolved).StartsWith("PhongThanEventTests_", StringComparison.Ordinal)) throw new InvalidOperationException("Unsafe cleanup path");
    Directory.Delete(resolved, true);
}
