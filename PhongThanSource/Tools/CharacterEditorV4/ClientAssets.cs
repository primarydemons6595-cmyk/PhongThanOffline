using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;

namespace PhongThanCharacterEditor;

public sealed class ClientAssets : IDisposable
{
    public string Root {get;private set;}
    public bool PreferPaks {get;private set;}
    public int PakCount=>paks.Count;
    public int PakReads {get;private set;}
    public int LooseReads {get;private set;}
    public readonly List<string> PackageWarnings=new();
    private readonly List<PakArchive> paks=new();
    private readonly Dictionary<string,Bitmap?> images=new(StringComparer.OrdinalIgnoreCase);
    private readonly Dictionary<string,string[][]> tables=new(StringComparer.OrdinalIgnoreCase);
    public readonly Dictionary<string,Dictionary<string,string>> Layout=new(StringComparer.OrdinalIgnoreCase);
    public readonly Dictionary<string,Dictionary<string,string>> StoreLayout=new(StringComparer.OrdinalIgnoreCase);
    public readonly List<string> Missing=new();
    public ClientAssets(string root){Root="";SetRoot(root);}
    public void SetRoot(string root){foreach(var pak in paks)pak.Dispose();paks.Clear();PackageWarnings.Clear();Root=Path.GetFullPath(root);var ini=Path.Combine(Root,"package.ini");if(File.Exists(ini)){var values=File.ReadAllLines(ini,Encoding.Latin1).Where(l=>l.Contains('=')).Select(l=>l.Split('=',2)).ToDictionary(v=>v[0].Trim(),v=>v[1].Trim(),StringComparer.OrdinalIgnoreCase);if(!values.TryGetValue("Mode",out var mode)||!mode.Equals("LOOSE_ONLY",StringComparison.OrdinalIgnoreCase)){string sub=values.GetValueOrDefault("Path","data").TrimStart('\\','/');for(int i=0;values.TryGetValue(i.ToString(),out var name)&&name!="";i++){try{paks.Add(new PakArchive(Path.Combine(Root,sub,name)));}catch(Exception ex){PackageWarnings.Add(name+": "+ex.Message);}}}}RefreshResources();}
    public void MountPaks(IEnumerable<string> files){var opened=new List<PakArchive>();try{foreach(var f in files)opened.Add(new PakArchive(f));}catch{foreach(var p in opened)p.Dispose();throw;}paks.InsertRange(0,opened);RefreshResources();}
    public void SetPreferPaks(bool value){PreferPaks=value;RefreshResources();}
    private void RefreshResources(){foreach(var b in images.Values)b?.Dispose();images.Clear();tables.Clear();Missing.Clear();ReadLayout("UiItem",Layout);ReadLayout("UiStoreBox",StoreLayout);}
    private void ReadLayout(string name,Dictionary<string,Dictionary<string,string>> target){target.Clear();var data=Read("Ui/ui3/"+name+".ini")??Read("Ui/ui4/"+name+".ini");if(data==null){var assembly=typeof(ClientAssets).Assembly;using var embedded=assembly.GetManifestResourceStream(assembly.GetManifestResourceNames().Single(n=>n.EndsWith(name+".layout.ini")));using var ms=new MemoryStream();embedded!.CopyTo(ms);data=ms.ToArray();}Dictionary<string,string>? section=null;foreach(var l in Encoding.Latin1.GetString(data).Split('\n')){string line=l.Trim();if(line.StartsWith('[')&&line.EndsWith(']'))target[line[1..^1]]=section=new(StringComparer.OrdinalIgnoreCase);else if(section!=null&&line.Contains('=')&&!line.StartsWith(';')){int eq=line.IndexOf('=');section[line[..eq].Trim()]=line[(eq+1)..].Trim();}}}
    public string StoreValue(string section,string key,string fallback="")=>StoreLayout.TryGetValue(section,out var s)&&s.TryGetValue(key,out var v)?v:fallback;
    public int StoreNumber(string section,string key,int fallback=0)=>int.TryParse(StoreValue(section,key),out int v)?v:fallback;
    public byte[]? Read(string relative){if(PreferPaks)foreach(var pak in paks){var b=pak.Get(relative);if(b!=null){PakReads++;return b;}}var file=Resolve(relative);if(File.Exists(file)){LooseReads++;return File.ReadAllBytes(file);}if(!PreferPaks)foreach(var pak in paks){var b=pak.Get(relative);if(b!=null){PakReads++;return b;}}return null;}
    public string Value(string section,string key,string fallback="")=>Layout.TryGetValue(section,out var s)&&s.TryGetValue(key,out var v)?v:fallback;
    public int Number(string section,string key,int fallback=0)=>int.TryParse(Value(section,key),out int v)?v:fallback;
    public string Resolve(string relative){string clean=relative.TrimStart('\\','/').Replace('\\',Path.DirectorySeparatorChar);string full=Path.GetFullPath(Path.Combine(Root,clean));if(!full.StartsWith(Root+Path.DirectorySeparatorChar,StringComparison.OrdinalIgnoreCase))throw new IOException("Resource outside client root.");if(File.Exists(full))return full;
        // Some extracts store Unicode Chinese names, others byte-preserving Latin-1 names.
        Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);string unicode=Encoding.GetEncoding(936).GetString(Encoding.Latin1.GetBytes(clean));string other=Path.GetFullPath(Path.Combine(Root,unicode));return File.Exists(other)?other:full;
    }
    public Bitmap? Image(string relative,int frame=0){string key=relative+"#"+frame;if(images.TryGetValue(key,out var found))return found;try{var data=Read(relative)??throw new FileNotFoundException(relative);var bitmap=Spr.Decode(data,frame);images[key]=bitmap;return bitmap;}catch(Exception ex){Missing.Add(relative+": "+ex.Message);images[key]=null;return null;}}
    public ItemVisual Visual(ItemRecord item){string[] equip={"meleeweapon","rangeweapon","armor","ring","amulet","boot","belt","helm","cuff","pendant","horse","instrument","jadependant"};string table=item.Genre==0&&item.DetailType>=0&&item.DetailType<equip.Length?equip[item.DetailType]:item.Genre switch{1=>"potion",2=>"eventitem",3=>"material",4=>"questkey",5=>"townportal",6=>"magicscript",7=>"skillbook",8=>"ibitem",_=>""};
        if(!tables.TryGetValue(table,out var rows)){var data=Read("settings/item/001/"+table+".txt");rows=data!=null?Encoding.Latin1.GetString(data).Split('\n').Skip(1).Where(l=>!string.IsNullOrWhiteSpace(l)).Select(l=>l.TrimEnd('\r').Split('\t')).ToArray():Array.Empty<string[]>();tables[table]=rows;}
        int lookupDetail=item.DetailType;bool starter=item.Genre==6&&item.DetailType==61000;if(starter){lookupDetail=272;var settings=Read("settings/item/PhongThanStarterBag.ini");if(settings!=null)foreach(var line in Encoding.Latin1.GetString(settings).Split('\n'))if(line.StartsWith("IconMagicScriptId="))int.TryParse(line.Split('=')[1].Trim(),out lookupDetail);}
        string[]? row=null;if(item.Genre==0&&item.TemplateRow>=0&&item.TemplateRow<rows.Length)row=rows[item.TemplateRow];else row=rows.FirstOrDefault(r=>r.Length>12&&N(r,1)==item.Genre&&N(r,item.Genre==6?3:2)==lookupDetail&&(item.Genre is 7 or 8?N(r,3)==item.ParticularType:true)&&(item.Genre==1?N(r,11)==item.Level:true));
        if(row==null||row.Length<8)return new(item.DisplayName,"",1,1,null);
        int imageColumn=Array.FindIndex(row,v=>v.Trim().EndsWith(".spr",StringComparison.OrdinalIgnoreCase));if(imageColumn<0)return new(Tcvn(row[0].TrimStart('#')),"",1,1,null);string image=row[imageColumn];return new(starter?"Túi tân thủ":Tcvn(row[0].TrimStart('#')),image,Math.Clamp(N(row,imageColumn+2),1,10),Math.Clamp(N(row,imageColumn+3),1,10),Image(image));
    }
    private static int N(string[] r,int n)=>r.Length>n&&int.TryParse(r[n],out int v)?v:0;
    public static string Tcvn(string text){const string from="µ¸¶·¹¨»¾¼½Æ©ÇÊÈÉË®ÌÐÎÏÑªÒÕÓÔÖ×ÝØÜÞßãáâä«åèæçé¬êíëìîïóñòô­õøö÷ùúýûüþ¡¢§£¤¥¦";const string to="àáảãạăằắẳẵặâầấẩẫậđèéẻẽẹêềếểễệìíỉĩịòóỏõọôồốổỗộơờớởỡợùúủũụưừứửữựỳýỷỹỵĂÂĐÊÔƠƯ";var b=new StringBuilder();foreach(char c in text){int i=from.IndexOf(c);b.Append(i>=0&&i<to.Length?to[i]:c);}return b.ToString();}
    public void Dispose(){foreach(var b in images.Values)b?.Dispose();images.Clear();foreach(var p in paks)p.Dispose();paks.Clear();}
}
public sealed record ItemVisual(string Name,string Sprite,int Width,int Height,Bitmap? Icon);

