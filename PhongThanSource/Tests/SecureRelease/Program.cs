using System.Security.Cryptography;
using System.Text;
using PhongThanStudio.Core;
using PhongThan.Event.Runtime;

string root = Path.GetFullPath(args.Length > 0 ? args[0] : Path.Combine(Path.GetTempPath(), "pt-secure-tests-" + Guid.NewGuid().ToString("N")));
Directory.CreateDirectory(root); int checks = 0;
void Check(bool ok, string label) { if (!ok) throw new Exception(label); Console.WriteLine("PASS " + label); checks++; }
void Reject(Action action, string label) { try { action(); } catch (Exception e) when (e is CryptographicException or InvalidDataException or EndOfStreamException) { Check(true, label); return; } throw new Exception("Accepted invalid data: " + label); }
using var signer = ECDsa.Create(ECCurve.NamedCurves.nistP256);
using var keys = new SecurePakKeys(RandomNumberGenerator.GetBytes(32), signer);
using var verifier = ECDsa.Create(); verifier.ImportSubjectPublicKeyInfo(signer.ExportSubjectPublicKeyInfo(), out _);
byte[] content = new byte[SecurePak.ChunkSize * 2 + 317]; RandomNumberGenerator.Fill(content); System.Array.Fill(content, (byte)65, SecurePak.ChunkSize, SecurePak.ChunkSize);
string input = Path.Combine(root, "payload.bin"); File.WriteAllBytes(input, content);
byte[] name = "\\settings\\private-test.txt"u8.ToArray(); uint id = Legacy.Id(name);
var entry = new PakInput(id, name, input, SHA256.HashData(content), content.Length);
string data = Path.Combine(root, "Data"); Directory.CreateDirectory(data);
string archive = Path.Combine(data, "release.pak"); byte[] archiveId = new byte[16]; archiveId[0] = 1;
byte[] digest = SecurePak.Write(archive, Side.Server, 0, new byte[32], archiveId, [entry], keys);
ReleaseCatalog.Write(Path.Combine(data,ReleaseCatalog.FileName),Side.Server,[new(0,"release.pak",digest)],signer);
using (var pak = new SecurePak(archive, keys.Master, verifier))
{
    pak.VerifyPayload(); Check(pak.Entries.Count == 1 && pak.Entries[0].Chunks.Count == 3, "multi-chunk roundtrip");
    Check(pak.Entries[0].Chunks.Any(c => c.Compressed) && pak.Entries[0].Chunks.Any(c => !c.Compressed), "compressed and raw chunks");
    using var restored = new MemoryStream(); foreach (var c in pak.Entries[0].Chunks) { byte[] plain = pak.ReadChunk(pak.Entries[0], c); restored.Write(plain); CryptographicOperations.ZeroMemory(plain); }
    Check(restored.ToArray().SequenceEqual(content), "exact restored bytes");
    Check(pak.Digest.SequenceEqual(digest), "signed manifest digest");
}
byte[] original = File.ReadAllBytes(archive);
Check(!Encoding.Latin1.GetString(original).Contains("private-test.txt"), "no plaintext filename");
string collisionSource = Path.Combine(root,"collision.bin"); File.WriteAllBytes(collisionSource,"second-path"u8.ToArray());
string collisionFolder=Path.Combine(root,"collision-runtime","Data");Directory.CreateDirectory(collisionFolder);
byte[] collisionArchiveId=new byte[16];collisionArchiveId[0]=3;
byte[] collisionDigest=SecurePak.Write(Path.Combine(collisionFolder,"release.pak"),Side.Server,0,new byte[32],collisionArchiveId,[entry,new PakInput(id,"\\settings\\collision-b.txt"u8.ToArray(),collisionSource,SHA256.HashData("second-path"u8),11,false,true)],keys);
ReleaseCatalog.Write(Path.Combine(collisionFolder,ReleaseCatalog.FileName),Side.Server,[new(0,"release.pak",collisionDigest)],signer);
using(var collision=new SecurePak(Path.Combine(collisionFolder,"release.pak"),keys.Master,verifier)) { collision.VerifyPayload();Check(collision.Entries.Count==2&&collision.Entries[0].Id==collision.Entries[1].Id,"legacy ID collision preserves both named resources"); }
void Mutate(int offset, string label, bool payload)
{
    string path = Path.Combine(root, label + ".pak"); byte[] corrupt = original.ToArray(); corrupt[offset] ^= 64; File.WriteAllBytes(path, corrupt);
    Reject(() => { using var p = new SecurePak(path, keys.Master, verifier); if (payload) p.VerifyPayload(); }, label);
}
Mutate(12, "header-tamper", false); Mutate(80, "manifest-tamper", false); Mutate(original.Length - 1, "chunk-tag-tamper", true);
// A recovered data key must not confer signing authority. Re-encrypting a
// changed chunk with a valid GCM tag still violates its signed content hash.
using (var p = new SecurePak(archive, keys.Master, verifier))
{
    var e = p.Entries[0]; var c = e.Chunks[0]; byte[] altered = p.ReadChunk(e, c); altered[0] ^= 1;
    byte[] h = original[..72], k = HMACSHA256.HashData(keys.Master, h.AsSpan(16, 16)), nonce = new byte[12], aad = new byte[92];
    BitConverter.GetBytes(c.Counter).CopyTo(nonce, 8); h.CopyTo(aad, 0); BitConverter.GetBytes(c.Counter).CopyTo(aad, 72); e.OpaqueId.CopyTo(aad, 76);
    byte[] forgedCipher = new byte[altered.Length], forgedTag = new byte[16]; using (var aes = new AesGcm(k, 16)) aes.Encrypt(nonce, altered, forgedCipher, forgedTag, aad);
    byte[] forged = original.ToArray(); int start = 72 + BitConverter.ToInt32(h, 64) + 16 + 64 + checked((int)c.Offset); forgedCipher.CopyTo(forged, start); forgedTag.CopyTo(forged, start + forgedCipher.Length);
    string forgedPath = Path.Combine(root, "data-key-forgery.pak"); File.WriteAllBytes(forgedPath, forged);
    Reject(() => { using var check = new SecurePak(forgedPath, keys.Master, verifier); check.VerifyPayload(); }, "data-key-does-not-authorize-release");
    CryptographicOperations.ZeroMemory(k); CryptographicOperations.ZeroMemory(altered);
}
Reject(() => { using var p = new SecurePak(archive, RandomNumberGenerator.GetBytes(32), verifier); }, "wrong-data-key");
using (var wrong = ECDsa.Create(ECCurve.NamedCurves.nistP256)) Reject(() => { using var p = new SecurePak(archive, keys.Master, wrong); }, "wrong-release-signer");
archiveId[0] = 2;
var removed = new PakInput(id, name, null, new byte[32], 0, true);
string patch = Path.Combine(data, "update_0000000001.pak"); SecurePak.Write(patch, Side.Server, 1, digest, archiveId, [removed], keys);
using (var pak = new SecurePak(patch, keys.Master, verifier)) Check(pak.Entries[0].Deleted && pak.Parent.SequenceEqual(digest), "signed deletion and parent chain");
// Native probe fixtures: separate from production keys and runtime.
var pub = signer.ExportParameters(false); byte[] blob = new byte[72]; BitConverter.GetBytes(0x31534345u).CopyTo(blob, 0); BitConverter.GetBytes(32).CopyTo(blob, 4); pub.Q.X!.CopyTo(blob, 8); pub.Q.Y!.CopyTo(blob, 40);
string Array(string n, byte[] b) => "static const unsigned char " + n + "[] = {" + string.Join(",", b.Select(v => "0x" + v.ToString("X2"))) + "};\n";
File.WriteAllText(Path.Combine(root, "PtReleaseKeys.fixture.h"), "#define PT_SECURE_KEYS_READY 1\n" + Array("ptReleaseMaster", keys.Master) + Array("ptReleasePublic", blob));
File.WriteAllText(Path.Combine(root, "fixture.json"), System.Text.Json.JsonSerializer.Serialize(new { Id = id.ToString("X8"), Size = content.Length, Resource = "\\settings\\private-test.txt" }));
// Leave base+patch deletion test intact; move patch out for the successful read fixture.
File.Move(patch, Path.Combine(root, "deletion.pak"));
void RuntimeFixture(string folder,string pakFile,IReadOnlyList<ReleaseCatalogEntry> records,string? extra=null)
{
    string target=Path.Combine(root,folder,"Data");Directory.CreateDirectory(target);File.Copy(pakFile,Path.Combine(target,"release.pak"));
    if(extra is not null) File.Copy(extra,Path.Combine(target,records[1].File));
    ReleaseCatalog.Write(Path.Combine(target,ReleaseCatalog.FileName),Side.Server,records,signer);
}
RuntimeFixture("forgery-runtime",Path.Combine(root,"data-key-forgery.pak"),[new(0,"release.pak",digest)]);
RuntimeFixture("tamper-runtime",Path.Combine(root,"chunk-tag-tamper.pak"),[new(0,"release.pak",digest)]);
using(var deletion=new SecurePak(Path.Combine(root,"deletion.pak"),keys.Master,verifier))
    RuntimeFixture("deletion-runtime",archive,[new(0,"release.pak",digest),new(1,"update_0000000001.pak",deletion.Digest.ToArray())],Path.Combine(root,"deletion.pak"));
