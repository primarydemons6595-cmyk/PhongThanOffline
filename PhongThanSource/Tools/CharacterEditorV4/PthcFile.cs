using System;
using System.Buffers.Binary;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Security.Cryptography;
using System.Diagnostics;

namespace PhongThanCharacterEditor;

public sealed class ItemRecord
{
    public int Index { get; internal set; }
    public int TemplateRow { get; set; }
    public int Genre { get; set; }
    public int Container { get; set; }
    public int SlotX { get; set; }
    public int SlotY { get; set; }
    public int DetailType { get; set; }
    public int ParticularType { get; set; }
    public int Level { get; set; }
    public int Series { get; set; }
    public int Luck { get; set; }
    public int Durability { get; set; }
    public int StackCount { get; set; }
    public int UpgradeLevel { get; set; }
    public int PhysicalValue { get; set; }
    public int MagicValue { get; set; }
    public int UpgradeDisplay => UpgradeLevel >= 0 && UpgradeLevel <= 15 ? UpgradeLevel : (UpgradeLevel & unchecked((int)0xff0000f0)) == 0x40000000 ? UpgradeLevel & 15 : -1;
    public string ContainerName => Container == 2 ? "Trang bị" : Container == 3 ? "Hành trang" : $"Container {Container}";
    public string DisplayName => $"G{Genre}-D{DetailType}-P{ParticularType} Lv{Level}";
}

public sealed partial class PthcFile
{
    public const uint MagicPhtc = 0x43544850;
    public const ushort FormatVersion = 1;
    public const int FileHeaderSize = 16;
    public const int StateHeaderSize = 408;
    public const int SkillRecordSize = 12;
    public const int TaskRecordSize = 20;
    public const int ItemRecordSize = 268;
    private byte[] _bytes = Array.Empty<byte>();
    private byte[] _original = Array.Empty<byte>();
    private byte[]? _companion;
    private byte[]? _embedded;
    private uint _loginUtc,_loginIp;
    private void ReadDisk(byte[] raw){var disk=PthcBundle.Read(raw);_bytes=disk.StateFile;_embedded=disk.Items;_loginUtc=disk.Utc;_loginIp=disk.Ip;Validate();_original=(byte[])raw.Clone();}
    private readonly List<int> _ordinals = new();
    private readonly Dictionary<int,byte[]> _entryOverrides = new();
    public string CompanionRoot { get; private set; } = "";
    public string CompanionStatus { get; private set; } = "Chưa kiểm tra";
    public bool CanSave => _companion != null;
    public uint Revision => ReadU32(ORevision);
    public int Gender => _bytes[FileHeaderSize+80];
    public int Profession => _bytes[FileHeaderSize+81];
    public string ProfessionName => Profession switch {0=>"Giáp Sĩ",1=>"Đạo Sĩ",2=>"Dị Nhân",3=>"Vũ Sĩ",_=>$"Không rõ ({Profession})"};
    public static string Hash(byte[] b) => Convert.ToHexString(SHA256.HashData(b)).ToLowerInvariant();

    private const int OSchemaVersion=0,OStateSize=4,ORevision=8,ORoleTime=12,ORoleName=16,OAccountName=48;
    private const int OBankMoney=196,OMoney=200,OFightLevel=208,OFightExperience=212;
    private const int ORemainingAttributePoints=272,ORemainingSkillPoints=276;
    private const int OFightSkillCount=392,OStateSkillCount=396,OTaskCount=400,OItemCount=404;

