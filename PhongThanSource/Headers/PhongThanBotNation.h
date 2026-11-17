#ifndef PHONGTHAN_BOT_NATION_H
#define PHONGTHAN_BOT_NATION_H
#include "PhongThanNationProtocol.h"
struct PTBotNationInfo {unsigned int id;int duty,members,money,cityMap,cityLevel,popularity,bronze,today,pending;char name[32],leader[32];};
#ifdef _SERVER
bool PhongThanBotNationInfo(int player,PTBotNationInfo* result);
bool PhongThanBotNationAt(int index,PTBotNationInfo* result);
int PhongThanBotNationPendingAt(int player,int index,char* target,int size);
int PhongThanBotNationAction(int player,int operation,unsigned int nation,int value,const char* target,const char* name);
int PhongThanBotCityRequirement(int land,int field);
#endif
#endif
