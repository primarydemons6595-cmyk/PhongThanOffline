from pathlib import Path
r=Path(__file__).resolve().parents[1];p=r/'Tests/Native/ThuongChuRuntimeTests.cpp';d=p.read_text('utf8')
d=d.replace('obj_npc=1','obj_npc=1,kind_dialoger=3,TASKVALUE_PT_COPPER_CASH=5100')
d=d.replace('int m_SubWorldIndex,m_RegionIndex','int m_Kind;char ActionScript[160];DWORD m_ActionScriptID;int m_SubWorldIndex,m_RegionIndex')
d=d.replace('int next;NpcSetType():next(100){}int Add(int,int w,int x,int y,BOOL){int n=next++;','int next;NpcSetType():next(100){}int Add(int,int w,int x,int y,BOOL){int n=100;while(n<MAX_NPC&&Npc[n].m_dwID)++n;')
d=d.replace('int PhongThanThuongChuCertificate(int){if(!certSpace)return 0;++certificates;return 1;}','int PhongThanThuongChuEconomy(int,int,int){return certSpace;}\nvoid PhongThanThuongChuBuffTick(int,unsigned long){}\nstatic DWORD g_FileName2Id(char*){return 123;}')
start=d.index('int main(){');d=d[:start]+'''static void Mode(int n){CreateDirectoryA("settings",0);CreateDirectoryA("settings\\\\phongthan",0);char value[10];sprintf(value,"%d",n);WritePrivateProfileStringA("Battle","Mode",value,"settings\\\\phongthan\\\\ThuongChu.ini");}
static bool JoinRole(int p,int side){Role(p,side);if(side==2)Npc[p].y=205*512;return Call(p,1,side)!=0;}
static void AtCamp(int p){int x,y;PtThuongChu::Base(PtThuongChu::members[p].side,x,y);Npc[p].ChangeWorld(PtThuongChu::Map,x*256,y*512);}
int main(){using namespace PtThuongChu;
 SubWorld[0].m_SubWorldID=1001;SubWorld[1].m_SubWorldID=1071;SubWorld[2].m_SubWorldID=1020;SubWorld[3].m_SubWorldID=1021;SubWorld[4].m_SubWorldID=1064;
 Mode(3);CHECK(PhongThanThuongChuOpen(600,1800));CHECK(match.mode==3&&Map==1064&&match.fightAt==10600&&match.endAt==12400);CHECK(!PhongThanThuongChuOpen(600,1800));
 Role(1,1);Npc[1].m_Level=119;CHECK(!Call(1,1,1));Npc[1].m_Level=120;CHECK(Call(1,1,1));CHECK(!Call(1,1,2));CHECK(!Call(1,2));
 for(int p=2;p<=10;++p)CHECK(JoinRole(p,p%2?1:2));CHECK(Count(1)==5&&Count(2)==5);
 Mode(1);clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.phase==2&&match.mode==3&&Map==1064);CHECK(units[30].kind==UnitService&&units[31].kind==UnitService);
 CHECK(Call(1,2)&&Call(2,2)&&Call(3,2));CHECK(World(1)==1064&&World(2)==1064&&World(3)==1064);CHECK(Player[1].revive.m_nSubWorldID==1064&&Player[1].revive.m_nMpsX==176*256);CHECK(PhongThanThuongChuRelation(1,2)==relation_enemy);CHECK(PhongThanThuongChuRelation(1,3)==relation_ally);
 CHECK(!Call(1,4,1));Player[1].m_ItemList.money=1000000;AtCamp(1);CHECK(Call(1,4,1));CHECK(Player[1].m_ItemList.money==960000&&Call(1,21)==1);CHECK(!Call(1,4,2));CHECK(!Call(1,5));
 for(int kill=0;kill<10;++kill)CHECK(PhongThanThuongChuDeath(2,1));CHECK(Call(1,20)==10);CHECK(Call(1,5)&&Call(1,14,1)==15&&Call(1,14,2)==0&&members[1].done==1);CHECK(!Call(1,5));
 CHECK(Call(1,4,4));Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,10,TRUE);int oldMoney=Player[1].m_ItemList.money,oldRuns=Call(1,21);CHECK(Call(1,17,3));CHECK(Call(1,21)==oldRuns&&Player[1].m_ItemList.money==oldMoney&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==9);
 CHECK(Call(1,16)&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==4&&Call(1,14,1)==30);CHECK(!Call(1,16));CHECK(Call(1,4,2));CHECK(Call(1,21)==3);CHECK(!Call(1,5));
 CHECK(PhongThanThuongChuDeath(units[21].index,1));CHECK(members[1].carried==1&&Call(1,27)==1);CHECK(Call(1,18));CHECK(Call(1,20)==1&&Call(1,26,1)==0);CHECK(!Call(1,18));
 CHECK(PhongThanThuongChuDeath(units[20].index,2));CHECK(members[2].carried==1);CHECK(PhongThanThuongChuDeath(2,1));CHECK(!members[2].carried&&units[20].respawn>0);
 CHECK(!units[6].index);clockSeconds+=599;PhongThanThuongChuTick();CHECK(!units[6].index);++clockSeconds;PhongThanThuongChuTick();CHECK(units[6].kind==UnitCommander&&units[11].kind==UnitCommander);
 CHECK(PhongThanThuongChuDeath(units[9].index,1));CHECK(units[15].kind==UnitCommanderFlag&&match.phase==2&&Call(1,26,1)==0);CHECK(PhongThanThuongChuDeath(units[15].index,1));CHECK(members[1].carried==2&&match.phase==2);
 Npc[1].ChangeWorld(1064,190*256,202*512);CHECK(!Call(1,18));CHECK(PhongThanThuongChuDeath(1,2));CHECK(!members[1].carried&&units[15].kind==UnitCommanderFlag&&!units[15].dead);CHECK(PhongThanThuongChuDeath(units[15].index,1));AtCamp(1);CHECK(Call(1,18)&&Call(1,26,1)==1&&match.phase==2);
 CHECK(PhongThanThuongChuDeath(units[10].index,1));CHECK(PhongThanThuongChuDeath(units[16].index,1));CHECK(Call(1,18)&&Call(1,26,1)==2&&match.phase==2);
 CHECK(PhongThanThuongChuDeath(units[11].index,1));CHECK(PhongThanThuongChuDeath(units[17].index,1));CHECK(Call(1,18));CHECK(match.phase==3&&match.winner==1&&Call(1,26,1)==3);CHECK(Player[1].m_cTask.GetSaveVal(TaskReward)==2000000&&Player[2].m_cTask.GetSaveVal(TaskReward)==700000);CHECK(!Call(1,8));
 clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0&&World(1)==1021&&Npc[1].m_CurrentCamp==7&&!Player[1].m_cPK.locked&&Player[1].revive.m_nSubWorldID==1001);CHECK(Call(1,8)==2000000&&!Call(1,8));CHECK(Player[1].m_cTask.GetSaveVal(TaskMerit)>0);
 // A new session globally chooses the new mode, retaining separate reputation.
 Mode(2);++clockSeconds;CHECK(PhongThanThuongChuOpen(600,1800));for(int p=1;p<=10;++p)CHECK(JoinRole(p,p%2?1:2));clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.mode==2&&Map==1064&&match.phase==2);CHECK(Call(1,2));AtCamp(1);Player[1].m_ItemList.money=1000000;
 Player[1].m_cTask.SetSaveVal(TaskQuestDay,Day(),TRUE);Player[1].m_cTask.SetSaveVal(TaskQuestRuns,3,TRUE);Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,2,TRUE);CHECK(!Call(1,4,4));CHECK(Player[1].m_ItemList.money==1000000&&Call(1,21)==3);
 Player[1].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,3,TRUE);CHECK(Call(1,4,4));CHECK(Player[1].m_ItemList.money==960000&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)==0&&Call(1,21)==4);
 for(int i=0;i<3;++i){CHECK(PhongThanThuongChuDeath(units[19].index,1));if(i<2){clockSeconds+=60;PhongThanThuongChuTick();}}CHECK(Call(1,20)==3);AtCamp(1);CHECK(Call(1,5)&&Call(1,14,1)==15&&Call(1,14,2)==0);
 Player[1].m_cTask.SetSaveVal(TaskQuestRuns,10,TRUE);CHECK(!Call(1,4,1));Player[1].m_cTask.SetSaveVal(TaskQuestDay,Day()-1,TRUE);CHECK(Call(1,4,1)&&Call(1,21)==1);
 clockSeconds=match.endAt;PhongThanThuongChuTick();CHECK(match.phase==3&&match.winner==1);clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0);
 Mode(1);++clockSeconds;CHECK(PhongThanThuongChuOpen(600,1800));CHECK(JoinRole(11,1));clockSeconds+=600;PhongThanThuongChuTick();CHECK(match.phase==3&&match.winner==0&&Map==1071);clockSeconds+=30;PhongThanThuongChuTick();CHECK(match.phase==0);
 printf("PASS %d production TGPT controller checks: one session mode for both sides, four quest gates, daily payments, no double awards, carrying/dropping/submitting commander flags, rewards and revival restoration\\n",checks);return 0;
}
'''
p.write_text(d,'utf8')
