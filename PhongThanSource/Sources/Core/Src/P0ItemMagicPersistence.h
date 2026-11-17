// P0 isolated Loose persistence experiment. Canonical role/item/wire ABI is unchanged.
#ifndef PT_P0_ITEM_MAGIC_PERSISTENCE_H
#define PT_P0_ITEM_MAGIC_PERSISTENCE_H
#include "NativeDropRolloutScope.h"
#include "../../../Headers/PhongThanMagicRetention.h"
#include "../../../Headers/PhongThanCharacterBundle.h"
#include "../../../Headers/PhongThanAttributeSchema.h"
#include "BlueMagicTableLoader.h" // SHA-256 primitive only; no magic-table loading.
#include "../../../Headers/PhongThanCharacter.h"
#include "../../../Headers/PhongThanAccountStorage.h"
#include "../../../Headers/PhongThanSetCatalogIdentity.h"
namespace P0ItemMagic {
#pragma pack(push,1)
struct Entry {DWORD ordinal,serial;int quality;KItemNormalAttrib magic[MAX_ITEM_MAGICATTRIB],base[MAX_ITEM_BASEATTRIB],require[6];};
struct Header {DWORD magic,version,entrySize,count;char stateHash[64],payloadHash[64];};
#pragma pack(pop)
inline int Root(std::string& root,std::string& error){
    char env[MAX_PATH],full[MAX_PATH];DWORD n=GetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",env,sizeof(env));if(!n)return NativeDropRollout::Root(root,error);
    if(n>=sizeof(env)||!GetFullPathNameA(env,sizeof(full),full,0)){error="P0_INVALID_ROOT";return -1;}
    root=full;for(size_t i=0;i<root.size();++i)if(root[i]=='/')root[i]='\\';
    std::string lower=root;for(i=0;i<lower.size();++i)if(lower[i]>='A'&&lower[i]<='Z')lower[i]+='a'-'A';
    if(lower.find("\\phongthanruntime-loose\\p0-bluemagic-isolated\\")==std::string::npos||GetFileAttributesA((root+"\\p0_isolated.flag").c_str())==(DWORD)-1){error="P0_ISOLATED_LOOSE_ROOT_REQUIRED";return -1;}return 1;
}
inline int AccountRoot(const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& root,std::string& error,bool create){
    int active=Root(root,error);if(active!=1)return active;
    char isolated[MAX_PATH];if(GetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",isolated,sizeof(isolated)))return 1;
    std::string directory;
    if(!PtAccountDirectory("CharacterStore",(const char*)state->AccountName,directory,create)){error="P0_ACCOUNT_DIRECTORY_INVALID";return -1;}
    root=directory+"\\NativeDropPersistenceV1";
    // Live exact item data lives in the canonical .pthc; create no sidecar folder.
    return 1;
}
inline bool StateHash(const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::string& key,std::string& error){
    DWORD size=PhongThanCharacterExpectedStateSize(state);if(!size||size>PHONGTHAN_CHARACTER_MAX_STATE_SIZE){error="P0_INVALID_STATE_SIZE";return false;}
    std::string bytes((const char*)state,size);((PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0])->StateSize=size;
    if(!PhongThanValidateCharacterState((const PHONGTHAN_CHARACTER_STATE_HEADER*)bytes.c_str(),size)||!BlueMagic::Sha256(bytes,key)){error="P0_STATE_HASH_FAILED";return false;}return true;
}
inline bool ReadFile(const std::string& path,std::string& bytes){
    bytes="";HANDLE f=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
    DWORD size=GetFileSize(f,0),got=0;bool ok=size!=INVALID_FILE_SIZE&&size<=1024*1024;if(ok){bytes.resize(size);ok=::ReadFile(f,size?&bytes[0]:0,size,&got,0)&&got==size;}CloseHandle(f);return ok;
}
inline bool ImmutableWrite(const std::string& path,const std::string& bytes,std::string& error){
    if(GetFileAttributesA(path.c_str())!=(DWORD)-1){std::string previous;if(ReadFile(path,previous)&&previous==bytes)return true;error="P0_IMMUTABLE_SNAPSHOT_CONFLICT";return false;}
    char suffix[64];sprintf(suffix,".tmp.%lu.%lu",GetCurrentProcessId(),GetCurrentThreadId());std::string temp=path+suffix;
    HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE){error="P0_TEMP_CREATE_FAILED";return false;}
    DWORD written=0;bool ok=WriteFile(f,bytes.c_str(),bytes.size(),&written,0)&&written==bytes.size()&&FlushFileBuffers(f);CloseHandle(f);
    if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_WRITE_THROUGH)!=0;
    if(!ok){DeleteFileA(temp.c_str());std::string previous;if(ReadFile(path,previous)&&previous==bytes)return true;error="P0_SNAPSHOT_COMMIT_FAILED";}return ok;
}
inline std::string RoleKey(const PHONGTHAN_CHARACTER_STATE_HEADER* s){std::string names((const char*)s->RoleName);names+='\0';names+=(const char*)s->AccountName;for(size_t i=0;i<names.size();++i)if(names[i]>='A'&&names[i]<='Z')names[i]+='a'-'A';std::string key;BlueMagic::Sha256(names,key);return key;}
inline std::map<std::string,bool>& BlockedRoles(){static std::map<std::string,bool> roles;return roles;}
inline void Block(const PHONGTHAN_CHARACTER_STATE_HEADER* s){BlockedRoles()[RoleKey(s)]=true;}
template<class List> class LoadGuard {
    List& list;const std::vector<Entry>& entries;const PHONGTHAN_CHARACTER_STATE_HEADER* state;bool active;
public:
    LoadGuard(List& l,const std::vector<Entry>& e,const PHONGTHAN_CHARACTER_STATE_HEADER* s,bool enabled):list(l),entries(e),state(s),active(enabled){}
    void Commit(){if(active)BlockedRoles().erase(RoleKey(state));active=false;}
    ~LoadGuard(){if(active){Block(state);for(size_t i=0;i<entries.size();++i){int index=ItemSet.SearchID(entries[i].serial);if(index>0){list.Remove(index);ItemSet.Remove(index);}}}}
};
inline void Capture(KItem& item,DWORD ordinal,Entry& e){
    ZeroMemory(&e,sizeof(e));e.ordinal=ordinal;e.serial=item.GetID();e.quality=item.GetQuality();memcpy(e.magic,item.m_aryMagicAttrib,sizeof(e.magic));memcpy(e.base,item.P0BaseAttributes(),sizeof(e.base));for(int i=0;i<6;++i)e.require[i]=*(KItemNormalAttrib*)item.GetRequirement(i);
}
inline bool Save(const PHONGTHAN_CHARACTER_STATE_HEADER* state,const std::vector<Entry>& entries,std::string& error){
    std::string root;int enabled=AccountRoot(state,root,error,true);if(enabled==0)return true;if(enabled<0)return false;
    if(BlockedRoles()[RoleKey(state)]){error="P0_SAVE_BLOCKED_AFTER_FAILED_ITEM_LOAD";return false;}
    std::string key;if(!StateHash(state,key,error)||entries.size()!=state->ItemCount){error="P0_STATE_ENTRY_COUNT_MISMATCH";return false;}
    std::map<DWORD,bool> ids;for(size_t i=0;i<entries.size();++i){if(entries[i].ordinal!=i||!entries[i].serial||entries[i].serial>=0xfffffffeUL||ids[entries[i].serial]){error="P0_INVALID_OR_DUPLICATE_SERIAL";return false;}ids[entries[i].serial]=true;}
    std::string payload;if(!entries.empty())payload.assign((const char*)&entries[0],entries.size()*sizeof(Entry));Header h;ZeroMemory(&h,sizeof(h));h.magic=0x474d3050;h.version=2;h.entrySize=sizeof(Entry);h.count=entries.size();memcpy(h.stateHash,key.c_str(),64);std::string digest;
    if(!BlueMagic::Sha256(payload,digest)){error="P0_HASH_FAILED";return false;}memcpy(h.payloadHash,digest.c_str(),64);
    std::string bytes((const char*)&h,sizeof(h));bytes+=payload;
    std::string normalized((const char*)state,PhongThanCharacterExpectedStateSize(state));
    ((PHONGTHAN_CHARACTER_STATE_HEADER*)&normalized[0])->StateSize=normalized.size();
    char isolated[MAX_PATH];if(!GetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",isolated,sizeof(isolated)))return PtCharacterBundle::Stage("CharacterStore",(const PHONGTHAN_CHARACTER_STATE_HEADER*)normalized.c_str(),bytes,error);
    if(!PtMagicRetention::Begin(root,(const PHONGTHAN_CHARACTER_STATE_HEADER*)normalized.c_str(),error))return false;
    // Both files are immutable and durable BEFORE the canonical state can be
    // submitted. Failed main save leaves an unreachable, harmless orphan.
    if(!ImmutableWrite(root+"\\"+key+".magic-v1",bytes,error))return false;
    if(!BlueMagic::Sha256(bytes,digest)){error="P0_HASH_FAILED";return false;}
    if(!ImmutableWrite(root+"\\"+key+".required",digest,error))return false;
    const std::string managed=root+"\\role-"+RoleKey(state)+".managed-v1";std::string first;
    if(GetFileAttributesA(managed.c_str())==(DWORD)-1){std::string legacy,legacyError;if(Root(legacy,legacyError)==1&&legacy!=root){std::string old;
        if(ReadFile(legacy+"\\role-"+RoleKey(state)+".managed-v1",old)){if(old.size()!=4||!*(const DWORD*)old.c_str()||state->Revision<*(const DWORD*)old.c_str()){error="P0_MANAGED_ROLE_REVISION_INVALID";return false;}if(!ImmutableWrite(managed,old,error))return false;}}}
    if(GetFileAttributesA(managed.c_str())!=(DWORD)-1){if(!ReadFile(managed,first)||first.size()!=sizeof(DWORD)||*(const DWORD*)first.c_str()==0||state->Revision<*(const DWORD*)first.c_str()){error="P0_MANAGED_ROLE_REVISION_INVALID";return false;}return true;}
    DWORD revision=state->Revision;if(!revision){error="P0_ZERO_REVISION";return false;}return ImmutableWrite(managed,std::string((const char*)&revision,sizeof(revision)),error);
}
inline bool Load(const PHONGTHAN_CHARACTER_STATE_HEADER* state,std::vector<Entry>& entries,bool& present,std::string& error){
    entries.clear();present=false;std::string root;int enabled=AccountRoot(state,root,error,false);if(enabled==0)return true;if(enabled<0)return false;
    std::string key;if(!StateHash(state,key,error))return false;
    // Legacy snapshots stay readable during migration. If either new file exists,
    // validate that pair and fail closed rather than hiding a partial write.
    if(GetFileAttributesA((root+"\\"+key+".magic-v1").c_str())==(DWORD)-1&&GetFileAttributesA((root+"\\"+key+".required").c_str())==(DWORD)-1){
        std::string legacy,legacyError;if(Root(legacy,legacyError)==1&&legacy!=root){
            if(GetFileAttributesA((legacy+"\\"+key+".magic-v1").c_str())!=(DWORD)-1||GetFileAttributesA((legacy+"\\"+key+".required").c_str())!=(DWORD)-1)root=legacy;
            else if(GetFileAttributesA((root+"\\role-"+RoleKey(state)+".managed-v1").c_str())==(DWORD)-1&&GetFileAttributesA((legacy+"\\role-"+RoleKey(state)+".managed-v1").c_str())!=(DWORD)-1)root=legacy;
        }
    }
    std::string embedded;char isolated[MAX_PATH];bool bundled=false;
    if(!GetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",isolated,sizeof(isolated))){
        std::string path;if(!PtCharacterBundle::Path("CharacterStore",state,path))return false;
        PtCharacterBundle::Bundle b;
        if(GetFileAttributesA(path.c_str())!=(DWORD)-1){
            if(!PtCharacterBundle::Read(path,b)){error="PTHC_CANONICAL_INVALID";return false;}
            if(b.unified){if(!PtCharacterBundle::Lookup(b,state,embedded)){error="PTHC_STATE_ITEMS_MISMATCH";return false;}bundled=true;if(embedded.empty())return true;}
        }
    }
    std::string file=root+"\\"+key+".magic-v1",marker=root+"\\"+key+".required";
    if(!bundled&&GetFileAttributesA(file.c_str())==(DWORD)-1&&GetFileAttributesA(marker.c_str())==(DWORD)-1){
        std::string managed=root+"\\role-"+RoleKey(state)+".managed-v1",first;
        if(GetFileAttributesA(managed.c_str())==(DWORD)-1)return true;
        if(!ReadFile(managed,first)||first.size()!=sizeof(DWORD)||*(const DWORD*)first.c_str()==0||state->Revision>=*(const DWORD*)first.c_str()){error="P0_MANAGED_ROLE_REQUIRED_SNAPSHOT_MISSING";return false;}
        return true; // Older committed legacy revision predates first attempted P0 save.
    }
    present=true;std::string bytes,required,digest;if((bundled?(bytes=embedded,false):(!ReadFile(file,bytes)||!ReadFile(marker,required)||!BlueMagic::Sha256(bytes,digest)||required!=digest))||bytes.size()<sizeof(Header)){error="P0_REQUIRED_MAGIC_SNAPSHOT_MISSING_OR_CORRUPT";return false;}
    Header h;memcpy(&h,bytes.c_str(),sizeof(h));std::string payload=bytes.substr(sizeof(h));
    if(h.magic!=0x474d3050||(h.version!=1&&h.version!=2)||h.entrySize!=sizeof(Entry)||h.count!=state->ItemCount||h.count>PHONGTHAN_CHARACTER_MAX_ITEMS||payload.size()!=h.count*sizeof(Entry)||memcmp(h.stateHash,key.c_str(),64)||!BlueMagic::Sha256(payload,digest)||memcmp(h.payloadHash,digest.c_str(),64)){error="P0_MAGIC_SNAPSHOT_SCHEMA_OR_BINDING_INVALID";return false;}
    entries.resize(h.count);if(h.count)memcpy(&entries[0],payload.c_str(),payload.size());std::map<DWORD,bool> ids;
    if(h.version==1)for(size_t row=0;row<entries.size();++row){
        Entry& e=entries[row];
        for(int a=0;a<MAX_ITEM_MAGICATTRIB;++a){int id=PhongThanAttributeFromSnapshotV1(e.magic[a].nAttribType);if(id<0){error="P0_INVALID_V1_MAGIC_TYPE";return false;}e.magic[a].nAttribType=id;}
        for(a=0;a<MAX_ITEM_BASEATTRIB;++a){if(e.base[a].nAttribType==-1)continue;int id=PhongThanAttributeFromSnapshotV1(e.base[a].nAttribType);if(id<0){error="P0_INVALID_V1_BASE_TYPE";return false;}e.base[a].nAttribType=id;}
        for(a=0;a<6;++a){if(e.require[a].nAttribType==-1)continue;int id=PhongThanAttributeFromSnapshotV1(e.require[a].nAttribType);if(id<0){error="P0_INVALID_V1_REQUIRE_TYPE";return false;}e.require[a].nAttribType=id;}
    }

    for(size_t i=0;i<entries.size();++i){const Entry& e=entries[i];if(e.ordinal!=i||!e.serial||e.serial>=0xfffffffeUL||ids[e.serial]){error="P0_DUPLICATE_OR_INVALID_SERIAL";return false;}ids[e.serial]=true;
        for(int a=0;a<MAX_ITEM_MAGICATTRIB;++a)if(e.magic[a].nAttribType<0||e.magic[a].nAttribType>=magic_normal_end){error="P0_INVALID_ATTRIBUTE_TYPE";return false;}}
    return true;
}
// A bound, hashed snapshot owns the actual rolled values. A regenerated RNG
// value is not an identity check: legitimate saved rolls can differ. Accept
// only authored random intervals, keeping fixed/unknown slots fail-closed.
inline bool ValidSavedBaseRoll(KItem& item,const Entry& e){
    if(item.GetGenre()!=item_equip)return false;
    const KBASICPROP_EQUIPMENT* data=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());
    if(!data||data->m_nDetailType!=item.GetDetailType()||data->m_nParticularType!=item.GetParticularType()||data->m_nLevel!=item.m_CommonAttrib.nLevel)return false;
    KItem generated=item,saved=item;saved.SetBaseAttrib(e.base);
    if(item.GetUpgradeState()&&(!generated.ApplyUpgradeState(0)||!saved.ApplyUpgradeState(0)))return false;
    for(int slot=0;slot<MAX_ITEM_BASEATTRIB;++slot){
        const KItemNormalAttrib& a=generated.P0BaseAttributes()[slot];
        const KItemNormalAttrib& b=saved.P0BaseAttributes()[slot];
        if(!memcmp(&a,&b,sizeof(a)))continue;
        const KEQCP_BASIC& range=data->m_aryPropBasic[slot];
        if(range.nType<=0||range.nType!=a.nAttribType||b.nAttribType!=a.nAttribType||
           range.sRange.nMax<=range.sRange.nMin||b.nValue[0]<range.sRange.nMin||b.nValue[0]>range.sRange.nMax||
           b.nValue[1]!=a.nValue[1]||b.nValue[2]!=a.nValue[2])return false;
    }
    return true;
}
// Family 310 was incorrectly name-matched to sets 55..59 by an older build.
// Accept only that exact historical payload, after base/requirements passed.
// Keep the original item/serial, remove only those phantom set activation slots.
inline bool NormalizeLegacyWhiteFamily(KItem& item,const Entry& e,KItemNormalAttrib* normalized){
 if(item.GetGenre()!=item_equip||item.GetParticular()!=310||e.quality!=equip_set||item.GetSetID()!=0)return false;
 const KBASICPROP_EQUIPMENT* source=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());
 if(!source||source->m_nSetId||source->m_nParticularType!=310||source->m_nLevel!=item.GetLevel())return false;
 for(int s=0;s<MAX_ITEM_MAGICATTRIB;++s)if(item.m_aryMagicAttrib[s].nAttribType)return false;
 int effect=0;
 for(int n=0;n<sizeof(ptSetCatalog)/sizeof(ptSetCatalog[0]);++n){const PhongThanSetCatalogIdentity& v=ptSetCatalog[n];
  if(v.detail==item.GetDetailType()&&v.row==item.GetRow()&&v.part==310&&v.level==item.GetLevel()){effect=v.effect;break;}}
 if(effect<55||effect>59)return false;
 KEQCP_VNG_FIXED effects[MAX_VNG_SET_ATTRIB];int count=ItemGen.Catalog().GetVngSetAttrib(effect,effects,MAX_VNG_SET_ATTRIB);
 if(count<=0||count>MAX_VNG_SET_ATTRIB||count+MAX_ITEM_NORMAL_MAGICATTRIB>MAX_ITEM_MAGICATTRIB)return false;
 KItemNormalAttrib expected[MAX_ITEM_MAGICATTRIB];ZeroMemory(expected,sizeof(expected));
 for(int at=0;at<count;++at){KItemNormalAttrib& dst=expected[MAX_ITEM_NORMAL_MAGICATTRIB+at];dst.nAttribType=effects[at].nType;memcpy(dst.nValue,effects[at].nValue,sizeof(dst.nValue));}
 if(memcmp(expected,e.magic,sizeof(expected)))return false;
 ZeroMemory(normalized,sizeof(expected));return true;
}
inline bool Restore(KItem& item,const Entry& e,std::string& error){
    if(memcmp(item.P0BaseAttributes(),e.base,sizeof(e.base))&&!ValidSavedBaseRoll(item,e)){error="P0_RECONSTRUCTED_BASE_MISMATCH";return false;}
    for(int i=0;i<6;++i)if(memcmp(item.GetRequirement(i),&e.require[i],sizeof(e.require[i]))){error="P0_RECONSTRUCTED_REQUIREMENT_MISMATCH";return false;}
    // The public setter sets durability=-1 for indestructible. Preserve the
    // canonical durability already restored by LoadPlayerItemList.
    int durability=item.GetDurability();
    KItemNormalAttrib normalized[MAX_ITEM_MAGICATTRIB];bool legacyWhite=NormalizeLegacyWhiteFamily(item,e,normalized);
    if(!item.SetMagicAttrib(legacyWhite?normalized:e.magic)){error="P0_NATIVE_MAGIC_RESTORE_MISMATCH";return false;}
    const int actualQuality=item.GetQuality();bool migrated=legacyWhite;
    if(actualQuality!=e.quality){
        const KBASICPROP_EQUIPMENT* source=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());
        if(source&&item.GetGenre()==item_equip){
            if(item.GetDetailType()==equip_signet&&e.quality==equip_normal&&actualQuality==equip_magic){migrated=true;for(int a=0;a<MAX_ITEM_MAGICATTRIB;++a)if(e.magic[a].nAttribType)migrated=false;}
            if(source->m_nSetId==0&&item.GetSetID()>0&&actualQuality==equip_set&&(e.quality==equip_normal||e.quality==equip_magic))migrated=true;
        }
        if(!migrated){error="P0_NATIVE_MAGIC_RESTORE_MISMATCH";return false;}
    }
    item.SetBaseAttrib(e.base);item.SetDurability(durability);item.SetID(e.serial);return true;
}
}
#endif
