static bool BotSummonSkill(Runtime& r,DWORD now){
 static DWORD last[Capacity];int i=&r-bots;KPlayer& p=Player[r.Player];KNpc& npc=Npc[p.m_nIndex];int old=0,type=0;if(p.m_cProfession.GetProfession()!=2||PhongThanSummonInfo(npc.m_Index,old,type)||now-last[i]<15000)return false;last[i]=now;
 for(int id=460;id>=450;--id)if(npc.m_SkillList.GetLevel(id)>0&&PhongThanCastSummon(npc.m_Index,id)){char message[100];sprintf(message,"learned_summon_cast_confirmed skill=%d",id);Log("SKILL",r.Config.Id,message);return true;}return false;
}
static int BotTrainParent(KPlayer& player){
 static KIniFile rules;static bool attempted=false,loaded=false;if(!attempted){attempted=true;loaded=rules.Load("\\settings\\skills.ini")!=0;}if(!loaded)return 0;
 KSkillList& learned=Npc[player.m_nIndex].m_SkillList;int currentLevel=Npc[player.m_nIndex].m_Level;
 for(int detail=0;detail<58;++detail){int id=detail<49?detail+3:450+detail-49;if(PhongThanSkillBookProfession(id)!=player.m_cProfession.GetProfession()||learned.GetLevel(id)>0||PhongThanSkillBookLevel(detail,id)>currentLevel)continue;
  char section[20],parents[512];sprintf(section,"%d",id);rules.GetString(section,"SkillParent","",parents,sizeof(parents));int missing=0,required=0;if(!PhongThanSkillParentsAllow(parents,learned,&missing,&required)&&missing>0&&learned.GetLevel(missing)>0&&learned.GetLevel(missing)<required)return missing;
 }return 0;
}
static int BotAttackSkill(Runtime& r,int target,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& npc=Npc[p.m_nIndex];int basic=PhongThanDefaultPhysicalSkill(r.Config.Profession),best=basic,bestScore=-1;
 int parent=BotTrainParent(p);
 for(int slot=1;slot<MAX_NPCSKILL;++slot){NPCSKILL& learned=npc.m_SkillList.m_Skills[slot];int id=learned.SkillId;if(learned.SkillLevel<=0||learned.TempSkill||!id)continue;
  ISkill* skill=g_SkillManager.GetSkill(id,learned.CurrentSkillLevel);if(!skill||!skill->IsTargetEnemy()||skill->IsAura()||skill->GetSkillStyle()==4||!npc.m_SkillList.CanCast(id,g_SubWorldSet.GetGameTime()))continue;
  int cost=skill->GetSkillCost(&npc);if(cost<0||!npc.Cost(skill->GetSkillCostType(),cost,TRUE))continue;
  if(!npc.SetActiveSkill(slot))continue;int a=-1,b=target,can=skill->CanCastSkill(npc.m_Index,a,b);if(can!=1&&can!=-1)continue;
  int score=id==basic?0:1000+learned.SkillLevel*10+skill->GetAttackRadius()/16;
  // Train usable profession skills by actual casts; favor one not yet at its learned cap.
  if(id!=basic&&learned.NextSkillExp>0)score+=200;
  if(id==parent)score+=1500;
  if(can==1)score+=100;if(score>bestScore){bestScore=score;best=id;}
 }
 int chosenSlot=npc.m_SkillList.FindSame(best);if(chosenSlot)npc.SetActiveSkill(chosenSlot);return best;
}
static void BotSkillCastLog(Runtime& r,int skill,DWORD now){
 static DWORD last[Capacity];static int previous[Capacity],level[Capacity];int i=&r-bots;KSkillList& learned=Npc[Player[r.Player].m_nIndex].m_SkillList;int current=learned.GetLevel(skill);
 if(previous[i]==skill&&level[i]==current&&now-last[i]<30000)return;last[i]=now;previous[i]=skill;level[i]=current;char text[150];sprintf(text,"attack_skill_requested skill=%d learned=%d exp=%d next=%d",skill,current,learned.GetExp(skill),learned.GetNextExp(skill));Log("SKILL",r.Config.Id,text);
}
