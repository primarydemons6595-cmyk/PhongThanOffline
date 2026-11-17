// Inside PtBotAIgent. All changes run on the world thread through player/NPC APIs.
struct BotProgressFrame {PTBProgressGoal goal;DWORD npc;int ordinal;};
struct BotProgressState {
 PTBProgressGoal goal;DWORD actor,npc,next,last,started,progress,refreshed;int ordinal,measure,have,recipeCursor;std::map<unsigned long,DWORD> blocked;
 std::vector<BotProgressFrame> parents;DWORD dependencyAt;
 bool scanning;int scanFamily,scanAt,scanPlan,scanBest,scanOrdinal,scanMap,scanLevel,scanProfession;DWORD scanGeneration,scanEntity;PTBProgressGoal scanChosen;

 BotProgressState():actor(0),npc(0),next(0),last(0),started(0),progress(0),refreshed(0),ordinal(0),measure(0),have(0),recipeCursor(0),dependencyAt(0),scanning(false),scanFamily(0),scanAt(0),scanPlan(0),scanBest(-2147483647),scanOrdinal(0),scanMap(0),scanLevel(0),scanProfession(-1),scanGeneration(0),scanEntity(0){ZeroMemory(&goal,sizeof(goal));ZeroMemory(&scanChosen,sizeof(scanChosen));}
};
struct BotKnownNpc {int index,capabilities;DWORD entity,script;};
static BotProgressState botCurriculum[Capacity];static std::vector<BotKnownNpc> botKnownNpcs;static DWORD botKnowledgeAt;
static std::vector<PTBProgressGoal> botRecipeGoals;static bool botRecipesLoaded;
static bool BotProgressMatch(KItem& item,const PTBProgressItem& need){
 int stars=item.m_CommonAttrib.nUpgradeLvl,level=item.GetLevel();
 return item.GetGenre()==need.genre&&item.GetDetailType()==need.detail&&(need.part<0||item.GetParticular()==need.part)&&
  (need.row<0||item.GetRow()==need.row)&&(!need.quality||item.GetPhongThanQuality()==need.quality-1)&&(!need.stars||(stars>=0&&stars<16&&(need.stars&(1U<<stars))))&&
  (!need.native||(level>=0&&level<16&&(need.native&(1U<<level))))&&
  ((need.genre!=0&&need.genre!=1)||((need.level<0||level==need.level)&&(need.series<0||need.series==1000||item.GetSeries()==need.series)));
}
static int BotProgressOwned(KPlayer& p,const PTBProgressItem& need){int n=0;for(PlayerItem* b=p.m_ItemList.GetFirstItem();b;b=p.m_ItemList.GetNextItem())if((BotBag(b->nPlace)||b->nPlace==pos_equip)&&b->nIdx>0&&b->nIdx<MAX_ITEM&&BotProgressMatch(Item[b->nIdx],need))n+=Item[b->nIdx].GetInventoryCount();return n;}
static int BotProgressSlot(KPlayer& p){static int cached[MAX_PLAYER];int player=p.GetPlayerIndex();if(player<=0||player>=MAX_PLAYER)return -1;int slot=cached[player]-1;if(slot>=0&&slot<count&&bots[slot].Player==player&&bots[slot].Config.Auto)return slot;for(int i=0;i<count;++i)if(bots[i].Player==player&&bots[i].Config.Auto){cached[player]=i+1;return i;}return -1;}
static int BotProgressRequired(KPlayer& p,KItem& item){int slot=BotProgressSlot(p);if(slot<0||!botCurriculum[slot].have)return 0;BotProgressState& s=botCurriculum[slot];int wanted=0;for(unsigned goal=0;goal<=s.parents.size();++goal){const PTBProgressGoal& g=goal==s.parents.size()?s.goal:s.parents[goal].goal;int amount=0;for(int n=0;n<g.needCount;++n)if(BotProgressMatch(item,g.needs[n]))amount+=g.needs[n].count;if(amount>wanted)wanted=amount;}return wanted;}
static int BotProgressMissing(KPlayer& p,KItem& item){int slot=BotProgressSlot(p);if(slot<0||!botCurriculum[slot].have)return 0;const PTBProgressGoal& g=botCurriculum[slot].goal;int missing=0;for(int n=0;n<g.needCount;++n)if(BotProgressMatch(item,g.needs[n])){int left=g.needs[n].count-BotProgressOwned(p,g.needs[n]);if(left>missing)missing=left;}return missing;}
static bool BotProgressReady(KPlayer& p,const PTBProgressGoal& g){for(int n=0;n<g.needCount;++n)if(BotProgressOwned(p,g.needs[n])<g.needs[n].count)return false;return true;}
static unsigned long BotProgressKey(const PTBProgressGoal& g){return ((unsigned long)g.provider*2654435761UL)^(g.id*7919UL)^(g.phase*193UL)^g.script;}
static void BotProgressClear(Runtime& r,Training& t,DWORD now,const char* reason,bool block){BotProgressState& s=botCurriculum[&r-bots];if(block){if(s.blocked.size()>64)s.blocked.clear();s.blocked[BotProgressKey(s.goal)]=now+300000;}s.have=0;s.npc=0;s.next=now+2000+(Hash(r.Config.Id,32)^now)%8000;t.QuestTarget=t.QuestMap=t.Target=0;t.Identity=0;t.Path.clear();Log("PROGRESSION",r.Config.Id,reason);
 if(!s.parents.empty()){BotProgressFrame frame=s.parents.back();s.parents.pop_back();s.goal=frame.goal;s.ordinal=frame.ordinal;s.npc=frame.npc;s.have=1;s.started=s.progress=now;s.last=0;s.measure=-1;Log("PROGRESSION",r.Config.Id,"dependency_finished_resume_saved_craft_goal");}}
