#include "PhongThanSocketRules.h"
static int PtSocketCompose(int player,const int* indexes,int* selected,int& recipe,int& money,bool preview,int position=0){
 PtSocketRules::Plan plan;int status=PtSocketRules::Quote(indexes,selected,!preview,plan,position);
 if(status!=PT_COMPOSE_OK)return status;
 recipe=plan.Recipe;money=plan.Money;
 if(preview){memcpy(selected,plan.Used,sizeof(plan.Used));return PT_COMPOSE_OK;}
#ifndef PHONGTHAN_COMPOSE_QUOTE_ONLY
 if(Player[player].m_ItemList.GetEquipmentMoney()<money)return PT_COMPOSE_NO_MONEY;
 ItemPos destination;KItem original=Item[indexes[plan.Main]],updated=original;
 if(!Player[player].m_ItemList.SearchPosition(original.GetWidth(),original.GetHeight(),&destination,true))return PT_COMPOSE_NO_ROOM;
 PT_SOCKET_STATE after;bool success=PtSocketRules::Resolve(plan,g_Random(10000),g_Random(100),g_Random(1000001),after);
 if(plan.Gem&&!success)return PT_COMPOSE_TABLE_ERROR;
 if(success){int param=0;if(!PtSocketStore(after,param))return PT_COMPOSE_TABLE_ERROR;updated.m_CommonAttrib.nParam=param;}
 bool detached[9],remainder[9];int counts[9];ZeroMemory(detached,sizeof(detached));ZeroMemory(remainder,sizeof(remainder));
 bool valid=true,placed=false;int result=PT_COMPOSE_TABLE_ERROR;
 for(int c=0;c<9;++c)counts[c]=indexes[c]?Item[indexes[c]].GetStackNum():0;
 for(int s=0;s<9;++s)if(plan.Used[s]){if(!Player[player].m_ItemList.Remove(indexes[s])){valid=false;break;}detached[s]=true;}
 if(valid){Item[indexes[plan.Main]]=updated;placed=Player[player].m_ItemList.Add(indexes[plan.Main],destination.nPlace,destination.nX,destination.nY,false)!=0;if(!placed){valid=false;result=PT_COMPOSE_NO_ROOM;}}
 for(int rem=0;rem<9&&valid;++rem)if(detached[rem]&&rem!=plan.Main&&counts[rem]>plan.Used[rem]){
  Item[indexes[rem]].SetStackNum(counts[rem]-plan.Used[rem]);remainder[rem]=Player[player].m_ItemList.Add(indexes[rem],pos_builditem,rem,0,false)!=0;if(!remainder[rem])valid=false;
 }
 if(valid&&!Player[player].Pay(money)){valid=false;result=PT_COMPOSE_NO_MONEY;}
 if(valid){for(int used=0;used<9;++used)if(detached[used]&&used!=plan.Main&&!remainder[used])ItemSet.Remove(indexes[used]);
  g_DebugLog("[LeiZhenZi] player=%d serial=%lu recipe=%d fee=%d success=%d param=%08X",player,(unsigned long)original.GetID(),plan.Recipe,money,success,(unsigned)updated.GetParam());return success?PT_COMPOSE_OK:PT_COMPOSE_SOCKET_FAILED;}
 if(placed)Player[player].m_ItemList.Remove(indexes[plan.Main]);
 for(int rb=0;rb<9;++rb)if(remainder[rb])Player[player].m_ItemList.Remove(indexes[rb]);
 Item[indexes[plan.Main]]=original;
 for(int back=0;back<9;++back)if(detached[back]){Item[indexes[back]].SetStackNum(counts[back]);if(!Player[player].m_ItemList.Add(indexes[back],pos_builditem,back,0,false))g_DebugLog("[LeiZhenZi] restore invariant player=%d slot=%d",player,back);}
 return result;
#else
 return PT_COMPOSE_NO_RECIPE;
#endif
}
