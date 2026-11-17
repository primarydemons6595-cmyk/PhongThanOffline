from pathlib import Path
r=Path(__file__).resolve().parents[2]/'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.App'
p=r/'CustomEventEditorView.cs';d=p.read_text('utf8')
d=d.replace('e.Goods.Where(g=>g.Id!=g.Id||g.Id!=(old?.Id??-1))','e.Goods.Where(g=>g.Id!=(old?.Id??-1))')
d=d.replace('var dialog=EventUi.Dialog(this,"NPC sự kiện • vị trí và chức năng",650,610);var panel=new StackPanel{Margin=new Thickness(20)};dialog.Content=panel;', 'var dialog=EventUi.Dialog(this,"NPC sự kiện • vị trí và chức năng",750,850);var panel=new StackPanel{Margin=new Thickness(20)};dialog.Content=new ScrollViewer{Content=panel,VerticalScrollBarVisibility=ScrollBarVisibility.Auto};var dialogue=new EventColorEditor("Hộp thoại NPC",old?.Dialogue??"");')
d=d.replace('panel.Children.Add(shop);panel.Children.Add(craft);EventUi.Hint(panel,','panel.Children.Add(shop);panel.Children.Add(craft);panel.Children.Add(dialogue);EventUi.Hint(panel,')
d=d.replace('shop.IsChecked==true,craft.IsChecked==true);','shop.IsChecked==true,craft.IsChecked==true,dialogue.Text);')
at=d.index('        EventUi.Hint(panel,"Giờ nấu')
fields='''        var description=new EventColorEditor("Nội dung giới thiệu công thức (để trống sẽ tự mô tả nguyên liệu)",old?.Description??"");
        var tooltip=new EventColorEditor("Tooltip thành phẩm trong game",old?.Tooltip??"");
        panel.Children.Add(description);panel.Children.Add(tooltip);
        var startText=new EventColorEditor("Khi bắt đầu nấu",old?.StartText??"",true);
        var waitText=new EventColorEditor("Khi chưa chín",old?.WaitText??"",true);
        var readyText=new EventColorEditor("Khi đã chín",old?.ReadyText??"",true);
        var burntText=new EventColorEditor("Khi bị cháy",old?.BurntText??"",true);
        var useText=new EventColorEditor("Khi sử dụng thành phẩm nhận thưởng",old?.UseText??"");
        var texts=new StackPanel();foreach(var text in new[]{startText,waitText,readyText,burntText,useText})texts.Children.Add(text);
        panel.Children.Add(new Expander{Header="Tùy chỉnh lời thông báo và màu",Content=texts,Margin=new Thickness(0,10,0,10)});
'''
d=d[:at]+fields+d[at:]
d=d.replace('EventUi.Number(silver),tool,rewards.ToArray());','EventUi.Number(silver),tool,rewards.ToArray(),description.Text,tooltip.Text,startText.Text,waitText.Text,readyText.Text,burntText.Text,useText.Text);')
p.write_text(d,'utf8')
p=r/'MainWindow.xaml.cs';d=p.read_text('utf-8-sig');d=d.replace('readonly List<EventManagerView> eventEditors = [];','readonly List<EventManagerView> eventEditors = [];\n    CustomEventEditorView? customEvents;')
d=d.replace('Add(questEvents,"Event khác",EventPage(EventManagementSection.Other));','var otherEvents=new TabControl();customEvents=new CustomEventEditorView(()=>Current);Add(otherEvents,"Tạo & quản lý sự kiện",customEvents);Add(otherEvents,"Sự kiện có sẵn",EventPage(EventManagementSection.Other));Add(questEvents,"Event khác",otherEvents);')
d=d.replace('eventEditors.Any(e => e.HasUnstaged)','customEvents?.HasUnstaged==true || eventEditors.Any(e => e.HasUnstaged)')
d=d.replace('foreach (var editor in eventEditors) editor.Reset();','customEvents?.Reset();foreach (var editor in eventEditors) editor.Reset();')
p.write_text(d,'utf8')
print('Added configurable NPC/cooking/tooltip text editors with colour previews and integrated Other Events workspace')
