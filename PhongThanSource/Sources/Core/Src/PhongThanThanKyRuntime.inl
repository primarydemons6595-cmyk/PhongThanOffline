// Original Lua supplies all effect numbers. Book lifetime/activation is the
// explicitly requested Loose policy; no level-specific duplicate skill IDs.
#include "PhongThanThanKyRuntime.h"
#include "PhongThanThanKyExperiment.h"
#include "../../../Headers/PhongThanRealmWorld.h"
#include <map>
#include <math.h>
#ifdef _SERVER
namespace PtTK {
struct Lease {DWORD serial;int item,part,rank,previous,count;};
struct State {
 DWORD entity,lastSecond,lastHeal;Lease books[15];
 int shield,shieldTicks,dashX,dashY,dashLeft,dashSpeed;
 int morphOld,morphTemplate,morphSkill,morphTicks,morphLife,morphPrevious;
 int autoGroup;DWORD autoNext[10];
 State(){memset(this,0,sizeof(*this));}
};
static std::map<int,State> states;
static BYTE active[MAX_NPC];
static State& Get(int actor){State& s=states[actor];if(s.entity!=Npc[actor].m_dwID){s=State();s.entity=Npc[actor].m_dwID;}active[actor]=1;return s;}
static bool Live(int n){return n>0&&n<MAX_NPC&&Npc[n].m_dwID&&Npc[n].m_RegionIndex>=0&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD;}
static bool Human(int n){return Live(n)&&Npc[n].IsPlayer()&&Npc[n].m_CurrentLife>0&&Npc[n].GetPlayerIdx()>0&&Npc[n].GetPlayerIdx()<MAX_PLAYER;}
static bool Carried(int p,int item){
 if(item<=0||item>=MAX_ITEM||!Item[item].GetID())return false;
 int slot=Player[p].m_ItemList.FindSame(item);if(slot<=0)return false;
 int place=Player[p].m_ItemList.m_Items[slot].nPlace;
 return (place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3)&&
  (!Item[item].m_CommonAttrib.nExpireTime||Item[item].m_CommonAttrib.nExpireTime>(int)KSG_GetCurSec());
}
struct Book {int part,skill,rank,profession;};
static Book books[128];static int bookCount=0;static bool booksLoaded=false;
static void LoadBooks(){
 if(booksLoaded)return;booksLoaded=true;KIniFile ini;
 if(!ini.Load("\\settings\\phongthan\\ThanKyBooks.ini"))return;
 int count=0;ini.GetInteger("Books","Count",0,&count);if(count<1||count>128)return;
 for(int i=0;i<count;++i){char section[32];sprintf(section,"Book_%d",i);Book q;
  ini.GetInteger(section,"Part",0,&q.part);ini.GetInteger(section,"Skill",0,&q.skill);
  ini.GetInteger(section,"Rank",0,&q.rank);ini.GetInteger(section,"Profession",-1,&q.profession);
  if(q.part<=0||PtThanKyGroup(q.skill)<0||q.rank<1||q.rank>10||q.profession<0||q.profession>2){bookCount=0;return;}
  for(int j=0;j<bookCount;++j)if(books[j].part==q.part){bookCount=0;return;}
  books[bookCount++]=q;
 }
}
static const Book* FindBook(int part){LoadBooks();for(int i=0;i<bookCount;++i)if(books[i].part==part)return books+i;return 0;}
static bool ValidLease(int actor,const Lease& b,int skill){
 if(!b.serial)return false;int p=Npc[actor].GetPlayerIdx();
 if(!Carried(p,b.item)||Item[b.item].GetGenre()!=6||Item[b.item].GetParticular()!=0)return false;
 // Lua/catalogue (6,1,key) normalizes to native (genre=6,detail=key,part=0).
 const Book* q=FindBook(Item[b.item].GetDetailType());
 return q&&q->profession==Player[p].m_cProfession.GetProfession()&&q->skill==skill&&b.count==Item[b.item].GetStackNum()&&PtThanKyLeaseMatches(b.serial,b.part,b.rank,Item[b.item].GetID(),q->part,q->rank,true);
}
// Native-only task slots, above the Lua task API ceiling (5000). Existing
// character task records persist these without changing the save/wire ABI.
static bool SaveRoom(int player,int group){
 int used=0,missing=0;
 for(int i=0;i<MAX_TASK;++i)if(Player[player].m_cTask.GetSaveStr(i)[0])++used;
 for(i=0;i<3;++i)if(!Player[player].m_cTask.GetSaveStr(TASKVALUE_PT_THANKY_BEGIN+group*3+i)[0])++missing;
 return used+missing<=PHONGTHAN_CHARACTER_MAX_TASKS;
}
static void SaveLease(int player,int group,const Lease& b){
 int key=TASKVALUE_PT_THANKY_BEGIN+group*3;
 if(!b.serial){for(int i=0;i<3;++i)Player[player].m_cTask.SetSaveVal(key+i,"",FALSE);}
 else{
  Player[player].m_cTask.SetSaveVal(key,(int)b.serial,FALSE);
  Player[player].m_cTask.SetSaveVal(key+1,b.part*16+b.rank,FALSE);
  Player[player].m_cTask.SetSaveVal(key+2,b.count*256+b.previous,FALSE);
 }
 Player[player].m_uMustSave=SAVE_REQUEST;
}
static bool Buff(int actor,int type,KMagicAttrib& value){
 if(!Live(actor))return false;
 for(KStateNode* n=(KStateNode*)Npc[actor].m_StateSkillList.GetHead();n;n=(KStateNode*)n->GetNext()){
  if(PtThanKyGroup(n->m_SkillID)<0&& !PtThanKyAuxiliary(n->m_SkillID))continue;
  if(n->m_LeftTime==0)continue;
  for(int i=0;i<MAX_SKILL_STATE;++i)if(n->m_State[i].nAttribType==type){value=n->m_State[i];for(int k=0;k<3;++k)value.nValue[k]=-value.nValue[k];return true;}
 }
 return false;
}
static void RemoveBuff(int actor,int id){
 bool changed=false;
 KStateNode* n=(KStateNode*)Npc[actor].m_StateSkillList.GetHead();
 while(n){KStateNode* next=(KStateNode*)n->GetNext();if(n->m_SkillID==id){
  int rank=n->m_Level;
  for(int i=0;i<MAX_SKILL_STATE;++i)if(n->m_State[i].nAttribType)Npc[actor].ModifyAttrib(actor,&n->m_State[i]);
  n->Remove();delete n;changed=true;
  // Send removal without creating an empty server state node, which would
  // swallow a same-frame recast of this skill at the same level.
  PhongThanThanKySyncRemoveBuff(actor,id,rank);
 }n=next;}
 if(changed)Npc[actor].RefreshPhongThanSkillStates();
}
static void ResetMorph(int actor,State& s){
 if(s.morphTicks){
  if(Npc[actor].m_NpcSettingIdx==s.morphTemplate)PhongThanThanKyMorphPlayer(Npc[actor].GetPlayerIdx(),-1,0);
  if(s.morphSkill>0){
   Npc[actor].m_SkillList.Remove(s.morphSkill);
   if(s.morphPrevious>0)Npc[actor].m_SkillList.Add(s.morphSkill,s.morphPrevious);
   PhongThanThanKyClearSelection(Npc[actor].GetPlayerIdx(),s.morphSkill);
   Player[Npc[actor].GetPlayerIdx()].SendSyncData_Skill();
  }
 }
 s.morphTicks=s.morphLife=s.morphSkill=s.morphPrevious=0;
}
static void ClearGroup(int actor,int group){
 State& s=Get(actor);int id=PtThanKySkill(group);RemoveBuff(actor,id);
 if(id==1552){s.shield=s.shieldTicks=s.dashLeft=0;}
 if(id==1553){s.autoGroup=0;memset(s.autoNext,0,sizeof(s.autoNext));}
 if(id==1556)ResetMorph(actor,s);
}
static void Revoke(int actor,int group){
 State& s=Get(actor);Lease old=s.books[group];if(!old.serial)return;
 ClearGroup(actor,group);int skill=PtThanKySkill(group);
 Npc[actor].m_SkillList.Remove(skill);
 if(old.previous>0)Npc[actor].m_SkillList.Add(skill,old.previous);
 memset(&s.books[group],0,sizeof(Lease));
 SaveLease(Npc[actor].GetPlayerIdx(),group,s.books[group]);
 PhongThanThanKyClearSelection(Npc[actor].GetPlayerIdx(),skill);
 Player[Npc[actor].GetPlayerIdx()].SendSyncData_Skill();
}
static bool Move(int actor,int x,int y){
 if(!Live(actor))return false;int world=Npc[actor].m_SubWorldIndex,r,mx,my,ox,oy;
 SubWorld[world].Mps2Map(x,y,&r,&mx,&my,&ox,&oy);
 if(r<0||SubWorld[world].TestBarrier(x,y))return false;
 if(Npc[actor].IsPlayer())return Npc[actor].SetPosU(x,y)!=0;
 if(!Npc[actor].SetPos(x,y))return false;Npc[actor].SendSyncData(0,TRUE);return true;
}
static bool VectorMove(int actor,int vx,int vy,int distance){
 int x,y;Npc[actor].GetMpsPos(&x,&y);double length=sqrt((double)vx*vx+(double)vy*vy);if(length<1)return false;
 int lastX=x,lastY=y;
 for(int d=8;d<=distance+7;d+=8){int travel=d>distance?distance:d;int nx=x+(int)(vx*travel/length),ny=y+(int)(vy*travel/length);int r,mx,my,ox,oy;
  SubWorld[Npc[actor].m_SubWorldIndex].Mps2Map(nx,ny,&r,&mx,&my,&ox,&oy);
  if(r<0||SubWorld[Npc[actor].m_SubWorldIndex].TestBarrier(nx,ny))break;
  lastX=nx;lastY=ny;
 }
 return (lastX!=x||lastY!=y)&&Move(actor,lastX,lastY);
}
static bool Recall(int actor){
 int pet=s_PtOwnerPet[actor];if(PtSummonOwner(pet)!=actor)return false;
 int x,y;Npc[actor].GetMpsPos(&x,&y);
 static const int dx[]={64,64,0,-64,-64,-64,0,64},dy[]={0,48,48,48,0,-48,-48,-48};
 for(int i=0;i<8;++i)if(Move(pet,x+dx[i],y+dy[i])){s_PtSummonTargets[actor].targetId=0;return true;}return false;
}
static bool MorphInfo(int actor,int rank,int& chosen,int& attack,int& scale){
 int skill,type;if(!PhongThanSummonInfo(actor,skill,type))return false;
 KTabFile allowed,groups;if(!allowed.Load("\\settings\\trans_to_creature.txt")||!groups.Load("\\settings\\trans_to_creature_group.txt"))return false;
 int row=0;for(int i=2;i<=allowed.GetHeight();++i){int id;allowed.GetInteger(i,"Level",0,&id);if(id==rank){row=i;break;}}
 if(!row)return false;chosen=-1;attack=scale=0;
 for(i=1;i<=20&&chosen<0;++i){char column[32];sprintf(column,"Group%d",i);int group;allowed.GetInteger(row,column,0,&group);if(!group)continue;
  for(int j=2;j<=groups.GetHeight();++j){int g,n;groups.GetInteger(j,"GroupId",0,&g);groups.GetInteger(j,"NpcId",-1,&n);if(g==group&&n==type){chosen=n;groups.GetInteger(j,"SkillId",0,&attack);groups.GetInteger(j,"MaxLifeScale",100,&scale);break;}}
 }
 if(chosen<0||attack<=0||!g_SkillManager.GetSkill(attack,1))return false;
 return true;
}
static bool Morph(int actor,int rank,int ticks){
 int chosen,attack,scale;if(!MorphInfo(actor,rank,chosen,attack,scale))return false;
 State& s=Get(actor);ResetMorph(actor,s);
 if(!PhongThanThanKyMorphPlayer(Npc[actor].GetPlayerIdx(),chosen,ticks))return false;
 s.morphOld=Npc[actor].m_NpcSettingIdx;s.morphTemplate=chosen;s.morphTicks=ticks;s.morphSkill=attack;
 // Let the normal reversible state layer own maximum HP. This survives
 // equipment/stat recalculation and cannot subtract a stale absolute delta.
 s.morphLife=scale-100;
 s.morphPrevious=Npc[actor].m_SkillList.GetLevel(attack);
 Npc[actor].m_SkillList.Add(attack,1,0,TRUE);Player[Npc[actor].GetPlayerIdx()].SendSyncData_Skill();return true;
}
struct Hit {int source,target,ratio,noDebuff;};static Hit hits[16];static int depth=0,overflow=0;static bool trueHit=false;
}

