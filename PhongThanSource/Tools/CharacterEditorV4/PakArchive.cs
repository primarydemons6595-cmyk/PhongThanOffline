using System.Text;

namespace PhongThanCharacterEditor;

// Read-only PACK reader; layout and NRV2B semantics follow the repository's
// XPackFile.cpp / build_offline_loose.py. No archive extraction or rewriting.
public sealed class PakArchive : IDisposable
{
    private readonly FileStream stream;
    private readonly Dictionary<uint,Entry> index=new();
    public string FilePath {get;}
    public int RejectedEntries {get;private set;}
    private sealed record Entry(uint Offset,int Size,uint Flags);
    public PakArchive(string file){FilePath=Path.GetFullPath(file);stream=new FileStream(file,FileMode.Open,FileAccess.Read,FileShare.Read);try{
        var h=Read(0,32);uint count=U32(h,4),table=U32(h,8),data=U32(h,12);if(Encoding.ASCII.GetString(h,0,4)!="PACK"||count==0||count>2_000_000||table<32||data<32||(long)table+count*16>stream.Length)throw new InvalidDataException("Invalid PACK header/index");
        var b=Read(table,checked((int)count*16));for(int i=0;i<count;i++){int o=i*16;uint id=U32(b,o),pos=U32(b,o+4),size=U32(b,o+8),flags=U32(b,o+12);if(id==0||size==0||size>256*1024*1024||pos<32||(long)pos+(flags&0xffffff)>stream.Length){RejectedEntries++;continue;}index.TryAdd(id,new(pos,(int)size,flags));}
    }catch{stream.Dispose();throw;}}
    public static uint Id(string path,Encoding encoding){var b=encoding.GetBytes("\\"+path.Replace('/','\\').TrimStart('\\'));uint id=0;unchecked{for(int i=0;i<b.Length;i++){byte ch=b[i];if(ch>=65&&ch<=90)ch+=32;int signed=(sbyte)ch;id=((id+(uint)((i+1)*signed))%0x8000000b)*0xffffffef;}}return id^0x12345678;}
    public byte[]? Get(string name){Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);foreach(var enc in new[]{Encoding.Latin1,Encoding.GetEncoding(936),Encoding.GetEncoding(1252)})if(index.TryGetValue(Id(name,enc),out var entry))return Extract(entry);return null;}
    private byte[] Extract(Entry e){int stored=(int)(e.Flags&0xffffff);uint method=e.Flags&0xff000000;if((method&0x10000000)!=0)return Frames(e,stored);return Decode(Read(e.Offset,stored),e.Size,method);}
    private byte[] Frames(Entry e,int stored){var b=Read(e.Offset,stored);if(b.Length<40||Encoding.ASCII.GetString(b,0,3)!="SPR")throw new InvalidDataException("PACK SPR header");int count=BitConverter.ToUInt16(b,12),colors=BitConverter.ToUInt16(b,14);if(count<1||count>4096||colors<1||colors>256)throw new InvalidDataException("PACK SPR count");Exception? last=null;
        foreach(bool palette in new[]{true,false})try{int table=32+(palette?colors*3:0),prefix=checked(table+count*8),input=prefix,total=prefix;if(prefix>b.Length)throw new InvalidDataException("PACK SPR table");var frames=new List<byte[]>();
            for(int i=0;i<count;i++){int o=table+i*8,packed=BitConverter.ToInt32(b,o),signed=BitConverter.ToInt32(b,o+4);long size=Math.Abs((long)signed);if(packed<=0||size<8||size>64*1024*1024||packed>b.Length-input||total+size>256*1024*1024)throw new InvalidDataException("PACK SPR frame range");var frame=Decode(b.AsSpan(input,packed).ToArray(),(int)size,signed<0?0:e.Flags&0x0f000000);frames.Add(frame);input+=packed;total+=(int)size;}
            if(input!=b.Length)throw new InvalidDataException("PACK SPR trailing bytes");var blob=new byte[total];Array.Copy(b,blob,table);int output=prefix;for(int i=0;i<count;i++){BitConverter.GetBytes(output-prefix).CopyTo(blob,table+i*8);BitConverter.GetBytes(frames[i].Length).CopyTo(blob,table+i*8+4);frames[i].CopyTo(blob,output);output+=frames[i].Length;}using var validation=Spr.Decode(blob);return blob;
        }catch(Exception ex) when(ex is InvalidDataException or OverflowException or ArgumentException){last=ex;}
        throw new InvalidDataException("PACK SPR frames rejected",last);
    }
    private static byte[] Decode(byte[] source,int size,uint method){if(method==0){if(source.Length!=size)throw new InvalidDataException("PACK raw size mismatch");return source;}if(method is not (0x01000000 or 0x20000000))throw new NotSupportedException($"PACK compression 0x{method:X8} chưa hỗ trợ");return Nrv2b(source,size);}
    public static byte[] Nrv2b(byte[] src,int expected){if(expected<0||expected>256*1024*1024)throw new InvalidDataException("NRV bound");var dst=new byte[expected];int input=0,output=0,last=1;uint bits=0;
        byte Byte(){if(input>=src.Length)throw new InvalidDataException("NRV truncated");return src[input++];}
        int Bit(){bits=(bits&127)!=0?bits*2:(uint)(Byte()*2+1);return (int)((bits>>8)&1);}
        while(true){while(Bit()!=0){if(output>=dst.Length)throw new InvalidDataException("NRV output overrun");dst[output++]=Byte();}uint offset=1;do{offset=offset*2+(uint)Bit();if(offset>0x1000002)throw new InvalidDataException("NRV offset");}while(Bit()==0);
            if(offset==2)offset=(uint)last;else{offset=unchecked((offset-3)*256+Byte());if(offset==uint.MaxValue)break;last=checked((int)++offset);}
            int length=Bit()*2+Bit();if(length==0){length=1;do{length=checked(length*2+Bit());if(length>expected)throw new InvalidDataException("NRV length");}while(Bit()==0);length+=2;}if(offset>0xd00)length++;
            if(offset==0||offset>output||(long)output+length+1>expected)throw new InvalidDataException("NRV backreference");for(int n=0;n<=length;n++){dst[output]=dst[output-(int)offset];output++;}
        }if(output!=expected||input!=src.Length)throw new InvalidDataException("NRV size/trailing mismatch");return dst;
    }
    private byte[] Read(long offset,int size){if(size<0||offset<0||offset+size>stream.Length)throw new InvalidDataException("PACK read bounds");byte[] b=new byte[size];stream.Position=offset;stream.ReadExactly(b);return b;}
    private static uint U32(byte[] b,int o)=>BitConverter.ToUInt32(b,o);
    public void Dispose()=>stream.Dispose();
}
