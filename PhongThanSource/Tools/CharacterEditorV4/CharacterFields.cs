using System.Buffers.Binary;
using System.Text;
namespace PhongThanCharacterEditor;

public sealed partial class PthcFile
{
    public int Fame {get=>TaskValue(4840);set=>SetTaskValue(4840,value);}
    public int PkValue {get=>ReadS32(268);set=>WriteS32(268,Math.Max(0,value));}
    public int Respect {get=>ReadS32(380);set{WriteS32(380,Math.Max(0,value));SetTaskValue(217,Math.Max(0,value));}}
    public int TaskValue(int id){
        int start=FileHeaderSize+StateHeaderSize+checked((int)(FightSkillCount+StateSkillCount)*12);
        for(int n=0;n<TaskCount;n++)if(BitConverter.ToInt32(_bytes,start+n*20)==id){
            string value=Encoding.ASCII.GetString(_bytes,start+n*20+4,16).Split('\0')[0];return int.TryParse(value,out int parsed)?parsed:0;
        }return 0;
    }
    public void SetTaskValue(int id,int value){
        if(id<0||id>=5000||value<0)throw new ArgumentOutOfRangeException(nameof(value));
        int start=FileHeaderSize+StateHeaderSize+checked((int)(FightSkillCount+StateSkillCount)*12),offset=-1;
        for(int n=0;n<TaskCount;n++)if(BitConverter.ToInt32(_bytes,start+n*20)==id)offset=start+n*20;
        if(offset<0){
            if(TaskCount>=255)throw new InvalidDataException("Nhân vật đã đủ số biến nhiệm vụ có thể lưu.");
            offset=start+checked((int)TaskCount*20);var next=new byte[_bytes.Length+20];
            _bytes.AsSpan(0,offset).CopyTo(next);_bytes.AsSpan(offset).CopyTo(next.AsSpan(offset+20));_bytes=next;
            WriteU32(OTaskCount,TaskCount+1);WriteU32(OStateSize,(uint)(_bytes.Length-16));
            BinaryPrimitives.WriteUInt32LittleEndian(_bytes.AsSpan(8,4),(uint)(_bytes.Length-16));
        }
        BinaryPrimitives.WriteInt32LittleEndian(_bytes.AsSpan(offset,4),id);_bytes.AsSpan(offset+4,16).Clear();Encoding.ASCII.GetBytes(value.ToString(System.Globalization.CultureInfo.InvariantCulture)).CopyTo(_bytes,offset+4);
    }
    public void RepairPosition(){
        int map=ReadS32(172);if(map<1001||map>1999)throw new InvalidDataException("Bản đồ hồi sinh không hợp lệ. Hãy chọn thành an toàn.");
        // Only use the persisted valid revival point. Missing points require an explicit town selection.
        int x=ReadS32(176),y=ReadS32(180);
        if(x<=0||y<=0)throw new InvalidDataException("Điểm hồi sinh cũng bị rỗng. Hãy chọn thành an toàn trong mục Khôi phục tọa độ.");
        WriteS32(172,map);WriteS32(184,map);WriteS32(188,x);WriteS32(192,y);
    }
    public void SetSafePosition(int map,int x,int y){
        if(map is <1001 or >1999||x<=0||y<=0||x>=300000||y>=300000)throw new InvalidDataException("Tọa độ an toàn không hợp lệ.");
        WriteS32(172,map);WriteS32(176,x);WriteS32(180,y);WriteS32(184,map);WriteS32(188,x);WriteS32(192,y);
    }
    public void RepairAtTown(string serverRoot,int runtimeMap){
        if(runtimeMap is <1002 or >1004)throw new InvalidDataException("Chọn một trong ba Tân Thủ Thôn.");
        string section="",target=(runtimeMap-1000).ToString();var entries=new Dictionary<string,string>();
        foreach(string raw in File.ReadAllLines(Path.Combine(serverRoot,"settings","revivepos.ini"),Encoding.Latin1)){
            string line=raw.Trim();if(line.StartsWith('[')&&line.EndsWith(']'))section=line[1..^1];
            else if(section==target&&line.Contains('=')){int split=line.IndexOf('=');entries[line[..split].Trim()]=line[(split+1)..].Trim();}
        }
        if(entries.TryGetValue("region",out string? choices))foreach(string key in choices.Split(',',StringSplitOptions.TrimEntries|StringSplitOptions.RemoveEmptyEntries)){
            if(entries.TryGetValue(key,out string? value)){var xy=value.Split(',');if(xy.Length==2&&int.TryParse(xy[0],out int x)&&int.TryParse(xy[1],out int y)&&x>0&&y>0){SetSafePosition(runtimeMap,x,y);return;}}
        }
        throw new InvalidDataException("Không có tọa độ hồi sinh gốc hợp lệ của thành đã chọn.");
    }
}
