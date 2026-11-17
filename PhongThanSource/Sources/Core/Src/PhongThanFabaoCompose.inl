#include "PhongThanFabaoRules.h"
static int PhongThanFabaoCompose(int player,const int* indexes,int* selected,int group,int& recipeId,int& fee,bool preview){
 PtFabao::Plan plan;int result=PtFabao::Quote(indexes,selected,group,plan);if(result!=PT_COMPOSE_OK)return result;
 recipeId=plan.rule.base.id;fee=plan.fee;
 if(preview){memcpy(selected,plan.used,sizeof(plan.used));return PT_COMPOSE_OK;}
#ifndef PHONGTHAN_COMPOSE_QUOTE_ONLY
 if(Player[player].m_ItemList.GetEquipmentMoney()<fee)return PT_COMPOSE_NO_MONEY;
 const bool upgrade=plan.rule.base.type==1;KItem original,updated;int output=0;
 bool bindSell=false,bindTrade=false,bindDrop=false;
 for(int b=0;b<9;++b)if(plan.used[b]){bindSell=bindSell||Item[indexes[b]].GetLockSell();bindTrade=bindTrade||Item[indexes[b]].GetLockTrade();bindDrop=bindDrop||Item[indexes[b]].GetLockDrop();}
 if(upgrade){original=Item[indexes[plan.main]];updated=original;if(!updated.ApplyUpgradeState(PhongThanEncodeUpgradeState(plan.stars,plan.rule.base.attributeId)))return PT_COMPOSE_TABLE_ERROR;
  updated.SetLockSell(updated.GetLockSell()||bindSell);updated.SetLockTrade(updated.GetLockTrade()||bindTrade);updated.SetLockDrop(updated.GetLockDrop()||bindDrop);
 }
 ItemPos destination;
 const KBASICPROP_EQUIPMENT* source=upgrade?0:ItemGen.CatalogEquipment(4,plan.row);
 if(!Player[player].m_ItemList.SearchPosition(upgrade?original.GetWidth():source->m_nWidth,upgrade?original.GetHeight():source->m_nHeight,&destination,true))return PT_COMPOSE_NO_ROOM;
 const bool success=plan.rule.base.chance==100||(int)g_Random(100)<plan.rule.base.chance;
 if(!upgrade&&success){int levels[MAX_ITEM_MAGICLEVEL]={0},series=Player[player].m_cProfession.GetProfession();
  for(int req=0;req<6;++req)if(source->m_aryPropReq[req].nType==magic_requireseries&&source->m_aryPropReq[req].nPara>=0&&source->m_aryPropReq[req].nPara<4)series=source->m_aryPropReq[req].nPara;
  output=ItemSet.Add(0,series,source->m_nLevel,0,4,source->m_nParticularType,levels,g_SubWorldSet.GetGameVersion(),0,plan.row);
  if(output<=0)return PT_COMPOSE_TABLE_ERROR;
  Item[output].SetLockSell(Item[output].GetLockSell()||bindSell);Item[output].SetLockTrade(Item[output].GetLockTrade()||bindTrade);Item[output].SetLockDrop(Item[output].GetLockDrop()||bindDrop);
 }
 int counts[9];bool detached[9]={0},remainder[9]={0};bool valid=true,placed=false;
 for(int s=0;s<9;++s)counts[s]=indexes[s]?Item[indexes[s]].GetStackNum():0;
 for(s=0;s<9&&valid;++s)if(plan.used[s]){if(!Player[player].m_ItemList.Remove(indexes[s]))valid=false;else detached[s]=true;}
 if(valid&&success){int target=output;if(upgrade){target=indexes[plan.main];Item[target]=updated;}
  placed=Player[player].m_ItemList.Add(target,destination.nPlace,destination.nX,destination.nY,false)!=0;if(!placed)valid=false;
 }
 for(int rem=0;rem<9&&valid;++rem)if(detached[rem]&&(!upgrade||rem!=plan.main)&&counts[rem]>plan.used[rem]){
  Item[indexes[rem]].SetStackNum(counts[rem]-plan.used[rem]);remainder[rem]=Player[player].m_ItemList.Add(indexes[rem],pos_builditem,rem,0,false)!=0;if(!remainder[rem])valid=false;
 }
 if(valid&&!Player[player].Pay(fee))valid=false;
 if(valid){for(int used=0;used<9;++used)if(detached[used]&&!remainder[used]&&(!upgrade||used!=plan.main))ItemSet.Remove(indexes[used]);
  g_DebugLog("[Fabao] player=%d recipe=%d group=%d upgrade=%d success=%d fee=%d",player,recipeId,plan.rule.base.group,upgrade,success,fee);
  return success?(upgrade?PT_COMPOSE_UPGRADE_OK:PT_COMPOSE_OK):PT_COMPOSE_FABAO_FAILED;
 }
 if(placed)Player[player].m_ItemList.Remove(upgrade?indexes[plan.main]:output);
 if(output>0)ItemSet.Remove(output);
 for(int rb=0;rb<9;++rb)if(remainder[rb])Player[player].m_ItemList.Remove(indexes[rb]);
 if(upgrade)Item[indexes[plan.main]]=original;
 for(int back=0;back<9;++back)if(detached[back]){Item[indexes[back]].SetStackNum(counts[back]);if(!Player[player].m_ItemList.Add(indexes[back],pos_builditem,back,0,false))g_DebugLog("[Fabao] rollback slot invariant player=%d slot=%d",player,back);}
 return PT_COMPOSE_TABLE_ERROR;
#else
 return PT_COMPOSE_NO_RECIPE;
#endif
}
