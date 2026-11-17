// Reads the authored story tables; progress/rewards belong exclusively to Lua
// callbacks selected from the NPC's current, authenticated dialog.
struct BotStoryNeed {PhongThanQuestItemTuple item;int count;};
struct BotStoryAction {
 DWORD script;int family,profession,level,task,from,to,mainPhase,step;
 std::vector<BotStoryNeed> needs;
};
struct BotStoryKill {int profession,tid,map,task,from,to,item;};
struct BotStoryPlan {
 int action,target,map,task,from,item,badAction;DWORD next,started,last,badUntil;unsigned long first;bool loaded;
 BotStoryPlan():action(-1),target(0),map(0),task(0),from(0),item(0),badAction(-1),next(0),started(0),last(0),badUntil(0),first(0),loaded(false){}
};
static std::map<DWORD,std::vector<int> > botStoryNpcs;
static int botMaoNpcs[3][4],botTrialNpcs[3],botGiaoNpcs[3],botLeQuanNpcs[3];
static const char* botMaoRoles[3][4]={{"giao","store","lo","giao"},{"guide","giao","store","guide"},{"guide","giao","store","guide"}};
static std::vector<BotStoryAction> botStoryActions;
static std::vector<BotStoryKill> botStoryKills;
static BotStoryPlan botStoryPlan[Capacity];
static int BotLuaInt(lua_State* L,int table,const char* key){lua_pushstring(L,key);lua_gettable(L,table);int value=lua_isnumber(L,-1)?(int)lua_tonumber(L,-1):0;lua_pop(L,1);return value;}
static int BotLuaAt(lua_State* L,int table,int key){lua_rawgeti(L,table,key);int value=lua_isnumber(L,-1)?(int)lua_tonumber(L,-1):0;lua_pop(L,1);return value;}
static std::string BotLuaString(lua_State* L,int table,const char* key){lua_pushstring(L,key);lua_gettable(L,table);const char* value=lua_isstring(L,-1)?lua_tostring(L,-1):0;std::string result=value?value:"";lua_pop(L,1);return result;}
static void BotReadStory(DWORD identity,KLuaScript* script,int family){
 lua_State* L=script->m_LuaState;int top=lua_gettop(L);const char* prefix=family==0?"PTG":"PT2";
 char global[40];sprintf(global,"%s_ROLE",prefix);lua_getglobal(L,global);std::string role=lua_isstring(L,-1)?lua_tostring(L,-1):"";lua_pop(L,1);if(role.empty())return;
 sprintf(global,"%s_STEPS",prefix);lua_getglobal(L,global);if(!lua_istable(L,-1)){lua_settop(L,top);return;}int rows=lua_gettop(L),total=lua_getn(L,rows);
 for(int i=1;i<=total;++i){lua_rawgeti(L,rows,i);int row=lua_gettop(L);if(lua_istable(L,row)&&BotLuaString(L,row,"role")==role){
  BotStoryAction a;a.script=identity;a.family=family;a.profession=family?BotLuaInt(L,row,"f"):0;a.level=BotLuaInt(L,row,"level");a.task=BotLuaInt(L,row,"task");a.from=BotLuaInt(L,row,"from");a.to=BotLuaInt(L,row,"to");a.mainPhase=BotLuaInt(L,row,"mainPhase");a.step=i;bool valid=true;
  lua_pushstring(L,"needs");lua_gettable(L,row);if(lua_istable(L,-1)){int needs=lua_gettop(L);for(int n=1;n<=lua_getn(L,needs);++n){lua_rawgeti(L,needs,n);if(lua_istable(L,-1)){int t=lua_gettop(L);BotStoryNeed need;need.count=BotLuaAt(L,t,7);if(need.count>0&&PhongThanDecodeQuestItemTuple(BotLuaAt(L,t,1),BotLuaAt(L,t,2),BotLuaAt(L,t,3),BotLuaAt(L,t,4),BotLuaAt(L,t,5),BotLuaAt(L,t,6),need.item))a.needs.push_back(need);else valid=false;}lua_pop(L,1);}}lua_pop(L,1);
  if(valid&&a.task>0&&a.task<5000&&a.profession>=0&&a.profession<=2)botStoryActions.push_back(a);
 }lua_pop(L,1);}lua_settop(L,top);
 // PTG_KILLS is the actual death-dispatch projection, not a guessed drop table.
 if(family==0){lua_getglobal(L,"PTG_KILLS");if(lua_istable(L,-1)){int table=lua_gettop(L);for(int k=1;k<=lua_getn(L,table);++k){lua_rawgeti(L,table,k);if(lua_istable(L,-1)){int t=lua_gettop(L);BotStoryKill kill;kill.profession=0;kill.tid=BotLuaAt(L,t,1);kill.map=BotLuaAt(L,t,2);kill.task=BotLuaAt(L,t,3);kill.from=BotLuaAt(L,t,4);kill.to=BotLuaAt(L,t,5);kill.item=BotLuaAt(L,t,6);bool seen=false;for(int j=0;j<(int)botStoryKills.size();++j)if(botStoryKills[j].tid==kill.tid&&botStoryKills[j].map==kill.map&&botStoryKills[j].task==kill.task&&botStoryKills[j].from==kill.from)seen=true;if(!seen&&kill.tid>=0&&kill.map>=1000)botStoryKills.push_back(kill);}lua_pop(L,1);}}lua_settop(L,top);}
}
#ifndef PTBOT_STORY_CATALOG_ONLY
static void BotLoadStory(){
 static bool loaded=false;if(loaded)return;loaded=true;std::vector<DWORD> seen;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].m_ActionScriptID){DWORD id=Npc[n].m_ActionScriptID;botStoryNpcs[id].push_back(n);KLuaScript* metadata=(KLuaScript*)g_GetScript(id);if(metadata&&metadata->m_LuaState){lua_State* L=metadata->m_LuaState;int top=lua_gettop(L);lua_getglobal(L,"PTMAO_NPC_CLASS");int f=lua_isnumber(L,-1)?(int)lua_tonumber(L,-1):-1;lua_getglobal(L,"PTMAO_NPC_ROLE");const char* role=lua_isstring(L,-1)?lua_tostring(L,-1):0;if(f>=0&&f<3&&role&&SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID==1002+f)for(int phase=0;phase<4;++phase)if(!strcmp(role,botMaoRoles[f][phase]))botMaoNpcs[f][phase]=n;lua_settop(L,top);const char* roleKeys[]={"PTG_ROLE","PT2_ROLE"};for(int family=0;family<2;++family){lua_getglobal(L,roleKeys[family]);const char* role=lua_isstring(L,-1)?lua_tostring(L,-1):0;int map=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;if(role){if(!strcmp(role,"giao")&&map==1002)botGiaoNpcs[0]=n;if(!strcmp(role,"giao_dao")&&map==1003)botGiaoNpcs[1]=n;if(!strcmp(role,"giao_di")&&map==1004)botGiaoNpcs[2]=n;}lua_settop(L,top);}lua_getglobal(L,"PT_STARTER_TASK");if(lua_isnumber(L,-1)&&(int)lua_tonumber(L,-1)==4700){int career=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID-1002;if(career>=0&&career<3)botLeQuanNpcs[career]=n;}lua_settop(L,top);lua_getglobal(L,"PTTR_ROLE");f=lua_isnumber(L,-1)?(int)lua_tonumber(L,-1):-1;if(f>=0&&f<3&&SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID==1002+f)botTrialNpcs[f]=n;lua_settop(L,top);}bool duplicate=false;for(int j=0;j<(int)seen.size();++j)if(seen[j]==id)duplicate=true;if(duplicate)continue;seen.push_back(id);KLuaScript* script=(KLuaScript*)g_GetScript(id);if(!script||!script->m_LuaState)continue;BotReadStory(id,script,0);BotReadStory(id,script,1);}
 char log[120];sprintf(log,"authored_story_actions=%d kill_rules=%d",botStoryActions.size(),botStoryKills.size());Log("QUEST","system",log);
}
static bool BotStoryState(KPlayer& p,const BotStoryAction& a){int task=a.family?(a.profession==1?1:2):3;return a.profession==p.m_cProfession.GetProfession()&&Npc[p.m_nIndex].m_Level>=a.level&&p.m_cTask.GetSaveVal(a.task)==a.from&&(!a.mainPhase||p.m_cTask.GetSaveVal(task)==a.mainPhase);}
static bool BotStoryHave(KPlayer& p,const BotStoryAction& a){
 for(int n=0;n<(int)a.needs.size();++n){int total=0;for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if(BotBag(item->nPlace)&&item->nIdx>0&&item->nIdx<MAX_ITEM&&PhongThanQuestItemMatches(Item[item->nIdx],a.needs[n].item)){int units=Item[item->nIdx].GetInventoryCount();total+=units>0?units:1;}if(total<a.needs[n].count)return false;}return true;
}
static int BotStoryItemCount(KPlayer& p,const PhongThanQuestItemTuple& tuple){int count=0;for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if(BotBag(item->nPlace)&&item->nIdx>0&&item->nIdx<MAX_ITEM&&PhongThanQuestItemMatches(Item[item->nIdx],tuple)){int n=Item[item->nIdx].GetInventoryCount();count+=n>0?n:1;}return count;}
static int BotStoryRequiredCount(KPlayer& p,KItem& item){int count=BotProgressRequired(p,item);for(int i=0;i<(int)botStoryActions.size();++i){const BotStoryAction& a=botStoryActions[i];if(!BotStoryState(p,a))continue;for(int n=0;n<(int)a.needs.size();++n)if(PhongThanQuestItemMatches(item,a.needs[n].item)&&a.needs[n].count>count)count=a.needs[n].count;}return count;}
static int BotStoryMissingCount(KPlayer& p,KItem& item){int count=BotProgressMissing(p,item);for(int i=0;i<(int)botStoryActions.size();++i){const BotStoryAction& a=botStoryActions[i];if(!BotStoryState(p,a))continue;for(int n=0;n<(int)a.needs.size();++n)if(PhongThanQuestItemMatches(item,a.needs[n].item)){int missing=a.needs[n].count-BotStoryItemCount(p,a.needs[n].item);if(missing>count)count=missing;}}return count;}
static bool BotStoryWantedItem(KPlayer& p,KItem& item){if(BotProgressMissing(p,item)>0)return true;for(int i=0;i<(int)botStoryActions.size();++i){const BotStoryAction& a=botStoryActions[i];if(!BotStoryState(p,a))continue;for(int n=0;n<(int)a.needs.size();++n)if(PhongThanQuestItemMatches(item,a.needs[n].item)&&BotStoryItemCount(p,a.needs[n].item)<a.needs[n].count)return true;}return false;}
static int BotStoryNpc(DWORD script){std::map<DWORD,std::vector<int> >::iterator found=botStoryNpcs.find(script);if(found==botStoryNpcs.end())return 0;for(int i=0;i<(int)found->second.size();++i){int n=found->second[i];if(n>0&&n<MAX_NPC&&Npc[n].m_Kind==kind_dialoger&&Npc[n].m_ActionScriptID==script&&Npc[n].m_RegionIndex>=0)return n;}return 0;}
static PTBotNpcVisits botNpcVisits;
static bool BotMaoLu(Runtime& r,Training& route,DWORD now){
 KPlayer& p=Player[r.Player];int phase=p.m_cTask.GetSaveVal(4960);if(phase>=5||phase<0||r.Config.Profession<0||r.Config.Profession>2)return false;
 KNpc& actor=Npc[p.m_nIndex];int home=1002+r.Config.Profession,current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 static DWORD last[Capacity],began[Capacity],retry[Capacity],diagnosed[Capacity];static int previous[Capacity];int slot=&r-bots;
 if((int)(now-retry[slot])<0)return false;
 if(!began[slot]||previous[slot]!=phase){began[slot]=now;previous[slot]=phase;}
 if(now-began[slot]>90000){retry[slot]=now+45000+(Hash(r.Config.Id,32)^now)%90000;began[slot]=0;route.Path.clear();Log("QUEST",r.Config.Id,"maolu_no_progress_backoff_other_work");return false;}
 if(current!=home){int edge=-1;if(BotRoute(r,current,home,&edge)<1||edge<0)return false;lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(phase==0){lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);if(now-last[slot]<2000)return true;last[slot]=now;int choice=QuestChoice(p,"pt_mao_take");
  if(choice<0){p.ExecuteScript(LOGIN_SCRIPT,"PTMaoOffer",0);return true;}
  PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);
  if(p.m_cTask.GetSaveVal(4960)==1){p.Save();Log("QUEST",r.Config.Id,"maolu_accepted_via_normal_login_offer");}return true;
 }
 int target=botMaoNpcs[r.Config.Profession][phase-1];
 if(!target)return false;if(NpcSet.GetDistance(p.m_nIndex,target)<1024&&!botNpcVisits.Acquire(slot,Npc[target].m_dwID,now)){retry[slot]=now+15000+(Hash(r.Config.Id,32)^now)%45000;return false;}int x=0,y=0;Npc[target].GetMpsPos(&x,&y);if(NpcSet.GetDistance(p.m_nIndex,target)>80){lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,x,y,now,64);return true;}
 lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);if(now-last[slot]<2000)return true;last[slot]=now;
 if(now-diagnosed[slot]>=15000){diagnosed[slot]=now;char message[440];KLuaScript* script=(KLuaScript*)g_GetScript(Npc[target].m_ActionScriptID);sprintf(message,"maolu_dialog_progress phase=%d series=%d npc=%d lastNpc=%d distance=%d token=%lu answers=%d first=%.48s script=%.90s",phase,actor.m_Series,target,p.m_nLastNpcIndex,NpcSet.GetDistance(p.m_nIndex,target),(unsigned long)p.m_UiDialogToken,p.m_nAvailableAnswerNum,p.m_nAvailableAnswerNum>0?p.m_szTaskAnswerFun[0]:"",script?script->m_szScriptName:"");Log("QUEST",r.Config.Id,message);}
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==target){int choice=QuestChoice(p,"pt_mao_claim");if(choice<0)choice=BotServiceChoice(p);if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);if(p.m_cTask.GetSaveVal(4960)>phase){p.Save();botNpcVisits.Release(slot);retry[slot]=now+3000+(Hash(r.Config.Id,32)^now)%12000;began[slot]=0;Log("QUEST",r.Config.Id,"maolu_visit_reward_confirmed_via_npc");}return true;}}
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.EntityId=Npc[target].m_dwID;p.DialogNpc((BYTE*)&request);return true;
}
static int BotStarterTrial(Runtime& r,Training& route,DWORD now){
 KPlayer& p=Player[r.Player];int phase=p.m_cTask.GetSaveVal(4963);if(phase>=3||phase<0||r.Config.Profession<0||r.Config.Profession>2)return 0;KNpc& actor=Npc[p.m_nIndex];
 static PTBotProgressBudget budget[Capacity];int slotBudget=&r-bots;unsigned long oldBlock=budget[slotBudget].blockedUntil;
 if(!budget[slotBudget].Observe(now,phase*1000+p.m_cTask.GetSaveVal(4964),180000,120000+(Hash(r.Config.Id,32)^now)%120000)){
  route.Target=0;route.QuestTarget=route.QuestMap=0;route.Path.clear();if(oldBlock!=budget[slotBudget].blockedUntil)Log("QUEST",r.Config.Id,"starter_trial_no_progress_switching_work");return 0;
 }
 if(phase==1){int target=r.Config.Profession+1,first=1005+r.Config.Profession*3,current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,best=2147483647,map=0;
  if(route.Target>0&&Monster(p.m_nIndex,route.Target)&&Npc[route.Target].m_dwID==route.Identity&&Npc[route.Target].m_NpcSettingIdx+1==target&&route.QuestMap>=first&&route.QuestMap<=first+2)return 2;
  for(int dest=first;dest<=first+2;++dest){if((unsigned long)time(0)<route.AvoidUntil&&dest==route.AvoidMap)continue;int hops=BotRoute(r,current,dest);if(hops<0)continue;std::vector<int>& candidates=trainingMaps[dest-1000];for(int entry=0;entry<(int)candidates.size();++entry){int n=candidates[entry];if(!Monster(p.m_nIndex,n)||Npc[n].m_NpcSettingIdx+1!=target)continue;int value=PTBotTrainScore(actor.m_Level,Npc[n].m_Level,hops,0,0,true);if(value>=0&&value<best){best=value;map=dest;}if(value>=0)break;}}
  if(!map)return 0;if(route.QuestTarget!=target||route.QuestMap!=map){route.Target=0;route.Path.clear();}route.QuestTarget=target;route.QuestMap=map;return 2;
 }
 int npc=botTrialNpcs[r.Config.Profession];if(npc<=0||npc>=MAX_NPC||Npc[npc].m_Kind!=kind_dialoger||Npc[npc].m_RegionIndex<0)return 0;int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[npc].GetMpsPos(&x,&y);route.QuestTarget=route.QuestMap=0;route.Target=0;
 if(current!=dest){int edge=-1;if(BotRoute(r,current,dest,&edge)<1||edge<0)return 0;lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return 1;}
 if(NpcSet.GetDistance(p.m_nIndex,npc)>80){lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,x,y,now,64);return 1;}
 lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);static DWORD last[Capacity];int slot=&r-bots;if(now-last[slot]<2000)return 1;last[slot]=now;
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==npc){int choice=QuestChoice(p,phase==0?"pt_trial_take":"pt_trial_claim");if(choice<0)choice=BotServiceChoice(p);if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);if(p.m_cTask.GetSaveVal(4963)!=phase){p.Save();Log("QUEST",r.Config.Id,"starter_trial_state_confirmed_via_npc");}return 1;}}
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.EntityId=Npc[npc].m_dwID;p.DialogNpc((BYTE*)&request);return 1;
}
static bool BotStarterGift(Runtime& r,Training& route,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];if(actor.m_Level<10||p.m_cTask.GetSaveVal(4700)==2||r.Config.Profession<0||r.Config.Profession>2)return false;
 static DWORD last[Capacity],started[Capacity],next[Capacity];static int progress[Capacity];int slot=&r-bots;if((int)(now-next[slot])<0)return false;int done=p.m_cTask.GetSaveVal(4702);if(!started[slot]||done!=progress[slot]){started[slot]=now;progress[slot]=done;}if(now-started[slot]>90000){started[slot]=0;next[slot]=now+120000;Log("QUEST",r.Config.Id,"starter_gift_requires_room_retrying_later");return false;}
 int npc=botLeQuanNpcs[r.Config.Profession];if(npc<=0||npc>=MAX_NPC||Npc[npc].m_Kind!=kind_dialoger||Npc[npc].m_RegionIndex<0)return false;int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[npc].GetMpsPos(&x,&y);
 route.Target=0;route.QuestTarget=route.QuestMap=0;
 if(current!=dest){int edge=-1;if(BotRoute(r,current,dest,&edge)<1||edge<0)return false;lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(NpcSet.GetDistance(p.m_nIndex,npc)>80){lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,x,y,now,64);return true;}
 lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);if(now-last[slot]<2000)return true;last[slot]=now;
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==npc){int choice=QuestChoice(p,"PhongThanClaimStarter");if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);if(p.m_cTask.GetSaveVal(4702)==done&&p.m_cTask.GetSaveVal(4700)!=2){next[slot]=now+180000;started[slot]=0;route.Path.clear();Log("QUEST",r.Config.Id,"starter_gift_no_capacity_switching_work");return false;}if(p.m_cTask.GetSaveVal(4700)==2){p.Save();Log("QUEST",r.Config.Id,"starter_gift_claimed_via_real_lequan");}return true;}}
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.EntityId=Npc[npc].m_dwID;p.DialogNpc((BYTE*)&request);return true;
}
// 0=no executable story opportunity, 1=handled movement/dialog, 2=kill objective.
static int BotStory(Runtime& r,Training& route,DWORD now){
 if(!r.Config.Auto)return 0;BotLoadStory();KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];BotStoryPlan& plan=botStoryPlan[&r-bots];
 if(p.GetLockState()||p.CheckTrading())return 0;
 if(!plan.loaded){plan.loaded=true;char path[160];sprintf(path,"BotAIgent\\agenda\\%s.ini",r.Config.Id);unsigned long utc=(unsigned long)time(0);plan.first=GetPrivateProfileIntA("Schedule","NextStory",0,path);if(!plan.first||plan.first+120<utc){plan.first=utc+5+(Hash(r.Config.Id,32)^now)%176;char value[32];sprintf(value,"%lu",plan.first);WritePrivateProfileStringA("Schedule","NextStory",value,path);}}
 if((unsigned long)time(0)<plan.first)return 0;

 if(plan.action>=0&&(!BotStoryState(p,botStoryActions[plan.action])||now-plan.started>180000)){if(now-plan.started>180000){plan.badAction=plan.action;plan.badUntil=now+180000;}plan.action=-1;plan.next=0;route.Path.clear();}
 PhongThanQuestItemTuple ownedGoal;bool collected=plan.target&&plan.item>0&&PhongThanDecodeQuestItemTuple(3,plan.item,0,0,0,0,ownedGoal)&&BotStoryItemCount(p,ownedGoal)>0;
 if(plan.target&&(p.m_cTask.GetSaveVal(plan.task)!=plan.from||collected)){plan.target=0;route.QuestTarget=0;route.QuestMap=0;route.Target=0;route.Path.clear();plan.next=0;}
 if(plan.action<0&&!plan.target){if((int)(now-plan.next)<0)return 0;plan.next=now+10000+(Hash(r.Config.Id,32)^now)%15000;
  int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,best=2147483647;
  for(int i=0;i<(int)botStoryActions.size();++i){BotStoryAction& a=botStoryActions[i];if((plan.badAction==i&&(int)(now-plan.badUntil)<0)||!BotStoryState(p,a)||!BotStoryHave(p,a))continue;int npc=BotStoryNpc(a.script);if(!npc)continue;int dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,current,dest);int peers=0;for(int b=0;b<count;++b)if(&bots[b]!=&r&&botStoryPlan[b].action==i)++peers;int score=PTBotObjectiveScore(PTBOT_STORY,hops,0,peers,Hash(r.Config.Id,32)^(i*7919));if(score>=0&&score<best){best=score;plan.action=i;}}
  if(plan.action>=0){plan.started=now;plan.last=0;route.Target=0;route.QuestTarget=route.QuestMap=0;route.Path.clear();Log("QUEST",r.Config.Id,"story_selected_from_actual_task_and_owned_requirements");}
  else for(int k=0;k<(int)botStoryKills.size();++k){BotStoryKill& kill=botStoryKills[k];if(kill.profession!=r.Config.Profession||p.m_cTask.GetSaveVal(kill.task)!=kill.from)continue;PhongThanQuestItemTuple reward;if(kill.item>0&&PhongThanDecodeQuestItemTuple(3,kill.item,0,0,0,0,reward)&&BotStoryItemCount(p,reward)>0)continue;for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Monster(p.m_nIndex,n)&&Npc[n].m_NpcSettingIdx==kill.tid&&SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID==kill.map&&PTBotTrainScore(actor.m_Level,Npc[n].m_Level,BotRoute(r,current,kill.map),0,0,true)>=0){plan.target=kill.tid+1;plan.map=kill.map;plan.task=kill.task;plan.from=kill.from;plan.item=kill.item;plan.started=now;route.Target=0;route.Path.clear();break;}if(plan.target)break;}
 }
 if(plan.target){if(now-plan.started>180000){plan.target=0;route.QuestTarget=route.QuestMap=0;route.Target=0;plan.next=now+60000;Log("QUEST",r.Config.Id,"story_combat_budget_exhausted_reconsidering");return 0;}route.QuestTarget=plan.target;route.QuestMap=plan.map;return 2;}
 if(plan.action<0){if(actor.m_Level<=20){if(BotMaoLu(r,route,now))return 1;int trial=BotStarterTrial(r,route,now);if(trial)return trial;if(BotStarterGift(r,route,now))return 1;}return 0;}BotStoryAction& action=botStoryActions[plan.action];int npc=BotStoryNpc(action.script);if(!npc){plan.action=-1;return 0;}
 int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[npc].GetMpsPos(&x,&y);
 if(current!=dest){int edge=-1;if(BotRoute(r,current,dest,&edge)<1||edge<0){plan.action=-1;return 0;}lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return 1;}
 if(NpcSet.GetDistance(p.m_nIndex,npc)>80){lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,x,y,now,64);return 1;}
 lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);if(now-plan.last<2000)return 1;plan.last=now;
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==npc){char preview[48],apply[48];sprintf(preview,action.family?"pt2_view%d":"ptg_a%d",action.step);sprintf(apply,action.family?"pt2_yes%d":"ptg_yes%d",action.step);int choice=QuestChoice(p,apply);if(choice<0)choice=QuestChoice(p,preview);if(choice<0)choice=BotServiceChoice(p);if(choice<0){plan.badAction=plan.action;plan.badUntil=now+180000;plan.action=-1;plan.next=now+60000;Log("QUEST",r.Config.Id,"story_choice_not_offered_no_bypass");return 0;}
  PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);
  if(p.m_cTask.GetSaveVal(action.task)!=action.from){char message[140];sprintf(message,"authored_story_step_confirmed task=%d from=%d to=%d",action.task,action.from,p.m_cTask.GetSaveVal(action.task));Log("QUEST",r.Config.Id,message);p.Save();plan.action=-1;plan.next=now+1500+(Hash(r.Config.Id,32)^now)%5000;}
  return 1;
 }
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.EntityId=Npc[npc].m_dwID;p.DialogNpc((BYTE*)&request);return 1;
}

#endif
