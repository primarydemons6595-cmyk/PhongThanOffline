// Reconstructed rules. All tuples below refer to original PC item tables.
// Unknown random rates are explicit server configuration, never client input.
#include "PhongThanPetSkillData.inl"
static bool PhongThanNearXichTungTu(int);
static int PtPetTrainingRate(const char* key,int fallback){
 KIniFile ini;ini.Load("\\settings\\PhongThanPetRestore.ini");int v=fallback;ini.GetInteger("Training",key,fallback,&v);return v>=0&&v<=100?v:-1;
}
static const PT_PET_SKILL_ROW* PtPetSkillMaterial(int material){
 for(int i=0;i<sizeof(ptPetSkillRows)/sizeof(ptPetSkillRows[0]);++i)if(ptPetSkillRows[i].Material==material)return ptPetSkillRows+i;return 0;
}
static void PtPetInitialAttributes(PT_PET_DATA& p,int kind){
 // Configured restoration: 1..5 initial points, +5 in the species attribute.
 for(int i=0;i<4;++i)p.BaseAttributes[i]=p.Attributes[i]=1+g_Random(5);
 int dominant=kind==0?3:kind==1?2:kind==2?1:0;
 p.BaseAttributes[dominant]+=5;p.Attributes[dominant]=p.BaseAttributes[dominant];
}
static int PtPetFirstSkill(int player){
 int choices[58],count=0;
 for(int i=0;i<sizeof(ptPetSkillRows)/sizeof(ptPetSkillRows[0]);++i){
  int skill=ptPetSkillRows[i].Skill;
  if(Npc[Player[player].m_nIndex].m_SkillList.GetLevel(skill)>0)choices[count++]=skill;
 }
 return count?choices[g_Random(count)]:0;
}
#include "PhongThanPetPairTraining.inl"
// Return 1=success, 2=charged random failure, negatives=no charge.
int PhongThanPetTraining(int player,int action,int value,int slot,int boost){
 PT_PET_RUNTIME*r=PtPetLoad(player);
 if(!r||r->Journal)return -2;
 if(action!=9&&(!r->Data.Owned||r->Data.Strength<=0||(r->Data.Flags&PT_PET_REST)))return -5;
 if(Player[player].GetLockState()||Player[player].CheckTrading()||(action==9?!PhongThanNearXichTungTu(player):!PtPetAtSage(player)))return -3;
 if(action==8)return PtPetSecondPurification(player);
 PT_PET_DATA next=r->Data;PhongThanQuestExchangeRow needs[4],rewards[1];int nc=0,rc=0,cash=0,rate=100;bool won=true;
 if(action==1){ // 10 raw Hoan -> one matching refined Hoan.
  if(value<0||value>7)return -1;
  if(!PtPetNeed(needs[nc++],3,546+value,0,10)||!PtPetNeed(rewards[0],3,5108+value,0,1))return -1;
  cash=1000000;rate=PtPetTrainingRate("RefineChance",50);rc=1;
 }else if(action==2){ // First purification.
  if(value<0||value>3||next.PurifyStage||next.Spirit<300)return -5;
  int skill=PtPetFirstSkill(player);if(!skill)return -6;
  if(!PtPetNeed(needs[nc++],3,5108+value*2,0,10)||!PtPetNeed(needs[nc++],8,415,2,1))return -1;
  cash=5000000;rate=PtPetTrainingRate(next.Type==1||next.Type==2?"PurifyWeakChance":"PurifyChance",next.Type==1||next.Type==2?30:50);
  next.PurifyStage=1;next.Type=13+value*2;next.FeedValue=0;
  PtPetInitialAttributes(next,value);next.Aptitude=1+g_Random(5);next.Sex=0;
  ZeroMemory(next.Skills,sizeof(next.Skills));next.Skills[0]=skill|(1<<16);
 }else if(action==3){ // Assign random Am/Duong; repeat allowed for pairing.
  if(next.PurifyStage<1)return -5;
  if(!PtPetNeed(needs[nc++],3,41,0,10)||!PtPetNeed(needs[nc++],8,415,2,1))return -1;
  cash=5000000;next.Sex=1+g_Random(2);
 }else if(action==4){
  if(next.PurifyStage<1)return -5;
  if(!PtPetNeed(needs[nc++],8,1027,2,1))return -1;
  next.Aptitude=1+g_Random(5);
 }else if(action==5){ // Learn/upgrade one of the four actual skill slots.
  const PT_PET_SKILL_ROW* row=PtPetSkillMaterial(value);
  if(!row||!PtPetSlotOpen(next,slot)||boost<0||boost>2)return -1;
  if(Npc[Player[player].m_nIndex].m_SkillList.GetLevel(row->Skill)<=0)return -6;
  int old=next.Skills[slot],level=old?(old>>16)+1:1;
  if((old&&(old&65535)!=row->Skill)||level>4)return -5;
  for(int i=0;i<4;++i)if(i!=slot&&(next.Skills[i]&65535)==row->Skill)return -5;
  if(!PtPetNeed(needs[nc++],3,value,0,level)||!PtPetNeed(needs[nc++],7,row->Book,row->Skill,level))return -1;
  if(boost&&!PtPetNeed(needs[nc++],8,boost==1?1409:5744,2,level))return -1;
  cash=2500000*level;rate=PtPetTrainingRate("LearnChance",50);
  if(boost==1&&rate>=0){rate+=10;if(rate>100)rate=100;}if(boost==2)rate=100;
  next.Skills[slot]=row->Skill|(level<<16);
 }else if(action==6){ // Identify a generic elixir into one skill of that tier.
  if(value<1||value>3)return -1;
  int choices[58],count=0;
  for(int i=0;i<sizeof(ptPetSkillRows)/sizeof(ptPetSkillRows[0]);++i)if(ptPetSkillRows[i].Tier==value)choices[count++]=i;
  if(!count||!PtPetNeed(needs[nc++],3,553+value,0,1)||!PtPetNeed(rewards[0],3,ptPetSkillRows[choices[g_Random(count)]].Material,0,1))return -1;
  cash=100000;rc=1;
 }else if(action==7){ // Revert identified elixir with original Linh Quang Kinh.
  const PT_PET_SKILL_ROW* row=PtPetSkillMaterial(value);if(!row)return -1;
  if(!PtPetNeed(needs[nc++],3,value,0,1)||!PtPetNeed(needs[nc++],8,1026,2,1)||!PtPetNeed(rewards[0],3,553+row->Tier,0,1))return -1;
  rc=1;
 }else if(action==9){ // Craft unappraised original PC elixirs at Xich Tung Tu.
  if(value<1||value>3||boost<0||boost>1)return -1;
  if(!PtPetNeed(needs[nc++],3,value==1?1012:552+value,0,5)||
     !PtPetNeed(rewards[0],3,553+value,0,1))return -1;
  if(boost&&!PtPetNeed(needs[nc++],8,1028,2,value))return -1;
  const int costs[3]={2000000,10000000,50000000};cash=costs[value-1];
  rate=boost?100:PtPetTrainingRate("CraftChance",50);rc=1;
 }else return -1;
 if(rate<0)return -1;won=g_Random(100)<rate;
 if(!won){next=r->Data;rc=0;}
 PtPetLog(next,won?50+action:70+action,value);
 if(!PtPetExchangeRows(player,r,next,needs,nc,rewards,rc,cash))return -4;
 if(won&&action==2)PtPetRemoveVisual(r);
 return won?1:2;
}
#ifndef PHONGTHAN_PET_NO_LUA
int LuaPetTrainingRestored(Lua_State*L){
 if(Lua_GetTopIndex(L)<2){Lua_PushNumber(L,-1);return 1;}
 int args[4]={0,0,0,0};
 for(int i=0;i<Lua_GetTopIndex(L)&&i<4;++i){double v=Lua_ValueToNumber(L,i+1);if(!Lua_IsNumber(L,i+1)||v<0||v>10000||v!=(int)v){Lua_PushNumber(L,-1);return 1;}args[i]=(int)v;}
 Lua_PushNumber(L,PhongThanPetTraining(GetPlayerIndex(L),args[0],args[1],args[2],args[3]));return 1;
}
int LuaPetTrainingInfoRestored(Lua_State*L){
 PT_PET_DATA p;ZeroMemory(&p,sizeof(p));PhongThanPetRead(GetPlayerIndex(L),&p);
 Lua_PushNumber(L,p.PurifyStage);Lua_PushNumber(L,p.FeedValue);Lua_PushNumber(L,p.Sex);
 for(int i=0;i<4;++i)Lua_PushNumber(L,p.Skills[i]);return 7;
}
#endif
