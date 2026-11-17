#include <windows.h>
#include <stdio.h>
#include <vector>
#include "../../Headers/PhongThanStarterWeapon.h"
#include "../../Headers/PhongThanStarterSpawn.h"
#include "../../Headers/PhongThanSpawn.h"
#include "../../Headers/PhongThanBotTraining.h"
#include "../../Headers/PhongThanBotSurvival.h"
#include "../../Headers/PhongThanBotPartySpeech.h"
#include "../../Headers/PhongThanBotName.h"
#include "../../Headers/PhongThanPlayerSleep.h"
#include "../../Headers/PhongThanBotEquipment.h"
#include "../../Headers/PhongThanRewardOwner.h"
#include "../../Headers/PhongThanBotSpeech.h"
#include "../../Headers/PhongThanBotNpcVisit.h"
#include "../../Headers/PhongThanBotRecovery.h"
static DWORD hash(const BYTE* p,int n){DWORD h=2166136261UL;while(n--)h=(h^*p++)*16777619UL;return h;}
int main(int argc,char** argv){
 if(argc==1){
  if(PTBotRecoverySurplus(135,1,true)!=125||PTBotRecoverySurplus(134,2,true)!=129||PTBotRecoverySurplus(9,1,true)||PTBotRecoverySurplus(4,2,true)||PTBotRecoverySurplus(100,1,false)||PTBotRecoverySurplus(100,3,true))return 44;
  for(int kind=1;kind<=2;++kind)for(int total=1;total<=500;++total)for(int stack=1;stack<=total;++stack){int sale=0,reserve=0;bool ok=PTBotRecoverySalePlan(total,kind,stack,true,sale,reserve);int keep=kind==1?10:5;if(ok&&(sale<1||sale>stack||reserve+sale!=stack||total-sale<keep))return 47;if(ok!=(total>keep))return 48;if(PTBotRecoverySalePlan(total,kind,stack,false,sale,reserve))return 49;}
  if(PTBotCarryEscapeScrolls(203,200,1,5)!=3||PTBotCarryEscapeScrolls(202,200,1,5)!=2||PTBotCarryEscapeScrolls(200,200,1,5)||PTBotCarryEscapeScrolls(203,200,1,3)||PTBotCarryEscapeScrolls(204,200,1,5)||PTBotCarryEscapeScrolls(203,200,0,5)||PTBotCarryEscapeScrolls(203,200,3,5)!=1)return 50;
  PTBotProgressBudget budget;if(!budget.Observe(100,1,1000,500)||!budget.Observe(1099,1,1000,500)||budget.Observe(1100,1,1000,500)||budget.Observe(1599,2,1000,500)||!budget.Observe(1600,2,1000,500)||!budget.Observe(2500,3,1000,500)||!budget.Observe(3499,3,1000,500)||budget.Observe(3500,3,1000,500))return 45;
  PTBotProgressBudget wrap;if(!wrap.Observe(0xffffff00UL,1,1000,500)||!wrap.Observe(0x00000100UL,1,1000,500)||wrap.Observe(0x00000400UL,1,1000,500)||wrap.Observe(0x00000500UL,2,1000,500)||!wrap.Observe(0x00000600UL,2,1000,500))return 46;

  PTBotNpcVisits visits;if(!visits.Acquire(0,123,100)||!visits.Acquire(1,123,100)||visits.Acquire(2,123,100)||!visits.Acquire(2,124,100)||!visits.Acquire(3,123,8101))return 42;visits.Release(3);if(!visits.Acquire(4,123,8101)||!visits.Acquire(5,123,8101)||visits.Acquire(6,123,8101))return 43;
  static int distances[256][256];int f,t,k;
  for(f=0;f<256;++f)for(t=0;t<256;++t)distances[f][t]=f==t?0:10000;
  for(k=0;k<sizeof(PTBotEdges)/sizeof(PTBotEdges[0]);++k){int a=PTBotEdges[k].From-1000,b=PTBotEdges[k].To-1000;if(a>=0&&a<256&&b>=0&&b<256&&a!=b)distances[a][b]=1;}
  for(k=0;k<256;++k)for(f=0;f<256;++f)for(t=0;t<256;++t)if(distances[f][k]+distances[k][t]<distances[f][t])distances[f][t]=distances[f][k]+distances[k][t];
  for(f=0;f<256;++f)for(t=0;t<256;++t){int first=-1,hops=PTBotRoute(f+1000,t+1000,&first),expected=distances[f][t]>=10000?-1:distances[f][t];if(hops!=expected||PTBotRoute(f+1000,t+1000)!=hops)return 38;if(hops>0&&(first<0||PTBotEdges[first].From!=f+1000||distances[PTBotEdges[first].To-1000][t]!=hops-1)){printf("ROUTE_FIRST_FAIL from=%d to=%d hops=%d first=%d edge=%d->%d remaining=%d\n",f+1000,t+1000,hops,first,first>=0?PTBotEdges[first].From:0,first>=0?PTBotEdges[first].To:0,first>=0?distances[PTBotEdges[first].To-1000][t]:-99);return 39;}}

  if(!PTBotCarryNeedsShop(160,200)||PTBotCarryNeedsShop(159,200)||!PTBotCarryCanPick(150,200,10,1,false)||PTBotCarryCanPick(160,200,10,1,false)||!PTBotCarryCanPick(190,200,10,1,true)||PTBotCarryCanPick(195,200,10,1,true)||PTBotCarryCanPick(209,200,0,1,true)||PTBotCarryCanPick(150,200,30,2,false))return 37;
  int blocked=-1;for(k=0;k<sizeof(PTBotEdges)/sizeof(PTBotEdges[0]);++k)if(PTBotEdges[k].From==1014&&PTBotEdges[k].To==1006)blocked=k;
  int alternative=-1;if(blocked<0||PTBotRouteWithout(1014,1002,blocked,&alternative)<1||alternative<0||alternative==blocked||PTBotEdges[alternative].From!=1014)return 40;
  for(f=1001;f<=1025;++f)for(t=1001;t<=1025;++t){int first=-1,hops=PTBotRouteWithout(f,t,blocked,&first);if(hops>0&&(first==blocked||PTBotEdges[first].From!=f))return 41;}
  for(int last=-1;last<8;++last)for(unsigned long seed=0;seed<100;++seed){int pick=PTBotSpeechVariant(seed,last,8);if(pick<0||pick>=8||(last>=0&&pick==last))return 36;}
  if(!PhongThanRewardOwnerAvailable(-999,true,true)||PhongThanRewardOwnerAvailable(-999,true,false)||PhongThanRewardOwnerAvailable(-1,false,true)||!PhongThanRewardOwnerAvailable(0,false,true))return 35;
  if(!PTBotShouldEquip(true,true,0,true,false,0)||PTBotShouldEquip(true,true,0,false,true,0)||PTBotShouldEquip(false,true,0,true,false,0)||PTBotShouldEquip(true,false,0,true,false,0))return 34;
  if(PTBotConservativeCeiling(1)!=5||PTBotConservativeCeiling(5)!=5||PTBotConservativeCeiling(13)!=13)return 32;
  for(int population=1;population<=128;++population){int batch=PTBotDecisionBatch(population);if(batch<1||batch>8||batch*18<population)return 33;}
  if(!PTBotShouldEquip(true,true,100,false,true,90)||PTBotShouldEquip(false,true,100,false,true,90)||PTBotShouldEquip(true,false,100,false,true,90)||PTBotShouldEquip(true,true,90,false,true,100)||PTBotShouldEquip(true,true,100,false,false,90)||!PTBotShouldEquip(true,true,100,true,false,0))return 30;
  if(PTBotTrainScore(11,5,0,0,0)>=0||PTBotTrainScore(11,5,0,0,0,true)<0||PTBotTrainScore(11,50,0,0,0,true)>=0)return 31;
  if(PTPlayerShouldShowSleep(true,0,false,999999)||!PTPlayerShouldShowSleep(false,0,false,3601)||PTPlayerShouldShowSleep(false,1,false,999999)||PTPlayerShouldShowSleep(false,0,true,999999)||PTPlayerShouldShowSleep(false,0,false,3600))return 29;
  char party[256];PTBotFoldPartySpeech("MOI TUI VOI!",party,sizeof(party));if(PTBotPartySpeechMode(party)!=1||PTBotPartySpeechMode("ban co to doi chua?")!=0||PTBotPartySpeechMode("vao doi tui di")!=2||PTBotPartySpeechMode("ok!")!=1||PTBotPartySpeechMode("dong y")!=1)return 27;
  char name[25];if(!PTBotNameField("4265204e612047616d696e67",2,name)||strcmp(name,"Be Na Gaming")||PTBotNameField("41420943",2,name)||PTBotNameField("../BadName",1,name)||!PTBotNameField("Hoang_Killa",1,name))return 28;
  for(int p=0;p<3;++p){int map,x,y;POINT revive;if(!PhongThanStarterSpawn(p,map,x,y)||!PhongThanResolveRevivalPoint(map,0,&revive)||revive.x!=x||revive.y!=y||!PhongThanResolveRevivalPoint(map,7,&revive)||revive.x!=x||revive.y!=y)return 1;
   PHONGTHAN_CHARACTER_ITEM_RECORD item;if(!PhongThanInitializeStarterWeapon(item,p)||item.TemplateRow!=90+p*10||item.Container!=2||item.SlotX!=3||PhongThanStarterWeaponClass(item.ScriptParam)!=p)return 2;
  }
  if(!PTBotShouldReturnForTraining(30000,60000,1037,1002,false,false,1,false)||PTBotShouldReturnForTraining(29999,60000,1037,1002,false,false,1,false)||PTBotShouldReturnForTraining(30000,60000,1002,1002,false,false,1,false)||PTBotShouldReturnForTraining(30000,60000,1037,1002,true,false,1,false)||PTBotShouldReturnForTraining(30000,60000,1037,1002,false,true,1,false)||PTBotShouldReturnForTraining(30000,60000,1037,1002,false,false,0,false)||PTBotShouldReturnForTraining(30000,60000,1037,1002,false,false,1,true))return 33;
  if(PTBotTrainScore(30,30,13,0,0)<0||PTBotTrainScore(30,30,256,0,0)>=0)return 32;
  if(PTBotTrainScore(1,20,0,0,0)>=0||PTBotTrainScore(10,1,0,0,0)>=0||PTBotTrainScore(10,10,-1,0,0)>=0)return 20;
  if(PTBotTrainScore(1,5,1,0,0)<0||PTBotTrainScore(1,6,1,0,0)>=0)return 24;
  if(PTBotTrainScore(1,1,1,0,0)>=PTBotTrainScore(1,3,1,0,0)||PTBotTrainScore(30,30,1,0,0)>=PTBotTrainScore(30,28,1,0,0))return 21;
  int edge=-1;if(PTBotRoute(1021,1002,&edge)!=3||edge<0||PTBotEdges[edge].From!=1021||PTBotRoute(1002,1002)!=0||PTBotRoute(0,1002)>=0)return 22;
  if(PTBotTrainScore(10,10,0,100,3000)<=PTBotTrainScore(10,10,1,0,0))return 23;
  if(!PTBotAwaitingTownRevival(0,true,false)||!PTBotAwaitingTownRevival(0,false,true)||PTBotAwaitingTownRevival(30,false,true)||PTBotAwaitingTownRevival(30,false,false))return 25;
  if(!PTBotTownRevivalConfirmed(30,1002,51584,102144,1002,51584,102144)||PTBotTownRevivalConfirmed(0,1002,51584,102144,1002,51584,102144)||PTBotTownRevivalConfirmed(30,1007,51584,102144,1002,51584,102144)||PTBotTownRevivalConfirmed(30,1002,54362,105436,1002,51584,102144))return 26;
  printf("PASS 3 professions: starter weapon/revival; dynamic level/danger/route/risk training decisions.\n");return 0;
 }
 if(argc!=3)return 3;
 if(strcmp(argv[1],"--inspect"))return 4;
 FILE* f=fopen(argv[2],"rb");if(!f)return 5;fseek(f,0,SEEK_END);int length=ftell(f);rewind(f);
 if(length<424||length>65552){fclose(f);return 6;}std::vector<BYTE> bytes(length);bool ok=fread(&bytes[0],1,length,f)==length;fclose(f);if(!ok)return 7;
 DWORD* h=(DWORD*)&bytes[0];PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[16];
 if(h[0]!=0x43544850||h[1]!=1||h[2]!=length-16||h[3]!=hash(&bytes[16],length-16)||!PhongThanValidateCharacterState(s,length-16)||strncmp((char*)s->AccountName,"bot",3))return 8;
 printf("role=%s profession=%u level=%d revision=%lu items=%lu revive=%d,%d,%d\n",s->RoleName,s->Profession,s->FightLevel,s->Revision,s->ItemCount,s->ReviveMapId,s->ReviveX,s->ReviveY);
 const PHONGTHAN_CHARACTER_TASK_RECORD* tasks=PhongThanCharacterTasks(s);for(unsigned ti=0;ti<s->TaskCount;++ti)if(tasks[ti].TaskId==4960||tasks[ti].TaskId==4963||tasks[ti].TaskId==4964||tasks[ti].TaskId==4700||tasks[ti].TaskId==1||tasks[ti].TaskId==2||tasks[ti].TaskId==3||tasks[ti].TaskId==21){char taskValue[17];memcpy(taskValue,tasks[ti].Value,16);taskValue[16]=0;int value=atoi(taskValue);printf("task=%d value=%d\n",tasks[ti].TaskId,value);}
 const PHONGTHAN_CHARACTER_ITEM_RECORD* items=PhongThanCharacterItems(s);
 for(unsigned i=0;i<s->ItemCount;++i)printf("item=%u row=%d container=%d slot=%d,%d starterClass=%d dur=%d physical=%d magic=%d locks=%d/%d/%d\n",i,items[i].TemplateRow,items[i].Container,items[i].SlotX,items[i].SlotY,PhongThanStarterWeaponClass(items[i].ScriptParam),items[i].Durability,items[i].PhysicalValue,items[i].MagicValue,items[i].LockSell,items[i].LockTrade,items[i].LockDrop);
 return 0;
}
