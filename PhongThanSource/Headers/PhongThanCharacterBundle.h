// Disk format 2: canonical state, exact item rolls and bounded in-flight saves
// share one atomic .pthc. The character network schema remains unchanged.
#ifndef PHONGTHAN_CHARACTER_BUNDLE_H
#define PHONGTHAN_CHARACTER_BUNDLE_H
#include "PhongThanMagicRetention.h"
#include <map>
namespace PtCharacterBundle {
enum {MaxBytes=4*1024*1024,MaxPending=8,EntryBytes=748};
#pragma pack(push,1)
struct FileHeader {DWORD magic;WORD version,headerSize;DWORD stateSize,checksum;};
struct Tail {DWORD magic,version,committedSize,pendingCount,loginUtc,loginIp,checksum,reserved;};
struct PendingHeader {DWORD revision,size;};
#pragma pack(pop)
struct Pending {DWORD revision;std::string magic;};
struct Bundle {std::string state,magic;std::vector<Pending> pending;DWORD loginUtc,loginIp;bool unified;Bundle():loginUtc(0),loginIp(0),unified(false){}};
inline bool MagicValid(const std::string& bytes,const PHONGTHAN_CHARACTER_STATE_HEADER* state=0){
 if(bytes.size()<144)return false;const DWORD* h=(const DWORD*)bytes.data();
 if(h[0]!=0x474d3050||(h[1]!=1&&h[1]!=2)||h[2]!=EntryBytes||h[3]>PHONGTHAN_CHARACTER_MAX_ITEMS||bytes.size()!=144+EntryBytes*h[3]||!PtMagicRetention::Key(bytes.data()+16))return false;
 std::string hash;if(!PtMagicRetention::Sha256(bytes.substr(144),hash)||memcmp(hash.data(),bytes.data()+80,64))return false;
 if(state){if(h[3]!=state->ItemCount||!PtMagicRetention::StateKey(state,hash)||memcmp(hash.data(),bytes.data()+16,64))return false;}
 return true;
}
inline bool Decode(const std::string& bytes,Bundle& out){
 out=Bundle();if(bytes.size()<sizeof(FileHeader)+sizeof(PHONGTHAN_CHARACTER_STATE_HEADER)||bytes.size()>MaxBytes)return false;
 FileHeader h;memcpy(&h,bytes.data(),sizeof(h));
 if(h.magic!=0x43544850||(h.version!=1&&h.version!=2)||(h.headerSize!=16&&!(h.version==1&&h.headerSize==0))||h.stateSize>PHONGTHAN_CHARACTER_MAX_STATE_SIZE||h.stateSize>bytes.size()-16)return false;
 out.state=bytes.substr(16,h.stateSize);const PHONGTHAN_CHARACTER_STATE_HEADER* state=(const PHONGTHAN_CHARACTER_STATE_HEADER*)out.state.data();
 if(PtMagicRetention::Hash(out.state.data(),out.state.size())!=h.checksum||!PhongThanValidateCharacterState(state,h.stateSize))return false;
 if(h.version==1)return bytes.size()==16+h.stateSize;
 size_t start=16+h.stateSize;if(bytes.size()-start<sizeof(Tail))return false;Tail t;memcpy(&t,bytes.data()+start,sizeof(t));
 std::string tail=bytes.substr(start);((Tail*)&tail[0])->checksum=0;
 if(t.magic!=0x32425450||t.version!=1||t.reserved||t.pendingCount>MaxPending||t.checksum!=PtMagicRetention::Hash(tail.data(),tail.size()))return false;
 size_t at=start+sizeof(Tail);if(t.committedSize>bytes.size()-at)return false;
 out.magic=bytes.substr(at,t.committedSize);at+=t.committedSize;
 if(!out.magic.empty()&&!MagicValid(out.magic,state))return false;
 std::map<std::string,bool> keys;
 for(DWORD i=0;i<t.pendingCount;++i){if(bytes.size()-at<sizeof(PendingHeader))return false;PendingHeader p;memcpy(&p,bytes.data()+at,sizeof(p));at+=sizeof(p);
  if(!p.revision||p.revision<=state->Revision||p.size>bytes.size()-at)return false;Pending entry;entry.revision=p.revision;entry.magic=bytes.substr(at,p.size);at+=p.size;
  if(!MagicValid(entry.magic)||keys[entry.magic.substr(16,64)])return false;keys[entry.magic.substr(16,64)]=true;out.pending.push_back(entry);
 }
 if(at!=bytes.size())return false;out.loginUtc=t.loginUtc;out.loginIp=t.loginIp;out.unified=true;return true;
}
inline bool Read(const std::string& path,Bundle& b){std::string bytes;return PtMagicRetention::Read(path,bytes,MaxBytes)&&Decode(bytes,b);}
inline bool Encode(const Bundle& b,std::string& bytes){
 if(b.pending.size()>MaxPending)return false;FileHeader h;h.magic=0x43544850;h.version=2;h.headerSize=16;h.stateSize=b.state.size();h.checksum=PtMagicRetention::Hash(b.state.data(),b.state.size());
 bytes.assign((const char*)&h,sizeof(h));bytes+=b.state;Tail t;ZeroMemory(&t,sizeof(t));t.magic=0x32425450;t.version=1;t.committedSize=b.magic.size();t.pendingCount=b.pending.size();t.loginUtc=b.loginUtc;t.loginIp=b.loginIp;
 size_t start=bytes.size();bytes.append((const char*)&t,sizeof(t));bytes+=b.magic;
 for(size_t i=0;i<b.pending.size();++i){PendingHeader p;p.revision=b.pending[i].revision;p.size=b.pending[i].magic.size();bytes.append((const char*)&p,sizeof(p));bytes+=b.pending[i].magic;}
 ((Tail*)&bytes[start])->checksum=PtMagicRetention::Hash(bytes.data()+start,bytes.size()-start);Bundle check;return Decode(bytes,check);
}
inline bool Write(const std::string& path,const Bundle& b){
 std::string bytes;if(!Encode(b,bytes))return false;
 // Callers hold the role mutex. A fixed temporary name cannot accumulate one
 // abandoned file per PID after crashes; the next save overwrites it safely.
 std::string temp=path+".tmp";HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD written=0;bool ok=WriteFile(f,bytes.data(),bytes.size(),&written,0)&&written==bytes.size()&&FlushFileBuffers(f);CloseHandle(f);
 if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
inline bool Identity(const Bundle& b,const PHONGTHAN_CHARACTER_STATE_HEADER* state){const PHONGTHAN_CHARACTER_STATE_HEADER* old=(const PHONGTHAN_CHARACTER_STATE_HEADER*)b.state.data();return !_stricmp((const char*)old->RoleName,(const char*)state->RoleName)&&!_stricmp((const char*)old->AccountName,(const char*)state->AccountName);}
inline bool Path(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& path){
 std::string directory,name;if(!PtAccountDirectory(store,(const char*)state->AccountName,directory,false)||!PtRoleFileName((const char*)state->RoleName,name))return false;path=directory+"\\"+name;
 if(PtMagicRetention::Regular(path)||_strnicmp((const char*)state->AccountName,"bot",3))return true;
 // Real bot roles have their canonical file under BotAIgent/characters.
 static std::map<std::string,std::string> cached;std::string role;if(!PtMagicRetention::Role(state,role))return false;
 if(cached.find(role)!=cached.end()){path=cached[role];return true;}
 WIN32_FIND_DATAA found;HANDLE search=FindFirstFileA("BotAIgent\\characters\\*.pthc",&found);if(search==INVALID_HANDLE_VALUE)return true;
 bool matched=false;do{if(found.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))continue;
  std::string candidate=std::string("BotAIgent\\characters\\")+found.cFileName;Bundle b;if(Read(candidate,b)&&Identity(b,state)){if(matched){FindClose(search);return false;}path=candidate;matched=true;}
 }while(FindNextFileA(search,&found));FindClose(search);if(matched)cached[role]=path;return true;
}
inline bool LegacyMagic(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& magic){
 std::string dir,key;if(!PtAccountDirectory(store,(const char*)state->AccountName,dir,false)||!PtMagicRetention::StateKey(state,key))return false;
 const std::string roots[2]={dir+"\\NativeDropPersistenceV1","NativeDropPersistenceV1"};
 for(int i=0;i<2;++i){std::string required,digest;if(PtMagicRetention::Read(roots[i]+"\\"+key+".magic-v1",magic)&&PtMagicRetention::Read(roots[i]+"\\"+key+".required",required,64)&&PtMagicRetention::Sha256(magic,digest)&&required==digest&&MagicValid(magic,state))return true;}magic="";return false;
}
inline bool Lookup(const Bundle& b,const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& magic,bool* pending=0){
 if(pending)*pending=false;if(!Identity(b,state))return false;std::string key;if(!PtMagicRetention::StateKey(state,key))return false;
 if(b.state==std::string((const char*)state,state->StateSize)){magic=b.magic;return true;}
 for(size_t i=0;i<b.pending.size();++i)if(b.pending[i].revision==state->Revision&&!memcmp(b.pending[i].magic.data()+16,key.data(),64)){magic=b.pending[i].magic;if(pending)*pending=true;return true;}return false;
}
inline bool StagePath(const std::string& path,const PHONGTHAN_CHARACTER_STATE_HEADER* state,const std::string& magic,std::string& error){
 if(!MagicValid(magic,state)){error="PTHC_ITEM_PAYLOAD_INVALID";return false;}std::string role;if(!PtMagicRetention::Role(state,role))return false;PtMagicRetention::Guard guard(role);if(!guard.Good())return false;
 Bundle b;if(!Read(path,b)||!Identity(b,state)){error="PTHC_STAGE_CANONICAL_INVALID";return false;}const PHONGTHAN_CHARACTER_STATE_HEADER* old=(const PHONGTHAN_CHARACTER_STATE_HEADER*)b.state.data();
 if(!b.unified){LegacyMagic("CharacterStore",old,b.magic);std::string dir,key;PtMagicRetention::Role(old,key);PtAccountDirectory("CharacterStore",(const char*)old->AccountName,dir,false);
  if(b.magic.empty()&&(PtMagicRetention::Regular(dir+"\\NativeDropPersistenceV1\\role-"+key+".managed-v1")||PtMagicRetention::Regular("NativeDropPersistenceV1\\role-"+key+".managed-v1"))){error="PTHC_LEGACY_ITEMS_MISSING";return false;}}
 std::string existing;if(Lookup(b,state,existing)){if(existing==magic)return true;if(b.state==std::string((const char*)state,state->StateSize)&&existing.empty()){b.magic=magic;return Write(path,b);}error="PTHC_ITEM_PAYLOAD_CONFLICT";return false;}
 if(state->Revision<=old->Revision){error="PTHC_STALE_STAGE";return false;}
 // Evict only superseded in-flight captures. The committed state/items survive.
 if(b.pending.size()==MaxPending)b.pending.erase(b.pending.begin());Pending p;p.revision=state->Revision;p.magic=magic;b.pending.push_back(p);
 if(!Write(path,b)){error="PTHC_STAGE_WRITE_FAILED";return false;}return true;
}
inline bool Stage(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state,const std::string& magic,std::string& error){std::string path;return Path(store,state,path)&&StagePath(path,state,magic,error);}
inline bool PendingAccount(const char* store,const PHONGTHAN_CHARACTER_STATE_HEADER* state){std::string path,role;if(!state||!PtMagicRetention::StateKey(state,role)||!Path(store,state,path)||!PtMagicRetention::Role(state,role))return false;PtMagicRetention::Guard guard(role);Bundle b;std::string magic;bool pending=false;return guard.Good()&&Read(path,b)&&Lookup(b,state,magic,&pending)&&pending;}
inline bool CommitPath(const char* store,const std::string& path,const PHONGTHAN_CHARACTER_STATE_HEADER* state,bool replace,std::string& error){
 if(!PhongThanValidateCharacterState(state,state->StateSize))return false;std::string role;if(!PtMagicRetention::Role(state,role))return false;PtMagicRetention::Guard guard(role);if(!guard.Good())return false;
 Bundle old,next;bool exists=GetFileAttributesA(path.c_str())!=(DWORD)-1;if(exists&&!replace)return false;
 if(exists&&(!Read(path,old)||!Identity(old,state))){error="PTHC_COMMIT_CANONICAL_INVALID";return false;}
 if(exists&&state->Revision<((const PHONGTHAN_CHARACTER_STATE_HEADER*)old.state.data())->Revision){error="PTHC_STALE_COMMIT";return false;}
 bool found=exists&&Lookup(old,state,next.magic);if(!found||next.magic.empty())LegacyMagic(store,state,next.magic);
 if(exists&&next.magic.empty()&&!old.magic.empty()){error="PTHC_COMMIT_ITEMS_MISSING";return false;}
 if(next.magic.empty()&&!PtMagicRetention::BeforeCommitAccount(store,state,error))return false;
 next.state.assign((const char*)state,state->StateSize);next.loginUtc=old.loginUtc;next.loginIp=old.loginIp;
 for(size_t i=0;i<old.pending.size();++i)if(old.pending[i].revision>state->Revision)next.pending.push_back(old.pending[i]);
 if(!Write(path,next)){error="PTHC_COMMIT_WRITE_FAILED";return false;}return true;
}
inline bool Login(const std::string& path,const PHONGTHAN_CHARACTER_STATE_HEADER* state,DWORD utc,DWORD ip){std::string role;if(!PtMagicRetention::Role(state,role))return false;PtMagicRetention::Guard guard(role);Bundle b;if(!guard.Good()||!Read(path,b)||!Identity(b,state))return false;if(!b.unified){const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)b.state.data();LegacyMagic("CharacterStore",s,b.magic);std::string error;if(b.magic.empty()&&!PtMagicRetention::BeforeCommitAccount("CharacterStore",s,error))return false;}b.loginUtc=utc;b.loginIp=ip;return Write(path,b);}
}
#endif
