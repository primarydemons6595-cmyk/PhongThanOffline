#include "../../../Headers/PhongThanTranNguyenTasks.h"
#include "PhongThanTranNguyen.h"
#include "KStrBase.h"
#ifndef PHONGTHAN_TRAN_NGUYEN_TEST
#include "KItemGenerator.h"
#endif
#include "PhongThanOrangeRecipe.h"
#include "../../../Headers/PhongThanForgeState.h"
#include "../../../Headers/PhongThanTranNguyenData.h"
#include <vector>
#include <map>
namespace PtTran {
using namespace PtTranTask;
static KTabFile recipes,costs,books;
static std::map<int,std::string> materialNames;
static std::vector<PhongThanComposeRecipe> catalog;
static bool ready=false;
static int Get(int p,int t){return Player[p].m_cTask.GetSaveVal(t);}
static void Set(int p,int t,int v){Player[p].m_cTask.SetSaveVal(t,v,TRUE);}
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsPlayer();}
static bool Near(int p){if(!Live(p))return false;int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex;
 return n>0&&n<MAX_NPC&&Npc[n].m_dwID&&Npc[n].m_Kind==kind_dialoger&&Npc[n].m_NpcSettingIdx==202&&Npc[n].m_ActionScriptID==0x378b6d9aUL&&Npc[n].m_SubWorldIndex==Npc[a].m_SubWorldIndex&&SubWorld[Npc[a].m_SubWorldIndex].m_SubWorldID==1052&&Npc[n].m_DialogRadius>0&&NpcSet.GetDistance(a,n)<=Npc[n].m_DialogRadius*2;
}
static bool Tables(){if(ready)return true;if(!recipes.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")||!costs.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_jin_qian_xu_qiu.txt")||!books.Load("\\settings\\item\\001\\compoundbook.txt"))return false;
 catalog.clear();for(int row=2;row<=recipes.GetHeight();++row){PhongThanComposeRecipe r;if(PhongThanReadOrangeRecipe(recipes,row,r)&&r.type==2&&r.group>=2&&r.group<=6&&r.chance==100&&r.output.genre==0&&r.output.quantity==1){int b=atoi(r.requiredRecipe);if(b>0&&b<3000)catalog.push_back(r);}}
 KTabFile materials;if(!materials.Load("\\settings\\item\\001\\material.txt"))return false;
 for(int mr=2;mr<=materials.GetHeight();++mr){int id=-1;char name[128];materials.GetInteger(mr,3,-1,&id);materials.GetString(mr,1,"",name,sizeof(name));if(id>=0)materialNames[id]=name;}
 ready=!catalog.empty();return ready;
}
static PhongThanComposeRecipe* Recipe(int id){if(!Tables())return 0;for(unsigned i=0;i<catalog.size();++i)if(catalog[i].id==id)return &catalog[i];return 0;}
static int Book(const PhongThanComposeRecipe& r){return atoi(r.requiredRecipe);}
static bool Learned(int p,int book){int index=BookIndex(book);return index>=0&&(Get(p,Books+index/30)&(1<<(index%30)))!=0;}
static void Learn(int p,int book){int index=BookIndex(book);if(index>=0)Set(p,Books+index/30,Get(p,Books+index/30)|(1<<(index%30)));}
static int Reputation(int p){int s=Get(p,Slot);return s>=1&&s<=5?Get(p,Credit+s-1):0;}
static int Requirement(int book){if(book>=1000)return 500;if(book>=500)return book%100<=3?101:book%100<=6?201:301;return book%100<=3?0:50;}
static bool Basic(int book){return book<500||(book<1000&&book%100<=3);}
static bool OpenedSecondTier(int p){if(Get(p,Tier3))return true;for(unsigned i=0;i<catalog.size();++i){int b=Book(catalog[i]);if(b>=500&&b<1000&&Learned(p,b))return true;}return false;}
static int Today(){return (int)((time(NULL)+25200)/86400);}
static int Count(int p){return Get(p,Day)==Today()?Get(p,Runs):0;}
static bool Owned(int p,int n,DWORD serial){if(n<=0||n>=MAX_ITEM||!Item[n].GetID()||Item[n].GetID()!=serial||!Player[p].m_ItemList.FindSame(n)||Item[n].GetLock()->IsLock())return false;
 int place=Player[p].m_ItemList.m_Items[Player[p].m_ItemList.FindSame(n)].nPlace;return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3;
}
static int EquipSlot(KItem& item){if(item.GetGenre()!=item_equip)return 0;switch(item.GetDetailType()){case equip_helm:return 1;case equip_belt:return 2;case equip_pendant:return 4;case equip_boots:return 3;case equip_armor:return 5;}return 0;}
static bool Match(KItem& item,const PhongThanRecipeItem& t){return item.GetGenre()==t.genre&&item.GetDetailType()==t.detail&&item.GetParticular()==t.part&&item.GetLevel()>=0&&item.GetLevel()<16&&(!t.upgradeMask||(t.upgradeMask&(1U<<item.GetLevel())));}
static bool Exchange(int p,PhongThanQuestExchangeRow* need,int nn,PhongThanQuestExchangeRow* reward,int nr){int old=Get(p,Sequence);return old>=0&&old<1000000000&&PhongThanQuestExchange(p,Sequence,old,old+1,need,nn,reward,nr,true)==1;}
static PhongThanQuestExchangeRow Tuple(int g,int d,int part,int count,int level=0){PhongThanQuestExchangeRow r;ZeroMemory(&r,sizeof(r));PhongThanDecodeQuestItemTuple(g,d,part,level,0,0,r.item);r.count=count;return r;}
static int Paper(int p,int book){for(unsigned i=0;i<sizeof(PT_TRAN_PAPERS)/sizeof(PT_TRAN_PAPERS[0]);++i)if(PT_TRAN_PAPERS[i].book==book){int detail=PT_TRAN_PAPERS[i].detail;PlayerItem* owned=Player[p].m_ItemList.GetFirstItem();while(owned){int n=owned->nIdx;if(Owned(p,n,Item[n].GetID())&&Item[n].GetGenre()==6&&Item[n].GetDetailType()==detail&&Item[n].GetParticular()==0)return n;owned=Player[p].m_ItemList.GetNextItem();}}return 0;}
static void Journal(int p){int phase=Get(p,Phase),flags=Get(p,Mask);char text[480];sprintf(text,PT_TRAN_PROGRESS,(flags&1)?1:0,(flags&2)?1:0,(flags&4)?1:0,(flags&8)?1:0,(flags&16)?1:0,(flags&32)?1:0);SendPhongThanTaskRecord(p,3089,phase,phase==1?1:2,text);}
static int Donation(int p,int n,DWORD serial,bool boosted){
 int slot=Get(p,Slot),credit=Reputation(p),required=credit<=50?1:credit<=100?2:credit<=150?3:credit<=200?4:credit<=300?5:credit<=400?6:credit<=500?7:8;
 if(Get(p,Phase)!=2||slot<1||slot>5||Count(p)>=5||credit>100000||!Owned(p,n,serial)||EquipSlot(Item[n])!=slot||Item[n].GetPhongThanQuality()!=1||Item[n].GetLevel()!=required)return 0;
 KItemList& bag=Player[p].m_ItemList;ItemPos pos;pos.nPlace=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nPlace;pos.nX=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nX;pos.nY=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nY;
 PhongThanQuestExchangeRow booster=Tuple(8,credit<100?255:credit<300?254:253,2,1);
 if(!bag.Remove(n))return 0;
 if(!Exchange(p,boosted?&booster:0,boosted?1:0,0,0)){bag.Add(n,pos.nPlace,pos.nX,pos.nY,false);return 0;}
 ItemSet.Remove(n);Set(p,Credit+slot-1,credit+(boosted?10:2));int count=Count(p);Set(p,Day,Today());Set(p,Runs,count+1);return 1;
}
static int Craft(int p,int id,int n,DWORD serial){PhongThanComposeRecipe* r=Recipe(id);if(!r||Get(p,Phase)!=2||Get(p,Slot)!=r->group-1||!Learned(p,Book(*r))||!Owned(p,n,serial)||Item[n].GetPhongThanQuality()!=0||Item[n].m_CommonAttrib.nUpgradeLvl||!Match(Item[n],r->inputs[0]))return 0;
 KItem updated=Item[n];if(!updated.ApplyForgeRecipe(id))return 0;
 PhongThanQuestExchangeRow needs[10];int nn=0;
 for(int i=1;i<r->inputCount;++i){const PhongThanRecipeItem& t=r->inputs[i];int level=0;if(t.genre==0){for(int l=0;l<16;++l)if(t.upgradeMask&(1U<<l)){if(level)return 0;level=l;}}
  needs[nn++]=Tuple(t.genre,t.detail,t.part,t.quantity,level);
 }
 int fee=0;if(!PhongThanReadComposeCost(costs,id,1,fee)||fee<0||!Player[p].Pay(fee))return 0;
 KItemList& bag=Player[p].m_ItemList;ItemPos pos;pos.nPlace=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nPlace;pos.nX=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nX;pos.nY=bag.m_Items[Player[p].m_ItemList.FindSame(n)].nY;
 if(!bag.Remove(n)){Player[p].Earn(fee);return 0;}
 if(!Exchange(p,needs,nn,0,0)){bag.Add(n,pos.nPlace,pos.nX,pos.nY,false);Player[p].Earn(fee);return 0;}
 Item[n]=updated;bag.Add(n,pos.nPlace,pos.nX,pos.nY,false);bag.SyncItem(n);return 1;
}
static int Teaching(int p,int id){PhongThanComposeRecipe* r=Recipe(id);if(!r||Get(p,Phase)!=2||Get(p,Slot)!=r->group-1||Learned(p,Book(*r))||Reputation(p)<Requirement(Book(*r))||(Book(*r)>=1000&&!Get(p,Tier3)))return 0;
 if(!Basic(Book(*r))){int n=Paper(p,Book(*r));if(!n)return 0;PhongThanQuestExchangeRow item=Tuple(6,1,Item[n].GetDetailType(),1);if(!Exchange(p,&item,1,0,0))return 0;}
 Learn(p,Book(*r));return 1;
}
static bool AdmissionSet(int p,int key,int* selected,int& signature){
 if(key<1||key>9)return false;ZeroMemory(selected,sizeof(int)*5);
 for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){
  int n=item->nIdx;if(!Owned(p,n,Item[n].GetID())||Item[n].GetPhongThanQuality()!=3)continue;
  PhongThanComposeRecipe* r=Recipe(PhongThanForgeRecipe(Item[n].GetUnderlyingParam()));if(!r)continue;int book=Book(*r);
  if(book<500||book>=1000||book%100<1||book%100>9)continue;
  int kind=((book%100-1)/3)*3+(book%100-1)%3+1,slot=r->group-2;
  if(kind==key&&slot>=0&&slot<5&&!selected[slot])selected[slot]=n;
 }
 unsigned hash=2166136261U;for(int i=0;i<5;++i){if(!selected[i])return false;hash=(hash^Item[selected[i]].GetID())*16777619U;}signature=hash&0x7fffffffU;return true;
}
static int Admission(int p,int key,int expected){
 if(Get(p,Phase)!=2||Reputation(p)<500||Get(p,Tier3))return 0;int chosen[5],signature;
 if(!AdmissionSet(p,key,chosen,signature)||expected!=signature)return 0;
 KItemList& bag=Player[p].m_ItemList;ItemPos positions[5];int detached=0;
 for(int i=0;i<5;++i){positions[i].nPlace=bag.m_Items[chosen[i]].nPlace;positions[i].nX=bag.m_Items[chosen[i]].nX;positions[i].nY=bag.m_Items[chosen[i]].nY;if(!bag.Remove(chosen[i]))break;++detached;}
 if(detached!=5){for(i=0;i<detached;++i)bag.Add(chosen[i],positions[i].nPlace,positions[i].nX,positions[i].nY,false);return 0;}
 Set(p,Tier3,1);for(i=0;i<5;++i)ItemSet.Remove(chosen[i]);return 1;
}
static void Field(Lua_State* L,const char* key,int value){lua_pushstring(L,key);lua_pushnumber(L,value);lua_settable(L,-3);}
static void Text(Lua_State* L,const char* key,const char* value){lua_pushstring(L,key);lua_pushstring(L,value);lua_settable(L,-3);}
static void Describe(const PhongThanComposeRecipe& r,char* text,int capacity){text[0]=0;
 for(int i=0;i<r.inputCount;++i){const PhongThanRecipeItem& in=r.inputs[i];const char* name="";
  if(in.genre==3){std::map<int,std::string>::iterator it=materialNames.find(in.detail);if(it!=materialNames.end())name=it->second.c_str();}
  else if(in.genre==0){for(int row=0;row<ItemGen.CatalogEquipmentCount(in.detail);++row){const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(in.detail,row);if(e&&e->m_nParticularType==in.part&&e->m_nLevel>=0&&e->m_nLevel<16&&(!in.upgradeMask||(in.upgradeMask&(1U<<e->m_nLevel)))){name=e->m_szName;break;}}}
  if(*name=='#')++name;char line[160];sprintf(line,in.genre==3?"<color=yellow>%s<color> x%d\n":"<color=white>%s<color> x%d\n",name,in.quantity);if(strlen(text)+strlen(line)<(unsigned)capacity)strcat(text,line);
 }
}
}
int LuaTranNguyenInfo(Lua_State* L){int p=GetPlayerIndex(L);lua_newtable(L);if(!PtTran::Near(p)||!PtTran::Tables())return 1;
 using namespace PtTran;Field(L,"phase",Get(p,Phase));Field(L,"mask",Get(p,Mask));Field(L,"slot",Get(p,Slot));Field(L,"credit",Reputation(p));Field(L,"remaining",5-Count(p));Field(L,"tier3",Get(p,Tier3));Field(L,"lingvalue",OpenedSecondTier(p)?5:1);
 PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);int at=FindIBBuffSlot(store,369);Field(L,"ling",at>=0?store->Entry[at].nTimes:0);return 1;
}
int LuaTranNguyenList(Lua_State* L){int p=GetPlayerIndex(L),mode=(int)Lua_ValueToNumber(L,1),id=(int)Lua_ValueToNumber(L,2),count=0;lua_newtable(L);if(!PtTran::Near(p)||!PtTran::Tables())return 1;using namespace PtTran;
 if(mode==1||mode==3){PhongThanComposeRecipe* r=mode==3?Recipe(id):0;for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){int n=item->nIdx;if(!Owned(p,n,Item[n].GetID()))continue;
   bool show=mode==1?EquipSlot(Item[n])==Get(p,Slot)&&Item[n].GetPhongThanQuality()==1:r&&Learned(p,Book(*r))&&Item[n].GetPhongThanQuality()==0&&!Item[n].m_CommonAttrib.nUpgradeLvl&&Match(Item[n],r->inputs[0]);if(!show)continue;
   lua_newtable(L);Field(L,"index",n);Field(L,"serial",Item[n].GetID());Text(L,"name",Item[n].GetName());Field(L,"level",Item[n].GetLevel());lua_rawseti(L,-2,++count);
  }
 }else if(mode==2||mode==4){for(unsigned i=0;i<catalog.size();++i){PhongThanComposeRecipe& r=catalog[i];if(r.group-1!=Get(p,Slot))continue;int book=Book(r);bool learned=Learned(p,book);if(mode==2&&!learned)continue;if(mode==4&&learned)continue;
   char key[24],name[128];sprintf(key,"%d",book);books.GetString(books.FindRow(key),2,"",name,sizeof(name));int fee=0;PhongThanReadComposeCost(costs,r.id,1,fee);
   char detail[480];Describe(r,detail,sizeof(detail));lua_newtable(L);Field(L,"id",r.id);Field(L,"book",book);Field(L,"require",Requirement(book));Field(L,"paper",Basic(book)?1:(Paper(p,book)>0));Field(L,"fee",fee);Text(L,"name",name);Text(L,"detail",detail);lua_rawseti(L,-2,++count);
  }
 }else if(mode==5){for(int key=1;key<=9;++key){int selected[5],signature;if(!AdmissionSet(p,key,selected,signature))continue;lua_newtable(L);Field(L,"id",key);Field(L,"stamp",signature);Text(L,"name",Item[selected[0]].GetName());char detail[480];detail[0]=0;for(int k=0;k<5;++k){if(strlen(detail)+strlen(Item[selected[k]].GetName())+2<sizeof(detail)){strcat(detail,Item[selected[k]].GetName());strcat(detail,"\n");}}Text(L,"detail",detail);lua_rawseti(L,-2,++count);}}
 return 1;
}
int LuaTranNguyenAction(Lua_State* L){int p=GetPlayerIndex(L),op=(int)Lua_ValueToNumber(L,1),a=(int)Lua_ValueToNumber(L,2),b=(int)Lua_ValueToNumber(L,3);DWORD c=(DWORD)Lua_ValueToNumber(L,4);int result=0;
 if(PtTran::Near(p)&&!Player[p].GetLockState()&&!Player[p].CheckTrading()&&PtTran::Tables()){using namespace PtTran;
  if(op==1&&Get(p,Phase)==0&&Npc[Player[p].m_nIndex].m_Level>=42){Set(p,Mask,0);Set(p,Phase,1);Journal(p);result=1;}
  else if(op==2&&Get(p,Phase)==1&&Get(p,Mask)==63){PhongThanQuestExchangeRow reward[2];reward[0]=Tuple(3,114,0,1);reward[1]=Tuple(3,116,0,1);result=PhongThanQuestExchange(p,Phase,1,2,0,0,reward,2,true);if(result==1){Player[p].DirectAddExp(120000);Journal(p);}}
  else if(op==3&&Get(p,Phase)==2&&Get(p,Slot)==0&&a>=1&&a<=5){Set(p,Slot,a);for(unsigned i=0;i<catalog.size();++i){int book=Book(catalog[i]);if(catalog[i].group==a+1&&book<500&&book%100<=3)Learn(p,book);}result=1;}
  else if(op==4)result=Donation(p,a,c,b!=0);
  else if(op==5&&Get(p,Phase)==2&&Get(p,Slot)>=1&&Get(p,Slot)<=5&&Reputation(p)<100000){PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);int slot=FindIBBuffSlot(store,369);if(slot>=0&&store->Entry[slot].nTimes>0){if(!--store->Entry[slot].nTimes)RemoveIBBuffFromStore(store,Player[p].m_nIndex,369);PersistPlayerIBBuffStore(p);Set(p,Credit+Get(p,Slot)-1,Reputation(p)+(OpenedSecondTier(p)?5:1));result=1;}}
  else if(op==6)result=Teaching(p,a);
  else if(op==7)result=Craft(p,a,b,c);
  else if(op==8&&Get(p,Phase)==2&&Reputation(p)>=50){PhongThanQuestExchangeRow reward=Tuple(3,82,0,1);int t=Credit+Get(p,Slot)-1,credit=Get(p,t);result=PhongThanQuestExchange(p,t,credit,credit-50,0,0,&reward,1,true);}
  else if(op==9)result=Admission(p,a,(int)c);
  else if(op==10&&Get(p,Phase)==1){Set(p,Phase,0);Set(p,Mask,0);Journal(p);result=1;}
  else if(op==11&&Get(p,Phase)==1){Journal(p);result=1;}
 }Lua_PushNumber(L,result);return 1;
}
void PhongThanTranNguyenKill(int n,int p){using namespace PtTran;if(!Live(p)||Get(p,Phase)!=1||n<=0||n>=MAX_NPC||Npc[n].IsPlayer()||Npc[n].m_Kind!=kind_normal||Npc[n].m_CurrentLife>0||Npc[n].m_SubWorldIndex!=Npc[Player[p].m_nIndex].m_SubWorldIndex)return;
 int tid=Npc[n].m_NpcSettingIdx,map=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID,flag=0;
 if(tid==546&&map==1027)flag=1;else if(tid==547&&map==1037)flag=2;else if(tid==548&&map==1023)flag=4;
 else if(tid==9&&(map==1011||map==1018))flag=8;else if(tid==8&&(map>=1008&&map<=1010))flag=16;else if(tid==11&&map==1016)flag=32;
 if(flag&&!(Get(p,Mask)&flag)){Set(p,Mask,Get(p,Mask)|flag);Journal(p);}
}
void PhongThanTranNguyenTick(){using namespace PtTran;static DWORD last=0,scanned=0,seen[MAX_PLAYER];DWORD now=GetTickCount();if(now-scanned<1000)return;scanned=now;bool needed=false;
 for(int p=1;p<MAX_PLAYER;++p){if(Live(p)&&Get(p,Phase)==1){needed=true;DWORD actor=Npc[Player[p].m_nIndex].m_dwID;if(seen[p]!=actor){Journal(p);seen[p]=actor;}}else seen[p]=0;}
 if(now-last<30000)return;last=now;if(!needed)return;
 for(unsigned i=0;i<3;++i){const PT_TRAN_TARGET& t=PT_TRAN_BOSSES[i];int world=g_SubWorldSet.SearchWorld(t.map);if(world<0)continue;bool found=false;
  for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&Npc[n].m_NpcSettingIdx==t.tid&&Npc[n].m_CurrentLife>0){found=true;break;}
  if(found)continue;int spawned=PhongThanSpawnNpc(t.tid,42,world,t.x,t.y,FALSE);if(spawned>0){g_StrCpyLen(Npc[spawned].Name,t.name,sizeof(Npc[spawned].Name));Npc[spawned].m_Kind=kind_normal;Npc[spawned].m_Camp=Npc[spawned].m_CurrentCamp=camp_animal;Npc[spawned].m_AiMode=1;Npc[spawned].m_CurrentLife=Npc[spawned].m_CurrentLifeMax=Npc[spawned].m_LifeMax=250000;Npc[spawned].m_DeathScriptID=0;Npc[spawned].m_Experience=0;}
 }
}
