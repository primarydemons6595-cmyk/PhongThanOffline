#include "../../../Headers/PhongThanTimedItems.h"
#include "../../../Headers/PhongThanBuffDuration.h"
// Narrow reconstructed PT consumable contract. Source values are loaded from
// explicit item descriptors, not the inherited IB positional-column reader.
#pragma once
#ifdef _SERVER
#include "PhongThanConsumableData.h"
#include "PhongThanWorldProtocol.h"
#include <time.h>
#include <stddef.h>
#include "../../../Headers/PhongThanAccountStorage.h"
struct PTConsumableBuff {DWORD end;int id[2],value[2];};
struct PTConsumablePool {int item;DWORD id,last;int rate;};
struct PTConsumableState {DWORD owner,entity,retry;char account[32],role[32];int loaded,needsApply;PTConsumableBuff buff[PT_CONSUMABLE_SLOTS];PTConsumablePool pool[2];DWORD autoDose[2];};
static PTConsumableState ptConsumableStates[MAX_PLAYER];
int PhongThanConsumableRecoveryKind(int genre,int detail,int part,const char* script){
 if(genre!=item_ibitem||!script||strcmp(script,"NONE")||!PTConsumableLoad())return 0;
 for(int i=0;i<ptConsumableCount;++i){PTConsumableDef& d=ptConsumableDefs[i];if(d.kind<=2&&d.detail==detail&&d.part==part)return d.kind;}
 return 0;
}
static BOOL PTConsumablePlayerValid(KPlayer& p)
{
    int index=(int)(&p-Player);
    return index>0 && index<MAX_PLAYER && p.m_nIndex>0 && p.m_nIndex<MAX_NPC;
}
static BOOL PTConsumableInBag(KPlayer& p,int item)
{
    if(item<=0 || item>=MAX_ITEM) return FALSE;
    int index=p.m_ItemList.FindSame(item);
    if(index<=0 || index>=MAX_PLAYER_ITEM) return FALSE;
    int place=p.m_ItemList.m_Items[index].nPlace;
    return place==pos_equiproom || place==pos_equiproomex || place==pos_ptbag2 || place==pos_ptbag3 || place==pos_immediacy;
}
// Versioned, atomic sidecar in the existing persistent CharacterStore junction.
// This reconstructed storage does not allocate quest tasks or combat skill IDs.
struct PTConsumableFile {DWORD magic,version,saved;char account[32],role[32];DWORD end[PT_CONSUMABLE_SLOTS],checksum;};
static DWORD PTConsumableUtc(){return (DWORD)time(NULL);}
static DWORD PTConsumableHash(const void* data,int size)
{
    const BYTE* b=(const BYTE*)data;DWORD hash=2166136261UL;
    for(int i=0;i<size;++i) hash=(hash^b[i])*16777619UL;
    return hash;
}
static BOOL PTConsumableFileIdentity(KPlayer& p,PTConsumableFile& file,char* path)
{
    if(!p.AccountName[0] || !p.Name[0] || !memchr(p.AccountName,0,32) || !memchr(p.Name,0,32)) return FALSE;
    ZeroMemory(&file,sizeof(file));file.magic=0x42544350UL;file.version=2;
    strcpy(file.account,p.AccountName);strcpy(file.role,p.Name);
    std::string directory,name;if(!PtAccountDirectory("CharacterStore",p.AccountName,directory)||!PtRoleFileName(p.Name,name))return FALSE;
    if(directory.size()+name.size()+7>=MAX_PATH)return FALSE;
    sprintf(path,"%s\\%s.ptcb",directory.c_str(),name.c_str());
    return TRUE;
}
static BOOL PTConsumableRead(KPlayer& p,PTConsumableState& s)
{
    PTConsumableFile identity,file;char path[MAX_PATH];
    if(!PTConsumableLoad() || !PTConsumableFileIdentity(p,identity,path)) return FALSE;
    HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h==INVALID_HANDLE_VALUE && (GetLastError()==ERROR_FILE_NOT_FOUND || GetLastError()==ERROR_PATH_NOT_FOUND)) {
        char legacy[MAX_PATH];sprintf(legacy,"CharacterStore\\ConsumableBuffs\\%08lx.ptcb",PTConsumableHash(identity.account,64));
        h=CreateFileA(legacy,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    }
    if(h==INVALID_HANDLE_VALUE) {
        DWORD error=GetLastError(),root=GetFileAttributesA("CharacterStore");
        return (error==ERROR_FILE_NOT_FOUND || error==ERROR_PATH_NOT_FOUND) &&
            root!=(DWORD)-1 && (root&FILE_ATTRIBUTE_DIRECTORY);
    }
    struct LegacyFile {DWORD magic,version,saved;char account[32],role[32];DWORD end[28],checksum;};
    DWORD read=0,size=GetFileSize(h,NULL);BOOL ok=FALSE;
    ZeroMemory(&file,sizeof(file));
    if(size==sizeof(LegacyFile)) {
        LegacyFile legacy;
        ok=ReadFile(h,&legacy,sizeof(legacy),&read,NULL) && read==sizeof(legacy) &&
            legacy.version==1 && legacy.checksum==PTConsumableHash(&legacy,sizeof(legacy)-sizeof(DWORD));
        if(ok) {memcpy(&file,&legacy,offsetof(LegacyFile,end));memcpy(file.end,legacy.end,sizeof(legacy.end));file.version=2;file.checksum=PTConsumableHash(&file,sizeof(file)-sizeof(DWORD));}
    } else if(size==sizeof(file)) ok=ReadFile(h,&file,sizeof(file),&read,NULL) && read==sizeof(file);
    CloseHandle(h);
    DWORD now=PTConsumableUtc();
    if(!ok || file.magic!=identity.magic || file.version!=2 ||
       !memchr(file.account,0,32) || !memchr(file.role,0,32) ||
       _stricmp(file.account,identity.account)!=0 || _stricmp(file.role,identity.role)!=0 || file.saved>now ||
       file.checksum!=PTConsumableHash(&file,sizeof(file)-sizeof(DWORD))) return FALSE;
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) {
        if(!file.end[i]) continue;
        PTConsumableDef* def=NULL;
        for(int d=0;d<ptConsumableCount;++d)
            if(ptConsumableDefs[d].kind>=3 && PhongThanTimedItemSlot(ptConsumableDefs[d].detail)==i) {def=&ptConsumableDefs[d];break;}
        if(!def || (file.end[i]>file.saved && file.end[i]-file.saved>PT_BUFF_REMAINING_MAX_SECONDS)) return FALSE;
        if(def->kind==4 && def->clock==0)
            file.end[i]=file.end[i]>file.saved ? now+(file.end[i]-file.saved) : 0;
    }
    for(int j=0;j<PT_CONSUMABLE_SLOTS;++j) if(file.end[j]>now) {
        for(int d=0;d<ptConsumableCount;++d) if(ptConsumableDefs[d].kind>=3 && PhongThanTimedItemSlot(ptConsumableDefs[d].detail)==j) {
            PTConsumableBuff& b=s.buff[j];b.end=file.end[j];
            for(int a=0;a<2;++a){b.id[a]=(ptConsumableDefs[d].kind==3||ptConsumableDefs[d].kind==5) ? ptConsumableDefs[d].id[a] : 0;b.value[a]=ptConsumableDefs[d].value[a];}
            s.needsApply=1;break;
        }
    }
    return TRUE;
}
static BOOL PTConsumableWrite(KPlayer& p,const PTConsumableState& s)
{
    PTConsumableFile file;char path[MAX_PATH],temporary[MAX_PATH];
    if(!PTConsumableFileIdentity(p,file,path)) return FALSE;
    // Root belongs to the server deployment; do not silently create another store.
    DWORD root=GetFileAttributesA("CharacterStore");
    if(root==(DWORD)-1 || !(root&FILE_ATTRIBUTE_DIRECTORY)) return FALSE;
    std::string directory;if(!PtAccountDirectory("CharacterStore",p.AccountName,directory,true))return FALSE;
    file.saved=PTConsumableUtc();
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) if(s.buff[i].end>file.saved) file.end[i]=s.buff[i].end;
    file.checksum=PTConsumableHash(&file,sizeof(file)-sizeof(DWORD));
    sprintf(temporary,"%s.%08lx.tmp",path,GetCurrentProcessId());
    HANDLE h=CreateFileA(temporary,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(h==INVALID_HANDLE_VALUE) return FALSE;
    DWORD written=0;BOOL ok=WriteFile(h,&file,sizeof(file),&written,NULL) && written==sizeof(file) && FlushFileBuffers(h);
    CloseHandle(h);
    if(ok) ok=MoveFileExA(temporary,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
    if(!ok) DeleteFileA(temporary);
    return ok;
}
static PTConsumableState& PTConsumableStateFor(KPlayer& p)
{
    int index=(int)(&p-Player);
    PTConsumableState& s=ptConsumableStates[index];
    DWORD entity=p.m_nIndex>0&&p.m_nIndex<MAX_NPC?Npc[p.m_nIndex].m_dwID:0;
    if(s.owner!=p.m_dwID || s.entity!=entity || _stricmp(s.account,p.AccountName) || _stricmp(s.role,p.Name)) {
        ZeroMemory(&s,sizeof(s));s.owner=p.m_dwID;s.entity=entity;
        strncpy(s.account,p.AccountName,31);strncpy(s.role,p.Name,31);
    }
    if(s.loaded!=1 && p.Name[0] && p.AccountName[0] && (!s.retry || GetTickCount()-s.retry>=5000)) {
        // A temporary read failure must not permanently poison this login.
        PTConsumableState loaded=s;BOOL ok=PTConsumableRead(p,loaded);s.retry=GetTickCount();
        if(ok){s=loaded;s.loaded=1;}else{s.loaded=-1;FILE* log=fopen("pt_consumable_trace.log","a");if(log){fprintf(log,"event=BUFF_LOAD_FAILED role=%.31s\n",p.Name);fclose(log);}}
    }
    return s;
}
BOOL PhongThanConsumableChatColor(KPlayer& p) {
    if(!PTConsumablePlayerValid(p))return FALSE;
    PTConsumableState& s=PTConsumableStateFor(p);
    return s.loaded==1 && (s.buff[PhongThanTimedItemSlot(233)].end>PTConsumableUtc() || s.buff[PhongThanTimedItemSlot(390)].end>PTConsumableUtc());
}
void PhongThanConsumableSnapshot(KPlayer& p,void* packet)
{
    PHONGTHAN_SELF_CONSUMABLES* out=(PHONGTHAN_SELF_CONSUMABLES*)packet;
    out->ConsumableVersion=2;
    PTConsumableState& s=PTConsumableStateFor(p);DWORD utc=PTConsumableUtc();
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) out->RemainingSeconds[i]=s.buff[i].end>utc ? s.buff[i].end-utc : 0;
}
static BOOL PTConsumableCharge(KPlayer& p,int item,int amount)
{
    if(amount<=0 || !PTConsumableInBag(p,item)) return FALSE;
    int left=Item[item].GetStackNum();
    if(left<amount) return FALSE;
    // IB UseCount is charges/capacity, including non-stackable Dao powders.
    if(left==amount) return p.m_ItemList.RemoveItem(item,0);
    Item[item].SetStackNum(left-amount);p.m_ItemList.SyncItem(item);return TRUE;
}
static void PTConsumableLog(const char* event,KPlayer& p,int item,int value)
{
    FILE* f=fopen("pt_consumable_trace.log","a");
    if(f){fprintf(f,"tick=%lu utc=%lu event=%s player=%lu item=%d value=%d life=%d/%d mana=%d/%d\n",GetTickCount(),PTConsumableUtc(),event,p.m_dwID,item,value,Npc[p.m_nIndex].m_CurrentLife,Npc[p.m_nIndex].m_CurrentLifeMax,Npc[p.m_nIndex].m_CurrentMana,Npc[p.m_nIndex].m_CurrentManaMax);fclose(f);}
}
void PhongThanConsumableClear(KPlayer& p)
{
    int index=(int)(&p-Player);
    if(index>0 && index<MAX_PLAYER) {
        PTConsumableState& s=ptConsumableStates[index];
        if(s.loaded==1 && s.owner==p.m_dwID) PTConsumableWrite(p,s);
        ZeroMemory(&s,sizeof(s));
    }
}
void PhongThanConsumableApply(KPlayer& p)
{
    if(!PTConsumablePlayerValid(p)) return;
    PTConsumableState& s=PTConsumableStateFor(p);DWORD now=PTConsumableUtc();
    s.needsApply=0;
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) if(s.buff[i].end>now && PhongThanTimedItemDetail(i)!=133)
        for(int a=0;a<2;++a) if(s.buff[i].id[a]) {
            KMagicAttrib attr;ZeroMemory(&attr,sizeof(attr));
            attr.nAttribType=s.buff[i].id[a];attr.nValue[0]=s.buff[i].value[a];
            Npc[p.m_nIndex].ModifyAttrib(p.m_nIndex,&attr);
        }
    KNpc& npc=Npc[p.m_nIndex];
    // Mount-only percentage is applied once after base, equipment and other buffs.
    if(npc.m_bRideHorse && s.buff[PhongThanTimedItemSlot(133)].end>now){npc.m_CurrentRunSpeed*=1.2f;npc.m_CurrentWalkSpeed*=1.2f;}
    if(npc.m_CurrentLife>npc.m_CurrentLifeMax) npc.m_CurrentLife=npc.m_CurrentLifeMax;
    if(npc.m_CurrentMana>npc.m_CurrentManaMax) npc.m_CurrentMana=npc.m_CurrentManaMax;
}
BOOL PhongThanConsumableAutoDose(KPlayer& p,int item)
{
    if(!PTConsumablePlayerValid(p)||!PTConsumableInBag(p,item)||!PTConsumableLoad()||
       Item[item].GetGenre()!=item_ibitem||strcmp(Item[item].GetScript(),"NONE")||
       p.m_cTrade.m_nIsTrading) return FALSE;
    KNpc& npc=Npc[p.m_nIndex];
    if(npc.m_CurrentLife<=0||npc.m_Doing==do_death||npc.m_Doing==do_revive)return FALSE;
    PTConsumableDef* def=NULL;
    for(int i=0;i<ptConsumableCount;++i)if(ptConsumableDefs[i].kind<=2&&
        ptConsumableDefs[i].detail==Item[item].GetDetailType()&&
        ptConsumableDefs[i].part==Item[item].GetParticular()){def=&ptConsumableDefs[i];break;}
    if(!def)return FALSE;
    int kind=def->kind-1;PTConsumableState& s=PTConsumableStateFor(p);DWORD now=GetTickCount();
    // A manually enabled reservoir already supplies its own timed doses.
    if(s.pool[kind].item||now-s.autoDose[kind]<500)return FALSE;
    int current=kind?npc.m_CurrentMana:npc.m_CurrentLife;
    int maximum=kind?npc.m_CurrentManaMax:npc.m_CurrentLifeMax;
    if(current<0||maximum<=current)return FALSE;
    int amount=min(def->value[0],min(maximum-current,Item[item].GetStackNum()));
    if(amount<=0||!PTConsumableCharge(p,item,amount))return FALSE;
    s.autoDose[kind]=now;
    int effect=kind?npc.m_PtCombat.ManaPotionEffect:npc.m_PtCombat.LifePotionEffect;int gain=amount+(int)((__int64)amount*effect/100);if(kind)npc.m_CurrentMana=min(maximum,npc.m_CurrentMana+gain);else npc.m_CurrentLife=min(maximum,npc.m_CurrentLife+gain);
    PTConsumableLog(kind?"AUTO_MANA_DOSE":"AUTO_LIFE_DOSE",p,item,amount);return TRUE;
}
int PhongThanUseConsumable(KPlayer& p,int item)
{
    if(item<=0 || item>=MAX_ITEM) return 0;
    if(Item[item].GetGenre()!=item_ibitem) return -1;
    if(strcmp(Item[item].GetScript(),"NONE")!=0) return -1;
    if(!PTConsumablePlayerValid(p)) return 0;
    // Never fall through to the incompatible inherited IB reader on load failure.
    if(!PTConsumableLoad()) return 0;
    PTConsumableDef* def=NULL;
    for(int i=0;i<ptConsumableCount;++i)
        if(ptConsumableDefs[i].detail==Item[item].GetDetailType() &&
           ptConsumableDefs[i].part==Item[item].GetParticular()) {def=&ptConsumableDefs[i];break;}
    if(!def) {
        FILE* missing=fopen("pt_item_use_missing.log","a");
        if(missing){fprintf(missing,"genre=8 detail=%d part=%d script=NONE reason=NATIVE_HANDLER_MISSING\n",Item[item].GetDetailType(),Item[item].GetParticular());fclose(missing);}
        return 0; // Unknown native items must not be consumed by the JX IB reader.
    }
    if(!PTConsumableInBag(p,item) || Item[item].GetLock()->IsLock() || p.GetLockState() ||
       (Item[item].m_CommonAttrib.nExpireTime>0 && Item[item].m_CommonAttrib.nExpireTime<=(int)KSG_GetCurSec()) ||
       p.m_cTrade.m_nIsTrading || Npc[p.m_nIndex].m_CurrentLife<=0 ||
       Npc[p.m_nIndex].m_Doing==do_death || Npc[p.m_nIndex].m_Doing==do_revive) return 0;
    PTConsumableState& s=PTConsumableStateFor(p);
    DWORD now=GetTickCount();
    if(def->kind<=2) {
        PTConsumablePool& pool=s.pool[def->kind-1];
        if(pool.item==item && pool.id==Item[item].GetID()) {
            ZeroMemory(&pool,sizeof(pool));PTConsumableLog("POOL_OFF",p,item,0);return 1;
        }
        if(Item[item].GetStackNum()<=0) return 0;
        pool.item=item;pool.id=Item[item].GetID();pool.last=now;pool.rate=def->value[0];
        PTConsumableLog("POOL_ON",p,item,pool.rate);return 1;
    }
    if(s.loaded!=1 || Item[item].GetStackNum()<=0) return 0;
    int slot=PhongThanTimedItemSlot(def->detail);
    if(slot<0) return 0;
    PTConsumableBuff& b=s.buff[slot];
    PTConsumableBuff previous=b;
    if(!PTAddBuffDuration(PTConsumableUtc(),b.end,def->seconds,b.end))return 0;
    for(int a=0;a<2;++a){b.id[a]=(def->kind==3||def->kind==5) ? def->id[a] : 0;b.value[a]=def->value[a];}
    if(!PTConsumableWrite(p,s)) {b=previous;PTConsumableLog("BUFF_SAVE_FAIL",p,item,0);return 0;}
    if(!PTConsumableCharge(p,item,1)) {b=previous;PTConsumableWrite(p,s);return 0;}
    // Rebuild from base/equipment/state so reuse never accumulates stale deltas.
    p.UpdataCurData();PTConsumableLog("BUFF_ON",p,item,def->seconds);return 1;
}
bool PhongThanCancelConsumable(int player,int slot){
 if(player<=0||player>=MAX_PLAYER||slot<0||slot>=PT_CONSUMABLE_SLOTS||PhongThanTimedItemDetail(slot)<0)return false;
 KPlayer& p=Player[player];if(!PTConsumablePlayerValid(p))return false;
 PTConsumableState& state=PTConsumableStateFor(p);if(state.loaded!=1||!state.buff[slot].end)return false;
 PTConsumableBuff old=state.buff[slot];ZeroMemory(&state.buff[slot],sizeof(state.buff[slot]));
 if(!PTConsumableWrite(p,state)){state.buff[slot]=old;return false;}
 p.UpdataCurData();return true;
}
void PhongThanConsumableTick(KPlayer& p)
{
    if(!PTConsumablePlayerValid(p)) return;
    PTConsumableState& s=PTConsumableStateFor(p);DWORD now=GetTickCount(),utc=PTConsumableUtc();BOOL changed=s.needsApply!=0;
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) if(s.buff[i].end && utc>=s.buff[i].end) {
        ZeroMemory(&s.buff[i],sizeof(s.buff[i]));changed=TRUE;
        PTConsumableLog("BUFF_EXPIRE",p,0,i);
    }
    if(changed) p.UpdataCurData();
    KNpc& npc=Npc[p.m_nIndex];
    for(int k=0;k<2;++k) {
        PTConsumablePool& pool=s.pool[k];
        if(!pool.item || now-pool.last<500) continue;
        pool.last=now;int item=pool.item;
        if(!PTConsumableInBag(p,item) || Item[item].GetID()!=pool.id) {
            ZeroMemory(&pool,sizeof(pool));continue;
        }
        if(npc.m_CurrentLife<=0 || npc.m_Doing==do_death || npc.m_Doing==do_revive || p.m_cTrade.m_nIsTrading) continue;
        int current=k ? npc.m_CurrentMana : npc.m_CurrentLife;
        int maximum=k ? npc.m_CurrentManaMax : npc.m_CurrentLifeMax;
        if(current<0 || maximum<=current) continue;
        int amount=min(pool.rate,min(maximum-current,Item[item].GetStackNum()));
        if(amount<=0 || !PTConsumableCharge(p,item,amount)) {ZeroMemory(&pool,sizeof(pool));continue;}
        int effect=k?npc.m_PtCombat.ManaPotionEffect:npc.m_PtCombat.LifePotionEffect;int gain=amount+(int)((__int64)amount*effect/100);if(k)npc.m_CurrentMana=min(maximum,npc.m_CurrentMana+gain);else npc.m_CurrentLife=min(maximum,npc.m_CurrentLife+gain);
        // No per-frame log; only actual dose/charge events.
        PTConsumableLog(k ? "MANA_DOSE" : "LIFE_DOSE",p,item,amount);
    }
}
#endif