public static class Spr
{
    public static Bitmap Decode(byte[] b,int frame=0){
        if(b.Length<40||b[0]!='S'||b[1]!='P'||b[2]!='R')throw new InvalidDataException("SPR header");
        int width=U16(b,4),height=U16(b,6),frames=U16(b,12),colors=U16(b,14);
        if(width<1||height<1||width>4096||height>4096||frames<1||frames>4096||frame<0||frame>=frames||colors<1||colors>256)throw new InvalidDataException("SPR bounds");
        Exception? last=null;foreach(bool indexed in new[]{true,false}){try{
            int table=32+(indexed?colors*3:0),data=checked(table+frames*8),entry=table+frame*8;
            if(entry+8>b.Length)throw new InvalidDataException("SPR table");int o=checked(data+(int)U32(b,entry)),length=checked((int)U32(b,entry+4)),end=checked(o+length);
            if(o<data||length<8||end>b.Length)throw new InvalidDataException("SPR frame range");int w=U16(b,o),h=U16(b,o+2),xoff=U16(b,o+4),yoff=U16(b,o+6);
            if(w<1||h<1||w>4096||h>4096||xoff+w>8192||yoff+h>8192)throw new InvalidDataException("SPR frame dimensions");
            int canvasW=Math.Max(width,xoff+w),canvasH=Math.Max(height,yoff+h);if((long)canvasW*canvasH>20_000_000)throw new InvalidDataException("SPR too large");var pixels=new int[canvasW*canvasH];int p=o+8,done=0;
            while(done<w*h){if(p+2>end)throw new InvalidDataException("SPR RLE truncated");int run=b[p++],alpha=b[p++];if(run<1||run>w*h-done||(indexed&&run>w-done%w))throw new InvalidDataException("SPR run");if(alpha!=0){int need=run*(indexed?1:3);if(p+need>end)throw new InvalidDataException("SPR colors");for(int n=0;n<run;n++){int r,g,blue;if(indexed){int index=b[p++];if(index>=colors)throw new InvalidDataException("SPR palette");int pal=32+index*3;r=b[pal];g=b[pal+1];blue=b[pal+2];}else{r=b[p++];g=b[p++];blue=b[p++];}int pixel=done+n;pixels[(pixel/w+yoff)*canvasW+pixel%w+xoff]=(alpha<<24)|(r<<16)|(g<<8)|blue;}}done+=run;}
            var bitmap=new Bitmap(canvasW,canvasH,PixelFormat.Format32bppArgb);var bits=bitmap.LockBits(new Rectangle(0,0,canvasW,canvasH),ImageLockMode.WriteOnly,PixelFormat.Format32bppArgb);try{Marshal.Copy(pixels,0,bits.Scan0,pixels.Length);}finally{bitmap.UnlockBits(bits);}return bitmap;
        }catch(Exception ex) when(ex is InvalidDataException or OverflowException or ArgumentException){last=ex;}}
        throw new InvalidDataException("SPR indexed/true-color không hợp lệ",last);
    }
    private static int U16(byte[] b,int o)=>BitConverter.ToUInt16(b,o);
    private static uint U32(byte[] b,int o)=>BitConverter.ToUInt32(b,o);
}
