#ifndef PHONGTHAN_TERRITORY_H
#define PHONGTHAN_TERRITORY_H
void PhongThanTerritoryTick();
void PhongThanTerritoryOpen(int player,int operation);
int PhongThanTerritoryCount();
int PhongThanTerritoryPlayerValue(int player,int field);
int PhongThanTerritoryBeginDonation(int player,int type,int tier,int before,int& after);
int PhongThanTerritoryFinishDonation(int player,bool loaded=false);
void PhongThanTerritoryCancelDonation(int player,int after);
int PhongThanTerritoryBeginCreate(int player,int land,int totem,int before,int& after);
int PhongThanTerritoryCityAt(int index,char* name,int& level);
int PhongThanTerritoryBeginBook(int player,int pool,int book,int before,int& after);
int PhongThanTerritoryBookCount(int player,int pool);
int PhongThanTerritoryAction(int player,int operation,int value,int second,const char* text);
int PhongThanTerritoryWorld(int map);
bool PhongThanTerritoryCityInfo(int map,char* name,int& owner,int& level,int& land,char* country);
#endif
