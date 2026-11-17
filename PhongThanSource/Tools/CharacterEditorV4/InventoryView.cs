using System.Drawing.Drawing2D;

namespace PhongThanCharacterEditor;

public sealed class InventoryView : Control
{
    public PthcFile? Character {get;set;}
    public ClientAssets Assets {get;}
    public new int Container {get;set;}=3;
    public bool Warehouse {get;set;}
    public int Selected {get;set;}=-1;
    public event Action<ItemRecord?>? ItemSelected;
    public event Action? Changed;
    public event Action<int>? PageSelected;
    private readonly ToolTip tooltip=new(){AutoPopDelay=15000,InitialDelay=120};
    private readonly List<(RectangleF rect,int index)> hit=new();
    private int down=-1,hover=-2;
    private float scale=1;
    private float originX=12;
    private float GridX=>Warehouse?Assets.StoreNumber("ItemBox","Left",20):Assets.Number("ItemBox","Left",284);
    private float GridY=>Warehouse?Assets.StoreNumber("ItemBox","Top",74):Assets.Number("ItemBox","Top",56);
    private float Cell=>Warehouse?Assets.StoreNumber("ItemBox","Width",218)/6f:44;
    private readonly (int slot,string section,string name)[] slots={
        (0,"Helm","Mũ"),(1,"Armor","Áo"),(2,"Belt","Đai"),(3,"Weapon","Vũ khí"),(4,"Boots","Giày"),
        (6,"Talisman1","Pháp bảo 1"),(7,"Talisman2","Pháp bảo 2"),(9,"Pendant","Phi phong"),(10,"Horse","Thú cưỡi"),(11,"Signet","Ấn"),(12,"Instrument","Pháp khí"),(5,"JadePendant","Ngọc bội")};
    public InventoryView(ClientAssets assets){Assets=assets;DoubleBuffered=true;BackColor=Color.FromArgb(16,21,28);ForeColor=Color.FromArgb(233,220,185);ResizeRedraw=true;MinimumSize=new Size(530,490);}
    private RectangleF SlotRect(string section){int x=Assets.Number(section,"Left",20)+38,y=Assets.Number(section,"Top",40)+38;var art=Assets.Image(Assets.Value(section,"Image"));return new(x,y,art?.Width??42,art?.Height??48);}
    private static void DrawImage(Graphics g,Bitmap? image,RectangleF rect){if(image==null)return;float s=Math.Min(1,Math.Min(rect.Width/image.Width,rect.Height/image.Height));g.DrawImage(image,rect.X+(rect.Width-image.Width*s)/2,rect.Y+(rect.Height-image.Height*s)/2,image.Width*s,image.Height*s);}
    protected override void OnPaint(PaintEventArgs e){base.OnPaint(e);hit.Clear();var g=e.Graphics;g.Clear(BackColor);scale=Math.Max(.5f,Math.Min((ClientSize.Width-24)/(Warehouse?280f:540f),(ClientSize.Height-24)/(Warehouse?540f:485f)));originX=Warehouse?(ClientSize.Width-256*scale)/2:12;g.TranslateTransform(originX,12);g.ScaleTransform(scale,scale);g.InterpolationMode=InterpolationMode.NearestNeighbor;
        if(!Warehouse){var bg=Assets.Image(Assets.Value("Main","Image"));if(bg!=null)g.DrawImageUnscaled(bg,0,0);
            string[] professions={"Warrior_","Wizard_","Sprite_","Archer_"};
            int frame=Character==null?1:Character.Profession>=0&&Character.Profession<professions.Length?
                Assets.Number("Equip",professions[Character.Profession]+(Character.Gender==0?"Male":"Female"),Character.Profession==3?-1:1):-1;
            var mannequin=frame>=0?Assets.Image(Assets.Value("Equip","Image"),frame):null;if(mannequin!=null)g.DrawImageUnscaled(mannequin,Assets.Number("Equip","Left",37),Assets.Number("Equip","Top",35));
            foreach(var slot in slots){var rect=SlotRect(slot.section);var art=Assets.Image(Assets.Value(slot.section,"Image"));DrawImage(g,art,rect);}
        }else{
            var bg=Assets.Image(Assets.StoreValue("Main","Image"));if(bg!=null){float top=GridY,sourceEnd=Assets.StoreNumber("ItemBox","Top",74)+Assets.StoreNumber("ItemBox","Height",293),newEnd=GridY+10*Cell;g.DrawImage(bg,new RectangleF(0,0,bg.Width,top),new RectangleF(0,0,bg.Width,top),GraphicsUnit.Pixel);g.DrawImage(bg,new RectangleF(0,top,bg.Width,newEnd-top),new RectangleF(0,top,bg.Width,sourceEnd-top),GraphicsUnit.Pixel);g.DrawImage(bg,new RectangleF(0,newEnd,bg.Width,bg.Height-sourceEnd),new RectangleF(0,sourceEnd,bg.Width,bg.Height-sourceEnd),GraphicsUnit.Pixel);}
            foreach(string section in new[]{"LeftBtn","RightBtn"}){var art=Assets.Image(Assets.StoreValue(section,"Image"));if(art!=null)g.DrawImageUnscaled(art,Assets.StoreNumber(section,"Left"),Assets.StoreNumber(section,"Top"));}
            using var pageFont=new Font("Segoe UI",9,FontStyle.Bold);g.DrawString("Kho "+(Container-4),pageFont,Brushes.Gold,105,29);if(Character!=null)g.DrawString(Character.BankMoney.ToString("N0"),pageFont,Brushes.Gold,55,GridY+10*Cell+3);
        }
        int cols=Warehouse?6:5,rows=Warehouse?10:7;float cell=Cell,gx=GridX,gy=GridY;
        using(var fill=new SolidBrush(Color.FromArgb(Warehouse?255:200,22,24,25)))g.FillRectangle(fill,gx,gy,cols*cell,rows*cell);
        using(var pen=new Pen(Color.FromArgb(100,151,132,86)))for(int x=0;x<cols;x++)for(int y=0;y<rows;y++)g.DrawRectangle(pen,gx+x*cell,gy+y*cell,cell,cell);
        if(Character!=null)foreach(var item in Character.GetItems()){
            RectangleF r;var visual=Assets.Visual(item);
            if(!Warehouse&&item.Container==2){var slot=slots.FirstOrDefault(s=>s.slot==item.SlotX);r=SlotRect(slot.section??"Helm");}
            else if(item.Container==Container){r=new(gx+item.SlotX*cell,gy+item.SlotY*cell,visual.Width*cell,visual.Height*cell);}
            else continue;
            hit.Add((r,item.Index));using(var fill=new SolidBrush(Color.FromArgb(80,40,34,19)))g.FillRectangle(fill,r);DrawImage(g,visual.Icon,r);
            using var border=new Pen(item.Index==Selected?Color.Gold:Color.FromArgb(110,167,204,187),item.Index==Selected?2:1);g.DrawRectangle(border,r.X,r.Y,r.Width,r.Height);
            using var font=new Font("Segoe UI",9,FontStyle.Bold);if(visual.Icon==null)g.DrawString("#"+item.TemplateRow,font,Brushes.Silver,r);
            if(item.UpgradeDisplay>0)g.DrawString("+"+item.UpgradeDisplay,font,Brushes.Gold,r.X+2,r.Y+1);
            if(item.StackCount>1)g.DrawString(item.StackCount.ToString(),font,Brushes.White,r.X+2,r.Bottom-16);
        }
        using var caption=new Font("Segoe UI",9);g.DrawString(Warehouse?"6 × 10 ô · chuyển túi/kho ở bảng bên phải":"Kéo vật phẩm để đổi ô hoặc đeo/tháo. Chọn vật phẩm để xem chi tiết.",caption,Brushes.Wheat,Warehouse?0:16,Warehouse?520:451);
    }
    private PointF Local(Point p)=>new((p.X-originX)/scale,(p.Y-12)/scale);
    private int Hit(Point p){var q=Local(p);for(int i=hit.Count-1;i>=0;i--)if(hit[i].rect.Contains(q))return hit[i].index;return -1;}
    protected override void OnMouseDown(MouseEventArgs e){base.OnMouseDown(e);down=Hit(e.Location);Selected=down;ItemSelected?.Invoke(down<0?null:Character?.GetItems()[down]);Invalidate();}
    protected override void OnMouseMove(MouseEventArgs e){base.OnMouseMove(e);int i=Hit(e.Location);if(i==hover)return;hover=i;if(i<0||Character==null){tooltip.SetToolTip(this,"");return;}var item=Character.GetItems()[i];var v=Assets.Visual(item);tooltip.SetToolTip(this,$"{v.Name}\nCấp {item.Level} · +{item.UpgradeDisplay} · {v.Width}×{v.Height}\nVị trí {item.Container}: {item.SlotX},{item.SlotY}\n{v.Sprite}");}
    protected override void OnMouseUp(MouseEventArgs e){base.OnMouseUp(e);var q=Local(e.Location);if(Warehouse&&q.Y>=28&&q.Y<48){if(q.X>=34&&q.X<90)Container=Math.Max(5,Container-1);else if(q.X>=154&&q.X<210)Container=Math.Min(10,Container+1);PageSelected?.Invoke(Container);Invalidate();down=-1;return;}if(down<0||Character==null)return;var item=Character.GetItems()[down];down=-1;try{
        if(!Warehouse){foreach(var slot in slots)if(SlotRect(slot.section).Contains(q)){Move(item,2,slot.slot,0);return;}}
        float gx=GridX,gy=GridY,cell=Cell;
        int x=(int)Math.Floor((q.X-gx)/cell),y=(int)Math.Floor((q.Y-gy)/cell);if(x>=0&&y>=0&&x<(Warehouse?6:5)&&y<(Warehouse?10:7))Move(item,Container,x,y);
    }catch(Exception ex){MessageBox.Show(ex.Message,"Không thể di chuyển",MessageBoxButtons.OK,MessageBoxIcon.Information);}}
    public new void Move(ItemRecord item,int container,int x,int y){
        if(Character==null||!Character.CanSave)throw new InvalidOperationException("Nhân vật thiếu companion; chỉ xem.");var visual=Assets.Visual(item);
        if(container==2){int[] details={7,2,6,0,5,8,4,3,3,9,10,11,12};if(item.Genre!=0||x<0||x>=details.Length||!(item.DetailType==details[x]||(x==3&&item.DetailType==1)))throw new InvalidOperationException("Loại vật phẩm không khớp ô trang bị.");if(Character.GetItems().Any(i=>i.Index!=item.Index&&i.Container==2&&i.SlotX==x))throw new InvalidOperationException("Ô đang có trang bị. Hãy tháo món hiện tại vào túi trước.");}
        else{int width=container is >=5 and <=10?6:5,height=container is >=5 and <=10?10:7;if(x<0||y<0||x+visual.Width>width||y+visual.Height>height)throw new InvalidOperationException("Vật phẩm vượt biên túi/kho.");var rect=new Rectangle(x,y,visual.Width,visual.Height);foreach(var other in Character.GetItems().Where(i=>i.Index!=item.Index&&i.Container==container)){var v=Assets.Visual(other);if(rect.IntersectsWith(new Rectangle(other.SlotX,other.SlotY,v.Width,v.Height)))throw new InvalidOperationException("Ô bị chiếm bởi vật phẩm khác.");}}
        item.Container=container;item.SlotX=x;item.SlotY=y;Character.UpdateItem(item);Changed?.Invoke();Invalidate();
    }
    protected override void Dispose(bool disposing){if(disposing)tooltip.Dispose();base.Dispose(disposing);}
}
