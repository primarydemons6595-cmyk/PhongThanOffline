// Original seven-trigram treasure: the map is consumed only by a successful dig.
#include "../../../Headers/PhongThanTreasureData.h"
namespace PtTreasure {
struct Context{DWORD actor,serial,until;int item,roll,amount,part;};
static Context contexts[MAX_PLAYER];
static bool Live(int p){return p>0&&p<MAX_PLAYER&&PhongThanIsLivePlayer(p)&&!Player[p].GetLockState()&&!Player[p].CheckTrading()&&Npc[Player[p].m_nIndex].m_CurrentLife>0;}
static int Map(int map){return map>0&&map<100?map+1000:map;}
static bool Position(int p,int& distance){
 distance=-1;if(!Live(p))return false;int actor=Player[p].m_nIndex,w=Npc[actor].m_SubWorldIndex;
 if(w<0||w>=MAX_SUBWORLD||Map(Player[p].m_cTask.GetSaveVal(381))!=SubWorld[w].m_SubWorldID)return false;
 int x,y;Npc[actor].GetMpsPos(&x,&y);x=x/32-Player[p].m_cTask.GetSaveVal(382);y=y/32-Player[p].m_cTask.GetSaveVal(383);
 if(x>100000||x<-100000||y>100000||y<-100000)return false;
 __int64 d=(__int64)x*x+(__int64)y*y;distance=d>2147483647?2147483647:(int)d;return true;
}
static bool Valid(int p){
 if(!Live(p))return false;Context& c=contexts[p];
 return c.serial&&c.actor==Npc[Player[p].m_nIndex].m_dwID&&(int)(c.until-GetTickCount())>=0&&PtTravel::Owned(p,c.item)&&Item[c.item].GetID()==c.serial&&Item[c.item].GetGenre()==item_magicscript&&Item[c.item].GetDetailType()==110;
}
static int Accept(int p,int trigram){
 if(!Live(p)||trigram<1||trigram>7||Player[p].m_cTask.GetSaveVal(382)!=0||Player[p].m_cTask.GetSaveVal(826)!=0||Player[p].m_cTask.GetSaveVal(561)!=0)return 0;
 int world=Npc[Player[p].m_nIndex].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID!=1021)return 0;
 int map=0,x=0,y=0;bool found=false;
 for(int attempt=0;attempt<256&&!found;++attempt){
  const PTTreasurePoint& at=ptTreasurePoints[g_Random(PT_TREASURE_POINT_COUNT)];if(at.trigram!=trigram)continue;
  map=at.map;int target=g_SubWorldSet.SearchWorld(map);if(target<0||target>=MAX_SUBWORLD)continue;
  x=at.x+(int)g_Random(41)-20;y=at.y+(int)g_Random(41)-20;
  if(SubWorld[target].TestBarrier(x*32,y*32)!=0)continue;
  int region,cx,cy,ox,oy;SubWorld[target].Mps2Map(x*32,y*32,&region,&cx,&cy,&ox,&oy);found=region>=0;
 }
 if(!found)return 0;
 PhongThanQuestExchangeRow need,reward;need.count=reward.count=1;
 if(!PhongThanDecodeQuestItemTuple(3,14+trigram,0,0,0,0,need.item)||!PhongThanDecodeQuestItemTuple(6,1,110,0,0,0,reward.item))return 0;
 if(!PhongThanQuestExchange(p,382,0,x,&need,1,&reward,1,false,0,true))return 0;
 KPlayer& actor=Player[p];actor.m_cTask.SetSaveVal(381,map,TRUE);actor.m_cTask.SetSaveVal(383,y,TRUE);actor.m_cTask.SetSaveVal(378,0,TRUE);actor.m_cTask.SetSaveVal(380,KSG_GetCurSec(),TRUE);actor.m_cTask.SetSaveVal(395,0,TRUE);actor.m_uMustSave=SAVE_REQUEST;return 1;
}
static int Dig(int p){
 int distance;if(!Valid(p)||Player[p].m_cTask.GetSaveVal(382)<=0||!Position(p,distance)||distance>25)return 0;
 Context& c=contexts[p];if(!c.roll){c.roll=1+g_Random(1000);c.amount=1+g_Random(3);c.part=0;if((c.roll>400&&c.roll<=575)||c.roll>898)c.amount=1+g_Random(2);}
 PhongThanQuestExchangeRow need,reward;need.count=1;int rewardCount=1,money=0,xp=0;
 if(!PhongThanDecodeQuestItemTuple(6,1,110,0,0,0,need.item))return 0;
 int genre=3,detail=0,part=0,level=0,quantity=c.amount;
 if(Player[p].m_cTask.GetSaveVal(826)>0||Player[p].m_cTask.GetSaveVal(561)>0)rewardCount=0;else if(c.roll<=100)detail=77;else if(c.roll<=160)detail=28;else if(c.roll<=230)detail=78;else if(c.roll<=250)detail=80;
 else if(c.roll<=400){genre=0;detail=4;level=1;quantity=1;
  if(!c.part){int first=c.roll<=320?0:c.roll<=370?6:c.roll<=390?10:14;int last=c.roll<=320?5:c.roll<=370?9:c.roll<=390?13:17;c.part=1+first+g_Random(last-first+1);}part=c.part-1;
 }else if(c.roll<=575){rewardCount=0;xp=Npc[Player[p].m_nIndex].m_Level*Npc[Player[p].m_nIndex].m_Level*c.amount;}
 else if(c.roll<=655){detail=62;quantity=1;}else if(c.roll<=695){detail=63;quantity=1;}else if(c.roll<=707){detail=64;quantity=1;}else if(c.roll<=708){detail=65;quantity=1;}
 else if(c.roll<=898){rewardCount=0;if(!c.part)c.part=500+g_Random(701);money=Npc[Player[p].m_nIndex].m_Level*c.part;}
 else{rewardCount=0;xp=Npc[Player[p].m_nIndex].m_Level*Npc[Player[p].m_nIndex].m_Level*c.amount;}
 if(rewardCount){reward.count=quantity;if(!PhongThanDecodeQuestItemTuple(genre,detail,part,level,0,0,reward.item))return 0;}
 if(money>0&&(Player[p].m_ItemList.GetEquipmentMoney()>2147483647-money||!Player[p].Earn(money)))return 0;
 int expected=Player[p].m_cTask.GetSaveVal(382);
 if(!PhongThanQuestExchange(p,382,expected,0,&need,1,&reward,rewardCount,true,0,true,c.item)){if(money>0)Player[p].Pay(money);return 0;}
 if(xp>0)Player[p].DirectAddExp(xp);
 Player[p].m_cTask.SetSaveVal(383,0,TRUE);Player[p].m_cTask.SetSaveVal(395,max(1,(int)KSG_GetCurSec()-Player[p].m_cTask.GetSaveVal(380)),TRUE);
 // Preserve progression for maps granted by the original seven-stage quest.
 int step=Player[p].m_cTask.GetSaveVal(826);if(step>=1&&step<=7&&Player[p].m_cTask.GetSaveVal(826+step)==1)Player[p].m_cTask.SetSaveVal(826+step,2,TRUE);
 c.serial=0;Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}
}
int LuaTreasureItem(Lua_State* L){
 int p=GetPlayerIndex(L),result=0,map=0,distance=-1,last=0;int op=(int)Lua_ValueToNumber(L,1),arg=(int)Lua_ValueToNumber(L,2);
 if(PtTreasure::Live(p)){
  if(op==0&&arg>0&&arg<MAX_ITEM&&PtTravel::Owned(p,arg)&&Item[arg].GetGenre()==item_magicscript&&Item[arg].GetDetailType()==110){
   PtTreasure::Context& c=PtTreasure::contexts[p];if(c.serial!=Item[arg].GetID()||c.actor!=Npc[Player[p].m_nIndex].m_dwID)ZeroMemory(&c,sizeof(c));c.item=arg;c.serial=Item[arg].GetID();c.actor=Npc[Player[p].m_nIndex].m_dwID;c.until=GetTickCount()+120000;
   map=PtTreasure::Map(Player[p].m_cTask.GetSaveVal(381));last=Player[p].m_cTask.GetSaveVal(378);PtTreasure::Position(p,distance);
   result=Player[p].m_cTask.GetSaveVal(382)>0?1:0;if(distance>=0)Player[p].m_cTask.SetSaveVal(378,distance,TRUE);
  }else if(op==1)result=PtTreasure::Accept(p,arg);else if(op==2)result=PtTreasure::Dig(p);
  else if(op==3&&Player[p].m_cTask.GetSaveVal(382)>0){
   bool exists=false;KItemList& bag=Player[p].m_ItemList;
   for(PlayerItem* it=bag.GetFirstItem();it;it=bag.GetNextItem())if(it->nIdx>0&&it->nIdx<MAX_ITEM&&Item[it->nIdx].GetGenre()==item_magicscript&&Item[it->nIdx].GetDetailType()==110)exists=true;
   int w=Npc[Player[p].m_nIndex].m_SubWorldIndex;
   if(!exists&&w>=0&&w<MAX_SUBWORLD&&SubWorld[w].m_SubWorldID==1021){
    PhongThanQuestExchangeRow reward;reward.count=1;
    if(PhongThanDecodeQuestItemTuple(6,1,110,0,0,0,reward.item)){int previous=Player[p].m_cTask.GetSaveVal(378);result=PhongThanQuestExchange(p,378,previous,previous?0:1,0,0,&reward,1);if(result)Player[p].m_uMustSave=SAVE_REQUEST;}
   }
  }
 }
 Lua_PushNumber(L,result);Lua_PushNumber(L,map);Lua_PushNumber(L,distance);Lua_PushNumber(L,last);return 4;
}
int PhongThanOpenTreasureItem(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_magicscript||Item[item].GetDetailType()!=110)return -1;
 if(!PtTreasure::Live(p)||!PtTravel::Owned(p,item))return 0;
 return Player[p].ExecuteScript("\\script\\phongthan\\items\\treasure_map.lua","main",item)?1:0;
}
