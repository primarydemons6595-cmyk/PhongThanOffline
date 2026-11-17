// Included inside PtThuongChu. Reference: TGPT mission/reward article.
// All participants share match.mode and Map; neither is chosen per player.
enum { TaskRepShang=5080,TaskRepZhou=5081,TaskMerit=5082,TaskQuestDay=5083,
 TaskQuestRuns=5084,TaskQuestKind=5085,TaskQuestProgress=5086,TaskQuestSession=5087,
 TaskBuffUntil=5088 };
enum { UnitMarshal=0,UnitGeneral=1,UnitFlag=2,UnitScout=3,UnitTotem=4,
 UnitCommander=5,UnitCommanderFlag=6,UnitService=7,UnitCount=32 };
static int Config(const char* key,int fallback){return GetPrivateProfileIntA("Battle",key,fallback,"settings\\phongthan\\ThuongChu.ini");}
static int Day(){time_t stamp=time(NULL);struct tm* date=localtime(&stamp);return date?(date->tm_year+1900)*10000+(date->tm_mon+1)*100+date->tm_mday:0;}
static int RepTask(int side){return side==1?TaskRepShang:TaskRepZhou;}
static void AddSaved(int p,int task,int delta){int old=Player[p].m_cTask.GetSaveVal(task);Player[p].m_cTask.SetSaveVal(task,PTThuongChuMatch::Points(old,delta),TRUE);}
static void Base(int side,int& x,int& y){x=Map==1071?(side==1?178:290):(side==1?176:216);y=Map==1071?(side==1?203:206):195;}
static bool AtBase(int p){if(!Own(p))return false;int x,y;Base(members[p].side,x,y);return Near(p,Map,x,y);}
static bool Transaction(int p){return Valid(p)&&!Player[p].CheckTrading()&&!Player[p].GetLockState()&&Npc[Player[p].m_nIndex].IsAlive();}
static int ActiveQuest(int p){return Own(p)&&Player[p].m_cTask.GetSaveVal(TaskQuestSession)==(int)generation?Player[p].m_cTask.GetSaveVal(TaskQuestKind):0;}
static void QuestProgress(int p,int kind,int amount){if(!Own(p)||members[p].left||World(p)!=Map||ActiveQuest(p)!=kind)return;
 int current=Player[p].m_cTask.GetSaveVal(TaskQuestProgress),goal=PTThuongChuMatch::QuestGoal(kind);if(current<goal)Player[p].m_cTask.SetSaveVal(TaskQuestProgress,current+amount>goal?goal:current+amount,TRUE);Archive(p);}
