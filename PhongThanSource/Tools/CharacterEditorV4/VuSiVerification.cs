using System.Buffers.Binary;
using System.Text;

namespace PhongThanCharacterEditor;

internal static class VuSiVerification
{
    public static int Run(string root)
    {
        string folder=Path.Combine(root,"analysis","event_handbook_20261026","studio-fixture-"+Guid.NewGuid().ToString("N"));Directory.CreateDirectory(folder);
        string file=Path.Combine(folder,"role_fixture.pthc"),comp=Path.Combine(folder,"NativeDropPersistenceV1");Directory.CreateDirectory(comp);
        byte[] bytes=new byte[16+408];
        void U32(int offset,uint value)=>BinaryPrimitives.WriteUInt32LittleEndian(bytes.AsSpan(offset,4),value);
        U32(0,PthcFile.MagicPhtc);BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(4,2),1);BinaryPrimitives.WriteUInt16LittleEndian(bytes.AsSpan(6,2),16);U32(8,408);
        U32(16,1);U32(20,408);U32(24,1);Encoding.ASCII.GetBytes("VuSiFixture").CopyTo(bytes,32);Encoding.ASCII.GetBytes("StudioFixture").CopyTo(bytes,64);
        bytes[16+81]=3;U32(16+208,60);for(int a=0;a<4;a++)U32(16+224+a*4,10);U32(16+244,100);U32(16+248,100);U32(16+252,100);
        uint checksum=2166136261;foreach(byte b in bytes.AsSpan(16))checksum=unchecked((checksum^b)*16777619);U32(12,checksum);File.WriteAllBytes(file,bytes);
        string hash=PthcFile.Hash(bytes[16..]);byte[] companion=new byte[144];BinaryPrimitives.WriteUInt32LittleEndian(companion,0x474d3050);BinaryPrimitives.WriteUInt32LittleEndian(companion.AsSpan(4),2);BinaryPrimitives.WriteUInt32LittleEndian(companion.AsSpan(8),748);
        Encoding.ASCII.GetBytes(hash).CopyTo(companion,16);Encoding.ASCII.GetBytes(PthcFile.Hash(Array.Empty<byte>())).CopyTo(companion,80);
        File.WriteAllBytes(Path.Combine(comp,hash+".magic-v1"),companion);File.WriteAllText(Path.Combine(comp,hash+".required"),PthcFile.Hash(companion));
        var actor=PthcFile.Load(file,comp);
        void Check(bool ok,string name){if(!ok)throw new InvalidDataException(name);}
        Check(actor.CanSave&&actor.Profession==3&&actor.ProfessionName=="Vũ Sĩ","Studio must recognize native profession 3");
        bool rejected=false;try{actor.PreviewPotential(Path.Combine(root,"PhongThanRuntime-Loose","Server"),60);}catch(InvalidDataException){rejected=true;}
        Check(rejected,"Unknown conversion history must not invent a base profile");
        actor.SetTaskValue(4894,50);actor.Money=7654321;
        string server=Path.Combine(root,"PhongThanRuntime-Loose","Server");var plan=actor.PreviewPotential(server,60);
        Check(plan.After.All(v=>v>=10)&&plan.Added>0,"Converted Vu Si potential must use original growth");
        actor.SupplementPotential(server,60);var again=actor.PreviewPotential(server,60);
        Check(again.Before.SequenceEqual(again.After)&&again.Added==0,"Potential repair must be idempotent");
        string exported=actor.ExportPackage(Path.Combine(folder,"export"));var reloaded=PthcFile.Load(exported);
        Check(reloaded.CanSave&&reloaded.ChecksumValid&&reloaded.Profession==3&&reloaded.Money==7654321&&reloaded.TaskValue(4894)==50,"Save/reload must preserve class, money and conversion history");
        File.WriteAllText(Path.Combine(folder,"PASS.txt"),"PASS Studio Vu Si recognition save/reload potential conversion history idempotence");
        return 0;
    }
}
