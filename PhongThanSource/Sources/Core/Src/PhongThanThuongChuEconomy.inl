// Included after QuestExchange in ScriptFuns.cpp. Item payments and rewards
// commit together through the normal inventory transaction with rollback.
#include "PhongThanComposeRecipe.h"
#include "KItemGenerator.h"
#include "../../../Headers/PhongThanUpgradeStats.h"
enum { PT_TC_BUFF_SKILL=4095 };
struct PTThuongChuPaymentContext {int player,silver;};
static bool PTThuongChuPaySilver(void* data){PTThuongChuPaymentContext& c=*(PTThuongChuPaymentContext*)data;return c.silver==0||Player[c.player].m_ItemList.CostMoney(c.silver)!=FALSE;}
int PhongThanThuongChuQuestPayment(int p,int silver,int copper){if(p<=0||p>=MAX_PLAYER||silver<0||copper<0||copper>5||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 PhongThanQuestExchangeRow need;if(!PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,need.item))return 0;need.count=copper;
 int task=5090,old=Player[p].m_cTask.GetSaveVal(task),next=old==2000000000?1:old+1;PTThuongChuPaymentContext context;context.player=p;context.silver=silver;
 return PhongThanEventTicketsPayment(p,&need,copper?1:0,task,old,next,PTThuongChuPaySilver,&context)==1;
}
static bool PTThuongChuNear(int p,int map,int x,int y){KNpc& n=Npc[Player[p].m_nIndex];if(n.m_SubWorldIndex<0||SubWorld[n.m_SubWorldIndex].m_SubWorldID!=map)return false;int px,py;n.GetMpsPos(&px,&py);return abs(px-x*256)<=768&&abs(py-y*512)<=768;}
static bool PTThuongChuStele(int p){return PTThuongChuNear(p,1001,189,191)||PTThuongChuNear(p,1001,212,205);}
static bool PTThuongChuKing(int p,int side){return side==1?PTThuongChuNear(p,1021,237,180):side==2&&PTThuongChuNear(p,1020,159,189);}
static int PTThuongChuUpgradeRule(const KItem& item){static KTabFile recipes;static bool loaded=false;if(!loaded)loaded=recipes.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")!=0;if(!loaded)return 0;
 for(int row=2;row<=recipes.GetHeight();++row){PhongThanComposeRecipe recipe;if(PhongThanReadComposeRecipe(recipes,row,recipe)&&recipe.group==1&&recipe.type==1&&recipe.inputs[0].genre==item_equip&&recipe.inputs[0].detail==item.GetDetailType()&&recipe.inputs[0].part==item.GetParticularType())return recipe.attributeId;}return 0;}
