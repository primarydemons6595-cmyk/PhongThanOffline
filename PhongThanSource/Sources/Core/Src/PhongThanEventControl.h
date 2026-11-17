#ifndef PT_EVENT_CONTROL_H
#define PT_EVENT_CONTROL_H
#ifdef _SERVER
#include <string>
bool PhongThanEventOwnsSchedule(const char* path);
bool PhongThanEventOwns(int eventId);
bool PhongThanEventAccepting(int eventId);
int PhongThanEventValue(int eventId,int field);
void PhongThanEventBindNpc(int npcIndex);
bool PhongThanEventApplyDropProfile(int npcIndex,unsigned long entity,std::string slots[4]);
bool PhongThanEventLoadTable(const std::string& path,std::string& bytes,std::string& error,bool& handled);
void PhongThanEventControlTick();
#endif
#endif
