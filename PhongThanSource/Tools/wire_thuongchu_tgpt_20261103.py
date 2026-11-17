"""One-time migration of the previously tested traditional controller."""
from pathlib import Path
r=Path(__file__).resolve().parents[1];p=r/'Sources/Core/Src/PhongThanThuongChu.inl';d=p.read_bytes()
backup=r.parent/'analysis/thuongchu_tgpt_20261103/PhongThanThuongChu.before.inl'
if backup.exists():assert backup.read_bytes()==d
else:backup.write_bytes(d)
def rep(a,b):
 global d
 a=a.encode();b=b.encode();assert d.count(a)==1,(a,d.count(a));d=d.replace(a,b)
rep('extern int PhongThanThuongChuCertificate(int);','extern int PhongThanThuongChuEconomy(int,int,int);')
rep('EventId=2101,Map=1071,TaskCamp','EventId=2101,TaskCamp')
rep('int side,points,quest,kills,marshal,done;','int side,points,quest,kills,marshal,done,carried,flagSlot;')
rep('done(0),left(false)','done(0),carried(0),flagSlot(-1),left(false)')
rep('struct Unit {int index,side,slot;DWORD id;bool neutral,dead;Unit():index(0),side(0),slot(0),id(0),neutral(false),dead(false){}};', 'struct Unit {int index,side,slot,kind,x,y;DWORD id;unsigned long respawn;bool neutral,dead;Unit():index(0),side(0),slot(0),kind(0),x(0),y(0),id(0),respawn(0),neutral(false),dead(false){}};')
rep('static Unit units[6];','static Unit units[32];static int Map=1071;static bool modernStarted=false;')
rep('static std::map<std::string,Member> archived;','static bool Near(int,int,int,int);static void DropFlag(int);\nstatic std::map<std::string,Member> archived;')
rep('points<=1000','points<=2000000000')
rep('Player[p].m_cTask.SetSaveVal(TaskGranted,session,TRUE);Player[p].Save();','Player[p].m_cTask.SetSaveVal(5082,PTThuongChuMatch::Points(Player[p].m_cTask.GetSaveVal(5082),GetPrivateProfileIntA("Reward","Merit",0,path)),TRUE);Player[p].m_cTask.SetSaveVal(TaskGranted,session,TRUE);Player[p].Save();')
rep('if(World(p)==Map&&!Npc','if((World(p)==Map||(World(p)==1064&&Player[p].m_cTask.GetSaveVal(TaskCamp)>0))&&!Npc')
rep('if(Own(p)){members[p].left=true;','if(Own(p)){DropFlag(p);members[p].left=true;')
rep('u.slot=slot;u.neutral=neutral;','u.slot=slot;u.kind=neutral?0:1;u.neutral=neutral;')
rep('m.done==3','m.done>0')
rep('Points=%d\\n",generation,xp,m.points);','Points=%d\\nMerit=%d\\n",generation,xp,m.points,m.points);')
assert d.count(b'side==1?191:215')==2;d=d.replace(b'side==1?191:215',b'side==1?191:205')
rep('int x=m.side==1?178:290,y=m.side==1?203:206;','int x,y;if(Map==1071){x=m.side==1?178:290;y=m.side==1?203:206;}else{x=m.side==1?176:216;y=195;}')
rep('return xp;}\r\n}', 'return xp;}\r\n#include "PhongThanThuongChuTgpt.inl"\r\n}')
rep('if(match.phase||g_SubWorldSet.SearchWorld(Map)<0||registration<1||duration<1)return false;', 'if(match.phase||registration<1||duration<1)return false;int selected=GetRandomNumber(1,3);int forced=Config("Mode",0);if(forced>=1&&forced<=3)selected=forced;Map=selected==1?1071:1064;if(g_SubWorldSet.SearchWorld(Map)<0)return false;modernStarted=false;')
rep('for(int i=0;i<6;++i)Remove(units[i]);ZeroMemory','for(int i=0;i<32;++i)Remove(units[i]);ZeroMemory')
rep('match.Open(generation,registration,duration);','match.Open(generation,registration,duration,selected);')
rep('"MinimumPerSide",15','"MinimumPerSide",5')
rep('minimum=15;','minimum=5;')
rep('if(match.phase==2){\r\n  for(int captured','if(match.phase==2){\r\n  ModernTick(now);\r\n  if(match.mode==1)for(int captured')
rep('for(int slot=0;slot<3;++slot)if(match.marshal','if(match.mode==1)for(int slot=0;slot<3;++slot)if(match.marshal')
rep('Finish(now,match.Decide(GetRandomNumber(1,2)))','Finish(now,match.mode==1?match.Decide(GetRandomNumber(1,2)):match.score[1]==match.score[2]?0:match.score[1]>match.score[2]?1:2)')
rep('for(int i=0;i<6;++i)Remove(units[i]);match.Reset();','for(int i=0;i<32;++i)Remove(units[i]);match.Reset();')
rep('for(int i=0;i<6;++i){Unit& u=units[i];if(u.index==a', 'for(int i=0;i<32;++i){Unit& u=units[i];if(u.kind==UnitService)continue;if(u.index==a')
rep('Score(dead,-1);if(Own(p)', 'DropFlag(dead);Score(dead,-1);if(Own(p)')
rep('if(members[p].quest==1)++members[p].kills;', 'ShareQuest(p,1);')
rep('for(int i=0;i<6;++i){Unit& u=units[i];if(u.index!=victim', 'for(int i=0;i<32;++i){Unit& u=units[i];if(u.index!=victim')
rep('continue;if(u.dead)return true;u.dead=true;', 'continue;if(u.kind>=UnitFlag)return ModernDeath(u,p);if(u.dead)return true;u.dead=true;')
# Traditional marshal quest and free certificate are not TGPT missions/rewards.
start=d.index(b'   // Nearby members');end=d.index(b'   bool accepted=',start);d=d[:start]+d[end:]
start=d.index(b' case 4:');end=d.index(b' case 6:',start)
d=d[:start]+b' case 4:value=TakeQuest(p,arg);break;\r\n case 5:value=CompleteQuest(p);break;\r\n'+d[end:]
start=d.index(b' case 13:');end=d.index(b'\r\n Lua_PushNumber',start)
d=d[:start]+b''' case 13:if(Transaction(p))value=PhongThanThuongChuEconomy(p,1,arg);break;
 case 10:value=Own(p)?members[p].side:0;break;
 case 11:value=ActiveQuest(p);break;case 12:value=Player[p].m_cTask.GetSaveVal(TaskScore);break;
 case 14:value=Player[p].m_cTask.GetSaveVal(RepTask(arg==1?1:2));break;
 case 15:value=Player[p].m_cTask.GetSaveVal(TaskMerit);break;
 case 16:value=CompleteQuest(p,true);break;case 17:value=TakeQuest(p,arg,true);break;
 case 18:value=Deliver(p);break;case 19:value=match.phase?match.mode:0;break;
 case 20:value=Player[p].m_cTask.GetSaveVal(TaskQuestProgress);break;
 case 21:value=Player[p].m_cTask.GetSaveVal(TaskQuestDay)==Day()?Player[p].m_cTask.GetSaveVal(TaskQuestRuns):0;break;
 case 22:case 23:case 24:case 25:if(Transaction(p))value=PhongThanThuongChuEconomy(p,op-20,arg);break;
 case 26:value=match.flags[arg==1?1:2];break;
 case 27:value=Own(p)?members[p].carried:0;break;
 }
'''+d[end:]
p.write_bytes(d)
print('Migrated production battlefield controller')