struct PTThuongChuGearPick {int detail,part,level;};
static int PTThuongChuEquipment(int p,bool weapon,int tier,bool rental){
 typedef PTThuongChuGearPick Pick;std::vector<Pick> pool;int profession=Player[p].m_cProfession.GetProfession();
 for(int detail=0;detail<equip_detailnum;++detail){bool isWeapon=detail==equip_meleeweapon||detail==equip_rangeweapon;if(weapon!=isWeapon||(!weapon&&detail!=equip_armor&&detail!=equip_helm&&detail!=equip_boots&&detail!=equip_belt&&detail!=equip_pendant))continue;
  for(int row=0;row<ItemGen.CatalogEquipmentCount(detail);++row){const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(detail,row);if(!e||(!weapon&&e->m_nSetId<=0)||(weapon&&e->m_nSeries!=1000))continue;int required=0,role=-1;bool incompatible=false;
   for(int q=0;q<6;++q){const KEQCP_REQ& req=e->m_aryPropReq[q];if(req.nType==magic_requirelevel)required=req.nPara;if(req.nType==magic_requiremenpai)role=req.nPara;if(req.nType==magic_requireseries){if(role<0&&req.nPara>=0&&req.nPara<=2)role=req.nPara;if(req.nPara!=Npc[Player[p].m_nIndex].m_Series)incompatible=true;}if(req.nType==magic_requiresex&&req.nPara!=Npc[Player[p].m_nIndex].m_nSex)incompatible=true;}
   if(incompatible||role!=profession||required!=tier)continue;Pick pick;pick.detail=detail;pick.part=e->m_nParticularType;pick.level=e->m_nLevel;pool.push_back(pick);
  }
 }
 if(pool.empty())return 0;int first=GetRandomNumber(0,(int)pool.size()-1);
 for(int i=0;i<(int)pool.size();++i){Pick& pick=pool[(first+i)%pool.size()];int index=CreateVngNormalItem(0,pick.detail,pick.part,pick.level,0,0);if(index<=0)continue;
  KItem& item=Item[index];if(weapon?item.GetPhongThanQuality()<2:item.GetPhongThanQuality()!=4){ItemSet.Remove(index);continue;}
  item.LockItem(LOCK_STATE_FOREVER);item.m_CommonAttrib.bLockSell=TRUE;item.m_CommonAttrib.bLockTrade=TRUE;item.m_CommonAttrib.bLockDrop=TRUE;
  if(rental){item.SetExpireTime(KSG_GetCurSec()+3*86400);if(!weapon){int rule=PTThuongChuUpgradeRule(item);if(!rule||!item.ApplyUpgradeState(PhongThanEncodeUpgradeState(6,rule))){ItemSet.Remove(index);continue;}}}
  return index;
 }return 0;
}
void PhongThanThuongChuBuffTick(int p,unsigned long now){if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].m_nIndex<=0||Player[p].m_nIndex>=MAX_NPC)return;
 KNpc& npc=Npc[Player[p].m_nIndex];int expiry=Player[p].m_cTask.GetSaveVal(5088),remaining=expiry>(int)now?expiry-(int)now:0;KStateNode* existing=0;
 for(KStateNode* node=(KStateNode*)npc.m_StateSkillList.GetHead();node;node=(KStateNode*)node->GetNext())if(node->m_SkillID==PT_TC_BUFF_SKILL){existing=node;break;}
 if(existing&&remaining>0&&abs(existing->m_LeftTime-remaining*18)<36)return;
 if(existing){for(int a=0;a<MAX_SKILL_STATE;++a)if(existing->m_State[a].nAttribType>0&&existing->m_State[a].nAttribType<magic_normal_end)npc.ModifyAttrib(npc.m_Index,&existing->m_State[a]);existing->Remove();delete existing;}
 if(!remaining)return;
 KMagicAttrib attrs[3];ZeroMemory(attrs,sizeof(attrs));attrs[0].nAttribType=magic_lifemax_p;attrs[1].nAttribType=magic_manamax_p;attrs[2].nAttribType=Player[p].m_cProfession.GetProfession()==1?magic_magicenhance_p:magic_weapondamageenhance_p;
 for(int k=0;k<3;++k)attrs[k].nValue[0]=10;npc.SetStateSkillEffect(npc.m_Index,PT_TC_BUFF_SKILL,1,attrs,3,remaining*18,TRUE);
}
int PhongThanThuongChuEconomy(int p,int op,int arg){if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].GetLockState()||Player[p].CheckTrading())return 0;
 PhongThanQuestExchangeRow need,reward;ZeroMemory(&need,sizeof(need));ZeroMemory(&reward,sizeof(reward));if(!PhongThanDecodeQuestItemTuple(8,486,2,0,0,0,need.item))return 0;
 int task=0,before=0,after=0,needCount=0,rewardCount=0,prototypeIndex=0,result=0;KItem* prototype=0;
 if(op==1){if(!PTThuongChuKing(p,arg))return 0;task=arg==1?5080:5081;before=Player[p].m_cTask.GetSaveVal(task);if(before<50)return 0;after=before-50;reward.item=need.item;reward.count=1;rewardCount=1;}
 else if(op==5){if(!PTThuongChuStele(p))return 0;task=5073;before=Player[p].m_cTask.GetSaveVal(task);if(before<1200||Player[p].m_cTask.GetSaveVal(5088)>(int)time(NULL))return 0;after=before-1200;}
 else if(op>=2&&op<=4){bool weapon=op==3,rental=op!=4;if(rental?!PTThuongChuStele(p):!(PTThuongChuKing(p,1)||PTThuongChuKing(p,2)))return 0;
  task=5082;before=Player[p].m_cTask.GetSaveVal(task);int merit=op==2?15000:op==3?20000:0;if(before<merit)return 0;after=before-merit;
  need.count=op==2?20:op==3?30:200;needCount=1;
  bool success=true;if(op==4){int chance=GetPrivateProfileIntA("Battle","EquipmentSuccessPercent",50,"settings\\phongthan\\ThuongChu.ini");if(chance<1||chance>100)return 0;success=GetRandomNumber(1,100)<=chance;}
  if(success){int tier=arg;if(op==2&&tier!=80&&tier!=100)return 0;if(op==3&&tier!=90&&tier!=100)return 0;if(op==4){static const int tiers[2]={80,100};tier=tiers[GetRandomNumber(0,1)];weapon=false;}
   prototypeIndex=PTThuongChuEquipment(p,weapon,tier,rental);if(!prototypeIndex)return 0;prototype=&Item[prototypeIndex];if(!PhongThanDecodeQuestItemTuple(0,prototype->GetDetailType(),prototype->GetParticular(),prototype->GetLevel(),prototype->GetSeries(),0,reward.item)){ItemSet.Remove(prototypeIndex);return 0;}reward.count=1;rewardCount=1;
  }else{reward.item=need.item;reward.count=150;rewardCount=1;result=2;}
  // The transaction uses a unique compare-and-set token when no merit is spent.
  if(before==after){task=5089;before=Player[p].m_cTask.GetSaveVal(task);after=before==2000000000?1:before+1;}
 }else return 0;
 int committed=PhongThanQuestExchange(p,task,before,after,&need,needCount,&reward,rewardCount,true,prototype,true);
 if(prototypeIndex)ItemSet.Remove(prototypeIndex);if(!committed)return 0;
 if(op==5){Player[p].m_cTask.SetSaveVal(5088,(int)time(NULL)+2*86400,TRUE);PhongThanThuongChuBuffTick(p,(unsigned long)time(NULL));}
 Player[p].Save();return result?result:1;
}
