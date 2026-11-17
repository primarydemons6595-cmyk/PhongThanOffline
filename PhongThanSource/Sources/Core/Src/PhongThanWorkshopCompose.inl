#include "PhongThanWorkshopRules.h"
static int PhongThanWorkshopCompose(int player,const int* indexes,int* selected,int station,int& recipeId,int& fee,bool preview){
 PtWorkshop::Plan plan;int result=PtWorkshop::Quote(player,indexes,selected,station,plan);if(result!=PT_COMPOSE_OK)return result;recipeId=plan.recipe.id;fee=plan.recipe.fee;
 if(preview){memcpy(selected,plan.used,sizeof(int)*9);return PT_COMPOSE_OK;}
#ifndef PHONGTHAN_COMPOSE_QUOTE_ONLY
 const int* pool=plan.indexes;
 if(Player[player].m_ItemList.GetEquipmentMoney()<fee)return PT_COMPOSE_NO_MONEY;
 const PtWorkshop::Recipe& r=plan.recipe;bool transform=r.source>=0,bindSell=false,bindTrade=false,bindDrop=false;
 KItem original,updated,failed;if(transform){original=Item[pool[plan.main]];updated=failed=original;if(!PtWorkshop::Transition(updated,r.target)||(r.failure>=0&&!PtWorkshop::Transition(failed,r.failure)))return PT_COMPOSE_TABLE_ERROR;}
 if(transform){ItemPos probe;if(!Player[player].m_ItemList.SearchPosition(original.GetWidth(),original.GetHeight(),&probe,true))return PT_COMPOSE_NO_ROOM;}
 for(int b=0;b<plan.poolCount;++b)if(plan.used[b]){bindSell=bindSell||Item[pool[b]].GetLockSell();bindTrade=bindTrade||Item[pool[b]].GetLockTrade();bindDrop=bindDrop||Item[pool[b]].GetLockDrop();}
 if(transform){updated.SetLockSell(updated.GetLockSell()||bindSell);updated.SetLockTrade(updated.GetLockTrade()||bindTrade);updated.SetLockDrop(updated.GetLockDrop()||bindDrop);failed.SetLockSell(failed.GetLockSell()||bindSell);failed.SetLockTrade(failed.GetLockTrade()||bindTrade);failed.SetLockDrop(failed.GetLockDrop()||bindDrop);}
 int outputs[64]={0},created=0;bool valid=true;
 if(!transform){int row=r.target;if(r.output.genre==0){if(row<0){for(int find=0;find<ItemGen.CatalogEquipmentCount(r.output.detail);++find){const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(r.output.detail,find);if(e&&e->m_nParticularType==r.output.part&&e->m_nLevel==r.output.level){if(row>=0)return PT_COMPOSE_TABLE_ERROR;row=find;}}}const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(r.output.detail,row);if(!e||e->m_nParticularType!=r.output.part||e->m_nLevel!=r.output.level)return PT_COMPOSE_TABLE_ERROR;}
  for(int n=0;n<r.output.count;++n){int levels[MAX_ITEM_MAGICLEVEL]={0};int i=ItemSet.Add(r.output.genre,r.output.series<=5?r.output.series:0,r.output.level,0,r.output.detail,r.output.part,levels,g_SubWorldSet.GetGameVersion(),0,row);
   if(i<=0){valid=false;break;}outputs[created++]=i;if(!PtWorkshop::Matches(Item[i],r.output)||Item[i].GetStackNum()!=1||Item[i].GetWidth()<1||Item[i].GetHeight()<1){valid=false;break;}
   Item[i].SetLockSell(Item[i].GetLockSell()||bindSell);Item[i].SetLockTrade(Item[i].GetLockTrade()||bindTrade);Item[i].SetLockDrop(Item[i].GetLockDrop()||bindDrop);
  }
 }
 if(!valid){for(int clean=0;clean<created;++clean)ItemSet.Remove(outputs[clean]);return PT_COMPOSE_TABLE_ERROR;}
 const bool success=plan.chance>=100||(plan.chance>0&&(int)g_Random(100)<plan.chance);const bool lost=transform&&!success&&r.failure==-1;
 int counts[128],places[128],xs[128],ys[128];bool detached[128]={0},remainder[128]={0},placed[64]={0};bool mainPlaced=false;result=PT_COMPOSE_TABLE_ERROR;
 for(int s=0;s<plan.poolCount;++s){counts[s]=pool[s]?Item[pool[s]].GetStackNum():0;places[s]=pos_builditem;xs[s]=s;ys[s]=0;if(pool[s]&&plan.used[s]){int at=Player[player].m_ItemList.FindSame(pool[s]);if(at<=0||at>=MAX_PLAYER_ITEM){for(int clean=0;clean<created;++clean)ItemSet.Remove(outputs[clean]);return PT_COMPOSE_STALE;}const PlayerItem& entry=Player[player].m_ItemList.m_Items[at];places[s]=entry.nPlace;xs[s]=entry.nX;ys[s]=entry.nY;}}
 for(s=0;s<plan.poolCount&&valid;++s)if(plan.used[s]){if(!Player[player].m_ItemList.Remove(pool[s]))valid=false;else detached[s]=true;}
 if(valid&&transform&&!lost){Item[pool[plan.main]]=success?updated:failed;ItemPos pos;if(!Player[player].m_ItemList.SearchPosition(original.GetWidth(),original.GetHeight(),&pos,true)||!Player[player].m_ItemList.Add(pool[plan.main],pos.nPlace,pos.nX,pos.nY,false)){valid=false;result=PT_COMPOSE_NO_ROOM;}else mainPlaced=true;}
 for(int o=0;o<created&&valid&&success;++o){ItemPos pos;if(!Player[player].m_ItemList.SearchPosition(Item[outputs[o]].GetWidth(),Item[outputs[o]].GetHeight(),&pos,true)||!Player[player].m_ItemList.Add(outputs[o],pos.nPlace,pos.nX,pos.nY,false)){valid=false;result=PT_COMPOSE_NO_ROOM;}else placed[o]=true;}
 for(int rem=0;rem<plan.poolCount&&valid;++rem)if(detached[rem]&&(!transform||rem!=plan.main)&&counts[rem]>plan.used[rem]){Item[pool[rem]].SetStackNum(counts[rem]-plan.used[rem]);remainder[rem]=Player[player].m_ItemList.Add(pool[rem],places[rem],xs[rem],ys[rem],false)!=0;if(!remainder[rem])valid=false;}
 if(valid&&!Player[player].Pay(fee)){valid=false;result=PT_COMPOSE_NO_MONEY;}
 if(valid){for(int used=0;used<plan.poolCount;++used)if(detached[used]&&!remainder[used]&&(!transform||used!=plan.main||lost))ItemSet.Remove(pool[used]);for(int no=0;no<created&&!success;++no)ItemSet.Remove(outputs[no]);g_DebugLog("[Workshop] player=%d station=%d recipe=%d chance=%d success=%d fee=%d",player,station,r.id,plan.chance,success,fee);return success?(transform?PT_COMPOSE_UPGRADE_OK:PT_COMPOSE_OK):(lost?PT_COMPOSE_FAILED_LOST:PT_COMPOSE_FABAO_FAILED);}
 if(mainPlaced)Player[player].m_ItemList.Remove(pool[plan.main]);for(int rb=0;rb<created;++rb){if(placed[rb])Player[player].m_ItemList.Remove(outputs[rb]);ItemSet.Remove(outputs[rb]);}
 for(rb=0;rb<plan.poolCount;++rb)if(remainder[rb])Player[player].m_ItemList.Remove(pool[rb]);if(transform)Item[pool[plan.main]]=original;
 for(int back=0;back<plan.poolCount;++back)if(detached[back]){Item[pool[back]].SetStackNum(counts[back]);if(!Player[player].m_ItemList.Add(pool[back],places[back],xs[back],ys[back],false))g_DebugLog("[Workshop] rollback invariant player=%d slot=%d",player,back);}return result;
#else
 return PT_COMPOSE_NO_RECIPE;
#endif
}
