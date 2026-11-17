from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
service = ROOT / 'PhongThanSource/Sources/EventService/Program.cs'
text = service.read_text('utf8')
if 'internal static class EventServiceHost' not in text:
    text = text.replace('using System.Net;', 'using System.IO;\nusing System.Net;')
    anchor = 'if (args.Length < 2'
    pos = text.index(anchor)
    text = text[:pos] + 'namespace PhongThan.EventService;\n\ninternal static class EventServiceHost\n{\ninternal static async Task<int> RunAsync(string[] args, CancellationToken cancellation, Action<string> log)\n{\n' + text[pos:]
    text = text.replace('Console.Error.WriteLine(', 'log(').replace('Console.WriteLine(', 'log(')
    text = text.replace('using var stop = new CancellationTokenSource();\nConsole.CancelKeyPress += (_, e) => { e.Cancel = true; stop.Cancel(); };', 'using var stop = CancellationTokenSource.CreateLinkedTokenSource(cancellation);')
    text = text.replace('return 0;\n\nstatic async Task<T>', 'return 0;\n}\n\nstatic async Task<T>')
    text += '\n}\n'
    service.write_text(text, 'utf8')

app = ROOT / 'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.App'
p = app / 'MainWindow.xaml.cs'
text = p.read_text('utf8')
text = text.replace('EventManagerView? eventEditor;', 'readonly List<EventManagerView> eventEditors = [];')
text = text.replace('eventEditor?.HasUnstaged == true', 'eventEditors.Any(e => e.HasUnstaged)')
old = '        eventEditor = new EventManagerView(() => Current);\n        Add(ManagementTabs,"Event Phong Thần",eventEditor);'
new = '''        EventManagerView EventPage(EventManagementSection section)
        {
            var view = new EventManagerView(() => Current, section);eventEditors.Add(view);return view;
        }
        Add(ManagementTabs,"Boss Server",EventPage(EventManagementSection.Boss));
        Add(ManagementTabs,"Vạn Tiên Trận",EventPage(EventManagementSection.VanTien));
        var questEvents = new TabControl();
        Add(questEvents,"Vận Lương",EventPage(EventManagementSection.VanLuong));
        Add(questEvents,"Vận Tiêu",EventPage(EventManagementSection.VanTieu));
        Add(questEvents,"Event khác",EventPage(EventManagementSection.Other));
        Add(ManagementTabs,"Sự kiện",questEvents);'''
if old in text:
    text = text.replace(old, new)
else:
    assert 'EventPage(EventManagementSection.Boss)' in text
# Include event drafts in workspace switch guard as well as window closing.
old = 'if ((plan.Changes.Count > 0 || spawnDirty || mapEditor?.HasChanges==true || dropEditor?.HasUnstaged == true || resourceEditors.Any'
text = text.replace(old, 'if ((plan.Changes.Count > 0 || spawnDirty || mapEditor?.HasChanges==true || dropEditor?.HasUnstaged == true || eventEditors.Any(e => e.HasUnstaged) || resourceEditors.Any')
text = text.replace('foreach (var editor in tableEditors) editor.Reset();', 'foreach (var editor in eventEditors) editor.Reset();\n        foreach (var editor in tableEditors) editor.Reset();') if 'foreach (var editor in eventEditors) editor.Reset();' not in text else text
p.write_text(text, 'utf8')

p = app / 'EventManagerView.cs'
text = p.read_text('utf8')
text = text.replace('    readonly Func<Workspace> workspace;', '    readonly Func<Workspace> workspace;\n    readonly EventManagementSection section;')
text = text.replace('public EventManagerView(Func<Workspace> workspace)', 'public EventManagerView(Func<Workspace> workspace, EventManagementSection section)')
text = text.replace('        this.workspace = workspace;', '        this.workspace = workspace;this.section = section;')
text = text.replace('Button(actions, "Khởi động service",', 'Button(actions, "Mở app Event Phong Thần",')
text = text.replace('        Button(actions, "Dừng service", async () => { await Client.Shutdown();status.Text = "Service đang dừng. GameServer và các nhiệm vụ đã nhận tiếp tục hoạt động."; });\n', '')
text = text.replace('var next = await Client.Status();dashboard = next;int id = selected?.Id ?? next.Catalog.First().Id;', '''var next = await Client.Status();dashboard = next;
            var catalog = next.Catalog.Where(d => EventManagementSections.Contains(section, d)).ToArray();
            if (catalog.Length == 0)
            {
                events.ItemsSource = Array.Empty<Row>();selected = null;
                status.Text = "Chưa có adapter được khai báo cho mục này. Không có event nào được mở hoặc sửa.";return;
            }
            int id = selected is not null && catalog.Any(d => d.Id == selected.Id) ? selected.Id : catalog[0].Id;''')
text = text.replace('events.ItemsSource = next.Catalog.Select(d =>', 'events.ItemsSource = catalog.Select(d =>')
if 'public void Reset()' not in text:
    text = text.replace('    EventServiceClient Client =>', '''    public void Reset()
    {
        client?.Dispose();client = null;dashboard = null;selected = null;shownRevision = 0;
        loading = true;events.ItemsSource = null;loading = false;HasUnstaged = false;status.Text = "Kết nối app Event Phong Thần để tải cấu hình của mục này.";
    }
    EventServiceClient Client =>''')
p.write_text(text, 'utf8')
print('GUI host and separate Studio event modules wired.')
