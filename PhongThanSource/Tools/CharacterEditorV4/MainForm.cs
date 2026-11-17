namespace PhongThanCharacterEditor;

public sealed partial class MainForm : Form
{
    private readonly ClientAssets assets;
    private readonly InventoryView inventory,warehouse;
    private readonly TextBox filepath=new(){ReadOnly=true,Dock=DockStyle.Fill};
    private readonly Label status=new(){AutoSize=false,Height=58,Dock=DockStyle.Bottom,Padding=new Padding(12),ForeColor=Color.FromArgb(39,81,125)};
    private readonly Label heading=new(){AutoSize=false,Width=300,Height=60,Font=new Font("Segoe UI",14,FontStyle.Bold)};
    private readonly Label detail=new(){AutoSize=false,Height=105,Width=310};
    private readonly ListView all=new(){View=View.Details,FullRowSelect=true,HideSelection=false,Dock=DockStyle.Fill};
    private readonly Dictionary<string,NumericUpDown> stats=new();
    private readonly ComboBox destination=new(){DropDownStyle=ComboBoxStyle.DropDownList,Width=230};
    private readonly NumericUpDown destX=new(){Minimum=0,Maximum=12,Width=55},destY=new(){Minimum=0,Maximum=9,Width=55};
    private readonly Button save=new(){Text="Lưu file (offline)",AutoSize=true},export=new(){Text="Xuất bộ lưu…",AutoSize=true};
    private readonly NumericUpDown upgradeLevel=new(){Minimum=0,Maximum=12,Width=58};
    private readonly ComboBox upgradeRule=new(){DropDownStyle=ComboBoxStyle.DropDownList,Width=282};
    private readonly Label upgradeInfo=new(){Width=282,Height=34};
    private readonly Button upgradeMinus=new(){Text="−",Width=38,ForeColor=Color.FromArgb(30,45,64)},upgradePlus=new(){Text="+",Width=38,ForeColor=Color.FromArgb(30,45,64)},upgradeApply=new(){Text="Áp dụng",Width=112,ForeColor=Color.FromArgb(30,45,64)};
    private bool upgrading;
    private readonly string serverRoot;
    private PthcFile? character;
    private ItemRecord? selected;
    private bool dirty;
    public bool HasUnsavedChanges=>dirty;
    private readonly TabControl tabs;
    public MainForm(string clientRoot,string server,string? file=null){
        serverRoot=server;assets=new ClientAssets(clientRoot);Text="Phong Thần · Character Editor v4.1 · Cường hóa";ClientSize=new Size(1240,860);MinimumSize=new Size(1120,800);Font=new Font("Segoe UI",10);BackColor=Color.FromArgb(237,241,247);ForeColor=Color.FromArgb(30,45,64);
        var bar=new TableLayoutPanel{Dock=DockStyle.Top,Height=54,ColumnCount=4,Padding=new Padding(10)};bar.ColumnStyles.Add(new(SizeType.Absolute,160));bar.ColumnStyles.Add(new(SizeType.Percent,100));bar.ColumnStyles.Add(new(SizeType.Absolute,175));bar.ColumnStyles.Add(new(SizeType.Absolute,170));
        var open=new Button{Text="Mở nhân vật",Dock=DockStyle.Fill};open.Click+=(_,_)=>{if(!MayDiscard())return;using var d=new OpenFileDialog{Filter="Phong Thần (*.pthc)|*.pthc",InitialDirectory=Path.Combine(serverRoot,"CharacterStore")};if(d.ShowDialog()==DialogResult.OK)LoadCharacter(d.FileName);};
        bar.Controls.Add(open,0,0);bar.Controls.Add(filepath,1,0);bar.Controls.Add(save,2,0);bar.Controls.Add(export,3,0);
        var split=new SplitContainer{Size=new Size(1240,740),Dock=DockStyle.Fill,SplitterDistance=890,FixedPanel=FixedPanel.Panel2,Panel2MinSize=350};
        tabs=new TabControl{Dock=DockStyle.Fill};inventory=new InventoryView(assets){Dock=DockStyle.Fill};warehouse=new InventoryView(assets){Dock=DockStyle.Fill,Warehouse=true,Container=5};
        tabs.TabPages.Add(BuildInventory());tabs.TabPages.Add(BuildWarehouse());tabs.TabPages.Add(BuildSkills());
        var other=new TabPage("Tất cả vật phẩm");all.Columns.Add("#",40);all.Columns.Add("Tên vật phẩm",280);all.Columns.Add("Vùng",110);all.Columns.Add("Ô",60);all.Columns.Add("+",50);other.Controls.Add(all);tabs.TabPages.Add(other);split.Panel1.Controls.Add(tabs);
        var side=new Panel{Dock=DockStyle.Fill,Padding=new Padding(8)};split.Panel2.Controls.Add(side);heading.Dock=DockStyle.Top;heading.Height=34;
        var toolsTabs=new TabControl{Dock=DockStyle.Fill};side.Controls.Add(toolsTabs);side.Controls.Add(heading);
        TabPage actorPage=new TabPage("Nhân vật"),itemPage=new TabPage("Vật phẩm");toolsTabs.TabPages.Add(actorPage);toolsTabs.TabPages.Add(itemPage);
        var actorTools=new FlowLayoutPanel{Dock=DockStyle.Fill,FlowDirection=FlowDirection.TopDown,WrapContents=false,AutoScroll=true,Padding=new Padding(6)};
        var itemTools=new FlowLayoutPanel{Dock=DockStyle.Fill,FlowDirection=FlowDirection.TopDown,WrapContents=false,AutoScroll=true,Padding=new Padding(6)};
        actorPage.Controls.Add(actorTools);itemPage.Controls.Add(itemTools);
        BuildUpgradePanel(itemTools);
        foreach(var (key,label) in new[]{("level","Cấp độ"),("exp","Kinh nghiệm"),("money","Bạc trong túi"),("boundmoney","Bạc khóa (lượng)"),("bank","Tiền kho"),("attr","Điểm tiềm năng"),("skill","Điểm kỹ năng"),("fame","Danh vọng"),("pk","Điểm PK"),("respect","Điểm sư đồ / Kính trọng")}){actorTools.Controls.Add(new Label{Text=label,AutoSize=true});var n=new NumericUpDown{Width=290,Maximum=int.MaxValue,Minimum=0,ThousandsSeparator=true};n.ValueChanged+=(_,_)=>dirty=true;stats[key]=n;actorTools.Controls.Add(n);}
        var liveSave=new Button{Text="Lưu trực tiếp · giữ server chạy",Width=290,Height=36,BackColor=Color.FromArgb(43,108,176),ForeColor=Color.White,FlatStyle=FlatStyle.Flat};
        liveSave.Click+=async(_,_)=>{if(character==null)return;try{ApplyStats();liveSave.Enabled=false;await character.SaveThroughServerAsync(serverRoot);dirty=false;RefreshViews();status.Text="Đã lưu qua dịch vụ dữ liệu, có sao lưu. Server tiếp tục chạy.";}catch(Exception ex){MessageBox.Show(ex.Message,"Chưa lưu",MessageBoxButtons.OK,MessageBoxIcon.Information);}finally{liveSave.Enabled=true;}};
        actorTools.Controls.Add(liveSave);
        BuildRepairTools(actorTools);
        var resetPk=new Button{Text="Xóa PK",Width=290};resetPk.Click+=(_,_)=>stats["pk"].Value=0;actorTools.Controls.Add(resetPk);
        var fameAdd=new Button{Text="Cộng 1.000 danh vọng",Width=290};fameAdd.Click+=(_,_)=>stats["fame"].Value=Math.Min(int.MaxValue,stats["fame"].Value+1000);actorTools.Controls.Add(fameAdd);
        var safeTown=new ComboBox{Width=290,DropDownStyle=ComboBoxStyle.DropDownList};safeTown.Items.AddRange(new object[]{"Tân Thủ Thôn · bản đồ 1002","Tân Thủ Thôn · bản đồ 1003","Tân Thủ Thôn · bản đồ 1004"});safeTown.SelectedIndex=0;actorTools.Controls.Add(safeTown);
        var fixStuck=new Button{Text="Sửa kẹt tọa độ 0 → thôn đã chọn",Width=290};fixStuck.Click+=(_,_)=>Try(()=>{if(character==null)return;character.RepairAtTown(serverRoot,1002+safeTown.SelectedIndex);dirty=true;status.Text="Đã lấy tọa độ từ revivepos.ini gốc. Nhấn Lưu trực tiếp để áp dụng.";});actorTools.Controls.Add(fixStuck);
        var repair=new Button{Text="Khôi phục tọa độ về điểm hồi sinh",Width=290};repair.Click+=(_,_)=>Try(()=>{if(character==null)return;character.RepairPosition();dirty=true;status.Text="Đã sửa tọa độ trong bản chỉnh. Nhấn Lưu để áp dụng.";});actorTools.Controls.Add(repair);
        itemTools.Controls.Add(new Label{Text="VẬT PHẨM ĐANG CHỌN",AutoSize=true,Margin=new Padding(0,16,0,6)});itemTools.Controls.Add(detail);
        foreach(var c in Containers)destination.Items.Add(c);destination.SelectedIndex=1;itemTools.Controls.Add(destination);var xy=new FlowLayoutPanel{AutoSize=true};xy.Controls.Add(new Label{Text="Ô X/Y",AutoSize=true});xy.Controls.Add(destX);xy.Controls.Add(destY);itemTools.Controls.Add(xy);
        var move=new Button{Text="Chuyển tới ô",Width=290};move.Click+=(_,_)=>Try(()=>{if(selected==null)return;inventory.Move(selected,((ContainerOption)destination.SelectedItem!).Id,(int)destX.Value,(int)destY.Value);});itemTools.Controls.Add(move);
        var del=new Button{Text="Xóa vật phẩm đã chọn",Width=290};del.Click+=(_,_)=>Try(()=>{if(selected==null||character==null)return;if(!character.CanSave)throw new InvalidOperationException(character.CompanionStatus);if(MessageBox.Show("Xóa "+assets.Visual(selected).Name+" khỏi bản đang chỉnh?","Xóa vật phẩm",MessageBoxButtons.YesNo,MessageBoxIcon.Question)!=DialogResult.Yes)return;character.DeleteItem(selected.Index);SelectItem(null);dirty=true;RefreshViews();});itemTools.Controls.Add(del);
        itemTools.Controls.Add(new Label{Text="Cường hóa được tính lại bằng engine. Các chỉ số đồ khác chỉ xem.",Width=290,Height=48,Margin=new Padding(0,12,0,0)});
        foreach(var view in new[]{inventory,warehouse}){view.ItemSelected+=SelectItem;view.Changed+=()=>{dirty=true;RefreshViews();};}
        all.SelectedIndexChanged+=(_,_)=>{if(all.SelectedItems.Count>0&&character!=null)SelectItem(character.GetItems()[(int)all.SelectedItems[0].Tag!]);};
        save.Click+=(_,_)=>Try(()=>{ApplyStats();character!.SaveWithBackup();dirty=false;RefreshViews();status.Text="Đã lưu cùng companion, có backup riêng trước lần ghi.";});
        export.Click+=(_,_)=>Try(()=>{ApplyStats();using var d=new FolderBrowserDialog{Description="Chọn thư mục chứa bộ xuất mới"};if(d.ShowDialog()!=DialogResult.OK)return;var folder=Path.Combine(d.SelectedPath,"Character-"+character!.RoleName+"-"+DateTime.Now.ToString("yyyyMMdd-HHmmss"));var result=character.ExportPackage(folder);status.Text="Đã xuất: "+result;MessageBox.Show("Đã xuất CharacterStore + companion:\n"+folder,"Xuất thành công");});
        var resources=new FlowLayoutPanel{Dock=DockStyle.Top,Height=42,Padding=new Padding(10,2,10,2)};
        var chooseClient=new Button{Text="Chọn thư mục client…",AutoSize=true};chooseClient.Click+=(_,_)=>Try(()=>{using var d=new FolderBrowserDialog{SelectedPath=assets.Root};if(d.ShowDialog()!=DialogResult.OK)return;assets.SetRoot(d.SelectedPath);RefreshViews();status.Text="Client: "+assets.Root+" · "+assets.PakCount+" PAK";});
        var choosePak=new Button{Text="Nạp PAK…",AutoSize=true};choosePak.Click+=(_,_)=>Try(()=>{using var d=new OpenFileDialog{Filter="Kingsoft PACK (*.pak)|*.pak",Multiselect=true,InitialDirectory=assets.Root};if(d.ShowDialog()!=DialogResult.OK)return;assets.MountPaks(d.FileNames);RefreshViews();status.Text="Đã nạp "+assets.PakCount+" PAK · thứ tự chọn là thứ tự ưu tiên";});
        var prefer=new CheckBox{Text="Ưu tiên ảnh/bảng từ PAK",AutoSize=true,Padding=new Padding(5)};prefer.CheckedChanged+=(_,_)=>Try(()=>{assets.SetPreferPaks(prefer.Checked);RefreshViews();});resources.Controls.Add(chooseClient);resources.Controls.Add(choosePak);resources.Controls.Add(prefer);
        Controls.Add(split);Controls.Add(status);Controls.Add(resources);Controls.Add(bar);FormClosing+=(_,e)=>{if(!MayDiscard())e.Cancel=true;};if(file!=null)LoadCharacter(file);else{save.Enabled=export.Enabled=false;status.Text="Mở file .pthc để chỉnh nhân vật. Client: "+clientRoot;}
    }
    private TabPage BuildInventory(){var page=new TabPage("F4 · Trang bị & hành trang");var buttons=new FlowLayoutPanel{Dock=DockStyle.Bottom,Height=44,Padding=new Padding(4)};foreach(var (id,label) in new[]{(3,"Túi 1"),(4,"Túi 2"),(18,"Túi 3"),(19,"Túi 4")}){var b=new Button{Text=label,ForeColor=Color.Black,Width=105,Height=30};b.Click+=(_,_)=>{inventory.Container=id;inventory.Invalidate();status.Text=label+" · vị trí và kích thước ô theo client";};buttons.Controls.Add(b);}page.Controls.Add(inventory);page.Controls.Add(buttons);return page;}
    private TabPage BuildWarehouse(){var page=new TabPage("Kho · 6 trang");var combo=new ComboBox{Dock=DockStyle.Top,DropDownStyle=ComboBoxStyle.DropDownList};for(int i=0;i<6;i++)combo.Items.Add("Kho "+(i+1));combo.SelectedIndex=0;combo.SelectedIndexChanged+=(_,_)=>{warehouse.Container=5+combo.SelectedIndex;warehouse.Invalidate();};warehouse.PageSelected+=id=>combo.SelectedIndex=id-5;page.Controls.Add(warehouse);page.Controls.Add(combo);return page;}
    public void LoadCharacter(string file){Try(()=>{string roleComp=Path.Combine(Path.GetDirectoryName(Path.GetFullPath(file))!,"NativeDropPersistenceV1");string localComp=Directory.Exists(roleComp)?roleComp:Path.Combine(Directory.GetParent(Path.GetDirectoryName(Path.GetFullPath(file))!)!.FullName,"NativeDropPersistenceV1");character=PthcFile.Load(file,Directory.Exists(localComp)?localComp:Path.Combine(serverRoot,"NativeDropPersistenceV1"));filepath.Text=file;heading.Text=character.RoleName+" ? "+character.ProfessionName+" ("+character.Profession+")";stats["level"].Value=Math.Clamp(character.FightLevel,0,int.MaxValue);stats["exp"].Value=Math.Clamp(character.FightExperience,0,int.MaxValue);stats["money"].Value=Math.Clamp(character.Money,0,int.MaxValue);stats["boundmoney"].Value=Math.Clamp(character.BoundMoney,0,int.MaxValue);stats["bank"].Value=Math.Clamp(character.BankMoney,0,int.MaxValue);stats["attr"].Value=Math.Clamp(character.RemainingAttributePoints,0,int.MaxValue);stats["skill"].Value=Math.Clamp(character.RemainingSkillPoints,0,int.MaxValue);stats["fame"].Value=Math.Max(0,character.Fame);stats["pk"].Value=Math.Max(0,character.PkValue);stats["respect"].Value=Math.Max(0,character.Respect);inventory.Character=warehouse.Character=character;SelectItem(null);dirty=false;RefreshViews();});}
    private void ApplyStats(){if(character==null)throw new InvalidOperationException("Chưa mở nhân vật.");if(!character.CanSave)throw new InvalidOperationException(character.CompanionStatus);character.FightLevel=(int)stats["level"].Value;character.FightExperience=(int)stats["exp"].Value;character.Money=(int)stats["money"].Value;character.BoundMoney=(int)stats["boundmoney"].Value;character.BankMoney=(int)stats["bank"].Value;character.RemainingAttributePoints=(int)stats["attr"].Value;character.RemainingSkillPoints=(int)stats["skill"].Value;character.Fame=(int)stats["fame"].Value;character.PkValue=(int)stats["pk"].Value;character.Respect=(int)stats["respect"].Value;}
    private void SelectItem(ItemRecord? item){selected=item;inventory.Selected=warehouse.Selected=item?.Index??-1;if(item==null)detail.Text="Chọn một món trên F4, kho hoặc danh sách.";else{var v=assets.Visual(item);detail.Text=$"{v.Name}\nCấp {item.Level} · +{item.UpgradeDisplay} · {v.Width}×{v.Height}\n{ContainerName(item.Container)} · ô {item.SlotX},{item.SlotY}\nĐộ bền {item.Durability} · số lượng {item.StackCount}";destination.SelectedItem=Containers.FirstOrDefault(c=>c.Id==item.Container)??Containers[1];destX.Value=Math.Clamp(item.SlotX,0,12);destY.Value=Math.Clamp(item.SlotY,0,9);}UpdateUpgradeControls(item);inventory.Invalidate();warehouse.Invalidate();}
    private void BuildUpgradePanel(Control host){var group=new GroupBox{Text="CƯỜNG HÓA TRANG BỊ",Width=300,Height=166,ForeColor=Color.FromArgb(39,81,125),Margin=new Padding(0,8,0,10)};upgradeInfo.SetBounds(12,23,276,34);group.Controls.Add(upgradeInfo);upgradeLevel.SetBounds(12,63,58,27);upgradeMinus.SetBounds(76,61,36,30);upgradePlus.SetBounds(116,61,36,30);upgradeApply.SetBounds(158,61,126,30);group.Controls.Add(upgradeLevel);group.Controls.Add(upgradeMinus);group.Controls.Add(upgradePlus);group.Controls.Add(upgradeApply);upgradeRule.SetBounds(12,99,276,27);group.Controls.Add(upgradeRule);group.Controls.Add(new Label{Text="0–12 · không trừ tiền/nguyên liệu",Left=12,Top=134,Width=276,Height=26,ForeColor=Color.FromArgb(30,45,64),Font=new Font("Segoe UI",8)});host.Controls.Add(group);upgradeMinus.Click+=async(_,_)=>await ApplyUpgradeAsync(Math.Max(0,(selected?.UpgradeDisplay??0)-1));upgradePlus.Click+=async(_,_)=>await ApplyUpgradeAsync(Math.Min(12,(selected?.UpgradeDisplay??0)+1));upgradeApply.Click+=async(_,_)=>await ApplyUpgradeAsync((int)upgradeLevel.Value);UpdateUpgradeControls(null);}
    private void UpdateUpgradeControls(ItemRecord? item){
        upgradeRule.Items.Clear();bool equipment=item?.Genre==0;
        bool ready=equipment&&character?.CanSave==true&&!upgrading;
        upgradeInfo.Text=item==null?"Chọn vũ khí hoặc trang bị cần cường hóa.":assets.Visual(item).Name+" · hiện tại +"+item.UpgradeDisplay;
        upgradeLevel.Value=Math.Clamp(item?.UpgradeDisplay??0,0,12);
        if(equipment){
            try{foreach(var rule in UpgradeBridge.Rules(item!,serverRoot))upgradeRule.Items.Add(rule);}
            catch(Exception ex){status.Text="Không đọc được quy tắc cường hóa: "+ex.Message;}
            int oldRule=(item!.UpgradeLevel&0x40000000)!=0?(item.UpgradeLevel>>8)&65535:0;
            int selectedRule=0;
            for(int i=0;i<upgradeRule.Items.Count;i++)if(upgradeRule.Items[i] is UpgradeRule listed&&listed.Id==oldRule)selectedRule=i;
            if(upgradeRule.Items.Count>0)upgradeRule.SelectedIndex=selectedRule;
            upgradeRule.Enabled=ready&&oldRule==0;
        }else upgradeRule.Enabled=false;
        upgradeLevel.Enabled=upgradeApply.Enabled=ready;
        upgradeMinus.Enabled=ready&&(item?.UpgradeDisplay??0)>0;
        upgradePlus.Enabled=ready&&(item?.UpgradeDisplay??0)<12&&upgradeRule.Items.Count>0;
    }
    private async Task ApplyUpgradeAsync(int level){var owner=character;var item=selected;if(upgrading||owner==null||item==null||item.Genre!=0)return;try{int rule=level==0?0:(upgradeRule.SelectedItem as UpgradeRule)?.Id??0;if(level>0&&rule==0)throw new InvalidOperationException("Trang bị này chưa có quy tắc cường hóa được hỗ trợ.");upgrading=true;UseWaitCursor=true;upgradeApply.Enabled=upgradeMinus.Enabled=upgradePlus.Enabled=false;inventory.Enabled=warehouse.Enabled=all.Enabled=false;save.Enabled=export.Enabled=false;status.Text="Đang tính cường hóa và kiểm tra nạp lại vật phẩm…";await owner.UpgradeAsync(item.Index,level,rule,serverRoot);if(!ReferenceEquals(owner,character))return;dirty=true;RefreshViews();SelectItem(owner.GetItems()[item.Index]);status.Text="Đã đổi cường hóa thành +"+level+" trong bản đang chỉnh. Lưu hoặc Xuất bộ lưu để ghi dữ liệu.";}catch(Exception ex){MessageBox.Show(ex.Message,"Cường hóa chưa được áp dụng",MessageBoxButtons.OK,MessageBoxIcon.Information);}finally{upgrading=false;UseWaitCursor=false;inventory.Enabled=warehouse.Enabled=all.Enabled=true;save.Enabled=export.Enabled=character?.CanSave==true;UpdateUpgradeControls(selected);}}
    private void RefreshViews(){inventory.Invalidate();warehouse.Invalidate();all.BeginUpdate();all.Items.Clear();if(character!=null)foreach(var i in character.GetItems()){var row=new ListViewItem(i.Index.ToString()){Tag=i.Index};row.SubItems.AddRange(new[]{assets.Visual(i).Name,ContainerName(i.Container),$"{i.SlotX},{i.SlotY}",i.UpgradeDisplay.ToString()});all.Items.Add(row);}all.EndUpdate();RefreshSkills();save.Enabled=export.Enabled=character?.CanSave==true;if(character!=null)status.Text=$"{character.ItemCount} vật phẩm · revision {character.Revision} · "+character.CompanionStatus;}
    private bool MayDiscard()=>!dirty||MessageBox.Show("Bỏ các thay đổi chưa lưu?","Thay đổi chưa lưu",MessageBoxButtons.YesNo,MessageBoxIcon.Question)==DialogResult.Yes;
    private static void Try(Action action){try{action();}catch(Exception ex){MessageBox.Show(ex.Message,"Character Editor",MessageBoxButtons.OK,MessageBoxIcon.Information);}}
    protected override void Dispose(bool disposing){if(disposing)assets.Dispose();base.Dispose(disposing);}
    public sealed record ContainerOption(int Id,string Name){public override string ToString()=>Name;}
    public static readonly ContainerOption[] Containers={new(2,"Đang đeo"),new(3,"Túi 1"),new(4,"Túi 2"),new(18,"Túi 3"),new(19,"Túi 4"),new(5,"Kho 1"),new(6,"Kho 2"),new(7,"Kho 3"),new(8,"Kho 4"),new(9,"Kho 5"),new(10,"Kho 6"),new(1,"Trên tay"),new(13,"Phím nhanh"),new(14,"Trao nhiệm vụ"),new(15,"Ghép"),new(16,"Khảm / chế tạo"),new(17,"Kho ghép"),new(11,"Giao dịch"),new(12,"Giao dịch đối phương")};
    public static string ContainerName(int id)=>Containers.FirstOrDefault(c=>c.Id==id)?.Name??"Vùng "+id;
    public void RenderPreview(string filename,int page=0){CreateControl();Show();tabs.SelectedIndex=page;if(character!=null)SelectItem(character.GetItems().FirstOrDefault(i=>i.Genre==0));Application.DoEvents();using var bitmap=new Bitmap(Width,Height);DrawToBitmap(bitmap,new Rectangle(0,0,Width,Height));bitmap.Save(filename);Hide();}
}
