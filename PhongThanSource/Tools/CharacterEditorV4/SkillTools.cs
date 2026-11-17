using System.Text;

namespace PhongThanCharacterEditor;

public sealed record SkillVisual(string Name,int Maximum,Bitmap? Icon,string Description);
public sealed partial class MainForm
{
    Button? potentialAction,companionAction;
    public void ShowSkillEditor()=>tabs.SelectedIndex=2;
    public void ShowPotentialRepair()=>potentialAction?.PerformClick();
    public void ShowCompanionRepair()=>companionAction?.PerformClick();
    readonly ListView skillList=new(){View=View.Details,FullRowSelect=true,HideSelection=false,Dock=DockStyle.Fill,MultiSelect=false};
    readonly NumericUpDown skillLevel=new(){Minimum=1,Maximum=63,Width=72};
    readonly Button skillApply=new(){Text="Áp dụng cấp",AutoSize=true};
    readonly Label skillHelp=new(){Dock=DockStyle.Bottom,Height=72,Padding=new Padding(10)};
    readonly ImageList skillImages=new(){ImageSize=new Size(36,36),ColorDepth=ColorDepth.Depth32Bit};
    Dictionary<int,SkillVisual> ReadSkillCatalog(){
        var result=new Dictionary<int,SkillVisual>();var bytes=assets.Read("settings/Skills.txt");if(bytes==null)return result;
        var lines=Encoding.Latin1.GetString(bytes).Split('\n');var header=lines[0].TrimEnd('\r').Split('\t');
        int id=Array.IndexOf(header,"SkillId"),name=Array.IndexOf(header,"SkillName"),max=Array.IndexOf(header,"MaxLevel"),icon=Array.IndexOf(header,"SkillIcon"),desc=Array.IndexOf(header,"SkillDesc");
        if(new[]{id,name,max,icon,desc}.Any(i=>i<0))return result;
        foreach(var line in lines.Skip(1)){
            var row=line.TrimEnd('\r').Split('\t');if(row.Length<=new[]{id,name,max,icon,desc}.Max()||!int.TryParse(row[id],out int key))continue;
            int.TryParse(row[max],out int limit);
            result.TryAdd(key,new(ClientAssets.Tcvn(row[name].TrimStart('#')),Math.Clamp(limit,0,63),null,ClientAssets.Tcvn(row[desc].TrimStart('#'))));
        }return result;
    }
    TabPage BuildSkills(){
        var page=new TabPage("Kỹ năng");skillList.Columns.Add("Kỹ năng đã học",310);skillList.Columns.Add("ID",70);skillList.Columns.Add("Cấp",80);skillList.Columns.Add("Tối đa",80);skillList.Columns.Add("Tiến độ luyện",110);skillList.SmallImageList=skillImages;
        var tools=new FlowLayoutPanel{Dock=DockStyle.Top,Height=46,Padding=new Padding(8)};
        tools.Controls.Add(new Label{Text="Cấp kỹ năng",AutoSize=true,Margin=new Padding(0,5,8,0)});tools.Controls.Add(skillLevel);
        var plus=new Button{Text="+1 cấp",AutoSize=true};plus.Click+=(_,_)=>{if(skillLevel.Enabled&&skillLevel.Value<skillLevel.Maximum)skillLevel.Value++;};tools.Controls.Add(plus);tools.Controls.Add(skillApply);
        page.Controls.Add(skillList);page.Controls.Add(skillHelp);page.Controls.Add(tools);
        skillList.SelectedIndexChanged+=(_,_)=>{
            bool selected=skillList.SelectedItems.Count==1&&character?.CanSave==true;
            var entry=selected?((LearnedSkill Skill,SkillVisual Visual))skillList.SelectedItems[0].Tag!:default;
            selected&=entry.Visual?.Maximum>0;
            skillLevel.Enabled=skillApply.Enabled=selected;
            if(selected&&entry.Visual is not null){skillLevel.Maximum=entry.Visual.Maximum;skillLevel.Value=Math.Clamp(entry.Skill.Level,1,entry.Visual.Maximum);skillHelp.Text=entry.Visual.Name+"\n"+System.Text.RegularExpressions.Regex.Replace(entry.Visual.Description,@"<[^>]*>","");}
        };
        skillApply.Click+=(_,_)=>Try(()=>{
            if(character==null||skillList.SelectedItems.Count!=1)return;
            var entry=((LearnedSkill Skill,SkillVisual Visual))skillList.SelectedItems[0].Tag!;
            if(entry.Skill.Level==(int)skillLevel.Value)return;
            character.SetSkillLevel(entry.Skill.Index,(int)skillLevel.Value,entry.Visual.Maximum);dirty=true;RefreshSkills();
            status.Text="Đã chỉnh cấp kỹ năng trong bản nháp. Nhấn Lưu trực tiếp để áp dụng; tiến độ luyện của cấp cũ được đặt về 0.";
        });
        skillApply.Enabled=skillLevel.Enabled=false;return page;
    }
    void RefreshSkills(){
        skillList.BeginUpdate();skillList.Items.Clear();skillImages.Images.Clear();
        if(character!=null){var catalog=ReadSkillCatalog();
            // Resolve icons only for learned skills to keep large skill tables responsive.
            var raw=assets.Read("settings/Skills.txt");var iconPaths=new Dictionary<int,string>();
            if(raw!=null){var lines=Encoding.Latin1.GetString(raw).Split('\n');var h=lines[0].TrimEnd('\r').Split('\t');int id=Array.IndexOf(h,"SkillId"),ic=Array.IndexOf(h,"SkillIcon");if(id>=0&&ic>=0)foreach(var line in lines.Skip(1)){var r=line.TrimEnd('\r').Split('\t');if(r.Length>Math.Max(id,ic)&&int.TryParse(r[id],out int key))iconPaths.TryAdd(key,r[ic]);}}
            foreach(var skill in character.LearnedSkills()){
                var v=catalog.GetValueOrDefault(skill.Id,new SkillVisual("Kỹ năng #"+skill.Id,0,null,"Không tìm thấy trong bảng kỹ năng client; chỉ xem."));
                var row=new ListViewItem(v.Name){Tag=(skill,v)};
                if(iconPaths.TryGetValue(skill.Id,out string? path)&&assets.Image(path) is Bitmap bitmap){skillImages.Images.Add(skill.Id.ToString(),bitmap);row.ImageKey=skill.Id.ToString();}
                row.SubItems.AddRange(new[]{skill.Id.ToString(),skill.Level.ToString(),v.Maximum>0?v.Maximum.ToString():"Chỉ xem",skill.Experience.ToString()});skillList.Items.Add(row);
            }
        }
        skillList.EndUpdate();skillApply.Enabled=skillLevel.Enabled=false;
        skillHelp.Text="Chọn kỹ năng đã học để chỉnh cấp. Giới hạn đọc từ bảng Skills.txt, tối đa 63 theo engine. Lưu trực tiếp sau khi thoát nhân vật; server vẫn chạy.";
    }
    void BuildRepairTools(Control parent){
        var potential=new Button{Text="Bổ sung tiềm năng",Width=290,Height=36,AutoSize=true};
        potentialAction=potential;
        potential.Click+=(_,_)=>Try(()=>{
            if(character==null)return;ApplyStats();int level=(int)stats["level"].Value;
            var plan=character.PreviewPotential(serverRoot,level);string[] names={"Sức mạnh","Thân pháp","Thể chất","Trí lực"};
            string preview=string.Join("\n",Enumerable.Range(0,4).Select(i=>$"{names[i]}: {plan.Before[i]} → {plan.After[i]}"));
            if(plan.Before.SequenceEqual(plan.After)){status.Text="Thuộc tính đã đủ theo cấp hiện tại; không cộng thêm.";return;}
            if(MessageBox.Show(preview+$"\n\nDùng {plan.Consumed} điểm đang dư; bù thêm {plan.Added} điểm còn thiếu.\nGiữ các thuộc tính đang cao hơn định mức. Áp dụng vào bản nháp?","Bổ sung tiềm năng",MessageBoxButtons.YesNo,MessageBoxIcon.Question)!=DialogResult.Yes)return;
            character.SupplementPotential(serverRoot,level);stats["attr"].Value=character.RemainingAttributePoints;dirty=true;
            status.Text="Đã bù và phân thuộc tính theo bảng tăng trưởng của phái. Nhấn Lưu trực tiếp để áp dụng. Bấm lại không cộng trùng.";
        });parent.Controls.Add(potential);
        var companion=new Button{Text="Sửa lỗi Companion",Width=290,Height=36,AutoSize=true};
        companionAction=companion;
        companion.Click+=async(_,_)=>{
            if(character==null)return;var owner=character;
            if(owner.CanSave){status.Text="Companion đã hợp lệ, giữ nguyên thuộc tính và serial.";return;}
            if(dirty){MessageBox.Show("Nạp lại nhân vật trước khi sửa Companion để giữ bản gốc chính xác.","Companion");return;}
            using var dialog=new FolderBrowserDialog{Description="Chọn thư mục bản sao lưu để tìm Companion khớp. Thư mục dữ liệu hiện tại cũng được kiểm tra.",SelectedPath=Path.Combine(Directory.GetParent(serverRoot)!.FullName,"ManagerStudio","PhongThanStudio","Backups")};
            if(dialog.ShowDialog()!=DialogResult.OK)return;
            try{Enabled=false;UseWaitCursor=true;string result=await Task.Run(()=>owner.RepairCompanion(new[]{dialog.SelectedPath,Path.Combine(serverRoot,"NativeDropPersistenceV1")}));dirty=true;RefreshViews();status.Text=result;}
            catch(Exception ex){MessageBox.Show(ex.Message,"Chưa phục hồi Companion",MessageBoxButtons.OK,MessageBoxIcon.Information);}
            finally{Enabled=true;UseWaitCursor=false;}
        };parent.Controls.Add(companion);
    }
}