static bool BotProgressProto(KPlayer& p,const PTBProgressItem& need,KItem& item){
 if(need.genre!=item_equip)return false;const KBASICPROP_EQUIPMENT* data=0;int row=need.row;
 if(row>=0)data=ItemGen.CatalogEquipment(need.detail,row);
 else for(int i=0;i<ItemGen.CatalogEquipmentCount(need.detail);++i){const KBASICPROP_EQUIPMENT* v=ItemGen.CatalogEquipment(need.detail,i);if(v&&(need.part<0||v->m_nParticularType==need.part)&&(need.level<0||v->m_nLevel==need.level)&&(!need.native||(v->m_nLevel<16&&(need.native&(1U<<v->m_nLevel))))){row=i;data=v;break;}}
 if(!data)return false;return ItemGen.GetEquipmentPreviewByTemplateRow(need.detail,row,&item)&&!BotEquipmentMismatch(p,item);
}
static int BotProgressReward(KPlayer& p,const PTBProgressItem& reward){
 KItem prototype;if(!BotProgressProto(p,reward,prototype))return 0;int best=0;
 for(int part=0;part<itempart_num;++part)if(p.m_ItemList.CanEquip(&prototype,part)){
  int old=p.m_ItemList.GetEquipment(part);if(old<=0||old>=MAX_ITEM)return 1200;
  if(Item[old].GetGenre()!=item_equip)continue;int quality=prototype.GetPhongThanQuality()-Item[old].GetPhongThanQuality();int gain=BotGearScore(p,prototype)-BotGearScore(p,Item[old]);
  if(quality>0||gain>0){int score=500+quality*100+(gain>500?500:gain>0?gain:0);if(score>best)best=score;}
 }return best;
}
static bool BotProgressLua(KPlayer& p,DWORD scriptId,int ordinal,PTBProgressGoal& out,const PTBProgressItem* desired=0){
 KLuaScript* script=(KLuaScript*)g_GetScript(scriptId);if(!script||!script->m_LuaState)return false;lua_State* L=script->m_LuaState;int top=lua_gettop(L);lua_getglobal(L,"PTBotGoal");bool supported=lua_isfunction(L,-1)!=0;lua_settop(L,top);if(!supported)return false;
 int refs[3];const char* names[]={SCRIPT_PLAYERINDEX,SCRIPT_PLAYERID,SCRIPT_SUBWORLDINDEX};double values[]={p.GetPlayerIndex(),p.m_dwID,Npc[p.m_nIndex].m_SubWorldIndex};
 for(int c=0;c<3;++c){lua_getglobal(L,names[c]);refs[c]=lua_ref(L,1);lua_pushnumber(L,values[c]);lua_setglobal(L,names[c]);}
 bool called=desired?script->CallFunction("PTBotGoal",1,"dddd",ordinal,desired->genre,desired->detail,desired->part)!=0:script->CallFunction("PTBotGoal",1,"d",ordinal)!=0;bool ok=called&&lua_istable(L,-1);ZeroMemory(&out,sizeof(out));out.sourceRow=out.targetRow=-1;
 if(ok){int table=lua_gettop(L);out.provider=PTB_LUA;out.script=scriptId;out.id=BotLuaInt(L,table,"id");out.phase=BotLuaInt(L,table,"phase");out.priority=BotLuaInt(L,table,"priority");out.fee=BotLuaInt(L,table,"fee");out.relevant=BotLuaInt(L,table,"relevant");
  out.target=BotLuaInt(L,table,"target");out.targetMap=BotLuaInt(L,table,"targetMap");
  lua_pushstring(L,"choices");lua_gettable(L,table);if(lua_istable(L,-1))for(int i=0;i<4;++i){lua_rawgeti(L,-1,i+1);if(lua_isstring(L,-1))lstrcpynA(out.choices[i],lua_tostring(L,-1),48);lua_pop(L,1);}lua_pop(L,1);
  lua_pushstring(L,"needs");lua_gettable(L,table);if(lua_istable(L,-1)){int needs=lua_gettop(L);int total=lua_getn(L,needs);if(total>10)ok=false;for(int n=1;n<=total&&n<=10;++n){lua_rawgeti(L,needs,n);if(lua_istable(L,-1)){int row=lua_gettop(L);PTBProgressItem& item=out.needs[out.needCount++];item.genre=BotLuaAt(L,row,1);item.detail=BotLuaAt(L,row,2);item.part=BotLuaAt(L,row,3);item.level=BotLuaAt(L,row,4);item.series=BotLuaAt(L,row,5);item.count=BotLuaAt(L,row,7);item.row=-1;if(item.count<1||item.count>10000)ok=false;}lua_pop(L,1);}}lua_pop(L,1);
  lua_pushstring(L,"rewards");lua_gettable(L,table);if(lua_istable(L,-1)){lua_rawgeti(L,-1,1);if(lua_istable(L,-1)){int row=lua_gettop(L);out.reward.genre=BotLuaAt(L,row,1);out.reward.detail=BotLuaAt(L,row,2);out.reward.part=BotLuaAt(L,row,3);out.reward.level=BotLuaAt(L,row,4);out.reward.series=BotLuaAt(L,row,5);out.reward.count=BotLuaAt(L,row,7);out.reward.row=-1;}lua_pop(L,1);}lua_pop(L,1);
  if(out.id<=0||out.priority<1||out.priority>10000||out.fee<0||out.fee>1000000000)ok=false;
 }
 for(c=0;c<3;++c){if(refs[c]==LUA_REFNIL)lua_pushnil(L);else lua_getref(L,refs[c]);lua_setglobal(L,names[c]);if(refs[c]>=0)lua_unref(L,refs[c]);}lua_settop(L,top);return ok;
}
static void BotProgressKnowledge(DWORD now){
 if(botKnowledgeAt&&now-botKnowledgeAt<30000)return;botKnowledgeAt=now;botKnownNpcs.clear();int known=0;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].m_RegionIndex>=0&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD){BotKnownNpc e;e.index=n;e.entity=Npc[n].m_dwID;e.script=Npc[n].m_ActionScriptID?Npc[n].m_ActionScriptID:g_FileName2Id(Npc[n].ActionScript);e.capabilities=0;KLuaScript* script=(KLuaScript*)g_GetScript(e.script);if(script&&script->m_LuaState){lua_State* L=script->m_LuaState;int top=lua_gettop(L);lua_getglobal(L,"PTBotGoal");if(lua_isfunction(L,-1))e.capabilities|=1;lua_settop(L,top);}if(e.capabilities)++known;botKnownNpcs.push_back(e);}
 std::string text;char row[512];sprintf(row,"{\"Version\":1,\"Pid\":%lu,\"UtcSeconds\":%lu,\"RefreshSeconds\":30,\"Npcs\":%d,\"LuaCapabilities\":%d,\"Entries\":[",GetCurrentProcessId(),(unsigned long)time(0),botKnownNpcs.size(),known);text=row;
 for(unsigned entry=0;entry<botKnownNpcs.size();++entry){const BotKnownNpc& e=botKnownNpcs[entry];int n=e.index,x,y;Npc[n].GetMpsPos(&x,&y);char name[65]={0};for(int letter=0;letter<32&&Npc[n].Name[letter];++letter)sprintf(name+letter*2,"%02x",(unsigned char)Npc[n].Name[letter]);sprintf(row,"%s{\"Entity\":%lu,\"Template\":%d,\"Map\":%d,\"X\":%d,\"Y\":%d,\"Script\":%lu,\"NameHex\":\"%s\",\"LuaGoalContract\":%s}",entry?",":"",e.entity,Npc[n].m_NpcSettingIdx,SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID,x,y,e.script,name,(e.capabilities&1)?"true":"false");text+=row;}
 text+="]}";Atomic("BotAIgent\\npc-knowledge.json",text.c_str(),text.size(),false);
 WIN32_FILE_ATTRIBUTE_DATA recipeFile;static FILETIME recipeStamp={0,0};if(GetFileAttributesExA("settings\\phongthan\\EquipmentWorkshop.ini",GetFileExInfoStandard,&recipeFile)&&CompareFileTime(&recipeFile.ftLastWriteTime,&recipeStamp)){recipeStamp=recipeFile.ftLastWriteTime;botRecipesLoaded=false;botRecipeGoals.clear();}if(!botRecipesLoaded){for(int i=0;i<40000;++i){PTBProgressGoal g;int result=PhongThanBotRecipeGoal(i,&g);if(!result)break;if(result==1)botRecipeGoals.push_back(g);}botRecipesLoaded=true;}
}
static int BotProgressNpc(KPlayer& p,const PTBProgressGoal& g){int best=0,score=2147483647,current=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID;for(unsigned i=0;i<botKnownNpcs.size();++i){const BotKnownNpc& e=botKnownNpcs[i];int n=e.index;if(Npc[n].m_dwID!=e.entity||Npc[n].m_RegionIndex<0||Npc[n].m_SubWorldIndex<0)continue;int map=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;if(g.map&&g.map!=map)continue;if(!PTBotAutomaticAreaAllowed(p.m_cProfession.GetProfession(),Npc[p.m_nIndex].m_Level,map))continue;if(g.npc&&Npc[n].m_NpcSettingIdx!=g.npc)continue;if(g.script&&e.script!=g.script&&e.script!=g.alias)continue;int hops=PTBotRoute(current,map);if(hops<0)continue;int d=map==current?NpcSet.GetDistance(p.m_nIndex,n):0,value=hops*100000+d;if(d>=0&&value<score){score=value;best=n;}}return best;}
static bool BotProgressGoalArea(KPlayer& p,const PTBProgressGoal& g,int map){
 int profession=p.m_cProfession.GetProfession(),level=Npc[p.m_nIndex].m_Level;
 if(!PTBotAutomaticAreaAllowed(profession,level,map))return false;
 if(g.provider==PTB_LUA&&g.priority==10000&&(g.id==13||g.id==23||g.id==33)&&level<20&&profession>=0&&profession<3)return map==1002+profession;
 return true;
}
static bool BotProgressSelect(Runtime& r,Training& t,DWORD now){
 KPlayer& p=Player[r.Player];BotProgressState& state=botCurriculum[&r-bots];
 int scanMap=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID,scanLevel=Npc[p.m_nIndex].m_Level,scanProfession=p.m_cProfession.GetProfession();
 if(state.scanning&&(state.scanMap!=scanMap||state.scanLevel!=scanLevel||state.scanProfession!=scanProfession||state.scanGeneration!=botKnowledgeAt)){state.scanning=false;state.next=now;}
 if(!state.scanning){if((int)(now-state.next)<0)return false;static DWORD selectionAt,lastSelectionAt;static int selectionBudget;if(lastSelectionAt&&now-lastSelectionAt<125){state.next=now+125+(Hash(r.Config.Id,32)%1000);return false;}if(!selectionAt||now-selectionAt>=1000){selectionAt=now;selectionBudget=8;}if(!selectionBudget){state.next=now+1000+(Hash(r.Config.Id,32)%2000);return false;}--selectionBudget;lastSelectionAt=now;state.next=now+15000+(Hash(r.Config.Id,32)^now)%30000;BotProgressKnowledge(now);state.scanning=true;state.scanFamily=state.scanAt=state.scanPlan=0;state.scanBest=-2147483647;state.scanOrdinal=0;state.scanEntity=0;ZeroMemory(&state.scanChosen,sizeof(state.scanChosen));state.scanGeneration=botKnowledgeAt;state.scanMap=scanMap;state.scanLevel=scanLevel;state.scanProfession=scanProfession;}
 LARGE_INTEGER scanStarted;QueryPerformanceCounter(&scanStarted);int scanProcessed=0;
 int best=state.scanBest;PTBProgressGoal chosen=state.scanChosen;int ordinal=state.scanOrdinal;DWORD entity=state.scanEntity;

 for(int family=state.scanFamily;family<3;++family){int limit=family==0?1024:family==1?(int)botKnownNpcs.size():96;
  for(int at=state.scanAt;at<limit;++at){if(botDecisionMilliseconds>0&&scanProcessed>0){LARGE_INTEGER current;QueryPerformanceCounter(&current);if((current.QuadPart-scanStarted.QuadPart)*1000.0/g_PTServerPerformance.frequency.QuadPart>=3.0){state.scanFamily=family;state.scanAt=at;state.scanBest=best;state.scanChosen=chosen;state.scanOrdinal=ordinal;state.scanEntity=entity;return false;}}++scanProcessed;PTBProgressGoal g;int ord=at,npc=0;
   if(family==0){int found=PhongThanBotStudioGoal(r.Player,at,&g);if(!found)break;if(found!=1)continue;}
   else if(family==1){const BotKnownNpc& e=botKnownNpcs[at];if(!(e.capabilities&1)||Npc[e.index].m_dwID!=e.entity){state.scanPlan=0;continue;}for(int plan=state.scanPlan;plan<96;++plan){if(botDecisionMilliseconds>0&&scanProcessed>0){LARGE_INTEGER current;QueryPerformanceCounter(&current);if((current.QuadPart-scanStarted.QuadPart)*1000.0/g_PTServerPerformance.frequency.QuadPart>=3.0){state.scanFamily=family;state.scanAt=at;state.scanPlan=plan;state.scanBest=best;state.scanChosen=chosen;state.scanOrdinal=ordinal;state.scanEntity=entity;return false;}}++scanProcessed;if(!BotProgressLua(p,e.script,plan,g))break;g.map=SubWorld[Npc[e.index].m_SubWorldIndex].m_SubWorldID;g.npc=Npc[e.index].m_NpcSettingIdx;std::map<unsigned long,DWORD>::iterator b=state.blocked.find(BotProgressKey(g));if(b!=state.blocked.end()&&(int)(now-b->second)<0)continue;if(!BotProgressGoalArea(p,g,g.map))continue;int here=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,here,g.map);if(hops<0)continue;int distance=here==g.map?NpcSet.GetDistance(p.m_nIndex,e.index):0;if(distance<0)continue;int score=g.priority+(BotProgressReady(p,g)?50:0)-hops*50-distance/128;if(g.id>=20000&&g.id<30000&&!BotProgressReward(p,g.reward))continue;if(score>best){best=score;chosen=g;ordinal=plan;entity=e.entity;}}state.scanPlan=0;continue;}
   else {if(botRecipeGoals.empty())break;ord=(state.recipeCursor+at)%botRecipeGoals.size();g=botRecipeGoals[ord];int benefit=BotProgressReward(p,g.reward);if(g.provider==PTB_UPGRADE){bool owns=false;for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if((BotBag(item->nPlace)||item->nPlace==pos_equip)&&item->nIdx>0&&item->nIdx<MAX_ITEM&&BotProgressMatch(Item[item->nIdx],g.needs[0])&&Item[item->nIdx].m_CommonAttrib.nUpgradeLvl<3&&!BotEquipmentMismatch(p,Item[item->nIdx]))owns=true;if(!owns)continue;benefit=500;}if(!benefit)continue;if(g.provider==PTB_WORKSHOP&&(g.chance<50||g.phase>6))continue;g.priority=benefit;}
   if(g.fee>p.m_ItemList.GetEquipmentMoney()*(__int64)25/100&&g.provider!=PTB_PHI)continue;
   if(g.provider==PTB_PHI||g.provider==PTB_TRAN){if(g.reward.genre==item_equip&&g.reward.count>0){int benefit=BotProgressReward(p,g.reward);if(!benefit&&g.phase!=6)continue;g.priority+=benefit/2;}}
   if(g.provider==PTB_STUDIO_SHOP){int benefit=BotProgressReward(p,g.reward);if(!benefit)continue;if(g.point&&p.m_cTask.GetSaveVal(5230+g.point-1)<g.price)continue;g.priority=benefit+200;}
   std::map<unsigned long,DWORD>::iterator b=state.blocked.find(BotProgressKey(g));if(b!=state.blocked.end()&&(int)(now-b->second)<0)continue;npc=BotProgressNpc(p,g);if(!npc)continue;
   int distance=Npc[npc].m_SubWorldIndex==Npc[p.m_nIndex].m_SubWorldIndex?NpcSet.GetDistance(p.m_nIndex,npc):400;int score=g.priority+(BotProgressReady(p,g)?100:0)-distance/128+(Hash(r.Config.Id,32)^g.id)%71;
   if(score>best){best=score;chosen=g;ordinal=ord;entity=Npc[npc].m_dwID;}
  }
  state.scanAt=state.scanPlan=0;
 }
 state.scanning=false;state.scanFamily=state.scanAt=state.scanPlan=0;
 if(!botRecipeGoals.empty())state.recipeCursor=(state.recipeCursor+96)%botRecipeGoals.size();if(best==-2147483647)return false;state.goal=chosen;state.ordinal=ordinal;state.npc=entity;state.have=1;state.last=0;state.started=state.progress=now;state.measure=-1;t.Target=t.QuestTarget=t.QuestMap=0;t.Path.clear();char message[180];sprintf(message,"goal_selected provider=%d id=%d phase=%d needs=%d npc=%lu",chosen.provider,chosen.id,chosen.phase,chosen.needCount,entity);Log("PROGRESSION",r.Config.Id,message);return true;
}
static std::map<std::string,std::vector<PTBProgressItem> > botResourceDrops;
static bool BotProgressDrop(int tid,const PTBProgressItem& need){
 if(tid<0||tid>=MAX_NPCSTYLE||!g_pNpcTemplate[tid])return false;KNpcTemplate* data=g_pNpcTemplate[tid];
 for(int slot=0;slot<4;++slot){const char* path=data->m_LegacyDropFiles[slot];if(!path[0])continue;std::string key=path;std::map<std::string,std::vector<PTBProgressItem> >::iterator found=botResourceDrops.find(key);
  if(found==botResourceDrops.end()){KIniFile ini;std::vector<PTBProgressItem> drops;if(ini.Load((char*)path)){int count=0;ini.GetInteger("Main","Count",0,&count);if(count>512)count=512;for(int i=1;i<=count;++i){char sec[24];sprintf(sec,"%d",i);int rate=0;ini.GetInteger(sec,"RandRate",0,&rate);if(rate<=0)continue;PTBProgressItem d;ZeroMemory(&d,sizeof(d));ini.GetInteger(sec,"Genre",-1,&d.genre);ini.GetInteger(sec,"Detail",-1,&d.detail);ini.GetInteger(sec,"Particular",-1,&d.part);d.row=-1;drops.push_back(d);}}botResourceDrops[key]=drops;found=botResourceDrops.find(key);}
  for(unsigned d=0;d<found->second.size();++d){const PTBProgressItem& item=found->second[d];if(item.genre==need.genre&&item.detail==need.detail&&(need.part<0||item.part<0||item.part==need.part))return true;}
 }return false;
}
static bool BotProgressDependency(Runtime& r,Training& t,const PTBProgressItem& need,DWORD now){
 BotProgressState& s=botCurriculum[&r-bots];if(s.parents.size()>=4||(s.dependencyAt&&now-s.dependencyAt<30000))return false;s.dependencyAt=now;KPlayer& p=Player[r.Player];PTBProgressGoal chosen;int selected=-1,best=-2147483647;DWORD entity=0;
 for(unsigned i=0;i<botKnownNpcs.size();++i){const BotKnownNpc& e=botKnownNpcs[i];if(!(e.capabilities&1)||Npc[e.index].m_dwID!=e.entity)continue;
  for(int ordinal=0;ordinal<96;++ordinal){PTBProgressGoal g;if(!BotProgressLua(p,e.script,ordinal,g,&need))break;if(!g.relevant||g.fee>p.m_ItemList.GetEquipmentMoney()*(__int64)25/100)continue;unsigned long key=BotProgressKey(g);if(key==BotProgressKey(s.goal))continue;bool cycle=false;for(unsigned parent=0;parent<s.parents.size();++parent)if(BotProgressKey(s.parents[parent].goal)==key)cycle=true;if(cycle)continue;std::map<unsigned long,DWORD>::iterator blocked=s.blocked.find(key);if(blocked!=s.blocked.end()&&(int)(now-blocked->second)<0)continue;
   g.map=SubWorld[Npc[e.index].m_SubWorldIndex].m_SubWorldID;g.npc=Npc[e.index].m_NpcSettingIdx;if(!BotProgressGoalArea(p,g,g.map))continue;int hops=BotRoute(r,SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID,g.map);if(hops<0)continue;int score=g.priority-hops*50+(BotProgressReady(p,g)?100:0);if(score>best){best=score;chosen=g;selected=ordinal;entity=e.entity;}
  }
 }
 if(selected<0)return false;BotProgressFrame parent;parent.goal=s.goal;parent.npc=s.npc;parent.ordinal=s.ordinal;s.parents.push_back(parent);s.goal=chosen;s.ordinal=selected;s.npc=entity;s.started=s.progress=now;s.last=0;s.measure=-1;t.Target=t.QuestTarget=t.QuestMap=0;t.Path.clear();Log("PROGRESSION",r.Config.Id,"quest_selected_for_missing_recipe_material_or_formula");return true;
}
static bool BotProgressWithdraw(Runtime& r,Training& t,const PTBProgressItem& need,DWORD now){
 KPlayer& p=Player[r.Player];bool opened=false;const int permission[]={13,23,33};for(int q=0;q<3;++q)if(p.m_cTask.GetSaveVal(permission[q])==2||!strcmp(p.m_cTask.GetSaveStr(permission[q]),"2"))opened=true;if(!opened||PhongThanBankLocked(r.Player)||p.m_ItemList.Hand())return false;
 int owned=0;PlayerItem source;for(PlayerItem* b=p.m_ItemList.GetFirstItem();b;b=p.m_ItemList.GetNextItem())if(b->nPlace>=pos_repositoryroom&&b->nPlace<pos_repositoryroom+6&&b->nIdx>0&&b->nIdx<MAX_ITEM&&PhongThanBankAccess(r.Player,b->nPlace-pos_repositoryroom+1)&&BotProgressMatch(Item[b->nIdx],need)&&!Item[b->nIdx].GetLock()->IsLock()){owned=b->nIdx;source=*b;break;}if(!owned)return false;
 int banker=0,best=2147483647,current=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID;
 for(unsigned n=0;n<botKnownNpcs.size();++n){int i=botKnownNpcs[n].index;if(Npc[i].m_dwID!=botKnownNpcs[n].entity||Npc[i].m_SubWorldIndex<0||Npc[i].m_RegionIndex<0||(Npc[i].m_NpcSettingIdx!=768&&!(Npc[i].m_NpcSettingIdx==151&&!strcmp(Npc[i].Name,"\124\150\361\040\113\150\350"))))continue;int map=SubWorld[Npc[i].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,current,map);if(hops<0)continue;int score=hops*100000+(map==current?NpcSet.GetDistance(p.m_nIndex,i):0);if(score<best){best=score;banker=i;}}
 if(!banker)return false;int map=SubWorld[Npc[banker].m_SubWorldIndex].m_SubWorldID;
 if(map!=current){int edge=-1;if(BotRoute(r,current,map,&edge)<1||edge<0)return false;MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);lstrcpyA(r.State,"bank_materials_travel");return true;}
 int x,y;Npc[banker].GetMpsPos(&x,&y);if(NpcSet.GetDistance(p.m_nIndex,banker)>96){MoveTraining(r,t,x,y,now,64);lstrcpyA(r.State,"bank_materials_travel");return true;}
 if(!PhongThanBankNearTreasury(r.Player))return false;ItemPos dest;if(!p.m_ItemList.FindBagPosition(Item[owned].GetWidth(),Item[owned].GetHeight(),&dest))return false;
 DWORD serial=Item[owned].GetID();BotMarketClick(p,source.nPlace,source.nX,source.nY);if(p.m_ItemList.Hand()!=owned)return false;BotMarketClick(p,dest.nPlace,dest.nX,dest.nY);if(p.m_ItemList.Hand())BotMarketClick(p,source.nPlace,source.nX,source.nY);
 int at=p.m_ItemList.FindSame(owned);if(at>0&&BotBag(p.m_ItemList.m_Items[at].nPlace)&&Item[owned].GetID()==serial){p.Save();Log("PROGRESSION",r.Config.Id,"owned_recipe_material_withdrawn_at_unlocked_treasury");return true;}return false;
}
static int BotProgressGather(Runtime& r,Training& t,const PTBProgressItem& need,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,best=0,score=2147483647;
 if(t.QuestTarget>0&&t.QuestMap>=1000&&t.QuestMap<1256&&BotProgressDrop(t.QuestTarget-1,need)&&PTBotAutomaticAreaAllowed(r.Config.Profession,actor.m_Level,t.QuestMap)&&!trainingMaps[t.QuestMap-1000].empty())return 2;
 for(int level=1;level<256;++level)if(level<=actor.m_Level+5)for(unsigned i=0;i<trainingLevels[level].size();++i){int n=trainingLevels[level][i];if(!Monster(p.m_nIndex,n)||!BotProgressDrop(Npc[n].m_NpcSettingIdx,need))continue;int map=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;if(!PTBotAutomaticAreaAllowed(r.Config.Profession,actor.m_Level,map))continue;if((unsigned long)time(0)<t.AvoidUntil&&map==t.AvoidMap)continue;int hops=BotRoute(r,current,map);if(hops<0)continue;int distance=map==current?NpcSet.GetDistance(p.m_nIndex,n):0,value=hops*10000+distance+abs(actor.m_Level-level)*10;if(value<score){score=value;best=n;}}
 if(!best)return 0;int target=Npc[best].m_NpcSettingIdx+1,map=SubWorld[Npc[best].m_SubWorldIndex].m_SubWorldID;if(t.QuestTarget!=target||t.QuestMap!=map){t.Target=0;t.Path.clear();}t.QuestTarget=target;t.QuestMap=map;return 2;
}
static bool BotProgressCraft(Runtime& r,Training& t,DWORD now){
 KPlayer& p=Player[r.Player];BotProgressState& s=botCurriculum[&r-bots];PTBProgressGoal& g=s.goal;int indexes[9]={0},counts[9]={0},used=0;std::vector<int> owned;
 if(p.m_ItemList.Hand())return false;for(int b=0;b<9;++b)if(p.m_ItemList.GetBuildItem(b))return false;
 for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if((BotBag(item->nPlace)||item->nPlace==pos_equip)&&item->nIdx>0&&item->nIdx<MAX_ITEM)owned.push_back(item->nIdx);
 for(int need=0;need<g.needCount;++need){int left=g.needs[need].count;for(unsigned o=0;o<owned.size()&&left>0;++o){int i=owned[o];if(!Item[i].GetID()||Item[i].GetLock()->IsLock()||Item[i].IsTemp()||!BotProgressMatch(Item[i],g.needs[need]))continue;int slot=-1;for(int x=0;x<used;++x)if(indexes[x]==i)slot=x;if(slot<0){if(used>=9)return false;slot=used++;indexes[slot]=i;}int available=Item[i].GetStackNum()-counts[slot],take=available<left?available:left;if(take>0){counts[slot]+=take;left-=take;}}
  if(left)return false;
 }
 int fee=0;if(PhongThanBotRecipeQuote(r.Player,&g,indexes,counts,&fee)!=PT_COMPOSE_OK||fee>p.m_ItemList.GetEquipmentMoney()*(__int64)25/100)return false;
 ItemPos origins[9];DWORD serials[9]={0};int moved=0;
 for(int n=0;n<used;++n){int at=p.m_ItemList.FindSame(indexes[n]);if(at<=0)break;PlayerItem entry=p.m_ItemList.m_Items[at];origins[n].nPlace=entry.nPlace;origins[n].nX=entry.nX;origins[n].nY=entry.nY;serials[n]=Item[indexes[n]].GetID();BotMarketClick(p,entry.nPlace,entry.nX,entry.nY);if(p.m_ItemList.Hand()!=indexes[n])break;BotMarketClick(p,pos_builditem,n,0);if(p.m_ItemList.Hand()||p.m_ItemList.GetBuildItem(n)!=indexes[n])break;++moved;}
 if(moved==used){PHONGTHAN_COMPOSE_COMMAND q;ZeroMemory(&q,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PHONGTHAN_COMPOSE_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);q.MapId=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID;for(int n=0;n<used;++n){q.Items[n]=serials[n];q.Quantities[n]=counts[n];}g_ProtocolProcess.ProcessNetMsg(r.Player,(BYTE*)&q,sizeof(q));}
 // Remaining input/remainder objects keep their identities and go back through
 // normal inventory moves. Never delete items to make room for a retry.
 for(n=0;n<used;++n){int i=p.m_ItemList.GetBuildItem(n);if(!i)continue;ItemPos dest=origins[n];if(dest.nPlace==pos_equip||!p.m_ItemList.SearchPosition(Item[i].GetWidth(),Item[i].GetHeight(),&dest,true))if(!p.m_ItemList.FindBagPosition(Item[i].GetWidth(),Item[i].GetHeight(),&dest))continue;BotMarketClick(p,pos_builditem,n,0);if(p.m_ItemList.Hand()==i)BotMarketClick(p,dest.nPlace,dest.nX,dest.nY);if(p.m_ItemList.Hand())BotMarketClick(p,pos_builditem,n,0);}
 if(p.m_ItemList.Hand())BotStoreHand(p);p.Save();BotProgressClear(r,t,now,moved==used?"native_npc_compose_submitted_and_remainders_recovered":"compose_input_move_rejected_preserved",true);return moved==used;
}
static bool BotProgressCarryInputs(Runtime& r,const PTBProgressGoal& g){
 KPlayer& p=Player[r.Player];if(p.m_ItemList.Hand())return false;
 for(int n=0;n<g.needCount;++n){std::vector<int> selected;for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if((BotBag(item->nPlace)||item->nPlace==pos_equip)&&item->nPlace!=pos_equiproom&&item->nIdx>0&&item->nIdx<MAX_ITEM&&BotProgressMatch(Item[item->nIdx],g.needs[n]))selected.push_back(item->nIdx);
  for(unsigned o=0;o<selected.size();++o){int i=selected[o],at=p.m_ItemList.FindSame(i);if(at<=0)return false;PlayerItem source=p.m_ItemList.m_Items[at];POINT pos;if(!p.m_ItemList.m_Room[room_equipment].FindRoom(Item[i].GetWidth(),Item[i].GetHeight(),&pos))return false;BotMarketClick(p,source.nPlace,source.nX,source.nY);if(p.m_ItemList.Hand()!=i)return false;BotMarketClick(p,pos_equiproom,pos.x,pos.y);if(p.m_ItemList.Hand())BotMarketClick(p,source.nPlace,source.nX,source.nY);at=p.m_ItemList.FindSame(i);if(p.m_ItemList.Hand()||at<=0||p.m_ItemList.m_Items[at].nPlace!=pos_equiproom)return false;}
 }return true;
}
static void AppendProgressionStatus(FILE* f,Runtime& r){
 BotProgressState& s=botCurriculum[&r-bots];if(!r.Player||!s.have||s.actor!=Npc[Player[r.Player].m_nIndex].m_dwID)return;
 int npc=NpcSet.SearchID(s.npc),map=0,tid=0;if(npc>0&&npc<MAX_NPC&&Npc[npc].m_SubWorldIndex>=0&&Npc[npc].m_SubWorldIndex<MAX_SUBWORLD){map=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID;tid=Npc[npc].m_NpcSettingIdx;}
 fprintf(f,",\"GoalProvider\":%d,\"GoalId\":%d,\"GoalPhase\":%d,\"GoalNpcMap\":%d,\"GoalNpcTemplate\":%d",s.goal.provider,s.goal.id,s.goal.phase,map,tid);
}
static int BotProgression(Runtime& r,Training& t,DWORD now){
 if(!r.Config.Auto||!r.Config.Train||companions[&r-bots].Mode)return 0;KPlayer& p=Player[r.Player];BotProgressState& s=botCurriculum[&r-bots];KNpc& actor=Npc[p.m_nIndex];
 if(s.actor!=actor.m_dwID){s=BotProgressState();s.actor=actor.m_dwID;}if(!s.have&&!BotProgressSelect(r,t,now))return 0;
 int npc=NpcSet.SearchID(s.npc);if(npc<=0||npc>=MAX_NPC||Npc[npc].m_RegionIndex<0){BotProgressClear(r,t,now,"npc_unavailable_replan",true);return 0;}
 PTBProgressGoal current=s.goal;bool valid=true;
 if(!s.refreshed||now-s.refreshed>=1500){s.refreshed=now;
  if(s.goal.provider==PTB_LUA){valid=BotProgressLua(p,s.goal.script,s.ordinal,current)&&current.id==s.goal.id;current.map=s.goal.map;current.npc=s.goal.npc;}
  else if(s.goal.provider==PTB_STUDIO_QUEST||s.goal.provider==PTB_STUDIO_SHOP||s.goal.provider==PTB_TRAN||s.goal.provider==PTB_PHI)valid=PhongThanBotStudioGoal(r.Player,s.ordinal,&current)==1&&current.id==s.goal.id&&current.provider==s.goal.provider;
 }
 if(!valid){p.Save();BotProgressClear(r,t,now,"goal_completed_or_conditions_changed",false);return 0;}s.goal=current;
 if(current.fee>p.m_ItemList.GetEquipmentMoney()*(__int64)25/100){lstrcpyA(r.State,"saving_recipe_fee");return 0;}
 unsigned measure=current.phase*31U+current.progress;for(int n=0;n<current.needCount;++n)measure=measure*31U+BotProgressOwned(p,current.needs[n]);
 if(s.measure!=measure){s.measure=measure;s.progress=now;}if(now-s.progress>180000||now-s.started>600000){BotProgressClear(r,t,now,"no_progress_backoff_keep_items_and_points",true);return 0;}
 if(current.target&&(!current.needCount||!BotProgressReady(p,current))){int expected=current.target,map=current.targetMap;if(t.QuestTarget!=expected||t.QuestMap!=map){t.Target=0;t.Path.clear();}t.QuestTarget=expected;t.QuestMap=map;return 2;}
 if(!BotProgressReady(p,current)){
  if(BotLoot(r,t,now,true,true))return 1;
  if(BotShopPlan(r,t,now))return 1;
  for(int n=0;n<current.needCount;++n)if(BotProgressOwned(p,current.needs[n])<current.needs[n].count){if(BotProgressWithdraw(r,t,current.needs[n],now)||BotProgressDependency(r,t,current.needs[n],now))return 1;int gather=BotProgressGather(r,t,current.needs[n],now);if(gather)return gather;}
  lstrcpyA(r.State,"saving_for_recipe");return 0;
 }
 if(Npc[npc].m_SubWorldIndex!=actor.m_SubWorldIndex){int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){BotProgressClear(r,t,now,"npc_route_unreachable_backoff",true);return 0;}lstrcpyA(r.State,"npc_goal_travel");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return 1;}
 int x=0,y=0;Npc[npc].GetMpsPos(&x,&y);if(NpcSet.GetDistance(p.m_nIndex,npc)>96){lstrcpyA(r.State,"npc_goal_travel");MoveTraining(r,t,x,y,now,64);return 1;}
 t.Path.clear();t.Target=t.QuestTarget=t.QuestMap=0;actor.SendCommand(do_stand);actor.SetFightMode(0);lstrcpyA(r.State,"npc_goal_dialog");if(now-s.last<1500)return 1;s.last=now;
 if(p.m_nLastNpcIndex!=npc||(!p.m_UiDialogToken&&current.provider==PTB_LUA)){PHONGTHAN_ENTITY_REFERENCE q;ZeroMemory(&q,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);q.MapId=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;q.EntityId=s.npc;p.DialogNpc((BYTE*)&q);return 1;}
 if(current.provider==PTB_STUDIO_QUEST||current.provider==PTB_STUDIO_SHOP||current.provider==PTB_TRAN||current.provider==PTB_PHI){if((current.provider==PTB_PHI||current.provider==PTB_TRAN)&&!BotProgressCarryInputs(r,current)){BotProgressClear(r,t,now,"native_npc_inputs_need_safe_primary_bag_space",true);return 0;}int result=PhongThanBotStudioCommit(r.Player,&current);if(result>0){p.Save();BotProgressClear(r,t,now,"authoritative_npc_quest_or_point_exchange_confirmed",false);}else BotProgressClear(r,t,now,"native_npc_rejected_no_state_bypass",true);return 1;}
 if(current.provider==PTB_WORKSHOP||current.provider==PTB_UPGRADE){if(!BotProgressCraft(r,t,now))BotProgressClear(r,t,now,"craft_quote_or_input_condition_rejected",true);return 1;}
 if(p.m_UiDialogView==PHONGTHAN_VIEW_ROULETTE){PHONGTHAN_UI_NUMBER_REQUEST q;ZeroMemory(&q,sizeof(q));q.MapId=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;q.DialogToken=p.m_UiDialogToken;q.Value=1;::PhongThanEventNumberInput(p,q);p.Save();return 1;}
 int choice=-1;for(int choiceAt=0;choiceAt<4&&choice<0;++choiceAt)if(current.choices[choiceAt][0])choice=QuestChoice(p,current.choices[choiceAt]);
 if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST q;ZeroMemory(&q,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);q.MapId=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;q.DialogToken=p.m_UiDialogToken;q.View=p.m_UiDialogView;q.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&q);p.Save();return 1;}
 PHONGTHAN_ENTITY_REFERENCE q;ZeroMemory(&q,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);q.MapId=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;q.EntityId=s.npc;p.DialogNpc((BYTE*)&q);return 1;
}