bool PhongThanThanKyPreserveSkill(int player,int skill){
 if(player<=0||player>=MAX_PLAYER)return false;int actor=Player[player].m_nIndex,group=PtThanKyGroup(skill);
 if(group<0||!PtTK::Human(actor)||!PtTK::active[actor])return false;
 PtTK::State& state=PtTK::Get(actor);return PtTK::ValidLease(actor,state.books[group],skill);
}
bool PhongThanThanKyCanCast(int actor,int skill,int level){
 if(PtThanKyGroup(skill)<0){
  if(PhongThanIsSummon(skill)&&actor>0&&actor<MAX_NPC&&PtTK::active[actor]&&PtTK::Get(actor).morphTicks>0)return false;
  return true;
 }
 if(!PtTkExperiment::Enabled(skill)||!PtTK::Human(actor))return false;int p=Npc[actor].GetPlayerIdx();
 if(Npc[actor].m_Level<140||PtRealmWorld::IsRealmMap(SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID))return false;
 if(PtTK::active[actor]){PtTK::State& s=PtTK::Get(actor);int group=PtThanKyGroup(skill);
  if(s.books[group].serial&&!PtTK::ValidLease(actor,s.books[group],skill)){PtTK::Revoke(actor,group);return false;}
 }
 if(skill==1551){int pet=s_PtOwnerPet[actor];if(PtSummonOwner(pet)!=actor)return false;}
 if(skill==1556){int id,type,chosen,attack,scale;if(!PhongThanSummonInfo(actor,id,type))return false;
  KSkill* q=(KSkill*)g_SkillManager.GetSkill(skill,level);KMagicAttrib a;
  if(!q||!q->ReadRuntimeAttribute(magic_trans_to_creature_v,a)||!PtTK::MorphInfo(actor,a.nValue[0],chosen,attack,scale))return false;
 }
 return level>=1&&level<=10&&!Player[p].CheckTrading()&&!Player[p].GetLockState();
}
bool PhongThanThanKyIsActivationBook(int item){
 return item>0&&item<MAX_ITEM&&Item[item].GetID()&&Item[item].GetGenre()==6&&
  Item[item].GetParticular()==0&&PtTK::FindBook(Item[item].GetDetailType())!=0;
}
int PhongThanThanKyUseBook(int player,int item){
 if(player<=0||player>=MAX_PLAYER||!PtTK::Human(Player[player].m_nIndex)||!PtTK::Carried(player,item))return 0;
 if(Item[item].GetGenre()!=6||Item[item].GetParticular()!=0||Item[item].GetLock()->IsLock())return 0;
 const PtTK::Book* b=PtTK::FindBook(Item[item].GetDetailType());if(!b)return 0;
 if(!PtTkExperiment::Enabled(b->skill))return -3;
 int actor=Player[player].m_nIndex,group=PtThanKyGroup(b->skill);
 if(Player[player].GetLockState()||Player[player].CheckTrading()||Npc[actor].m_Level<140||Player[player].m_cProfession.GetProfession()!=b->profession||PtRealmWorld::IsRealmMap(SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID))return 0;
 if(!g_SkillManager.GetSkill(b->skill,b->rank))return 0;
 PtTK::State& s=PtTK::Get(actor);PtTK::Lease old=s.books[group];
 if(old.serial==Item[item].GetID()&&old.part==b->part&&old.rank==b->rank&&PtTK::ValidLease(actor,old,b->skill)){
  // Re-send an unchanged lease and repair a slot lost by an older stat rebuild.
  // A repeat activation never spends another stone or resets a live cooldown.
  int slot=Npc[actor].m_SkillList.FindSame(b->skill);
  if(!slot){slot=Npc[actor].m_SkillList.Add(b->skill,b->rank,0,TRUE);if(!slot)return 0;}
  NPCSKILL& learned=Npc[actor].m_SkillList.m_Skills[slot];learned.SkillLevel=learned.CurrentSkillLevel=b->rank;learned.AddLevel=0;learned.TempSkill=TRUE;
  Player[player].SendSyncData_Skill();return 2;
 }
 int stone=0;for(int i=1;i<MAX_PLAYER_ITEM;++i){int n=Player[player].m_ItemList.m_Items[i].nIdx;if(n>0&&n<MAX_ITEM&&PtTK::Carried(player,n)&&Item[n].GetGenre()==3&&Item[n].GetDetailType()==1360&&!Item[n].GetLock()->IsLock()){stone=n;break;}}
 if(!stone)return -1;
 if(!PtTK::SaveRoom(player,group))return 0;
 // Reserve/validate the single stable skill slot before payment.
 int previous=old.serial?old.previous:Npc[actor].m_SkillList.GetLevel(b->skill);
 int existing=Npc[actor].m_SkillList.FindSame(b->skill);NPCSKILL before;if(existing)before=Npc[actor].m_SkillList.m_Skills[existing];
 int slot=Npc[actor].m_SkillList.Add(b->skill,b->rank,0,TRUE);if(!slot)return 0;
 if(!Player[player].m_ItemList.RemoveItem(stone,1)){
  if(existing)Npc[actor].m_SkillList.m_Skills[existing]=before;else Npc[actor].m_SkillList.Remove(b->skill);return 0;
 }
 // Add() normally only raises learned levels. A book lease must also support
 // replacing/downgrading a prior lease, without persisting it as learned.
 Npc[actor].m_SkillList.m_Skills[slot].SkillLevel=b->rank;
 Npc[actor].m_SkillList.m_Skills[slot].CurrentSkillLevel=b->rank;
 Npc[actor].m_SkillList.m_Skills[slot].AddLevel=0;
 Npc[actor].m_SkillList.m_Skills[slot].TempSkill=TRUE;
 PtTK::ClearGroup(actor,group);
 s.books[group].serial=Item[item].GetID();s.books[group].item=item;s.books[group].part=b->part;s.books[group].rank=b->rank;s.books[group].previous=previous;s.books[group].count=Item[item].GetStackNum();
 PtTK::SaveLease(player,group,s.books[group]);
 PhongThanThanKyClearSelection(player,b->skill);
 Player[player].SendSyncData_Skill();Player[player].m_uMustSave=SAVE_REQUEST;return 1;
}
void PhongThanThanKyRestore(int player){
 if(player<=0||player>=MAX_PLAYER)return;int actor=Player[player].m_nIndex;
 if(!PtTK::Live(actor)||!Npc[actor].IsPlayer())return;
 PtTK::State& s=PtTK::Get(actor);
 for(int g=0;g<15;++g){PtTK::Lease b;memset(&b,0,sizeof(b));int key=TASKVALUE_PT_THANKY_BEGIN+g*3;
  b.serial=(DWORD)Player[player].m_cTask.GetSaveVal(key);if(!b.serial)continue;
  int packed=Player[player].m_cTask.GetSaveVal(key+1),meta=Player[player].m_cTask.GetSaveVal(key+2);
  b.part=packed/16;b.rank=packed%16;b.previous=meta%256;b.count=meta/256;
  for(int i=1;i<MAX_PLAYER_ITEM;++i){int item=Player[player].m_ItemList.m_Items[i].nIdx;if(item>0&&item<MAX_ITEM&&Item[item].GetID()==b.serial){b.item=item;break;}}
  int skill=PtThanKySkill(g);
  if(b.count>0&&b.previous>=0&&b.previous<MAX_SKILLLEVEL&&PtTK::ValidLease(actor,b,skill)){
   int slot=Npc[actor].m_SkillList.Add(skill,b.rank,0,TRUE);
   if(slot){s.books[g]=b;NPCSKILL& learned=Npc[actor].m_SkillList.m_Skills[slot];learned.SkillLevel=learned.CurrentSkillLevel=b.rank;learned.AddLevel=0;learned.TempSkill=TRUE;continue;}
  }
  PtTK::Lease empty;memset(&empty,0,sizeof(empty));PtTK::SaveLease(player,g,empty);
  if(b.previous>0&&b.previous<MAX_SKILLLEVEL)Npc[actor].m_SkillList.Add(skill,b.previous);
 }
 Player[player].SendSyncData_Skill();
}
int PhongThanThanKyCarriedBookKey(int player,int skill,int rank){
 if(player<=0||player>=MAX_PLAYER)return 0;
 for(int i=1;i<MAX_PLAYER_ITEM;++i){int item=Player[player].m_ItemList.m_Items[i].nIdx;
  if(item<=0||item>=MAX_ITEM||!PtTK::Carried(player,item)||Item[item].GetGenre()!=6||Item[item].GetParticular()!=0||Item[item].GetLock()->IsLock())continue;
  const PtTK::Book* b=PtTK::FindBook(Item[item].GetDetailType());if(b&&b->skill==skill&&b->rank==rank)return b->part;
 }return 0;
}
bool PhongThanThanKyCast(int actor,const KSkill* skill,int x,int y){
 int id=skill->GetSkillId(),group=PtThanKyGroup(id);if(group<0||PtTkExperiment::NewGroup(id)>=0||id==1547||id==1548||id==1554||id==1560)return false;
 if(!PhongThanThanKyCanCast(actor,id,(int)skill->GetRuntimeSkillLevel()))return true;
 PtTK::State& s=PtTK::Get(actor);KMagicAttrib a;
 if(id==1551){PtTK::Recall(actor);return true;}
 if(id==1553&&s.autoGroup){PtTK::ClearGroup(actor,group);return true;}
 if(id==1556){if(!skill->ReadRuntimeAttribute(magic_trans_to_creature_v,a)||!PtTK::Morph(actor,a.nValue[0],a.nValue[1]))return true;}
 if(id==1552&&skill->ReadRuntimeAttribute(magic_forward_shield_p,a)){
  s.shield=PtThanKyShield(Npc[actor].m_CurrentLifeMax,a.nValue[0],a.nValue[2]);s.shieldTicks=a.nValue[1];
  if(skill->ReadRuntimeAttribute(magic_forward_move_b,a)){int px,py;Npc[actor].GetMpsPos(&px,&py);if(x==-1&&PtTK::Live(y))Npc[y].GetMpsPos(&x,&y);
   if(x==px&&y==py){x=px+g_DirCos(Npc[actor].m_Dir,64);y=py+g_DirSin(Npc[actor].m_Dir,64);}
   double len=sqrt((double)(x-px)*(x-px)+(double)(y-py)*(y-py));if(len>0){s.dashX=(int)((x-px)*1024/len);s.dashY=(int)((y-py)*1024/len);s.dashLeft=a.nValue[0];s.dashSpeed=a.nValue[2];}
  }
 }
 KMagicAttrib state[MAX_SKILL_STATE];int count=0;
 for(int i=0;i<skill->GetStateAttribsNum()&&count<MAX_SKILL_STATE;++i)state[count++]=skill->GetStateAttribs()[i];
 if(id==1556&&count<MAX_SKILL_STATE){KMagicAttrib& hp=state[count++];hp.nAttribType=magic_lifemax_p;hp.nValue[0]=s.morphLife;hp.nValue[1]=s.morphTicks;hp.nValue[2]=1;}
 if(count)Npc[actor].SetStateSkillEffect(actor,id,(int)skill->GetRuntimeSkillLevel(),state,count,state[0].nValue[1]);
 if(id==1553&&skill->ReadRuntimeAttribute(magic_auto_activate_skills_v,a)){s.autoGroup=a.nValue[0];memset(s.autoNext,0,sizeof(s.autoNext));}
 return true;
}
void PhongThanThanKyState(int source,int target,int skill,int level,void* raw,int count){
 if(skill!=1547||!PtTK::Live(source)||!PtTK::Live(target)||Npc[target].m_CurrentLife<=0||Npc[source].m_SubWorldIndex!=Npc[target].m_SubWorldIndex)return;
 KMagicAttrib* a=(KMagicAttrib*)raw;int flat=0,percent=0,power=0,interval=GAME_FPS;
 for(int i=0;i<count;++i){if(a[i].nAttribType==magic_life_kmfc_dot_by_level_v){flat=a[i].nValue[0];interval=a[i].nValue[2];}if(a[i].nAttribType==magic_life_kmfc_dot_by_self_p)percent=a[i].nValue[0];if(a[i].nAttribType==magic_life_kmfc_dot_by_casterpower_p)power=a[i].nValue[0];}
 PtTK::State& s=PtTK::Get(target);DWORD now=SubWorld[Npc[target].m_SubWorldIndex].m_dwCurrentTime;
 if(s.lastHeal&&now-s.lastHeal<(DWORD)(interval>0?interval:GAME_FPS))return;s.lastHeal=now;
 int heal=PtThanKyHeal(Npc[target].m_CurrentLifeMax,Npc[target].m_CurrentLife,flat,percent,Npc[source].m_PtCombat.AdditionalHeal,power);
 Npc[target].m_CurrentLife+=heal;if(heal)Npc[target].NormalSync();
}
bool PhongThanThanKyImmediate(int source,int target,void* raw){
 KMagicAttrib& a=*(KMagicAttrib*)raw;
 if(a.nAttribType!=magic_move_back_b&&a.nAttribType!=magic_no_miss_v&&a.nAttribType!=magic_damage_ratio_p&&a.nAttribType!=magic_damage_unchange_status_b)return false;
 if(a.nAttribType==magic_move_back_b&&PtTK::Live(source)&&PtTK::Live(target)&&source!=target){int x,y,tx,ty;Npc[source].GetMpsPos(&x,&y);Npc[target].GetMpsPos(&tx,&ty);PtTK::VectorMove(target,tx-x,ty-y,a.nValue[0]);}
 return true;
}
void PhongThanThanKyTick(int actor){
 if(actor<=0||actor>=MAX_NPC||!PtTK::active[actor])return;
 PtTK::State& s=PtTK::Get(actor);if(!PtTK::Live(actor))return;
 if(s.shieldTicks>0&&!--s.shieldTicks)s.shield=0;
 if(s.morphTicks>0&&(!Npc[actor].IsAlive()||s.morphTicks==1))PtTK::ResetMorph(actor,s);else if(s.morphTicks>1)--s.morphTicks;
 if(s.dashLeft>0){int step=s.dashSpeed<s.dashLeft?s.dashSpeed:s.dashLeft;if(step<=0||!Npc[actor].IsAlive()||!PtTK::VectorMove(actor,s.dashX,s.dashY,step))s.dashLeft=0;else s.dashLeft-=step;}
 DWORD now=SubWorld[Npc[actor].m_SubWorldIndex].m_dwCurrentTime;
 if(now-s.lastSecond<GAME_FPS)return;s.lastSecond=now;
 if(!Npc[actor].IsPlayer())return;
 for(int g=0;g<15;++g)if(s.books[g].serial&&!PtTK::ValidLease(actor,s.books[g],PtThanKySkill(g)))PtTK::Revoke(actor,g);
 if(!s.autoGroup||!Npc[actor].IsAlive())return;
 KMagicAttrib drain;if(!PtTK::Buff(actor,magic_auto_activate_skills_v,drain)){s.autoGroup=0;return;}
 if(PtTK::Buff(actor,magic_mana_drain_v,drain)){if(Npc[actor].m_CurrentMana<drain.nValue[0]){PtTK::ClearGroup(actor,4);return;}Npc[actor].m_CurrentMana-=drain.nValue[0];}
 static KTabFile table;static bool loaded=false;if(!loaded)loaded=table.Load("\\settings\\auto_activate_skills.txt")!=0;if(!loaded)return;
 int row=s.autoGroup+1;if(row<2||row>table.GetHeight())return;
 for(int i=0;i<10;++i){char key[40];int id,interval;sprintf(key,"Skill%d",i+1);table.GetInteger(row,key,0,&id);sprintf(key,"Skill%dInterval",i+1);table.GetInteger(row,key,0,&interval);
  if(id<=0||interval<=0||now<s.autoNext[i])continue;int rank=Npc[actor].m_SkillList.GetCurrentLevel(id);KSkill* q=rank>0?(KSkill*)g_SkillManager.GetSkill(id,rank):0;
  if(q&&Npc[actor].m_SkillList.CanCast(id,now)&&Npc[actor].Cost(q->GetSkillCostType(),q->GetSkillCost(0),TRUE,TRUE)&&Npc[actor].Cost(q->GetSkillCostType(),q->GetSkillCost(0),FALSE,TRUE)){
   q->Cast(actor,-1,actor);Npc[actor].m_SkillList.SetNextCastTime(id,now,now+q->GetDelayPerCast(FALSE));s.autoNext[i]=now+interval;
  }
 }
}
void PhongThanThanKyHitBegin(int source,int target,int skill,int level,int& ar){
 if(PtTK::depth>=16){++PtTK::overflow;return;}PtTK::Hit& h=PtTK::hits[PtTK::depth++];h.source=source;h.target=target;h.ratio=100;h.noDebuff=0;
 if(!PtTK::Live(source)||!PtTK::Live(target)||level<=0)return;
 if(skill!=1548&&skill!=1549&&skill!=1550&&skill!=1554&&skill!=1555)return;
 KSkill* q=(KSkill*)g_SkillManager.GetSkill(skill,level);KMagicAttrib a;if(!q)return;
 if(q->ReadRuntimeAttribute(magic_damage_ratio_p,a))h.ratio=Npc[target].IsPlayer()?a.nValue[0]:a.nValue[1];
 if(q->ReadRuntimeAttribute(magic_damage_unchange_status_b,a))h.noDebuff=a.nValue[0]!=0;
 if(q->ReadRuntimeAttribute(magic_no_miss_v,a)&&a.nValue[0]>=100)ar=FALSE;
}
void PhongThanThanKyHitEnd(){if(PtTK::overflow>0)--PtTK::overflow;else if(PtTK::depth>0)--PtTK::depth;}
bool PhongThanThanKyNoDebuff(){return PtTK::depth>0&&PtTK::hits[PtTK::depth-1].noDebuff!=0;}
bool PhongThanThanKySkipDebuff(int skill,int level){
 if(skill<1548||skill>1550||level<1||level>10)return false;
 KSkill* q=(KSkill*)g_SkillManager.GetSkill(skill,level);KMagicAttrib a;
 return q&&q->ReadRuntimeAttribute(magic_damage_unchange_status_b,a)&&a.nValue[0]!=0;
}
int PhongThanThanKyDamage(int source,int target,int damage,int type,bool reflected){
 if(damage<=0||reflected||!PtTK::Live(source)||!PtTK::Live(target))return damage;
 if(PtTK::depth>0&&!PtTK::overflow&&!PtTK::trueHit){const PtTK::Hit& h=PtTK::hits[PtTK::depth-1];if(h.source==source&&h.target==target)damage=PtThanKyRate(damage,h.ratio);}
 KMagicAttrib a;
 if(!PtTK::trueHit&&Npc[source].IsPlayer()){int p=Npc[source].GetPlayerIdx();if(Player[p].m_ItemList.GetWeaponType()==0&&Player[p].m_ItemList.GetWeaponParticular()==0&&PtTK::Buff(source,magic_damage_ratio_by_short_weapon_p,a))damage=PtThanKySaturate((__int64)damage+PtThanKyRate(damage,a.nValue[0]));}
 if(PtTK::active[target])damage=PtThanKyAbsorb(damage,PtTK::Get(target).shield);
 return damage;
}
void PhongThanThanKyAfterHit(int source,int target){
 if(PtTK::trueHit||!PtTK::Human(source)||!PtTK::Live(target)||!Npc[target].IsAlive())return;
 int p=Npc[source].GetPlayerIdx();KMagicAttrib a;
 if(Player[p].m_ItemList.GetWeaponType()!=0||Player[p].m_ItemList.GetWeaponParticular()!=0||!PtTK::Buff(source,magic_extra_real_damage_with_short_weapon_v,a))return;
 int damage=Npc[target].IsPlayer()?a.nValue[0]:a.nValue[2];if(damage<=0)return;
 PtTK::trueHit=true;Npc[target].CalcDamage(source,-1,damage,damage,damage_magic,FALSE);PtTK::trueHit=false;
}
#else
void PhongThanThanKyRestore(int){}
bool PhongThanThanKyPreserveSkill(int,int skill){return PtThanKyGroup(skill)>=0;}
int PhongThanThanKyCarriedBookKey(int,int,int){return 0;}
bool PhongThanThanKyCanCast(int,int skill,int){return PtTkExperiment::Enabled(skill);}
bool PhongThanThanKyCast(int,const KSkill*,int,int){return false;}
void PhongThanThanKyTick(int){}
void PhongThanThanKyState(int,int,int,int,void*,int){}
bool PhongThanThanKyImmediate(int,int,void*){return false;}
int PhongThanThanKyUseBook(int,int){return 0;}
void PhongThanThanKyHitBegin(int,int,int,int,int&){}
void PhongThanThanKyHitEnd(){}
int PhongThanThanKyDamage(int,int,int n,int,bool){return n;}
bool PhongThanThanKyNoDebuff(){return false;}
bool PhongThanThanKySkipDebuff(int,int){return false;}
void PhongThanThanKyAfterHit(int,int){}
#endif
