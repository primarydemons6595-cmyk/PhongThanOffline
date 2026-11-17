#ifdef _SERVER
// User-approved Loose experiment. Never claimed to be recovered VNG boss rates.
#include "PhongThanRealmTrialRuntime.h"
#include "PhongThanDropRules.h"
#include "PhongThanQuestItemTuple.h"
#include "../../../Headers/PhongThanRewardOwner.h"
#include "../../../Headers/PhongThanRealmTrialBosses.h"
namespace PtRealmTrial {
struct BossConfig {bool valid,enabled;bool active[14];PtDropTable tables[14];};
static DWORD consumed[MAX_NPC];
static BossConfig& BossSettings(){static BossConfig config;static bool loaded=false;if(loaded)return config;loaded=true;ZeroMemory(&config,sizeof(config));
 if(GetFileAttributesA("loose_only.flag")==(DWORD)-1)return config;
 KIniFile ini;int version=0,enabled=0;
 if(!ini.Load("\\settings\\phongthan\\RealmBossExperimental.ini")||!PtDropInt(ini,"Main","Version",0,&version)||version!=1||!PtDropInt(ini,"Main","Enabled",0,&enabled)||enabled<0||enabled>1)return config;
 config.enabled=enabled==1;
 for(int n=0;n<14;++n){char section[48],raw[32],table[64];sprintf(section,"Template_%d",ptRealmTrialBosses[n].type);
  if(!ini.GetString(section,"Enabled","",raw,sizeof(raw)))continue;
  if(!PtDropInt(ini,section,"Enabled",0,&enabled)||enabled<0||enabled>1)return config;
  if(!enabled)continue;
  ini.GetString(section,"Table","",table,sizeof(table));if(!PtDropReadTable(ini,table,config.tables[n]))return config;config.active[n]=true;
 }
 config.valid=true;return config;
}
static int BossSlot(int npc){
 if(npc<=0||npc>=MAX_NPC||!Npc[npc].m_dwID||Npc[npc].m_Kind!=kind_normal||Npc[npc].IsPlayer())return -1;
 for(int n=0;n<14;++n)if(Npc[npc].m_NpcSettingIdx==ptRealmTrialBosses[n].type&&Npc[npc].m_DeathScriptID==ptRealmTrialBosses[n].script)return n;
 return -1;
}
static void RealmTrialTrace(int npc,int owner,const char* event,int row,int result){
 FILE* f=fopen("realm_boss_experiment.tsv","ab");if(!f)return;
 fprintf(f,"LOOSE_EXPERIMENT\t%lu\t%d\t%lu\t%d\t%s\t%d\t%d\n",GetTickCount(),Npc[npc].m_NpcSettingIdx,Npc[npc].m_dwID,owner,event,row,result);fclose(f);
}
static int Emit(KNpc& npc,int owner,const PtDropTable& table){
    if (npc.m_Kind != kind_normal || owner <= 0 || owner >= MAX_PLAYER ||
        !PhongThanRewardOwnerAvailable(Player[owner].m_nNetConnectIdx,Player[owner].m_bIsBot!=0,Player[owner].m_PlayerDBLoad!=0) || npc.m_SubWorldIndex < 0 ||
        npc.m_SubWorldIndex >= MAX_SUBWORLD || npc.m_RegionIndex < 0) return 0;
    int ownerNpc = Player[owner].m_nIndex;
    if (ownerNpc <= 0 || ownerNpc >= MAX_NPC || !Npc[ownerNpc].IsPlayer() ||
        Npc[ownerNpc].m_SubWorldIndex != npc.m_SubWorldIndex) return 0;
    int i, emitted=0;
    for (i = 0; i < table.count; ++i)
    {
        const PtDropEntry& e = table.entries[i]; PhongThanQuestItemTuple tuple;
        if (!PhongThanDecodeQuestItemTuple(e.genre,e.detail,e.particular,e.level,e.series,e.luck,tuple))
        { RealmTrialTrace(npc.m_Index,owner,"IDENTITY_REJECT",i+1,0); return 0; }
    }
    RealmTrialTrace(npc.m_Index,owner,"PLAN",table.count,table.draws);
    for (i = 0; i < table.draws; ++i)
    {
        int picked = PtDropPick(table,g_Random(table.range));
        if (picked < 0) { RealmTrialTrace(npc.m_Index,owner,"NO_DROP",0,0); continue; }
        const PtDropEntry& e = table.entries[picked]; PhongThanQuestItemTuple tuple;
        PhongThanDecodeQuestItemTuple(e.genre,e.detail,e.particular,e.level,e.series,e.luck,tuple);
        for (int q = 0; q < e.quantity; ++q)
        {
            int magic[MAX_ITEM_MAGICLEVEL]; ZeroMemory(magic,sizeof(magic));
            int item = ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,
                tuple.detail,tuple.particular,magic,g_SubWorldSet.GetGameVersion());
            if (item <= 0) { RealmTrialTrace(npc.m_Index,owner,"ITEM_REJECT",picked+1,0); continue; }
            if (!PhongThanQuestItemMatches(Item[item],tuple) || !Item[item].GetName()[0] ||
                Item[item].GetWidth() <= 0 || Item[item].GetHeight() <= 0)
            { ItemSet.Remove(item); RealmTrialTrace(npc.m_Index,owner,"ITEM_REJECT",picked+1,0); continue; }
            char objectKind[32]; objectKind[0] = 0;
            if (Item[item].GetObjIdx() <= 0 ||
                !ObjSet.m_cTabFile.GetString(Item[item].GetObjIdx()+1,3,"",objectKind,sizeof(objectKind)) ||
                strcmp(objectKind,"Item") != 0)
            { ItemSet.Remove(item); RealmTrialTrace(npc.m_Index,owner,"OBJECT_TEMPLATE_REJECT",picked+1,0); continue; }
            int x, y; npc.GetMpsPos(&x,&y); POINT pos; pos.x = x; pos.y = y;
            SubWorld[npc.m_SubWorldIndex].GetFreeObjPos(pos);
            KMapPos map; map.nSubWorld = npc.m_SubWorldIndex;
            SubWorld[map.nSubWorld].Mps2Map(pos.x,pos.y,&map.nRegion,&map.nMapX,&map.nMapY,&map.nOffX,&map.nOffY);
            if (map.nRegion < 0) { ItemSet.Remove(item); continue; }
            KObjItemInfo info; ZeroMemory(&info,sizeof(info));
            info.m_nItemID = item; info.m_nItemWidth = Item[item].GetWidth(); info.m_nItemHeight = Item[item].GetHeight();
            info.m_nColorID = Item[item].GetColorItem(); info.m_nGenre = Item[item].GetGenre(); info.m_nDetailType = Item[item].GetDetailType();
            info.m_nMovieFlag = info.m_nSoundFlag = 1;
            g_StrCpyLen(info.m_szName,Item[item].GetName(),sizeof(info.m_szName));
            int object = ObjSet.Add(Item[item].GetObjIdx(),map,info);
            if (object <= 0) ItemSet.Remove(item); else {Object[object].SetItemBelong(owner);++emitted;}
            RealmTrialTrace(npc.m_Index,owner,"DROP_OBJECT",picked+1,object);
        }
    }
    return emitted;
}
}
bool PhongThanRealmTrialBossOwns(int npc){int slot=PtRealmTrial::BossSlot(npc);if(slot<0)return false;PtRealmTrial::BossConfig& c=PtRealmTrial::BossSettings();return c.valid&&c.enabled&&c.active[slot];}
void PhongThanRealmTrialBossReset(int npc){if(npc>0&&npc<MAX_NPC)PtRealmTrial::consumed[npc]=0;}
int LuaRealmTrialBossDeath(Lua_State* L){
 int result=0,owner=GetPlayerIndex(L),npc=0;
 if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){double raw=Lua_ValueToNumber(L,1);if(raw>0&&raw<MAX_NPC&&(int)raw==raw)npc=(int)raw;}
 if(owner>0&&owner<MAX_PLAYER&&PhongThanRealmTrialBossOwns(npc)&&Npc[npc].m_Doing==do_death&&PtRealmTrial::consumed[npc]!=Npc[npc].m_dwID){
  int actor=Player[owner].m_nIndex;
  if(actor>0&&actor<MAX_NPC&&Npc[actor].IsPlayer()&&Npc[actor].GetPlayerIdx()==owner&&Npc[actor].m_SubWorldIndex==Npc[npc].m_SubWorldIndex&&PhongThanRewardOwnerAvailable(Player[owner].m_nNetConnectIdx,Player[owner].m_bIsBot!=0,Player[owner].m_PlayerDBLoad!=0)){
   PtRealmTrial::consumed[npc]=Npc[npc].m_dwID;
   int slot=PtRealmTrial::BossSlot(npc);result=PtRealmTrial::Emit(Npc[npc],owner,PtRealmTrial::BossSettings().tables[slot]);
   PtRealmTrial::RealmTrialTrace(npc,owner,"DEATH_SETTLED",0,result);
  }
 }
 Lua_PushNumber(L,result);return 1;
}

#endif