    public string FilePath { get; private set; } = "";
    public string RoleName => ReadCString(ORoleName,32);
    public string AccountName => ReadCString(OAccountName,32);
    public int BankMoney { get=>ReadS32(OBankMoney); set=>WriteS32(OBankMoney,value); }
    public int Money { get=>ReadS32(OMoney); set=>WriteS32(OMoney,value); }
    public const int BoundMoneyTaskId=4847;
    int TaskOffset => checked(FileHeaderSize+StateHeaderSize+(int)(FightSkillCount+StateSkillCount)*SkillRecordSize);
    public int BoundMoney {
        get {for(int i=0;i<TaskCount;i++){int at=TaskOffset+i*TaskRecordSize;if(BinaryPrimitives.ReadInt32LittleEndian(_bytes.AsSpan(at,4))==BoundMoneyTaskId)return int.TryParse(Encoding.ASCII.GetString(_bytes,at+4,16).Split('\0')[0],out int amount)?Math.Max(0,amount):0;}return 0;}
        set {
            if(value<0)throw new ArgumentOutOfRangeException(nameof(value));
            for(int i=0;i<TaskCount;i++){int at=TaskOffset+i*TaskRecordSize;if(BinaryPrimitives.ReadInt32LittleEndian(_bytes.AsSpan(at,4))==BoundMoneyTaskId){_bytes.AsSpan(at+4,16).Clear();Encoding.ASCII.GetBytes(value.ToString(System.Globalization.CultureInfo.InvariantCulture)).CopyTo(_bytes,at+4);return;}}
            if(value==0)return;
            if(TaskCount>=255||StateSize+TaskRecordSize>65536)throw new InvalidOperationException("Không đủ chỗ lưu thêm ví Bạc khóa trong bản ghi nhân vật.");
            int end=TaskOffset+(int)TaskCount*TaskRecordSize;var next=new byte[_bytes.Length+TaskRecordSize];Buffer.BlockCopy(_bytes,0,next,0,end);Buffer.BlockCopy(_bytes,end,next,end+TaskRecordSize,_bytes.Length-end);_bytes=next;
            BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(end,4),BoundMoneyTaskId);Encoding.ASCII.GetBytes(value.ToString(System.Globalization.CultureInfo.InvariantCulture)).CopyTo(_bytes,end+4);WriteU32(OTaskCount,TaskCount+1);
            uint size=(uint)(_bytes.Length-FileHeaderSize);BinaryPrimitives.WriteUInt32LittleEndian(_bytes.AsSpan(8,4),size);WriteU32(OStateSize,size);
        }
    }

    public int FightLevel { get=>ReadS32(OFightLevel); set=>WriteS32(OFightLevel,value); }
    public int FightExperience { get=>ReadS32(OFightExperience); set=>WriteS32(OFightExperience,value); }
    public int RemainingAttributePoints { get=>ReadS32(ORemainingAttributePoints); set=>WriteS32(ORemainingAttributePoints,value); }
    public int RemainingSkillPoints { get=>ReadS32(ORemainingSkillPoints); set=>WriteS32(ORemainingSkillPoints,value); }
    public uint FightSkillCount=>ReadU32(OFightSkillCount); public uint StateSkillCount=>ReadU32(OStateSkillCount);
    public uint TaskCount=>ReadU32(OTaskCount); public uint ItemCount=>ReadU32(OItemCount);
    public uint StoredChecksum=>BinaryPrimitives.ReadUInt32LittleEndian(_bytes.AsSpan(12,4));
    public uint CalculatedChecksum=>Fnv1a(_bytes.AsSpan(FileHeaderSize,StateSize));
    public bool ChecksumValid=>StoredChecksum==CalculatedChecksum;
    public int StateSize=>checked((int)BinaryPrimitives.ReadUInt32LittleEndian(_bytes.AsSpan(8,4)));

    public static PthcFile Load(string path, string? companionRoot=null){var f=new PthcFile{FilePath=Path.GetFullPath(path)};f.ReadDisk(File.ReadAllBytes(path));f._ordinals.AddRange(System.Linq.Enumerable.Range(0,(int)f.ItemCount));f.CompanionRoot=companionRoot??f.FindCompanionRoot();f.LoadCompanion();return f;}
    private static string AsciiLower(string s)=>new string(s.Select(c=>c is >= 'A' and <= 'Z'?(char)(c+32):c).ToArray());
    public static string AccountComponent(string account){if(string.IsNullOrEmpty(account)||account.Length>=32)throw new InvalidDataException("Tài khoản không hợp lệ.");string v=AsciiLower(account);bool plain=v.All(c=>c is >= 'a' and <= 'z' or >= '0' and <= '9' or '_' or '-');if(v is "con" or "prn" or "aux" or "nul" or "consumablebuffs" || (v.Length==4&&(v.StartsWith("com")||v.StartsWith("lpt"))&&char.IsAsciiDigit(v[3])))plain=false;return plain?v:"~"+Convert.ToHexString(Encoding.Latin1.GetBytes(v)).ToLowerInvariant();}
    private string FindCompanionRoot(){string parent=Path.GetDirectoryName(FilePath)!;string key=Hash(_bytes[16..]);var candidates=new List<string>{Path.Combine(parent,"NativeDropPersistenceV1")};var current=new DirectoryInfo(parent);for(int i=0;i<4&&current!=null;i++,current=current.Parent){candidates.Add(Path.Combine(current.FullName,"NativeDropPersistenceV1"));candidates.Add(Path.Combine(current.FullName,AccountComponent(AccountName),"NativeDropPersistenceV1"));}return candidates.FirstOrDefault(d=>File.Exists(Path.Combine(d,key+".magic-v1"))||File.Exists(Path.Combine(d,key+".required")))??candidates[0];}
    private bool IsServerStore(){for(var d=new DirectoryInfo(Path.GetDirectoryName(FilePath)!);d!=null;d=d.Parent)if(d.Name.Equals("CharacterStore",StringComparison.OrdinalIgnoreCase)&&d.Parent?.Name.Equals("Server",StringComparison.OrdinalIgnoreCase)==true)return true;return false;}
    private void LoadCompanion(){try{var key=Hash(_bytes[16..]);var c=_embedded??File.ReadAllBytes(Path.Combine(CompanionRoot,key+".magic-v1"));var required=_embedded!=null?Hash(c):File.ReadAllText(Path.Combine(CompanionRoot,key+".required"));if(c.Length<144||BitConverter.ToUInt32(c,0)!=0x474d3050||(BitConverter.ToUInt32(c,4)!=1&&BitConverter.ToUInt32(c,4)!=2)||BitConverter.ToUInt32(c,8)!=748||BitConverter.ToUInt32(c,12)!=ItemCount||c.Length!=144+748*ItemCount||Encoding.ASCII.GetString(c,16,64)!=key||Encoding.ASCII.GetString(c,80,64)!=Hash(c[144..])||required!=Hash(c))throw new InvalidDataException("Companion không khớp.");var serials=new HashSet<uint>();for(int i=0;i<ItemCount;i++){var o=144+i*748;uint serial=BitConverter.ToUInt32(c,o+4);if(BitConverter.ToUInt32(c,o)!=(uint)i||serial==0||serial>=0xfffffffe||!serials.Add(serial))throw new InvalidDataException("Serial/ordinal không hợp lệ.");for(int a=0;a<20;a++){int type=BitConverter.ToInt32(c,o+12+a*16);if(type<0||type>=(BitConverter.ToUInt32(c,4)==1?357:387))throw new InvalidDataException("Magic type không hợp lệ.");}}_companion=c;CompanionStatus="Companion hợp lệ · giữ nguyên thuộc tính/serial";}catch(Exception ex){_companion=null;CompanionStatus="Chỉ xem: thiếu hoặc lỗi companion. "+ex.Message;}}
    public byte[] ItemBytes(int index)=>_bytes.AsSpan(ItemAbs(index,0),268).ToArray();
    public byte[] UpgradeInput(int index){if(!CanSave||index<0||index>=ItemCount)throw new InvalidOperationException("Vật phẩm/companion không hợp lệ.");var input=new byte[1016];ItemBytes(index).CopyTo(input,0);int ordinal=_ordinals[index];if(_entryOverrides.TryGetValue(ordinal,out var changed))changed.CopyTo(input,268);else Array.Copy(_companion!,144+ordinal*748,input,268,748);return input;}
    public async Task UpgradeAsync(int index,int level,int rule,string serverRoot){
        var input=UpgradeInput(index);if(BitConverter.ToInt32(input,8)!=0)throw new InvalidOperationException("Chỉ cường hóa trang bị.");
        var output=await UpgradeBridge.RunAsync(input,serverRoot,level,rule);
        if(!UpgradeInput(index).SequenceEqual(input))throw new IOException("Vật phẩm đã thay đổi trong lúc tính. Hãy chọn lại.");
        for(int i=0;i<268;i++)if((i<252||i>=256)&&input[i]!=output[i])throw new InvalidDataException("Native đã đổi metadata ngoài mức cường hóa.");
        if(!input.AsSpan(268,12).SequenceEqual(output.AsSpan(268,12)))throw new InvalidDataException("Native đã đổi ordinal/serial/quality.");
        int encoded=BitConverter.ToInt32(output,252);int expected=level==0?0:0x40000000|(rule<<8)|level;if(encoded!=expected)throw new InvalidDataException("Native trả sai mức/rule.");
        _entryOverrides[_ordinals[index]]=output[268..];ItemWriteS32(index,252,encoded);
    }

    private int ItemsStateOffset => checked(StateHeaderSize + (int)FightSkillCount*SkillRecordSize + (int)StateSkillCount*SkillRecordSize + (int)TaskCount*TaskRecordSize);
    private int ItemAbs(int index,int fieldOffset)=>FileHeaderSize+ItemsStateOffset+index*ItemRecordSize+fieldOffset;
    private int ItemS32(int i,int o)=>BinaryPrimitives.ReadInt32LittleEndian(_bytes.AsSpan(ItemAbs(i,o),4));
    private void ItemWriteS32(int i,int o,int v)=>BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(ItemAbs(i,o),4),v);

    public List<ItemRecord> GetItems(){
        var list=new List<ItemRecord>();
        for(int i=0;i<(int)ItemCount;i++) list.Add(new ItemRecord{
            Index=i, TemplateRow=ItemS32(i,4), Genre=ItemS32(i,8), Container=ItemS32(i,12), SlotX=ItemS32(i,16), SlotY=ItemS32(i,20),
            DetailType=ItemS32(i,24), ParticularType=ItemS32(i,28), Level=ItemS32(i,32), Series=ItemS32(i,36), Luck=ItemS32(i,48), Durability=ItemS32(i,52),
            StackCount=ItemS32(i,220), UpgradeLevel=ItemS32(i,252), PhysicalValue=ItemS32(i,256), MagicValue=ItemS32(i,260)
        });
        return list;
    }

    public void UpdateItem(ItemRecord x){
        if(x.Index<0||x.Index>=(int)ItemCount) throw new ArgumentOutOfRangeException(nameof(x.Index));
        if(x.UpgradeLevel!=ItemS32(x.Index,252)||x.PhysicalValue!=ItemS32(x.Index,256)||x.MagicValue!=ItemS32(x.Index,260)||x.Durability!=ItemS32(x.Index,52)||x.StackCount!=ItemS32(x.Index,220))throw new InvalidOperationException("Chỉ di chuyển vật phẩm; thuộc tính/cường hóa cần engine native để tính lại.");
        ItemWriteS32(x.Index,12,x.Container); ItemWriteS32(x.Index,16,x.SlotX); ItemWriteS32(x.Index,20,x.SlotY);
        ItemWriteS32(x.Index,52,x.Durability); ItemWriteS32(x.Index,220,x.StackCount); ItemWriteS32(x.Index,252,x.UpgradeLevel);
        ItemWriteS32(x.Index,256,x.PhysicalValue); ItemWriteS32(x.Index,260,x.MagicValue);
    }

    public void DeleteItem(int index){
        if(index<0||index>=(int)ItemCount) throw new ArgumentOutOfRangeException(nameof(index));
        int start=ItemAbs(index,0), end=start+ItemRecordSize;
        var next=new byte[_bytes.Length-ItemRecordSize];
        Buffer.BlockCopy(_bytes,0,next,0,start);
        Buffer.BlockCopy(_bytes,end,next,start,_bytes.Length-end);
        _bytes=next;
        _ordinals.RemoveAt(index);
        uint newCount=ItemCount-1;
        WriteU32(OItemCount,newCount);
        uint newStateSize=(uint)(_bytes.Length-FileHeaderSize);
        BinaryPrimitives.WriteUInt32LittleEndian(_bytes.AsSpan(8,4),newStateSize);
        WriteU32(OStateSize,newStateSize);
    }

    public void SaveWithBackup(){
        if(IsServerStore() && Process.GetProcessesByName("GameServer").Length>0)throw new InvalidOperationException("Hãy logout và dừng GameServer trước khi ghi trực tiếp; có thể Xuất bộ lưu khi server đang chạy.");
        if(Hash(File.ReadAllBytes(FilePath))!=Hash(_original))throw new IOException("File đã thay đổi bên ngoài editor. Hãy nạp lại.");
        string canonical="role_"+Convert.ToHexString(Encoding.Latin1.GetBytes(AsciiLower(RoleName))).ToLowerInvariant()+".pthc";
        if(!Path.GetFileName(FilePath).Equals(canonical,StringComparison.OrdinalIgnoreCase))throw new IOException("File đang mang tên lưu riêng. Dùng Xuất bộ lưu để tránh ghi đè nhân vật mới.");
        if(!CanSave)throw new InvalidOperationException(CompanionStatus);
        File.Copy(FilePath,FilePath+".before-editor.bak",true);
        WriteState(FilePath,CompanionRoot);_original=File.ReadAllBytes(FilePath);_ordinals.Clear();_ordinals.AddRange(Enumerable.Range(0,(int)ItemCount));_entryOverrides.Clear();LoadCompanion();
    }
    public string ExportPackage(string destination){if(Directory.Exists(destination)&&Directory.EnumerateFileSystemEntries(destination).Any())throw new IOException("Chọn thư mục xuất mới/rỗng.");Directory.CreateDirectory(destination);var store=Path.Combine(destination,"CharacterStore",AccountComponent(AccountName));Directory.CreateDirectory(store);var file=Path.Combine(store,"role_"+Convert.ToHexString(Encoding.Latin1.GetBytes(AsciiLower(RoleName))).ToLowerInvariant()+".pthc");WriteState(file,Path.Combine(store,"NativeDropPersistenceV1"),false);return file;}
    private void WriteState(string target,string companionRoot,bool advance=true){
        if(_companion==null)throw new InvalidOperationException(CompanionStatus);ValidateStructureOnly();
        var next=(byte[])_bytes.Clone();BinaryPrimitives.WriteUInt32LittleEndian(next.AsSpan(24,4),checked(Revision+1));
        BinaryPrimitives.WriteUInt32LittleEndian(next.AsSpan(12,4),Fnv1a(next.AsSpan(16)));
        var key=Hash(next[16..]);var c=new byte[144+748*_ordinals.Count];Array.Copy(_companion,c,144);BinaryPrimitives.WriteUInt32LittleEndian(c.AsSpan(12,4),(uint)_ordinals.Count);
        for(int i=0;i<_ordinals.Count;i++){if(_entryOverrides.TryGetValue(_ordinals[i],out var changed))changed.CopyTo(c,144+748*i);else Array.Copy(_companion,144+748*_ordinals[i],c,144+748*i,748);BinaryPrimitives.WriteUInt32LittleEndian(c.AsSpan(144+i*748,4),(uint)i);}
        Encoding.ASCII.GetBytes(key).CopyTo(c,16);Encoding.ASCII.GetBytes(Hash(c[144..])).CopyTo(c,80);
        Atomic(target,PthcBundle.Write(next,c,_loginUtc,_loginIp));if(advance){_bytes=next;_embedded=c;}
    }
    private static void Immutable(string path,byte[] data){if(File.Exists(path)){if(!File.ReadAllBytes(path).SequenceEqual(data))throw new IOException("Snapshot conflict: "+path);return;}Atomic(path,data,false);}
    private static void Atomic(string path,byte[] data,bool replace=true){string temp=path+".tmp."+Guid.NewGuid().ToString("N");try{using(var f=new FileStream(temp,FileMode.CreateNew,FileAccess.Write,FileShare.None)){f.Write(data);f.Flush(true);}File.Move(temp,path,replace);}finally{if(File.Exists(temp))File.Delete(temp);}}
    private void Validate(){ValidateStructureOnly();if(!ChecksumValid)throw new InvalidDataException($"Checksum không hợp lệ. Stored=0x{StoredChecksum:X8}, Calculated=0x{CalculatedChecksum:X8}");}
    private void ValidateStructureOnly(){
        if(_bytes.Length<FileHeaderSize+StateHeaderSize)throw new InvalidDataException("File quá nhỏ.");
        if(BinaryPrimitives.ReadUInt32LittleEndian(_bytes.AsSpan(0,4))!=MagicPhtc)throw new InvalidDataException("Không phải file PHTC.");
        if(BinaryPrimitives.ReadUInt16LittleEndian(_bytes.AsSpan(4,2))!=FormatVersion)throw new InvalidDataException("FormatVersion không hỗ trợ.");
        if(BinaryPrimitives.ReadUInt16LittleEndian(_bytes.AsSpan(6,2))!=FileHeaderSize)throw new InvalidDataException("HeaderSize không đúng.");
        if(StateSize<StateHeaderSize||FileHeaderSize+StateSize!=_bytes.Length)throw new InvalidDataException("StateSize không khớp kích thước file.");
        if(ReadU32(OSchemaVersion)!=1||ReadU32(OStateSize)!=(uint)StateSize)throw new InvalidDataException("Schema/StateSize không hợp lệ.");
        if(StateSize>65536||FightSkillCount>512||StateSkillCount>512||TaskCount>255||ItemCount>512||!_bytes.AsSpan(32,32).Contains((byte)0)||!_bytes.AsSpan(64,32).Contains((byte)0))throw new InvalidDataException("Record bounds hoặc tên nhân vật/tài khoản không hợp lệ.");
        long expected=StateHeaderSize+(long)FightSkillCount*12+(long)StateSkillCount*12+(long)TaskCount*20+(long)ItemCount*ItemRecordSize;
        if(expected!=StateSize)throw new InvalidDataException($"Kích thước record không khớp. Expected={expected}, StateSize={StateSize}");
    }
    private int Abs(int o)=>FileHeaderSize+o; private int ReadS32(int o)=>BinaryPrimitives.ReadInt32LittleEndian(_bytes.AsSpan(Abs(o),4));
    private uint ReadU32(int o)=>BinaryPrimitives.ReadUInt32LittleEndian(_bytes.AsSpan(Abs(o),4));
    private void WriteS32(int o,int v)=>BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(Abs(o),4),v);
    private void WriteU32(int o,uint v)=>BinaryPrimitives.WriteUInt32LittleEndian(_bytes.AsSpan(Abs(o),4),v);
    private string ReadCString(int o,int len){var s=_bytes.AsSpan(Abs(o),len);int n=s.IndexOf((byte)0);if(n<0)n=len;return Encoding.Latin1.GetString(s[..n]);}
    private static uint Fnv1a(ReadOnlySpan<byte> data){uint h=2166136261u;foreach(byte b in data){h^=b;h*=16777619u;}return h;}
}
