// Included only by KFile.cpp. VC6-compatible CNG adapter; no handwritten cipher.
// Windows CNG verifies GCM into a private scratch buffer. Callers see bytes only
// after BCryptDecrypt succeeds. Each open stream retains at most one chunk.
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#ifdef PT_RELEASE_TEST_KEYS
#include "PtReleaseKeys.fixture.h"
#else
#include "PtReleaseKeys.generated.h"
#endif

namespace PtSecure {
typedef unsigned __int64 U64;
typedef std::vector<unsigned char> Bytes;
static void Wipe(void* p, size_t n) { volatile unsigned char* v=(volatile unsigned char*)p; while(n--) *v++=0; }
static void Wipe(Bytes& b) { if(!b.empty()) Wipe(&b[0],b.size()); b.clear(); }
struct Secret : Bytes { ~Secret(){ Wipe(*this); } };
static DWORD U32(const unsigned char* p) { DWORD v; memcpy(&v,p,4); return v; }
static U64 U64At(const unsigned char* p) { U64 v; memcpy(&v,p,8); return v; }
static void Put32(unsigned char* p,DWORD v) { memcpy(p,&v,4); }

#pragma pack(push,8)
struct AuthInfo {
 ULONG size,version; unsigned char* nonce; ULONG nonceSize;
 unsigned char* aad; ULONG aadSize; unsigned char* tag; ULONG tagSize;
 unsigned char* mac; ULONG macSize,aadCount; U64 dataCount; ULONG flags;
};
#pragma pack(pop)
struct Crypto {
 HMODULE dll;
 typedef LONG (WINAPI *OpenT)(void**,LPCWSTR,LPCWSTR,ULONG);
 typedef LONG (WINAPI *CloseT)(void*,ULONG);
 typedef LONG (WINAPI *GetT)(void*,LPCWSTR,unsigned char*,ULONG,ULONG*,ULONG);
 typedef LONG (WINAPI *SetT)(void*,LPCWSTR,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *CreateHashT)(void*,void**,unsigned char*,ULONG,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *HashDataT)(void*,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *FinishT)(void*,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *DestroyT)(void*);
 typedef LONG (WINAPI *SymT)(void*,void**,unsigned char*,ULONG,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *DecryptT)(void*,unsigned char*,ULONG,void*,unsigned char*,ULONG,unsigned char*,ULONG,ULONG*,ULONG);
 typedef LONG (WINAPI *ImportT)(void*,void*,LPCWSTR,void**,unsigned char*,ULONG,ULONG);
 typedef LONG (WINAPI *VerifyT)(void*,void*,unsigned char*,ULONG,unsigned char*,ULONG,ULONG);
 OpenT open;CloseT close;GetT get;SetT set;CreateHashT createHash;HashDataT hashData;FinishT finish;DestroyT destroyHash,destroyKey;SymT sym;DecryptT decrypt;ImportT importKey;VerifyT verify;
 Crypto():dll(NULL){}
 bool Init(){
  if(dll)return true;
  char system[MAX_PATH];UINT n=GetSystemDirectoryA(system,sizeof(system));if(!n||n+12>=sizeof(system))return false;strcat(system,"\\bcrypt.dll");dll=LoadLibraryA(system);if(!dll)return false;
#define PT_CNG(member,type,name) member=(type)GetProcAddress(dll,name);if(!member)return false;
  PT_CNG(open,OpenT,"BCryptOpenAlgorithmProvider") PT_CNG(close,CloseT,"BCryptCloseAlgorithmProvider")
  PT_CNG(get,GetT,"BCryptGetProperty") PT_CNG(set,SetT,"BCryptSetProperty")
  PT_CNG(createHash,CreateHashT,"BCryptCreateHash") PT_CNG(hashData,HashDataT,"BCryptHashData") PT_CNG(finish,FinishT,"BCryptFinishHash")
  PT_CNG(destroyHash,DestroyT,"BCryptDestroyHash") PT_CNG(destroyKey,DestroyT,"BCryptDestroyKey")
  PT_CNG(sym,SymT,"BCryptGenerateSymmetricKey") PT_CNG(decrypt,DecryptT,"BCryptDecrypt")
  PT_CNG(importKey,ImportT,"BCryptImportKeyPair") PT_CNG(verify,VerifyT,"BCryptVerifySignature")
#undef PT_CNG
  return true;
 }
 bool Hash(const unsigned char* data,ULONG size,unsigned char result[32],const unsigned char* secret=NULL,ULONG secretSize=0){
  void* alg=NULL;void* hash=NULL;DWORD objectSize=0,got=0;Secret object;
  if(open(&alg,L"SHA256",NULL,secret?8:0)<0)return false;
  bool ok=get(alg,L"ObjectLength",(unsigned char*)&objectSize,4,&got,0)>=0&&objectSize<65536;
  if(ok){object.resize(objectSize);ok=createHash(alg,&hash,&object[0],objectSize,(unsigned char*)secret,secretSize,0)>=0;}
  if(ok)ok=hashData(hash,(unsigned char*)data,size,0)>=0&&finish(hash,result,32,0)>=0;
  if(hash)destroyHash(hash);close(alg,0);return ok;
 }
 bool Verify(const Bytes& signedBytes,const unsigned char signature[64],unsigned char digest[32]){
  void* alg=NULL;void* key=NULL;if(!Hash(&signedBytes[0],signedBytes.size(),digest))return false;
  if(open(&alg,L"ECDSA_P256",NULL,0)<0)return false;
  bool ok=importKey(alg,NULL,L"ECCPUBLICBLOB",&key,(unsigned char*)ptReleasePublic,sizeof(ptReleasePublic),0)>=0;
  if(ok)ok=verify(key,NULL,digest,32,(unsigned char*)signature,64,0)>=0;
  if(key)destroyKey(key);close(alg,0);return ok;
 }
 bool Decrypt(const unsigned char keyBytes[32],const unsigned char header[72],DWORD counter,const unsigned char* opaque,const Bytes& cipher,const unsigned char tag[16],Secret& result){
  void* alg=NULL;void* key=NULL;DWORD objectSize=0,got=0,written=0;Secret object;unsigned char nonce[12]={0},aad[92];Put32(nonce+8,counter);memcpy(aad,header,72);Put32(aad+72,counter);if(opaque)memcpy(aad+76,opaque,16);
  if(open(&alg,L"AES",NULL,0)<0)return false;
  bool ok=set(alg,L"ChainingMode",(unsigned char*)L"ChainingModeGCM",sizeof(L"ChainingModeGCM"),0)>=0&&get(alg,L"ObjectLength",(unsigned char*)&objectSize,4,&got,0)>=0&&objectSize<65536;
  if(ok){object.resize(objectSize);ok=sym(alg,&key,&object[0],objectSize,(unsigned char*)keyBytes,32,0)>=0;}
  AuthInfo info;memset(&info,0,sizeof(info));info.size=sizeof(info);info.version=1;info.nonce=nonce;info.nonceSize=12;info.aad=aad;info.aadSize=opaque?92:76;info.tag=(unsigned char*)tag;info.tagSize=16;
  result.resize(cipher.size());
  if(ok){LONG status=decrypt(key,(unsigned char*)&cipher[0],cipher.size(),&info,NULL,0,&result[0],result.size(),&written,0);
#ifdef PT_RELEASE_READER_DIAGNOSTICS
   if(status<0)fprintf(stderr,"CNG_DECRYPT_STATUS=%08lX counter=%lu stored=%u\n",status,counter,cipher.size());
#endif
   ok=status>=0&&written==cipher.size();}
  if(key)destroyKey(key);close(alg,0);if(!ok)Wipe(result);return ok;
 }
};
struct Chunk { U64 offset;DWORD stored,plain,counter,compression;unsigned char hash[32]; };
struct Pack {
 FILE* file;unsigned char header[72],key[32],digest[32];U64 start,size;DWORD sequence,side;
 Pack():file(NULL) {memset(key,0,32);}
 ~Pack(){if(file)fclose(file);Wipe(key,32);}
};
struct Entry {Pack* pack;DWORD id,length,ordinal;unsigned char opaque[16];std::string name;bool deleted,pathOnly;std::vector<Chunk> chunks;};
struct IdentityLess {
 bool operator()(const Entry* a,const Entry* b)const {
  if(a->id!=b->id)return a->id<b->id;if(a->pathOnly!=b->pathOnly)return a->pathOnly<b->pathOnly;
  unsigned n=a->name.size()<b->name.size()?a->name.size():b->name.size();
  for(unsigned i=0;i<n;++i){unsigned char x=a->name[i],y=b->name[i];if(x>='A'&&x<='Z')x+='a'-'A';if(y>='A'&&y<='Z')y+='a'-'A';if(x!=y)return x<y;}
  return a->name.size()<b->name.size();
 }
};
struct Stream {Entry* entry;DWORD position;int cached;Secret plain;bool error,eof;Stream():entry(NULL),position(0),cached(-1),error(false),eof(false){}};
static LONG lockState=0;static CRITICAL_SECTION mutex;
static bool FirstLock(){
#if _MSC_VER <= 1200
 return InterlockedCompareExchange((PVOID*)&lockState,(PVOID)1,NULL)==NULL;
#else
 return InterlockedCompareExchange(&lockState,1,0)==0;
#endif
}
struct Guard {Guard(){if(FirstLock()){InitializeCriticalSection(&mutex);InterlockedExchange(&lockState,2);}while(lockState!=2)Sleep(0);EnterCriticalSection(&mutex);}~Guard(){LeaveCriticalSection(&mutex);}};
struct State {
 Crypto crypto;std::vector<Pack*> packs;std::vector<Entry*> allEntries;std::map<DWORD,Entry*> effective;std::map<std::string,Entry*> named;std::map<std::string,bool> deletedNames;std::map<FILE*,Stream*> streams;int mounted;std::string root;
 State():mounted(0){}
 ~State(){for(std::map<FILE*,Stream*>::iterator s=streams.begin();s!=streams.end();++s)delete s->second;for(unsigned e=0;e<allEntries.size();++e)delete allEntries[e];for(unsigned i=0;i<packs.size();++i)delete packs[i];}
};
static State& Data();
static std::string Lower(const std::string& value);
static std::string ContextRoot();
static std::string ExeRoot(){char path[MAX_PATH];DWORD n=GetModuleFileNameA(NULL,path,sizeof(path));if(!n||n>=sizeof(path))return "";char* slash=strrchr(path,'\\');if(!slash)return "";*slash=0;return path;}
static bool Exists(const std::string& p){DWORD a=GetFileAttributesA(p.c_str());return a!=(DWORD)-1&&!(a&FILE_ATTRIBUTE_DIRECTORY);}
static bool Required(){
#ifdef PT_SECURE_RELEASE_PROFILE
 return true;
#else
 std::string root=ContextRoot();return !Exists(root+"\\loose_only.flag")&&Exists(root+"\\Data\\release.pak");
#endif
}
// One mount namespace per data root. The default context root is the exe
// folder (the client chain, exactly the historical behavior). An embedded
// (offline) process additionally mounts the server chain from its
// ServerData root: the host switches the context root while server code
// runs, and every lookup then resolves against that root's catalog.
// States are keyed by lower-cased root path and mount lazily on first use.
static std::string s_ptContextRoot;
static std::map<std::string,State*>& PtStates(){static std::map<std::string,State*> states;return states;}
static std::string ContextRoot(){return s_ptContextRoot.empty()?ExeRoot():s_ptContextRoot;}
static void SetContextRoot(const char* root){s_ptContextRoot=root?root:"";}
static State& Data(){
 std::map<std::string,State*>& states=PtStates();std::string key=Lower(ContextRoot());
 std::map<std::string,State*>::iterator it=states.find(key);
 if(it!=states.end())return *it->second;
 State* created=new State;states[key]=created;return *created;
}
// Archive I/O consistently uses the CRT descriptor. VC6 stdio may retain a
// single look-ahead byte even with _IONBF; mixing fread with _lseeki64 shifts
// odd-sized reads after random seeks and invalidates otherwise valid GCM tags.
static bool Read(FILE* f,void* p,unsigned n){unsigned done=0;while(done<n){int got=_read(_fileno(f),(unsigned char*)p+done,n-done);if(got<=0)return false;done+=got;}return true;}
struct Reader {
 const Secret& bytes;unsigned pos;bool ok;Reader(const Secret& b):bytes(b),pos(0),ok(true){}
 bool Take(void* p,unsigned n){if(!ok||pos>bytes.size()||n>bytes.size()-pos){ok=false;return false;}if(n)memcpy(p,&bytes[pos],n);pos+=n;return true;}
 DWORD Dword(){DWORD v=0;Take(&v,4);return v;}U64 Qword(){U64 v=0;Take(&v,8);return v;}
};
static bool NameValid(const std::string& name){
 if(name.size()<2||name.size()>=260||name[0]!='\\'||name.find('\0')!=std::string::npos)return false;
 std::string part;for(unsigned i=1;i<=name.size();++i){char c=i==name.size()?'\\':name[i];if(c=='\\'){if(part.empty()||part=="."||part==".."||part[part.size()-1]=='.'||part[part.size()-1]==' ')return false;part="";}else{if((unsigned char)c<32||strchr(":/*?\"<>|",c))return false;part+=c;}}return true;
}
static bool LoadPack(Pack* p,std::vector<Entry*>& entries){
 State& s=Data();if(!Read(p->file,p->header,72)||memcmp(p->header,"PTSEC002",8)||U32(p->header+68)!=2)return false;
 p->side=U32(p->header+8);p->sequence=U32(p->header+12);DWORD size=U32(p->header+64);if(p->side>1||size<4||size>128*1024*1024)return false;
 Bytes cipher(size);unsigned char tag[16],signature[64];if(!Read(p->file,&cipher[0],size)||!Read(p->file,tag,16)||!Read(p->file,signature,64))return false;
 p->start=72+(U64)size+16+64;p->size=_filelengthi64(_fileno(p->file));if(p->size<p->start)return false;
 Bytes signedBytes(72+size+16);memcpy(&signedBytes[0],p->header,72);memcpy(&signedBytes[72],&cipher[0],size);memcpy(&signedBytes[72+size],tag,16);
 if(!s.crypto.Verify(signedBytes,signature,p->digest)||!s.crypto.Hash(p->header+16,16,p->key,ptReleaseMaster,32))return false;
 Secret plain;if(!s.crypto.Decrypt(p->key,p->header,0,NULL,cipher,tag,plain))return false;
 Reader r(plain);DWORD count=r.Dword();if(count>400000)return false;
 std::map<DWORD,bool> ids,counters;U64 expected=0;
 for(DWORD i=0;i<count;++i){
  Entry* e=new Entry;e->pack=p;e->id=r.Dword();r.Take(e->opaque,16);DWORD flags=r.Dword();unsigned short len=0;r.Take(&len,2);
  if(!r.ok||!e->id||(flags==0&&ids[e->id])||flags>3||len<2||len>=260){delete e;return false;}if(flags==0)ids[e->id]=true;
  char name[260]={0};r.Take(name,len);e->name.assign(name,len);e->length=r.Dword();unsigned char hash[32];r.Take(hash,32);DWORD chunks=r.Dword();e->deleted=(flags&1)!=0;e->pathOnly=(flags&2)!=0;
  if(!r.ok||!NameValid(e->name)||e->length>0x7fffffff||chunks>8192||(e->deleted&&(e->length||chunks))){delete e;return false;}
  U64 total=0;
  for(DWORD j=0;j<chunks;++j){Chunk c;c.offset=r.Qword();c.stored=r.Dword();c.plain=r.Dword();c.counter=r.Dword();c.compression=r.Dword();r.Take(c.hash,32);
   if(!r.ok||c.offset!=expected||!c.stored||c.stored>256*1024+1024||!c.plain||c.plain>256*1024||!c.counter||counters[c.counter]||c.compression>1||(!c.compression&&c.stored!=c.plain)){delete e;return false;}
   counters[c.counter]=true;expected=c.offset+c.stored+16;if(expected>p->size-p->start){delete e;return false;}total+=c.plain;e->chunks.push_back(c);
  }
  if(total!=e->length){delete e;return false;}entries.push_back(e);
 }
 return r.ok&&r.pos==plain.size()&&expected==p->size-p->start;
}
static bool PackOrder(const Pack* a,const Pack* b){return a->sequence<b->sequence;}
static std::string Lower(const std::string& value){std::string name=value;for(unsigned i=0;i<name.size();++i)if(name[i]>='A'&&name[i]<='Z')name[i]+='a'-'A';return name;}
static bool Mount(){
 State& s=Data();if(s.mounted)return s.mounted==1;s.mounted=-1;s.root=ContextRoot();
 if(!PT_SECURE_KEYS_READY||s.root.empty()||!s.crypto.Init())return false;
 // The signed catalog pins every required Base and Patch, including the tail.
 // Removing the newest file must never silently roll the game back.
 FILE* catalog=fopen((s.root+"\\Data\\release.catalog").c_str(),"rb");if(!catalog)return false;
 __int64 catalogSize=_filelengthi64(_fileno(catalog));if(catalogSize<80||catalogSize>32768){fclose(catalog);return false;}
 Bytes signedCatalog((unsigned)catalogSize-64);unsigned char catalogSignature[64],catalogDigest[32];bool catalogRead=Read(catalog,&signedCatalog[0],signedCatalog.size())&&Read(catalog,catalogSignature,64);fclose(catalog);
 if(!catalogRead||!s.crypto.Verify(signedCatalog,catalogSignature,catalogDigest))return false;
 Secret catalogBody;catalogBody.assign(signedCatalog.begin(),signedCatalog.end());Reader catalogReader(catalogBody);
 char magic[8];catalogReader.Take(magic,8);DWORD catalogRole=catalogReader.Dword(),packageCount=catalogReader.Dword();
 if(memcmp(magic,"PTCAT002",8)||catalogRole>1||!packageCount||packageCount>256)return false;
 std::vector<std::string> files;std::vector<Bytes> expected;
 for(DWORD c=0;c<packageCount;++c){DWORD seq=catalogReader.Dword();unsigned short len=0;catalogReader.Take(&len,2);if(seq!=c||len<5||len>64)return false;char name[65]={0};catalogReader.Take(name,len);Bytes digest(32);catalogReader.Take(&digest[0],32);
  if(!catalogReader.ok||strstr(name,"..")||strcmp(name+len-4,".pak"))return false;
  for(unsigned q=0;q<len;++q)if(!((name[q]>='a'&&name[q]<='z')||(name[q]>='0'&&name[q]<='9')||name[q]=='_'||name[q]=='-'||name[q]=='.'))return false;
  files.push_back(s.root+"\\Data\\"+name);expected.push_back(digest);
 }
 if(!catalogReader.ok||catalogReader.pos!=catalogBody.size())return false;
 std::vector<Entry*> entries;bool ok=true;
 for(unsigned i=0;i<files.size();++i){Pack* p=new Pack;p->file=fopen(files[i].c_str(),"rb");if(p->file)setvbuf(p->file,NULL,_IONBF,0);s.packs.push_back(p);if(!p->file||!LoadPack(p,entries)||p->side!=catalogRole||memcmp(p->digest,&expected[i][0],32)){
#ifdef PT_RELEASE_READER_DIAGNOSTICS
 fprintf(stderr,"PACK_REJECTED index=%u path=%s\n",i,files[i].c_str());
#endif
 ok=false;break;}}
 unsigned char parent[32]={0};DWORD role=s.packs.empty()?0:s.packs[0]->side;
 for(unsigned n=0;ok&&n<s.packs.size();++n){Pack* p=s.packs[n];if(p->sequence!=n||p->side!=role||memcmp(p->header+32,parent,32)){ok=false;break;}memcpy(parent,p->digest,32);}
 if(role==0&&!Exists(s.root+"\\Data\\serverlist.pak"))ok=false;
 if(ok){
  std::map<Entry*,Entry*,IdentityLess> active;
  // Catalog order is already validated; apply entries once, avoiding an O(PAKs
  // * resources) search as the number of incremental patches grows.
  for(unsigned k=0;k<entries.size();++k){Entry* e=entries[k];if(!e)continue;
   if(e->deleted){active.erase(e);s.deletedNames[Lower(e->name)]=true;}else active[e]=e;
   e->ordinal=s.allEntries.size();s.allEntries.push_back(e);entries[k]=NULL;
  }
  // Resolve logical identity first. Removing one native-ID alias must not
  // remove a still-live filename, and changing an ID winner must not resurrect
  // a deleted path through a colliding legacy hash.
  for(std::map<Entry*,Entry*,IdentityLess>::iterator live=active.begin();live!=active.end();++live){Entry* e=live->second;
   if(!e->pathOnly){if(s.effective.find(e->id)!=s.effective.end()){ok=false;break;}s.effective[e->id]=e;}
   s.named[Lower(e->name)]=e;s.deletedNames.erase(Lower(e->name));
  }
 }
 for(unsigned z=0;z<entries.size();++z)if(entries[z])delete entries[z];
 if(ok){
  std::vector<Entry*> kept;
  for(unsigned old=0;old<s.allEntries.size();++old){Entry* e=s.allEntries[old];
   std::map<DWORD,Entry*>::iterator id=s.effective.find(e->id);std::map<std::string,Entry*>::iterator name=s.named.find(Lower(e->name));
   bool needed=(id!=s.effective.end()&&id->second==e)||(name!=s.named.end()&&name->second==e);
   if(needed){e->ordinal=kept.size();kept.push_back(e);}else delete e;
  }
  s.allEntries.swap(kept);
 }
 if(!ok){
#ifdef PT_RELEASE_READER_DIAGNOSTICS
 fprintf(stderr,"MOUNT_REJECTED_CHAIN packs=%u\n",s.packs.size());
#endif
 return false;}s.mounted=1;return true;
}
static DWORD HashName(const std::string& path){DWORD hash=0;int n=0;for(unsigned i=0;i<path.size();++i){signed char c=path[i];if(c>='A'&&c<='Z')c+='a'-'A';hash=(hash+(++n)*c)%0x8000000b*0xffffffef;}return hash^0x12345678;}
static bool Logical(const char* input,std::string& name){
 if(!input||!*input)return false;name=input;for(unsigned i=0;i<name.size();++i)if(name[i]=='/')name[i]='\\';
 std::string root=ContextRoot();if(name.size()>root.size()&&!_strnicmp(name.c_str(),root.c_str(),root.size())&&name[root.size()]=='\\')name=name.substr(root.size()+1);
 while(name.size()>2&&name.substr(0,2)==".\\")name=name.substr(2);if(name.empty())return false;if(name[0]!='\\')name="\\"+name;return NameValid(name);
}
static bool ResourceName(const std::string& logical){
 const char* roots[]={"\\settings\\","\\script\\","\\maps\\","\\ui\\","\\spr\\","\\sound\\","\\music\\","\\font\\","\\_loose_id\\","\\datascript\\","\\you_xi_zi_yuan\\","\\\xd3\xce\xcf\xb7\xd7\xca\xd4\xb4\\"};
 for(unsigned i=0;i<sizeof(roots)/sizeof(roots[0]);++i)if(!_strnicmp(logical.c_str(),roots[i],strlen(roots[i])))return true;return !_stricmp(logical.c_str(),"\\serverlist.ini");
}
static bool ReaderFailure(int code){
#ifdef PT_RELEASE_READER_DIAGNOSTICS
 fprintf(stderr,"SECURE_READER_FAILURE=%d\n",code);
#endif
 return false;
}
static bool ChunkLoad(Stream& stream,unsigned index){
 if(stream.cached==(int)index)return true;Wipe(stream.plain);stream.cached=-1;
 Entry& e=*stream.entry;Chunk& c=e.chunks[index];Pack& p=*e.pack;Bytes cipher(c.stored);unsigned char tag[16];
#ifdef PT_RELEASE_READER_DIAGNOSTICS
 fprintf(stderr,"CHUNK id=%08lX sequence=%lu start=%I64u offset=%I64u stored=%lu counter=%lu opaque=",e.id,p.sequence,p.start,c.offset,c.stored,c.counter);for(unsigned q=0;q<16;++q)fprintf(stderr,"%02X",e.opaque[q]);fprintf(stderr,"\n");
#endif
 if(_lseeki64(_fileno(p.file),p.start+c.offset,SEEK_SET)<0||!Read(p.file,&cipher[0],c.stored)||!Read(p.file,tag,16))return ReaderFailure(1);
#ifdef PT_RELEASE_READER_DIAGNOSTICS
 fprintf(stderr,"CIPHER=");for(unsigned a=0;a<8&&a<cipher.size();++a)fprintf(stderr,"%02X",cipher[a]);fprintf(stderr," TAG=");for(unsigned b=0;b<16;++b)fprintf(stderr,"%02X",tag[b]);fprintf(stderr,"\n");
#endif
 Secret verified;if(!Data().crypto.Decrypt(p.key,p.header,c.counter,e.opaque,cipher,tag,verified))return ReaderFailure(2);
 if(c.compression){
  // The bundled legacy zlib.dll exports the ZLIB_WINAPI (__stdcall) ABI.
  // Treating it as cdecl corrupts the optimized VC6 caller's stack.
  typedef int (WINAPI *Uncompress)(unsigned char*,unsigned long*,const unsigned char*,unsigned long);
  static HMODULE zlib=NULL;static Uncompress unzip=NULL;if(!zlib){zlib=LoadLibraryA((Data().root+"\\zlib.dll").c_str());if(zlib)unzip=(Uncompress)GetProcAddress(zlib,"uncompress");}
  if(!unzip)return ReaderFailure(3);stream.plain.resize(c.plain);unsigned long length=c.plain;if(unzip(&stream.plain[0],&length,&verified[0],verified.size())!=0||length!=c.plain){Wipe(stream.plain);return ReaderFailure(4);}
 }else stream.plain.swap(verified);
 unsigned char digest[32];if(!Data().crypto.Hash(&stream.plain[0],stream.plain.size(),digest)||memcmp(digest,c.hash,32)){Wipe(stream.plain);return ReaderFailure(5);}
 stream.cached=index;return true;
}
static unsigned ReadStream(Stream& s,void* buffer,unsigned length){
 if(s.error||!buffer)return 0;unsigned done=0;Entry& e=*s.entry;if(length>e.length-s.position)length=e.length-s.position;
 for(unsigned i=0,base=0;i<e.chunks.size()&&done<length;++i){Chunk& c=e.chunks[i];if(s.position<base+c.plain){
   if(!ChunkLoad(s,i)){s.error=true;return 0;}unsigned offset=s.position-base,n=c.plain-offset;if(n>length-done)n=length-done;memcpy((unsigned char*)buffer+done,&s.plain[offset],n);done+=n;s.position+=n;
  }base+=c.plain;
 }
 return done;
}
static bool Diagnostic(const char* path){
 std::string lower=path?path:"";for(unsigned i=0;i<lower.size();++i)if(lower[i]>='A'&&lower[i]<='Z')lower[i]+='a'-'A';
 return lower.find(".log")!=std::string::npos||lower.find(".jsonl")!=std::string::npos||lower.find("_diag")!=std::string::npos||lower.find("_trace")!=std::string::npos||lower.find("studio_live_world")!=std::string::npos;
}
}

