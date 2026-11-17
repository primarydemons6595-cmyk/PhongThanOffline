#ifdef _SERVER
#include "PhongThanJadeRules.h"
#include "PhongThanGemRules.h"
#include "KMagicDesc.h"
#include "PhongThanXichTungTu.h"
static bool PtJadePlayer(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_nIndex>0&&Npc[Player[p].m_nIndex].m_CurrentLife>0&&!Player[p].GetLockState()&&!Player[p].CheckTrading();}
static bool PtJadeNearLei(int p){
 if(!PtJadePlayer(p))return false;int a=Player[p].m_nIndex,n=Player[p].m_nLastNpcIndex,w=Npc[a].m_SubWorldIndex;
 return w>=0&&w<MAX_SUBWORLD&&n>0&&n<MAX_NPC&&Npc[n].m_SubWorldIndex==w&&Npc[n].m_RegionIndex>=0&&
  PhongThanIsLeiZhenZi(SubWorld[w].m_SubWorldID,Npc[n].m_NpcSettingIdx,Npc[n].m_ActionScriptID)&&NpcSet.GetDistance(a,n)<=Npc[n].m_DialogRadius*2;
}
static int PtJadeEquipped(int p){int i=Player[p].m_ItemList.GetEquipment(itempart_cuff);return i>0&&i<MAX_ITEM&&Item[i].GetGenre()==item_equip&&Item[i].GetDetailType()==equip_shipin?i:0;}
static int PtJadeRefine(int p,int group,DWORD serial){
 if(!PtJadePlayer(p)||group<1||group>5)return 0;
 int i=PtJadeEquipped(p);if(!i||Item[i].GetID()!=serial||Item[i].GetLock()->IsLock())return 0;
 KItem after=Item[i];int rolled[MAX_ITEM_MAGICLEVEL];
 if(!PtJade::Roll(after.GetRow(),group,after.m_GeneratorParam.nGeneratorLevel,rolled,g_Random(1000000),g_Random(1000000)))return -2;
 memcpy(after.m_GeneratorParam.nGeneratorLevel,rolled,sizeof(rolled));if(!PtJade::Apply(after))return 0;
 PhongThanQuestExchangeRow needs[3];int count=2;
 if(!PhongThanDecodeQuestItemTuple(6,1,739,1,0,0,needs[0].item)||!PhongThanDecodeQuestItemTuple(3,1044+group,0,0,0,0,needs[1].item))return 0;
 needs[0].count=1;needs[1].count=5;
 if(Item[i].m_GeneratorParam.nGeneratorLevel[group-1]){
  if(!PhongThanDecodeQuestItemTuple(8,1081,2,0,0,0,needs[2].item))return 0;needs[2].count=1;count=3;
 }
 int value=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_HANDBOOK_RECEIPT);
 if(!PhongThanQuestExchange(p,TASKVALUE_PT_HANDBOOK_RECEIPT,value,value==1?0:1,needs,count,0,0,true))return -3;
 memcpy(Item[i].m_GeneratorParam.nGeneratorLevel,after.m_GeneratorParam.nGeneratorLevel,sizeof(rolled));
 for(int g=0;g<5;++g)Item[i].m_aryMagicAttrib[PtJade::AttributeSlot+g]=after.m_aryMagicAttrib[PtJade::AttributeSlot+g];
 Player[p].UpdataCurData();Player[p].m_ItemList.SyncItemMagicAttrib(i);Player[p].SendCurNormalSyncData();
 return 1;
}
static bool PtJadeBag(int place){return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3;}
static int PtJadePolished(int input,unsigned draw){
 int output=0;
 if(input>=253&&input<=280)output=input+28;
 else if(input==374||input==383||input==392||input==401||input==5231)output=input+1+draw%8;
 else if(input>=5225&&input<=5227)output=input+3;
 return output&&PtGem::Identity(item_materials,output,0,0)?output:0;
}
static int PtJadePolishFee(int detail){
 return 100000;
}
static int PtJadeRaw(int detail){
 if(detail>=281&&detail<=301)return detail-28;
 if(detail>=375&&detail<=382)return 374;
 if(detail>=384&&detail<=391)return 383;
 if(detail>=393&&detail<=400)return 392;
 if(detail>=402&&detail<=409)return 401;
 return 0;
}
static int PtJadeFindBag(int p,DWORD serial,ItemPos* pos=0){
 for(PlayerItem* o=Player[p].m_ItemList.GetFirstItem();o;o=Player[p].m_ItemList.GetNextItem()){
  int i=o->nIdx;if(PtJadeBag(o->nPlace)&&i>0&&i<MAX_ITEM&&Item[i].GetID()==serial&&!Item[i].GetLock()->IsLock()&&!Item[i].IsTemp()){
   if(pos){pos->nPlace=o->nPlace;pos->nX=o->nX;pos->nY=o->nY;}return i;
  }
 }return 0;
}
static int PtJadePolish(int p,DWORD serial,DWORD* newSerial=0){
 if(!PtJadeNearLei(p))return 0;KItemList& bag=Player[p].m_ItemList;
 int index=0;ItemPos pos;
 for(PlayerItem* o=bag.GetFirstItem();o;o=bag.GetNextItem())if(PtJadeBag(o->nPlace)&&o->nIdx>0&&o->nIdx<MAX_ITEM&&Item[o->nIdx].GetID()==serial){index=o->nIdx;pos.nPlace=o->nPlace;pos.nX=o->nX;pos.nY=o->nY;break;}
 if(!index||Item[index].GetGenre()!=item_materials||Item[index].GetLock()->IsLock()||Item[index].IsTemp())return 0;
 int detail=PtJadePolished(Item[index].GetDetailType(),g_Random(8)),stack=Item[index].GetStackNum();
 int fee=PtJadePolishFee(Item[index].GetDetailType());
 if(!detail||stack<1||bag.GetEquipmentMoney()<fee)return -3;
 int output=ItemSet.Add(item_materials,0,0,0,detail,0);if(output<=0)return 0;
 Item[output].SetLockSell(Item[index].GetLockSell());Item[output].SetLockTrade(Item[index].GetLockTrade());Item[output].SetLockDrop(Item[index].GetLockDrop());
 bool detached=stack==1;if(detached){if(!bag.Remove(index)){ItemSet.Remove(output);return 0;}}else{Item[index].SetStackNum(stack-1);bag.SyncItem(index);}
 ItemPos destination;bool placed=bag.SearchPosition(Item[output].GetWidth(),Item[output].GetHeight(),&destination,true)&&bag.Add(output,destination.nPlace,destination.nX,destination.nY,false);
 if(placed&&Player[p].Pay(fee)){if(newSerial)*newSerial=Item[output].GetID();if(detached)ItemSet.Remove(index);return 1;}
 if(placed)bag.Remove(output);ItemSet.Remove(output);
 if(detached){if(!bag.Add(index,pos.nPlace,pos.nX,pos.nY,false))g_DebugLog("[Jade] polish rollback invariant player=%d",p);}else{Item[index].SetStackNum(stack);bag.SyncItem(index);}
 return -3;
}
static int PtJadeRevert(int p,DWORD serial){
 if(!PtJadeNearLei(p))return 0;
 ItemPos pos;int index=PtJadeFindBag(p,serial,&pos);if(!index||Item[index].GetGenre()!=item_materials)return 0;
 int raw=PtJadeRaw(Item[index].GetDetailType()),gem=PtGem::Identity(item_materials,Item[index].GetDetailType(),0,0);PT_SOCKET_ATTRIBUTE attr;
 if(!raw||!gem||Item[index].GetStackNum()!=1||!PtGem::Decode(Item[index].GetParam(),gem,attr))return 0;
 int payment=1;PhongThanQuestItemTuple mirror;
 if(!PhongThanDecodeQuestItemTuple(8,509,2,0,0,0,mirror))return 0;
 for(PlayerItem* owned=Player[p].m_ItemList.GetFirstItem();owned;owned=Player[p].m_ItemList.GetNextItem()){
  int i=owned->nIdx;
  if(PtJadeBag(owned->nPlace)&&i>0&&i<MAX_ITEM&&Item[i].GetID()&&!Item[i].GetLock()->IsLock()&&
     Item[i].GetStackNum()>0&&PhongThanQuestItemMatches(Item[i],mirror)){payment=0;break;}
 }
 PhongThanQuestExchangeRow need,reward;
 if(!PhongThanDecodeQuestItemTuple(payment?4:8,payment?47:509,payment?0:2,0,0,0,need.item)||
    !PhongThanDecodeQuestItemTuple(3,raw,0,0,0,0,reward.item))return 0;
 need.count=payment?18:1;reward.count=1;
 int temp=ItemSet.Add(item_materials,0,0,0,raw,0);if(temp<=0)return 0;
 KItem prototype=Item[temp];ItemSet.Remove(temp);
 prototype.SetLockSell(Item[index].GetLockSell());prototype.SetLockTrade(Item[index].GetLockTrade());prototype.SetLockDrop(Item[index].GetLockDrop());
 KItemList& bag=Player[p].m_ItemList;if(!bag.Remove(index))return 0;
 int value=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_HANDBOOK_RECEIPT);
 if(PhongThanQuestExchange(p,TASKVALUE_PT_HANDBOOK_RECEIPT,value,value==1?0:1,&need,1,&reward,1,true,&prototype,true)){ItemSet.Remove(index);return 1;}
 if(!bag.Add(index,pos.nPlace,pos.nX,pos.nY,false))g_DebugLog("[Jade] revert rollback invariant player=%d",p);
 return -3;
}
static int PtJadeExtract(int p,DWORD serial){
 if(!PtJadeNearLei(p))return 0;KItemList& bag=Player[p].m_ItemList;
 int index=0;ItemPos pos;
 for(PlayerItem* owned=bag.GetFirstItem();owned;owned=bag.GetNextItem())if(PtJadeBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM&&Item[owned->nIdx].GetID()==serial){
  index=owned->nIdx;pos.nPlace=owned->nPlace;pos.nX=owned->nX;pos.nY=owned->nY;break;}
 if(!index||Item[index].GetGenre()!=item_equip||Item[index].GetDetailType()!=equip_shipin||Item[index].GetLock()->IsLock()||Item[index].IsTemp())return 0;
 int capacity=0,quality=0;if(!PtJade::Shape(Item[index].GetRow(),capacity,quality))return 0;
 PhongThanQuestExchangeRow reward;if(!PhongThanDecodeQuestItemTuple(3,1045+g_Random(5),0,0,0,0,reward.item))return 0;reward.count=1;
 int temp=ItemSet.Add(item_materials,0,0,0,reward.item.detail,0);if(temp<=0)return 0;
 KItem prototype=Item[temp];ItemSet.Remove(temp);
 prototype.SetLockSell(Item[index].GetLockSell());prototype.SetLockTrade(Item[index].GetLockTrade());prototype.SetLockDrop(Item[index].GetLockDrop());
 if(!bag.Remove(index))return 0;
 int value=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_HANDBOOK_RECEIPT);
 if(PhongThanQuestExchange(p,TASKVALUE_PT_HANDBOOK_RECEIPT,value,value==1?0:1,0,0,&reward,1,true,&prototype)){
  ItemSet.Remove(index);return 1;}
 if(!bag.Add(index,pos.nPlace,pos.nX,pos.nY,false))g_DebugLog("[Jade] extraction rollback invariant player=%d serial=%lu",p,(unsigned long)serial);
 return -3;
}
int LuaJadeService(Lua_State* L){
 int p=GetPlayerIndex(L),result=0,n=Lua_GetTopIndex(L);
 if(PtJadePlayer(p)&&n>=1&&Lua_IsNumber(L,1)){
  int op=(int)Lua_ValueToNumber(L,1);
  if(op==0&&n==1){int i=PtJadeEquipped(p);Lua_PushNumber(L,i?(double)Item[i].GetID():0);return 1;}
  if(op==1&&n==3&&Lua_IsNumber(L,2)&&Lua_IsNumber(L,3)){
   double group=Lua_ValueToNumber(L,2),id=Lua_ValueToNumber(L,3);
   if(group>=1&&group<=5&&group==(int)group&&id>0&&id<=4294967295.0&&id==(DWORD)id)result=PtJadeRefine(p,(int)group,(DWORD)id);
  }else if((op==2||op==3||op==5)&&n==2&&Lua_IsNumber(L,2)){
   double id=Lua_ValueToNumber(L,2);if(id>0&&id<=4294967295.0&&id==(DWORD)id){
    if(op==5){Lua_NewTable(L);int i=PtJadeNearLei(p)?PtJadeFindBag(p,(DWORD)id):0;
     if(i&&Item[i].GetGenre()==item_materials){const char* name=Item[i].GetName();while(*name=='#'||*name=='$')++name;Lua_PushString(L,name);Lua_RawSetI(L,-2,1);
      Lua_PushNumber(L,PtJadePolishFee(Item[i].GetDetailType()));Lua_RawSetI(L,-2,2);
      static bool descriptionsReady=false;if(!descriptionsReady)descriptionsReady=g_MagicDesc.Init()!=0;
      const char* desc=descriptionsReady&&Item[i].m_aryMagicAttrib[0].nAttribType?g_MagicDesc.GetDesc(&Item[i].m_aryMagicAttrib[0]):"";Lua_PushString(L,desc?desc:"");Lua_RawSetI(L,-2,3);
      Lua_PushNumber(L,PtJadeRaw(Item[i].GetDetailType())!=0);Lua_RawSetI(L,-2,4);
      Lua_PushNumber(L,PtGem::Maximum(Item[i],0));Lua_RawSetI(L,-2,5);
     }return 1;
    }
    if(op==2)result=PtJadeExtract(p,(DWORD)id);
    else {DWORD output=0;result=PtJadePolish(p,(DWORD)id,&output);Lua_PushNumber(L,result);Lua_PushNumber(L,output);return 2;}
   }
  }else if(op==4&&n==2&&Lua_IsNumber(L,2)){
   double id=Lua_ValueToNumber(L,2);
   if(id>0&&id<=4294967295.0&&id==(DWORD)id)result=PtJadeRevert(p,(DWORD)id);
  }
 }Lua_PushNumber(L,result);return 1;
}
int LuaJadeList(Lua_State* L){
 int p=GetPlayerIndex(L),count=0,mode=Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):0;Lua_NewTable(L);
 if(PtJadeNearLei(p))for(PlayerItem* o=Player[p].m_ItemList.GetFirstItem();o;o=Player[p].m_ItemList.GetNextItem()){
  int i=o->nIdx;if(!PtJadeBag(o->nPlace)||i<=0||i>=MAX_ITEM||Item[i].GetLock()->IsLock())continue;
  if(mode==0){if(Item[i].GetGenre()!=item_equip||Item[i].GetDetailType()!=equip_shipin)continue;}
  else if(mode==1){if(Item[i].GetGenre()!=item_materials||!PtJadePolished(Item[i].GetDetailType(),0))continue;}
  else if(mode==2){if(Item[i].GetGenre()!=item_materials||!PtJadeRaw(Item[i].GetDetailType()))continue;}else continue;
  if(++count>=MAX_PLAYER_ITEM)break;Lua_NewTable(L);Lua_PushNumber(L,Item[i].GetID());Lua_RawSetI(L,-2,1);const char* name=Item[i].GetName();while(*name=='#'||*name=='$')++name;Lua_PushString(L,name);Lua_RawSetI(L,-2,2);Lua_RawSetI(L,-2,count);
 }return 1;
}
#endif
