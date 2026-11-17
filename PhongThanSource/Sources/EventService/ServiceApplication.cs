using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using PhongThan.Event.Runtime;

namespace PhongThan.EventService;

internal static class ServiceApplication
{
    [STAThread]
    public static int Main(string[] args)
    {
        var app = new Application();
        bool smoke = args.Length == 4 && args[2] is "--smoke-render" or "--smoke-render-bots" or "--smoke-render-bot-create";
        Window window = smoke&&args[2]=="--smoke-render-bot-create"
            ?new BotCreateWindow(args[1])
            :new ServiceWindow(smoke ? args[..2] : args, !smoke,smoke&&args[2]=="--smoke-render-bots");
        if (smoke) window.Loaded += (_, _) =>
        {
            window.UpdateLayout();var bitmap = new RenderTargetBitmap((int)window.ActualWidth, (int)window.ActualHeight, 96, 96, PixelFormats.Pbgra32);bitmap.Render(window);
            var png = new PngBitmapEncoder();png.Frames.Add(BitmapFrame.Create(bitmap));using (var file = File.Create(args[3])) png.Save(file);
            window.Close();
        };
        return app.Run(window);
    }
}

// Operational host with bot administration; no development editor is required.
internal sealed class ServiceWindow : Window
{
    readonly TextBox runtime = new() { MinWidth = 420, Margin = new Thickness(5) };
    readonly TextBlock process = new() { Margin = new Thickness(5), TextWrapping = TextWrapping.Wrap };
    readonly TextBlock bridge = new() { Margin = new Thickness(5), TextWrapping = TextWrapping.Wrap };
    readonly TextBox log = new() { IsReadOnly = true, AcceptsReturn = true, FontFamily = new FontFamily("Consolas"), VerticalScrollBarVisibility = ScrollBarVisibility.Auto, HorizontalScrollBarVisibility = ScrollBarVisibility.Auto, Margin = new Thickness(5) };
    readonly DataGrid events = new() { IsReadOnly = true, AutoGenerateColumns = false, CanUserAddRows = false, Margin = new Thickness(5) };
    readonly DataGrid botList = new() { IsReadOnly = true, AutoGenerateColumns = false, CanUserAddRows = false, Margin = new Thickness(5),MinColumnWidth=60 };
    readonly TextBox botLog = new() { IsReadOnly = true, AcceptsReturn = true, FontFamily = new FontFamily("Consolas"), VerticalScrollBarVisibility = ScrollBarVisibility.Auto, HorizontalScrollBarVisibility = ScrollBarVisibility.Auto, Margin = new Thickness(5) };
    readonly TextBox botFilter = new() { Width=190,Margin=new Thickness(5),ToolTip="Lọc theo ID bot, hoạt động hoặc lỗi" };
    readonly CheckBox botPause = new() { Content="Dừng cuộn",Margin=new Thickness(5) };
    readonly TextBlock botStatus = new() { Margin=new Thickness(5),TextWrapping=TextWrapping.Wrap };
    readonly Button start = new() { Content = "Chạy service", Margin = new Thickness(5), Padding = new Thickness(12, 5, 12, 5) };
    readonly Button stop = new() { Content = "Dừng service", Margin = new Thickness(5), Padding = new Thickness(12, 5, 12, 5), IsEnabled = false };
    readonly DispatcherTimer timer = new() { Interval = TimeSpan.FromSeconds(2) };
    readonly string[] initialArgs;
    CancellationTokenSource? cancellation;
    Task<int>? running;
    EventServiceClient? client;
    bool refreshing, exiting, closing, monitoring;
    int monitoredPid;
    int logLines;
    string? logPath;
    readonly object logSync = new();
    sealed record EventRow(string Name, string State, string Next, string Revision);
    sealed record BotRow(string Name,string State,int Map,int X,int Y,int Level,string Life,string Error,string Id);
    static BotRow[] BotRows(BotDashboard state)=>state.Configuration.Bots.Where(b=>!b.Deleted).Select(b=>{var live=state.Native?.Bots.FirstOrDefault(n=>n.Id==b.Id);return new BotRow(b.Name,state.Fresh&&live is not null?BotConsole.State(live.State):"Chờ server",live?.Map??0,live?.X??0,live?.Y??0,live?.Level??0,live is null?"—":$"{live.Life}/{live.MaxLife}",live?.Error??"",b.Id);}).ToArray();