string oddFolder=Path.Combine(root,"odd-runtime","Data");Directory.CreateDirectory(oddFolder);byte[] oddName="\\settings\\private-tests.txt"u8.ToArray();byte[] oddId=new byte[16];oddId[0]=4;
byte[] oddDigest=SecurePak.Write(Path.Combine(oddFolder,"release.pak"),Side.Server,0,new byte[32],oddId,[entry with { Id=Legacy.Id(oddName),Name=oddName }],keys);
ReleaseCatalog.Write(Path.Combine(oddFolder,ReleaseCatalog.FileName),Side.Server,[new(0,"release.pak",oddDigest)],signer);
Check(BitConverter.ToUInt32(File.ReadAllBytes(Path.Combine(oddFolder,"release.pak")),64)%2==1,"odd-sized manifest regression fixture");
void Variant(string folder,List<PakInput> baseline,List<PakInput> updates,byte seed)
{
    string d=Path.Combine(root,folder,"Data");Directory.CreateDirectory(d);byte[] firstId=new byte[16],nextId=new byte[16];firstId[0]=seed;nextId[0]=(byte)(seed+1);
    byte[] first=SecurePak.Write(Path.Combine(d,"release.pak"),Side.Server,0,new byte[32],firstId,baseline,keys);
    byte[] next=SecurePak.Write(Path.Combine(d,"update_0000000001.pak"),Side.Server,1,first,nextId,updates,keys);
    ReleaseCatalog.Write(Path.Combine(d,ReleaseCatalog.FileName),Side.Server,[new(0,"release.pak",first),new(1,"update_0000000001.pak",next)],signer);
}
var other=new PakInput(id,"\\settings\\collision-b.txt"u8.ToArray(),collisionSource,SHA256.HashData("second-path"u8),11,false,true);
Variant("alias-removal-runtime",[entry,entry with {Id=id^0x10203040}], [removed],5);
Variant("winner-change-runtime",[entry,other],[entry with {PathOnly=true},other with {PathOnly=false},removed,other with {Source=null,Hash=new byte[32],Length=0,Deleted=true}],7);
Variant("deleted-collision-runtime",[entry with {PathOnly=true},other with {PathOnly=false}],[removed with {PathOnly=true}],9);
PipelineTests.Run(root, Check);
LocalAiPackagingTests.Run(root,Check);
Console.WriteLine($"ALL_PASS checks={checks}; fixture={root}");
