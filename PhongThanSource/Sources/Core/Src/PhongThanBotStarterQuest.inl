// Kế Tục adapter. Every accept/claim goes through a real NPC's issued UI choices.
// Task values are read only here; normal Lua and kill dispatch own all progress/rewards.
static void QuestDelay(Runtime& r,unsigned long delay){
 CreateDirectoryA("BotAIgent\\agenda",0);char path[160],value[32];sprintf(path,"BotAIgent\\agenda\\%s.ini",r.Config.Id);sprintf(value,"%lu",(unsigned long)time(0)+delay);WritePrivateProfileStringA("Schedule","NextStarterQuest",value,path);
}
static unsigned long QuestWhen(Runtime& r){
 char path[160];sprintf(path,"BotAIgent\\agenda\\%s.ini",r.Config.Id);unsigned long value=GetPrivateProfileIntA("Schedule","NextStarterQuest",0,path);
 if(!value){QuestDelay(r,15+(Hash(r.Config.Id,32)^(DWORD)time(0))%106);value=GetPrivateProfileIntA("Schedule","NextStarterQuest",0,path);}return value;
}
static int QuestChoice(KPlayer& player,const char* callback){
 for(int i=0;i<player.m_nAvailableAnswerNum&&i<MAX_ANSWERNUM;++i)if(!strcmp(player.m_szTaskAnswerFun[i],callback))return i;return -1;
}
static int QuestTake(Runtime& r){
 static const int targets[3][2]={{1,4},{2,5},{3,7}};KNpc& actor=Npc[Player[r.Player].m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 int best=-1,score=2147483647;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Monster(actor.m_Index,n))for(int choice=0;choice<2;++choice)if(Npc[n].m_NpcSettingIdx+1==targets[r.Config.Profession][choice]){
  int dest=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;int value=PTBotTrainScore(actor.m_Level,Npc[n].m_Level,BotRoute(r,map,dest),0,(Hash(r.Config.Id,32)^Npc[n].m_dwID)%100);
  if(value>=0&&value<score){score=value;best=choice+1;}
 }return best;
}
static bool StarterQuest(Runtime& r,Training& t,DWORD now){
 KPlayer& player=Player[r.Player];int state=player.m_cTask.GetSaveVal(993),target=state&255,need=((state>>8)&255)*10;
 int countKilled=player.m_cTask.GetSaveVal(994);
 int desired=(state&&countKilled<need)?target:0;
 if(t.QuestTarget!=desired){t.QuestTarget=desired;t.Target=0;t.Path.clear();}
 if(state&&need>0&&countKilled<need)return false;
 if(!state&&(!r.Config.Train||(unsigned long)time(0)<QuestWhen(r)))return false;
 if(r.Config.Profession<0||r.Config.Profession>=3)return false;
 int npc=0;for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&!stricmp(Npc[n].ActionScript,PTBotQuestNpcs[r.Config.Profession])){npc=n;break;}
 if(!npc){QuestDelay(r,90);return false;}
 KNpc& actor=Npc[player.m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID;
 int px=0,py=0,x=0,y=0;actor.GetMpsPos(&px,&py);Npc[npc].GetMpsPos(&x,&y);
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){QuestDelay(r,90);return false;}lstrcpyA(r.State,"quest_travel");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(abs(px-x)+abs(py-y)>96){lstrcpyA(r.State,"quest_travel");MoveTraining(r,t,x,y,now,64);return true;}
 lstrcpyA(r.State,"quest_dialog");actor.SendCommand(do_stand);
 if(player.m_UiDialogToken){
  int selection=QuestChoice(player,"ptk_open");if(selection<0)selection=QuestChoice(player,"ptk_finish");
  if(selection<0){int take=QuestTake(r);if(take>0)selection=QuestChoice(player,take==1?"ptk_take1":"ptk_take2");}
  PHONGTHAN_UI_CHOICE_REQUEST choice;ZeroMemory(&choice,sizeof(choice));PhongThanInitializeWireHeader(&choice.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(choice),PHONGTHAN_WIRE_FLAG_REQUEST,0);
  choice.MapId=map;choice.DialogToken=player.m_UiDialogToken;choice.View=player.m_UiDialogView;choice.Selection=selection;
  player.ProcessPlayerSelectFromUI((BYTE*)&choice);
  int nextState=player.m_cTask.GetSaveVal(993);
  if(nextState&&!state){t.Target=0;Log("QUEST",r.Config.Id,"starter_quest_accepted_via_npc");}
  else if(state&&!nextState){QuestDelay(r,120+(Hash(r.Config.Id,32)^(DWORD)time(0))%481);Log("QUEST",r.Config.Id,"starter_quest_claimed_via_npc");}
  else if(selection<0)QuestDelay(r,90+(Hash(r.Config.Id,32)^(DWORD)time(0))%151);
  return true;
 }
 if(now-r.LastAction<2500)return true;r.LastAction=now;
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=map;request.EntityId=Npc[npc].m_dwID;player.DialogNpc((BYTE*)&request);return true;
}
