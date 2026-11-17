using System.Buffers.Binary;
using System.Text;
namespace PhongThanCharacterEditor;

// Only the disk envelope changes. Editors work with the unchanged canonical state.
internal static class PthcBundle
{
    private static string Hash(byte[] bytes)=>Convert.ToHexString(System.Security.Cryptography.SHA256.HashData(bytes)).ToLowerInvariant();
    internal static uint Fnv(ReadOnlySpan<byte> bytes){uint h=2166136261;foreach(byte b in bytes)h=unchecked((h^b)*16777619);return h;}
    internal static (byte[] StateFile,byte[]? Items,uint Utc,uint Ip) Read(byte[] file){
        if(file.Length<424||file.Length>4*1024*1024||BitConverter.ToUInt32(file)!=0x43544850||(BitConverter.ToUInt16(file,6)!=16&&!(BitConverter.ToUInt16(file,4)==1&&BitConverter.ToUInt16(file,6)==0)))throw new InvalidDataException("Invalid PTHC header.");
        int n=checked((int)BitConverter.ToUInt32(file,8));if(n<408||n>65536||n>file.Length-16)throw new InvalidDataException("Invalid PTHC size.");
        var canonical=file[..(16+n)];ushort version=BitConverter.ToUInt16(file,4);
        if(version==1){if(file.Length!=canonical.Length)throw new InvalidDataException("Unexpected legacy trailer.");BinaryPrimitives.WriteUInt16LittleEndian(canonical.AsSpan(6,2),16);return(canonical,null,0,0);}
        if(version!=2||file.Length-canonical.Length<32)throw new InvalidDataException("Unsupported PTHC format.");
        var tail=file[canonical.Length..];uint checksum=BitConverter.ToUInt32(tail,24);Array.Clear(tail,24,4);
        if(BitConverter.ToUInt32(tail)!=0x32425450||BitConverter.ToUInt32(tail,4)!=1||BitConverter.ToUInt32(tail,28)!=0||checksum!=Fnv(tail))throw new InvalidDataException("PTHC item block checksum failed.");
        int items=checked((int)BitConverter.ToUInt32(tail,8)),pending=checked((int)BitConverter.ToUInt32(tail,12)),at=32;
        if(items<0||items>tail.Length-at||pending>8)throw new InvalidDataException("PTHC item block bounds.");
        byte[]? magic=items==0?null:tail.AsSpan(at,items).ToArray();at+=items;
        if(magic!=null)CheckMagic(magic,Hash(canonical[16..]));
        var keys=new HashSet<string>();
        for(int i=0;i<pending;i++){
            if(tail.Length-at<8)throw new InvalidDataException("PTHC pending bounds.");
            uint revision=BitConverter.ToUInt32(tail,at);int size=checked((int)BitConverter.ToUInt32(tail,at+4));at+=8;
            if(revision<=BitConverter.ToUInt32(canonical,24)||size<144||size>tail.Length-at)throw new InvalidDataException("PTHC pending bounds.");
            var entry=tail.AsSpan(at,size).ToArray();CheckMagic(entry);if(!keys.Add(Encoding.ASCII.GetString(entry,16,64)))throw new InvalidDataException("Duplicate pending block.");at+=size;
        }
        if(at!=tail.Length)throw new InvalidDataException("Unexpected PTHC trailing bytes.");
        BinaryPrimitives.WriteUInt16LittleEndian(canonical.AsSpan(4,2),1);
        return(canonical,magic,BitConverter.ToUInt32(tail,16),BitConverter.ToUInt32(tail,20));
    }
    private static void CheckMagic(byte[] c,string? key=null){
        if(c.Length<144||BitConverter.ToUInt32(c)!=0x474d3050||BitConverter.ToUInt32(c,4) is not(1 or 2)||BitConverter.ToUInt32(c,8)!=748||BitConverter.ToUInt32(c,12)>512||c.Length!=144+748*(long)BitConverter.ToUInt32(c,12)||Encoding.ASCII.GetString(c,80,64)!=Hash(c[144..])||(key!=null&&Encoding.ASCII.GetString(c,16,64)!=key))throw new InvalidDataException("Invalid embedded item attributes.");
    }
    internal static byte[] Write(byte[] canonical,byte[] items,uint utc,uint ip){
        CheckMagic(items,Hash(canonical[16..]));var file=new byte[canonical.Length+32+items.Length];canonical.CopyTo(file,0);
        BinaryPrimitives.WriteUInt16LittleEndian(file.AsSpan(4,2),2);var tail=file.AsSpan(canonical.Length);
        BinaryPrimitives.WriteUInt32LittleEndian(tail,0x32425450);BinaryPrimitives.WriteUInt32LittleEndian(tail[4..],1);
        BinaryPrimitives.WriteUInt32LittleEndian(tail[8..],(uint)items.Length);BinaryPrimitives.WriteUInt32LittleEndian(tail[16..],utc);BinaryPrimitives.WriteUInt32LittleEndian(tail[20..],ip);
        items.CopyTo(tail[32..]);BinaryPrimitives.WriteUInt32LittleEndian(tail[24..],Fnv(tail));return file;
    }
}