    public ServiceWindow(string[] args, bool autoStart = true,bool showBots=false)
    {
        initialArgs = args;Title = "Event Phong Thần";Width = 1050;Height = 700;MinWidth = 760;MinHeight = 500;FontFamily = new FontFamily("Segoe UI");FontSize = 13;Background = Brushes.WhiteSmoke;
        var root = new DockPanel { Margin = new Thickness(10) };Content = root;
        var top = new StackPanel();DockPanel.SetDock(top, Dock.Top);root.Children.Add(top);
        top.Children.Add(new TextBlock { Text = "EVENT PHONG THẦN", FontSize = 23, FontWeight = FontWeights.SemiBold, Margin = new Thickness(5) });
        top.Children.Add(new TextBlock { Text = "Chạy song song với GameServer • Tạo và theo dõi bot tại tab BotAIgent", Margin = new Thickness(5), TextWrapping = TextWrapping.Wrap });
        top.Children.Add(runtime);
        var buttons = new WrapPanel();buttons.Children.Add(start);buttons.Children.Add(stop);top.Children.Add(buttons);top.Children.Add(process);top.Children.Add(bridge);
        foreach (var col in new[] { ("Sự kiện", "Name", 280d), ("GameServer xác nhận", "State", 240d), ("Lịch tiếp theo (VN)", "Next", 180d), ("Revision lượt", "Revision", 110d) })
            events.Columns.Add(new DataGridTextColumn { Header = col.Item1, Binding = new Binding(col.Item2), Width = col.Item3 });
        foreach(var col in new[]{("Tên bot","Name"),("Trạng thái","State"),("Map","Map"),("X","X"),("Y","Y"),("Cấp","Level"),("Máu","Life"),("Lỗi","Error"),("ID","Id")})botList.Columns.Add(new DataGridTextColumn{Header=col.Item1,Binding=new Binding(col.Item2),Width=col.Item2=="Id"?220:100});
        var body = new Grid();body.RowDefinitions.Add(new RowDefinition());body.RowDefinitions.Add(new RowDefinition { Height = new GridLength(180) });body.Children.Add(events);Grid.SetRow(log, 1);body.Children.Add(log);
        var tabs=new TabControl();tabs.Items.Add(new TabItem{Header="Event",Content=body});
        var botPanel=new DockPanel();var botTop=new StackPanel();DockPanel.SetDock(botTop,Dock.Top);botPanel.Children.Add(botTop);botTop.Children.Add(botStatus);
        var botActions=new WrapPanel();botTop.Children.Add(botActions);
        var addBot=new Button{Content="Thêm bot",Margin=new Thickness(5),Padding=new Thickness(8,4,8,4)};
        addBot.Click+=async(_,_)=>{
            var dialog=new BotCreateWindow(runtime.Text){Owner=this};
            if(dialog.ShowDialog()==true){
                await Refresh();
                MessageBox.Show(this,$"Đã tạo {dialog.CreatedCount} bot. "+(dialog.BotsRunning?"Bot sẽ vào game khi GameServer sẵn sàng.":"Bấm Chạy bot để cho bot vào game khi GameServer sẵn sàng."),"Thêm bot",MessageBoxButton.OK,MessageBoxImage.Information);
            }
        };botActions.Children.Add(addBot);
        foreach(var action in new[]{("Chạy bot",true),("Dừng và lưu bot",false)}){
            var button=new Button{Content=action.Item1,Margin=new Thickness(5),Padding=new Thickness(8,4,8,4)};
            button.Click+=async(_,_)=>{try{using var api=new EventServiceClient(runtime.Text);var current=await api.Bots();await api.ControlBots(new(current.Configuration.Revision,action.Item2,current.Configuration.LocalAiEnabled));}catch(Exception ex){botStatus.Text=ex.Message;}};botActions.Children.Add(button);
        }
        var consoleActions=new WrapPanel();consoleActions.Children.Add(new TextBlock{Text="Lọc console:",VerticalAlignment=VerticalAlignment.Center});consoleActions.Children.Add(botFilter);consoleActions.Children.Add(botPause);
        var export=new Button{Content="Xuất log",Margin=new Thickness(5)};export.Click+=(_,_)=>{try{var dialog=new Microsoft.Win32.SaveFileDialog{Filter="Log|*.log",FileName="BotAIgent.log"};if(dialog.ShowDialog(this)==true)File.WriteAllText(dialog.FileName,botLog.Text);}catch(Exception ex){botStatus.Text=ex.Message;}};consoleActions.Children.Add(export);
        var consolePanel=new DockPanel();DockPanel.SetDock(consoleActions,Dock.Top);consolePanel.Children.Add(consoleActions);botLog.Height=140;consolePanel.Children.Add(botLog);
        var console=new Expander{Header="Console bot",IsExpanded=false,Content=consolePanel,Margin=new Thickness(5,0,5,0)};
        var botBody=new Grid();botBody.RowDefinitions.Add(new RowDefinition());botBody.RowDefinitions.Add(new RowDefinition{Height=GridLength.Auto});botBody.Children.Add(botList);Grid.SetRow(console,1);botBody.Children.Add(console);botPanel.Children.Add(botBody);tabs.Items.Add(new TabItem{Header="BotAIgent",Content=botPanel});root.Children.Add(tabs);
        runtime.Text = args.Length >= 2 && args[0] == "--root" ? Path.GetFullPath(args[1]) : DiscoverRoot();
        if(showBots){tabs.SelectedIndex=1;var preview=new BotAIgentStore(runtime.Text).Status(DateTimeOffset.UtcNow);botList.ItemsSource=BotRows(preview);botLog.Text=string.Join("\n",preview.Logs.Select(l=>BotConsole.Format(preview.Configuration.Bots,l)));botStatus.Text="BotAIgent • trạng thái và console đọc từ runtime hiện tại";}
        start.Click += async (_, _) => await Start();stop.Click += async (_, _) => await Stop();
        timer.Tick += async (_, _) => await Refresh();
        process.Text = "Service chưa chạy";bridge.Text = "Chờ kết nối GameServer.";
        Loaded += async (_, _) => { timer.Start();if (autoStart) await Start(); };
        Closing += OnClosing;
    }
    static string DiscoverRoot()
    {
        if(EventRuntimeLayout.IsRelease(AppContext.BaseDirectory)) return AppContext.BaseDirectory;
        for (var dir = new DirectoryInfo(AppContext.BaseDirectory);dir is not null;dir = dir.Parent)
            if (Directory.Exists(Path.Combine(dir.FullName, "Server")) && Directory.Exists(Path.Combine(dir.FullName, "Client"))) return dir.FullName;
        return "";
    }
    void Append(string text)
    {
        if (!Dispatcher.CheckAccess()) { Dispatcher.BeginInvoke(() => Append(text));return; }
        if (++logLines > 1000) { log.Clear();logLines = 1; }
        log.AppendText($"[{DateTime.Now:HH:mm:ss}] {text}{Environment.NewLine}");log.ScrollToEnd();
        if (logPath is not null)
        {
            try
            {
                lock (logSync)
                {
                    if (File.Exists(logPath) && new FileInfo(logPath).Length > 2097152) File.Move(logPath, logPath + ".1", true);
                    File.AppendAllText(logPath, $"{DateTimeOffset.UtcNow:O} {text}{Environment.NewLine}");
                }
            }
            catch (Exception ex) when (ex is IOException or UnauthorizedAccessException) { /* The GUI remains available if disk logging fails. */ }
        }
    }
    async Task Start()
    {
        if (running is { IsCompleted: false } || closing) return;
        try
        {
            string path = Path.GetFullPath(runtime.Text);if (!EventRuntimeLayout.Valid(path)) throw new InvalidDataException("Chọn runtime Loose hoặc Server Base.");
            // A second window attaches to the existing host instead of starting
            // another scheduler. Closing a monitor does not stop the service.
            try{
                using var probe=new EventServiceClient(path);var existing=await probe.Status();
                if(existing.ServicePid!=Environment.ProcessId){monitoring=true;monitoredPid=existing.ServicePid;runtime.IsReadOnly=true;start.IsEnabled=false;stop.IsEnabled=true;Append("Kết nối service đang chạy, PID "+monitoredPid);await Refresh();return;}
            }catch(Exception ex) when(ex is IOException or System.Net.Http.HttpRequestException or TaskCanceledException){ }
            cancellation?.Dispose();cancellation = new CancellationTokenSource();
            var args = new List<string> { "--root", path };
            if (initialArgs.Length == 4 && initialArgs[2] == "--port") { args.Add("--port");args.Add(initialArgs[3]); }
            if(EventRuntimeLayout.IsRelease(path)||!RuntimeDiagnostics.Enabled(EventRuntimeLayout.Server(path))) logPath=null;
            else {string logFolder = Path.Combine(EventRuntimeLayout.Server(path), "EventService", "logs");Directory.CreateDirectory(logFolder);logPath = Path.Combine(logFolder, "Event.log");}
            runtime.IsReadOnly = true;start.IsEnabled = false;stop.IsEnabled = true;process.Text = $"Đang khởi động • PID {Environment.ProcessId}";Append("Khởi động service tại " + path);
            running = Task.Run(() => EventServiceHost.RunAsync(args.ToArray(), cancellation.Token, Append));
            int code = await running;
            process.Text = $"Service đã dừng • mã {code}";Append(process.Text);
        }
        catch (Exception ex) { process.Text = "Service chưa chạy: " + ex.Message;Append(process.Text); }
        finally
        {
            if(!monitoring){
            client?.Dispose();client = null;events.ItemsSource = null;
            bridge.Text = "Service đã dừng. Kiểm tra trạng thái lượt đang chạy trực tiếp trên GameServer.";
            runtime.IsReadOnly = false;start.IsEnabled = !closing;stop.IsEnabled = false;
            }
        }
    }
    async Task Stop()
    {
        if(monitoring){
            try{using var api=new EventServiceClient(runtime.Text);await api.Shutdown();using var owner=Process.GetProcessById(monitoredPid);if(string.Equals(owner.MainModule?.FileName,Path.Combine(EventRuntimeLayout.Server(runtime.Text),"PhongThan.Event.exe"),StringComparison.OrdinalIgnoreCase))owner.CloseMainWindow();}
            catch(Exception ex){Append(ex.Message);}
            monitoring=false;client?.Dispose();client=null;runtime.IsReadOnly=false;start.IsEnabled=!closing;stop.IsEnabled=false;process.Text="Đã ngắt theo dõi service.";return;
        }
        if (running is null || running.IsCompleted) return;
        stop.IsEnabled = false;process.Text = "Đang dừng service…";cancellation?.Cancel();
        try { await running; } catch (Exception ex) { Append(ex.Message); }
    }
    async Task Refresh()
    {
        if (refreshing || (!monitoring&&running is not { IsCompleted: false })) return;refreshing = true;
        try
        {
            client ??= new EventServiceClient(runtime.Text);var state = await client.Status();
            process.Text = $"Service đang chạy • PID {state.ServicePid} • Cấu hình r{state.Configuration.Revision}";
            bridge.Text = state.Native is null ? "GameServer chưa kết nối hoặc heartbeat đã hết hạn." : $"GameServer PID {state.Native.Pid} • revision r{state.Native.Revision} • " + (state.Native.LeaseValid ? "Heartbeat hợp lệ" : "Chưa xác nhận lease");
            events.ItemsSource = state.Catalog.Select(d =>
            {
                var rule = state.Configuration.Events.Single(r => r.Id == d.Id);var native = state.Native?.Events.FirstOrDefault(e => e.Id == d.Id);
                string phase = native is null ? "Chưa có kết nối" : native.Phase switch { < 0 => "Adapter chưa sẵn sàng", 1 => "Đang đăng ký / nhận lượt", 2 => "Đang hoạt động", _ => "Đã đóng" };
                if (!rule.Enabled) phase += " • Tắt nhận mới";
                return new EventRow(d.Name, phase, EventRules.Next(rule, DateTimeOffset.UtcNow)?.ToString("dd/MM HH:mm") ?? "—", native?.ActiveRevision.ToString() ?? "—");
            }).ToArray();
            var bots=await client.Bots();botList.ItemsSource=BotRows(bots);
            botStatus.Text=$"BotAIgent • {(bots.Configuration.Running?"Bật":"Tắt")} • {bots.Configuration.Bots.Count(b=>b.Enabled&&!b.Deleted)} bot được bật • "+(bots.Fresh?$"GameServer PID {bots.Native!.Pid} • revision {bots.Native.Revision}":"Chưa có trạng thái GameServer mới")+" • AI local tắt";
            if(botPause.IsChecked!=true){botLog.Text=string.Join("\n",bots.Logs.Select(l=>BotConsole.Format(bots.Configuration.Bots,l)).Where(l=>string.IsNullOrEmpty(botFilter.Text)||l.Contains(botFilter.Text,StringComparison.OrdinalIgnoreCase)));botLog.ScrollToEnd();}
        }
        catch (Exception ex) { bridge.Text = "Chưa đọc được trạng thái: " + ex.Message;client?.Dispose();client = null; }
        finally { refreshing = false; }
    }
    async void OnClosing(object? sender, CancelEventArgs e)
    {
        if (exiting) return;e.Cancel = true;if (closing) return;closing = true;timer.Stop();start.IsEnabled = false;
        if(!monitoring)await Stop();exiting = true;client?.Dispose();cancellation?.Dispose();_ = Dispatcher.BeginInvoke(Close);
    }
}
