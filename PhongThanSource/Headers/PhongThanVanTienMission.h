#ifndef PHONGTHAN_VAN_TIEN_MISSION_H
#define PHONGTHAN_VAN_TIEN_MISSION_H
// Only the four restored PT arenas use the recovered mission dispatchers.
// Legacy missions in other worlds retain their existing scripts.
static void PhongThanMissionScriptPath(char* path, unsigned long mission, int world)
{
    if (world >= 1067 && world <= 1070 && mission == (unsigned long)(world - 1066))
        sprintf(path, "\\script\\phongthan\\van_tien\\mission%02d.lua", mission);
    else
        sprintf(path, "\\script\\missions\\mission%02d.lua", mission);
}
#endif
