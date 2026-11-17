#include "../../../Headers/PhongThanPills.h"
#ifdef _SERVER
static PTPillsState PTReadPills(KPlayer& player) {
    PTPillsState s;int* v=(int*)&s;
    for(int i=0;i<9;++i)v[i]=player.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_BEGIN+i);
    if(s.Count<0 || s.Count>84)s.Count=0;if(s.NewCount<0 || s.NewCount>12)s.NewCount=0;
    if(s.MadeToday<0 || s.MadeToday>12)s.MadeToday=0;
    if(s.Seconds<0 || s.Seconds>=86400)s.Seconds=0;
    return s;
}
static void PTWritePills(KPlayer& player,const PTPillsState& s) {
    const int* v=(const int*)&s;
    for(int i=0;i<9;++i)if(player.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_BEGIN+i)!=v[i])
        player.m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_BEGIN+i,v[i],i!=3);
}
void PhongThanPillsRestore(KPlayer& player) {
    PTPillsState s=PTReadPills(player);KIniFile ini;int interval=3600;
    if(ini.Load("\\settings\\PhongThanAlchemy.ini"))ini.GetInteger("Alchemy","OfflineSecondsPerPill",3600,&interval);
    PTPillsRestore(s,(int)KSG_GetCurSec(),interval);PTWritePills(player,s);
}
void PhongThanPillsTick(KPlayer& player) {
    PTPillsState s=PTReadPills(player);int now=(int)KSG_GetCurSec();
    if(s.Armed)s.LastSeen=now;
    PTPillsExpire(s,now);PTWritePills(player,s);
}
int PhongThanPillsBonus(KPlayer& player) {
    return player.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_UNTIL)>(int)KSG_GetCurSec()?
        player.m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_BONUS):0;
}
#endif