static void ShareQuest(int p,int kind){for(int peer=1;peer<MAX_PLAYER;++peer)if(Own(peer)&&members[peer].side==members[p].side&&World(peer)==Map&&(peer==p||(Player[p].m_cTeam.m_nFlag&&Player[peer].m_cTeam.m_nFlag&&Player[p].m_cTeam.m_nID==Player[peer].m_cTeam.m_nID&&NpcSet.GetDistance(Player[p].m_nIndex,Player[peer].m_nIndex)<=1024)))QuestProgress(peer,kind,1);}
static int TakeQuest(int p,int kind,bool reroll=false){if(!Own(p)||match.phase!=2||!AtBase(p)||!Transaction(p)||kind<1||kind>4)return 0;
 KPlayer& role=Player[p];int day=Day();if(role.m_cTask.GetSaveVal(TaskQuestDay)!=day){role.m_cTask.SetSaveVal(TaskQuestDay,day,TRUE);role.m_cTask.SetSaveVal(TaskQuestRuns,0,TRUE);}
 int current=ActiveQuest(p),runs=role.m_cTask.GetSaveVal(TaskQuestRuns);if(reroll?(!current||current==kind):(current||!PTThuongChuMatch::CanTakeQuest(runs)))return 0;
 int copper=reroll?1:PTThuongChuMatch::QuestCopper(runs),silver=reroll?0:40000;
 if(!PhongThanThuongChuQuestPayment(p,silver,copper))return 0;
 if(!reroll)role.m_cTask.SetSaveVal(TaskQuestRuns,runs+1,TRUE);
 role.m_cTask.SetSaveVal(TaskQuestSession,generation,TRUE);role.m_cTask.SetSaveVal(TaskQuestKind,kind,TRUE);role.m_cTask.SetSaveVal(TaskQuestProgress,0,TRUE);members[p].quest=kind;Archive(p);role.Save();return 1;
}
static int CompleteQuest(int p,bool quick=false){if(!Own(p)||match.phase!=2||!AtBase(p)||!Transaction(p))return 0;int kind=ActiveQuest(p);if(!kind)return 0;
 KPlayer& role=Player[p];if(quick){if(!PhongThanThuongChuQuestPayment(p,0,5))return 0;}
 else if(role.m_cTask.GetSaveVal(TaskQuestProgress)<PTThuongChuMatch::QuestGoal(kind))return 0;
 role.m_cTask.SetSaveVal(TaskQuestKind,0,TRUE);role.m_cTask.SetSaveVal(TaskQuestProgress,0,TRUE);members[p].quest=0;++members[p].done;
 Score(p,10);AddSaved(p,RepTask(members[p].side),15);Archive(p);role.Save();return 1;
}
static bool SpawnModern(int slot,int side,int kind,int x,int y){if(slot<0||slot>=UnitCount||side<1||side>2)return false;int world=g_SubWorldSet.SearchWorld(Map);if(world<0)return false;
 Unit& u=units[slot];Remove(u);int tid=kind==UnitTotem?(side==1?417:418):(kind==UnitFlag||kind==UnitCommanderFlag)?(side==1?419:420):kind==UnitService?190:(side==1?579:580);
 int n=NpcSet.Add(MAKELONG(120,tid),world,x*256,y*512,TRUE);if(n<=0||n>=MAX_NPC)return false;
 KNpc& npc=Npc[n];npc.SetCurrentCamp(side==1?camp_justice:camp_evil);npc.SetFightMode(enumFightActive);npc.m_btSpecial=npc_gold;
 u.index=n;u.id=npc.m_dwID;u.side=side;u.slot=slot;u.kind=kind;u.x=x;u.y=y;u.neutral=false;
 const char* label=kind==UnitTotem?(side==1?"\126\313\164\040\124\346\040\124\150\255\254\156\147":"\126\313\164\040\124\346\040\103\150\165"):kind==UnitScout?(side==1?"\124\150\270\155\040\121\165\251\156\040\124\150\255\254\156\147":"\124\150\270\155\040\121\165\251\156\040\103\150\165"):kind==UnitCommander?(side==1?"\124\150\350\156\147\040\123\157\270\151\040\124\150\255\254\156\147":"\124\150\350\156\147\040\123\157\270\151\040\103\150\165"):(side==1?"\103\352\040\124\150\255\254\156\147":"\103\352\040\103\150\165");
 lstrcpynA(npc.Name,label,sizeof(npc.Name));
 if(kind==UnitService){npc.m_Kind=kind_dialoger;npc.SetFightMode(0);char script[]="\\script\\phongthan\\thuong_chu\\mission_npc.lua";lstrcpynA(npc.ActionScript,script,sizeof(npc.ActionScript));npc.m_ActionScriptID=g_FileName2Id(script);lstrcpynA(npc.Name,side==1?"\126\250\156\040\124\162\344\156\147":"\113\150\255\254\156\147\040\124\150\255\356\156\147",sizeof(npc.Name));}
 return true;
}
static void StartModern(){
 // Slots0..5 remain the traditional marshal/general front lines.
 // Slots6..11 commanders,12..17 their flags,18..19 totems,20..21 normal flags,
 // 22..29 flag-taking scouts,30..31 authoritative camp services.
 for(int side=1;side<=2;++side){int x,y;Base(side,x,y);SpawnModern(29+side,side,UnitService,x,y);
  int tx=Map==1064?(side==1?191:202):(side==1?226:248),ty=Map==1064?(side==1?201:203):210;
  SpawnModern(17+side,side,UnitTotem,tx,ty);SpawnModern(19+side,side,UnitFlag,tx,ty-4);
  for(int i=0;i<4;++i)SpawnModern(22+(side-1)*4+i,side,UnitScout,tx+(side==1?2:-2),ty-6+i*4);
 }
}
static void DropFlag(int p){Member& m=members[p];if(!m.carried)return;int slot=m.flagSlot;if(slot>=0&&slot<UnitCount){Unit& u=units[slot];if(u.kind==UnitCommanderFlag){int x=u.x*256,y=u.y*512;if(Valid(p)&&World(p)==Map)Npc[Player[p].m_nIndex].GetMpsPos(&x,&y);SpawnModern(slot,u.side,u.kind,x/256,y/512);}else u.respawn=(unsigned long)time(NULL)+15;}m.carried=0;m.flagSlot=-1;Archive(p);}
static int Deliver(int p){if(!Own(p)||match.phase!=2||!AtBase(p)||!Transaction(p)||!members[p].carried)return 0;
 Member& m=members[p];bool commander=m.carried==2;int slot=m.flagSlot;m.carried=0;m.flagSlot=-1;if(slot>=0&&slot<UnitCount){if(commander)Remove(units[slot]);else units[slot].respawn=(unsigned long)time(NULL)+15;}
 QuestProgress(p,2,1);int points=Config(commander?"CommanderFlagPoints":"FlagPoints",commander?100:10);Score(p,points);match.score[m.side]+=points;Archive(p);
 match.DeliverFlag(m.side,commander,(unsigned long)time(NULL));if(match.phase==3){match.phase=2;Finish((unsigned long)time(NULL),m.side);}Player[p].Save();return 1;
}
static void ModernTick(unsigned long now){
 if(!modernStarted){StartModern();modernStarted=true;}
 if(match.mode==3&&now>=match.fightAt+600){static const int positions[2][3][2]={{{192,195},{185,201},{191,207}},{{204,194},{207,201},{205,208}}};
  for(int side=1;side<=2;++side)for(int k=0;k<3;++k){int slot=6+(side-1)*3+k;if(!units[slot].index&&!units[slot].dead)SpawnModern(slot,side,UnitCommander,positions[side-1][k][0],positions[side-1][k][1]);}}
 for(int slot=12;slot<30;++slot){Unit& u=units[slot];if(u.dead&&u.respawn&&now>=u.respawn){bool carried=false;for(int p=1;p<MAX_PLAYER;++p)if(Own(p)&&members[p].carried&&members[p].flagSlot==slot)carried=true;if(!carried)SpawnModern(slot,u.side,u.kind,u.x,u.y);}}
 for(int p=1;p<MAX_PLAYER;++p)if(members[p].carried&&(!Own(p)||World(p)!=Map||Npc[Player[p].m_nIndex].m_CurrentLife<=0))DropFlag(p);
}
static bool ModernDeath(Unit& u,int p){if(u.kind==UnitService)return true;if(u.dead)return true;u.dead=true;
 if(match.phase!=2||!Own(p)||members[p].left||World(p)!=Map||u.side==members[p].side){u.respawn=(unsigned long)time(NULL)+15;return true;}
 int kind=u.kind;if(kind==UnitCommander){Score(p,Config("CommanderKillPoints",50));match.score[members[p].side]+=Config("CommanderKillPoints",50);SpawnModern(12+u.slot-6,u.side,UnitCommanderFlag,u.x,u.y);}
 else if(kind==UnitFlag||kind==UnitCommanderFlag){if(!members[p].carried){members[p].carried=kind==UnitCommanderFlag?2:1;members[p].flagSlot=u.slot;u.respawn=0;Archive(p);}else u.respawn=(unsigned long)time(NULL)+15;}
 else if(kind==UnitTotem){Score(p,50);match.score[members[p].side]+=50;ShareQuest(p,4);u.respawn=(unsigned long)time(NULL)+Config("TotemRespawnSeconds",60);}
 else if(kind==UnitScout){Score(p,Config("ScoutKillPoints",2));match.score[members[p].side]+=Config("ScoutKillPoints",2);ShareQuest(p,3);u.respawn=(unsigned long)time(NULL)+15;}
 return true;
}
