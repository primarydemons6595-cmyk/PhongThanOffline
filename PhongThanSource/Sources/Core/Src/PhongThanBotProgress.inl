struct BotProgressWatch {DWORD last,entity;int map,x,y,life;BotProgressWatch():last(0),entity(0),map(0),x(0),y(0),life(0){}};
static BotProgressWatch botProgress[Capacity];
static void BotWatchProgress(Runtime& r,DWORD now){
 if(!botPolicy.recover||!r.Player)return;int slot=&r-bots;KPlayer& p=Player[r.Player];int n=p.m_nIndex;if(n<=0||n>=MAX_NPC)return;
 KNpc& actor=Npc[n];if(actor.m_SubWorldIndex<0||actor.m_SubWorldIndex>=MAX_SUBWORLD)return;
 Training& route=training[slot];BotProgressWatch& state=botProgress[slot];int x,y;actor.GetMpsPos(&x,&y);int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 int life=route.Target>0&&route.Target<MAX_NPC&&Npc[route.Target].m_dwID==route.Identity?Npc[route.Target].m_CurrentLife:0;
 if(!state.last||state.entity!=actor.m_dwID||state.map!=map||abs(x-state.x)+abs(y-state.y)>=32||state.life!=life){state.last=now;state.entity=actor.m_dwID;state.map=map;state.x=x;state.y=y;state.life=life;return;}
 if(now-state.last<45000||p.CheckTrading()||p.GetLockState()||p.m_PTrade.nTrade||actor.m_CurrentLife<=0||p.m_bSleepMode)return;
 bool moving=!strcmp(r.State,"npc_goal_travel")||!strcmp(r.State,"bank_materials_travel")||!strcmp(r.State,"travel_training")||!strcmp(r.State,"shopping_travel")||!strcmp(r.State,"quest_travel")||!strcmp(r.State,"bank_travel")||!strcmp(r.State,"boss_travel")||!strcmp(r.State,"event_travel")||!strcmp(r.State,"thuong_chu_registering")||!strcmp(r.State,"thuong_chu_entering");
 if(!moving&&strcmp(r.State,"combat_blocked"))return;
 route.Path.clear();route.Next=0;route.Progress.Reset();++route.Failures;state.last=now;
 if(route.Target>0){route.BadIdentity=route.Identity;route.BadUntil=now+30000;route.Target=0;route.Identity=0;}
 r.GoalEntity=0;actor.SendCommand(do_stand);lstrcpyA(r.State,"replanning");Log("RECOVERY",r.Config.Id,"no_progress_replan_preserved_character_items_and_quests");
}
