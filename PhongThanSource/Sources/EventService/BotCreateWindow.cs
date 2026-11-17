using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using PhongThan.Event.Runtime;

namespace PhongThan.EventService;

internal sealed class BotCreateWindow : Window
{
    readonly string root;
    readonly TextBox count=new(){Text="1",Width=90};
    readonly TextBox name=new(){IsEnabled=false};
    readonly CheckBox random=new(){Content="Tên ngẫu nhiên",IsChecked=true};
    readonly ComboBox group=new(){SelectedIndex=0};
    readonly TextBlock error=new(){TextWrapping=TextWrapping.Wrap,Foreground=System.Windows.Media.Brushes.Firebrick};
    readonly TextBlock fileInfo=new(){Text="TXT UTF-8 / Unicode: mỗi dòng một tên, bỏ qua dòng trống.",TextWrapping=TextWrapping.Wrap};
    readonly Button import=new(){Content="Nhập tên từ TXT",Padding=new Thickness(8,4,8,4)};
    readonly Button clearFile=new(){Content="Bỏ danh sách",Margin=new Thickness(8,0,0,0),IsEnabled=false};
    string? nameFile;
    string[] importedNames=[];
    readonly Button create=new(){Content="Tạo bot",Padding=new Thickness(16,6,16,6),IsDefault=true};
    readonly Button cancel=new(){Content="Đóng",Padding=new Thickness(16,6,16,6),IsCancel=true};
    bool saving;
    public int CreatedCount {get;private set;}
    public bool BotsRunning {get;private set;}

    public BotCreateWindow(string runtimeRoot)
    {
        root=runtimeRoot;Title="Thêm bot";Width=490;Height=Math.Min(600,SystemParameters.WorkArea.Height-40);ResizeMode=ResizeMode.NoResize;
        WindowStartupLocation=WindowStartupLocation.CenterOwner;ShowInTaskbar=false;
        var panel=new StackPanel{Margin=new Thickness(20)};Content=new ScrollViewer{Content=panel,VerticalScrollBarVisibility=ScrollBarVisibility.Auto};
        panel.Children.Add(new TextBlock{Text="Tạo nhân vật bot mới",FontSize=20,FontWeight=FontWeights.SemiBold,Margin=new Thickness(0,0,0,12)});
        Add("Số lượng",count);
        foreach(string text in new[]{"Chia đều 6 nhóm phái / giới tính","Giáp Sĩ — Nam","Giáp Sĩ — Nữ","Đạo Sĩ — Nam","Đạo Sĩ — Nữ","Dị Nhân — Nam","Dị Nhân — Nữ"})group.Items.Add(text);
        Add("Phái / giới tính",group);
        random.Margin=new Thickness(0,8,0,8);panel.Children.Add(random);
        Add("Tên bot (khi tạo một nhân vật)",name);
        random.Checked+=(_,_)=>name.IsEnabled=false;random.Unchecked+=(_,_)=>name.IsEnabled=nameFile is null;
        var files=new StackPanel{Orientation=Orientation.Horizontal,Margin=new Thickness(0,12,0,6)};files.Children.Add(import);files.Children.Add(clearFile);panel.Children.Add(files);panel.Children.Add(fileInfo);
        import.Click+=(_,_)=>{
            var picker=new Microsoft.Win32.OpenFileDialog{Title="Chọn danh sách tên bot — mỗi dòng một tên",Filter="Danh sách tên (*.txt)|*.txt",CheckFileExists=true};
            if(picker.ShowDialog(this)!=true)return;
            try{
                var names=BotNameList.Read(picker.FileName);importedNames=names;nameFile=picker.FileName;count.Text=names.Length.ToString(CultureInfo.InvariantCulture);count.IsReadOnly=true;
                random.IsChecked=false;random.IsEnabled=false;name.IsEnabled=false;clearFile.IsEnabled=true;
                fileInfo.Text=$"{System.IO.Path.GetFileName(nameFile)} • {names.Length} tên\n"+string.Join(", ",names.Take(3))+(names.Length>3?", …":"");error.Text="";
            }catch(Exception ex){error.Text=ex.Message;}
        };
        clearFile.Click+=(_,_)=>{nameFile=null;importedNames=[];count.Text="1";count.IsReadOnly=false;random.IsEnabled=true;random.IsChecked=true;clearFile.IsEnabled=false;fileInfo.Text="TXT UTF-8 / Unicode: mỗi dòng một tên, bỏ qua dòng trống.";error.Text="";};
        panel.Children.Add(new TextBlock{Text="Tọa độ tự đặt tại Tân Thủ Thôn đúng phái. Bot dùng chế độ tự động; tên tiếng Việt được chuyển TCVN3 khi gửi vào game.",TextWrapping=TextWrapping.Wrap,Margin=new Thickness(0,12,0,10)});
        panel.Children.Add(error);
        var actions=new StackPanel{Orientation=Orientation.Horizontal,HorizontalAlignment=HorizontalAlignment.Right,Margin=new Thickness(0,10,0,0)};
        cancel.Margin=new Thickness(0,0,8,0);actions.Children.Add(cancel);actions.Children.Add(create);panel.Children.Add(actions);
        create.Click+=async(_,_)=>await Save();
        Closing+=OnClosing;
        void Add(string label,Control field){panel.Children.Add(new TextBlock{Text=label,Margin=new Thickness(0,4,0,4)});panel.Children.Add(field);}
    }
    async Task Save()
    {
        if(saving)return;
        if(!int.TryParse(count.Text,NumberStyles.None,CultureInfo.InvariantCulture,out int quantity)||quantity is <1 or >128){error.Text="Số lượng phải từ 1 đến 128; tổng số hồ sơ không vượt 128.";return;}
        string? selectedFile=nameFile;string selectedName=name.Text;bool useRandom=random.IsChecked==true;int selectedGroup=group.SelectedIndex-1;var listedNames=importedNames.ToArray();
        saving=true;create.IsEnabled=cancel.IsEnabled=import.IsEnabled=clearFile.IsEnabled=false;error.Text="";
        try{
            if(selectedFile is not null&&!BotNameList.Read(selectedFile).SequenceEqual(listedNames))throw new InvalidDataException("File đã thay đổi sau khi xem danh sách. Hãy chọn lại TXT trước khi tạo.");
            using var api=new EventServiceClient(root);var current=await api.Bots();
            var profiles=selectedFile is null
                ?BotCreation.ServiceBatch(selectedName,quantity,useRandom,selectedGroup,current.Configuration.Bots)
                :BotCreation.ServiceNamedBatch(listedNames,selectedGroup,current.Configuration.Bots);
            var saved=await api.CreateBots(new(current.Configuration.Revision,profiles));
            CreatedCount=profiles.Length;BotsRunning=saved.Running;saving=false;DialogResult=true;
        }catch(Exception ex){error.Text=ex.Message;}
        finally{saving=false;create.IsEnabled=cancel.IsEnabled=import.IsEnabled=true;clearFile.IsEnabled=nameFile is not null;}
    }
    void OnClosing(object? sender,CancelEventArgs e){if(saving)e.Cancel=true;}
}