bool PtReleaseEnabled(){return PtSecure::Required();}
bool PtReleaseMount(){PtSecure::Guard guard;return PtSecure::Required()&&PtSecure::Mount();}
void PtReleaseSetContextRoot(const char* root){PtSecure::SetContextRoot(root);}
unsigned long PtReleaseNextName(unsigned long cursor,const char* prefix,char* name,unsigned long capacity){
 PtSecure::Guard guard;if(!prefix||!name||!capacity||!PtSecure::Required()||!PtSecure::Mount())return 0;
 PtSecure::State& state=PtSecure::Data();unsigned length=strlen(prefix);
 for(unsigned i=cursor;i<state.allEntries.size();++i){PtSecure::Entry* e=state.allEntries[i];if(e->deleted||e->name.size()+1>capacity||_strnicmp(e->name.c_str(),prefix,length))continue;
  std::map<std::string,PtSecure::Entry*>::iterator winner=state.named.find(PtSecure::Lower(e->name));if(winner==state.named.end()||winner->second!=e)continue;
  strcpy(name,e->name.c_str());return i+1;
 }return 0;
}
bool PtReleaseLocate(unsigned long id,const char* name,unsigned long* entry,unsigned long* length){
 PtSecure::Guard guard;if(!PtSecure::Required()||!PtSecure::Mount())return false;PtSecure::Entry* found=NULL;
 if(name){std::string logical;if(!PtSecure::Logical(name,logical))return false;std::string lower=PtSecure::Lower(logical);std::map<std::string,PtSecure::Entry*>::iterator named=PtSecure::Data().named.find(lower);if(named!=PtSecure::Data().named.end())found=named->second;else {if(PtSecure::Data().deletedNames.find(lower)!=PtSecure::Data().deletedNames.end())return false;id=PtSecure::HashName(logical);}}
 if(!found){std::map<DWORD,PtSecure::Entry*>::iterator byId=PtSecure::Data().effective.find(id);if(byId!=PtSecure::Data().effective.end())found=byId->second;}
 if(!found||found->deleted)return false;if(entry)*entry=found->ordinal;if(length)*length=found->length;return true;
}
unsigned long PtReleaseReadEntry(unsigned long entry,unsigned long offset,void* buffer,unsigned long length){PtSecure::Guard guard;if(!PtSecure::Required()||!PtSecure::Mount()||entry>=PtSecure::Data().allEntries.size())return 0;PtSecure::Entry* e=PtSecure::Data().allEntries[entry];if(e->deleted||offset>e->length)return 0;PtSecure::Stream stream;stream.entry=e;stream.position=offset;return PtSecure::ReadStream(stream,buffer,length);}
bool PtReleaseFind(unsigned long id,unsigned long* length){PtSecure::Guard guard;if(!PtSecure::Required()||!PtSecure::Mount())return false;std::map<DWORD,PtSecure::Entry*>::iterator e=PtSecure::Data().effective.find(id);if(e==PtSecure::Data().effective.end())return false;if(length)*length=e->second->length;return true;}
unsigned long PtReleaseReadId(unsigned long id,unsigned long offset,void* buffer,unsigned long length){
 PtSecure::Guard guard;DWORD size;if(!PtReleaseFind(id,&size)||offset>size)return 0;PtSecure::Stream s;s.entry=PtSecure::Data().effective[id];s.position=offset;return PtSecure::ReadStream(s,buffer,length);
}
FILE* PtReleaseFopen(const char* path,const char* mode){
 FILE* stream=NULL;if(PtReleaseOpenVirtual(path,mode,&stream))return stream;if(!PtReleaseEnabled())return PtDiagnosticsFopen(path,mode);return fopen(PtReleaseQuietPath(path)?"NUL":path,mode);
}
bool PtReleaseQuietPath(const char* path){return PtReleaseEnabled()&&PtSecure::Diagnostic(path);}
bool PtReleaseIsVirtual(FILE* stream){PtSecure::Guard guard;return PtSecure::Data().streams.find(stream)!=PtSecure::Data().streams.end();}
bool PtReleaseOpenVirtual(const char* path,const char* mode,FILE** stream){
 if(stream)*stream=NULL;if(!PtReleaseEnabled()||PtSecure::Diagnostic(path))return false;if(!path||!mode)return true;
 std::string logical;bool valid=PtSecure::Logical(path,logical);bool reading=mode[0]=='r'&&!strchr(mode,'+');
 if(valid&&reading){PtSecure::Guard guard;if(!PtSecure::Mount())return true;
  PtSecure::Entry* entry=NULL;std::map<std::string,PtSecure::Entry*>::iterator named=PtSecure::Data().named.find(PtSecure::Lower(logical));
  if(named!=PtSecure::Data().named.end()){if(named->second->deleted)return true;entry=named->second;}
  else {if(PtSecure::Data().deletedNames.find(PtSecure::Lower(logical))!=PtSecure::Data().deletedNames.end())return true;std::map<DWORD,PtSecure::Entry*>::iterator e=PtSecure::Data().effective.find(PtSecure::HashName(logical));if(e!=PtSecure::Data().effective.end())entry=e->second;}
  if(entry){
   PtSecure::Stream* s=new PtSecure::Stream;s->entry=entry;FILE* token=(FILE*)s;PtSecure::Data().streams[token]=s;if(stream)*stream=token;return true;
  }
 }
 return valid&&PtSecure::ResourceName(logical); // No loose fallback or decrypted temp files.
}
FILE* PtReleaseFreopen(const char* path,const char* mode,FILE* stream){return freopen(PtReleaseEnabled()&&PtSecure::Diagnostic(path)?"NUL":path,mode,stream);}
size_t PtReleaseFread(void* buffer,size_t size,size_t count,FILE* file){
 PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);if(it==PtSecure::Data().streams.end())return fread(buffer,size,count,file);
 if(!size||!count||count>0xffffffff/size)return 0;PtSecure::Stream& s=*it->second;unsigned want=size*count;if(want>s.entry->length-s.position)s.eof=true;return PtSecure::ReadStream(s,buffer,want)/size;
}
int PtReleaseFclose(FILE* file){PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);if(it==PtSecure::Data().streams.end())return fclose(file);delete it->second;PtSecure::Data().streams.erase(it);return 0;}
int PtReleaseFseek(FILE* file,long offset,int origin){PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);if(it==PtSecure::Data().streams.end())return fseek(file,offset,origin);PtSecure::Stream& s=*it->second;__int64 pos=(origin==SEEK_SET?0:origin==SEEK_CUR?s.position:origin==SEEK_END?s.entry->length:-0x7fffffffI64)+offset;if(pos<0||pos>s.entry->length)return -1;s.position=(DWORD)pos;s.eof=false;return 0;}
long PtReleaseFtell(FILE* file){PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);return it==PtSecure::Data().streams.end()?ftell(file):it->second->position;}
char* PtReleaseFgets(char* line,int count,FILE* file){
 PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);if(it==PtSecure::Data().streams.end())return fgets(line,count,file);if(!line||count<2)return NULL;
 int n=0;while(n<count-1){unsigned char c;if(!PtSecure::ReadStream(*it->second,&c,1)){it->second->eof=true;break;}line[n++]=c;if(c=='\n')break;}line[n]=0;return n?line:NULL;
}
int PtReleaseFgetc(FILE* file){unsigned char c;return PtReleaseFread(&c,1,1,file)==1?c:EOF;}
int PtReleaseFeof(FILE* file){PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);return it==PtSecure::Data().streams.end()?feof(file):it->second->eof;}
int PtReleaseFerror(FILE* file){PtSecure::Guard guard;std::map<FILE*,PtSecure::Stream*>::iterator it=PtSecure::Data().streams.find(file);return it==PtSecure::Data().streams.end()?ferror(file):it->second->error;}
