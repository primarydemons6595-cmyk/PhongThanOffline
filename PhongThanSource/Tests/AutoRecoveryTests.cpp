#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CLIENT_PLAYER_INDEX 0
#define pos_equiproom 1
#define pos_immediacy 2
#define item_medicine 1
#define PT_AUTO_RECOVERY_TEST
struct ItemPos {int nPlace,nX,nY;};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct KItem {int genre,detail,effect,part;int GetParticular(){return part;}int GetGenre(){return genre;}int GetDetailType(){return detail;}bool HasMedicineEffect(int id){return effect==id;}} Item[8];
struct Inventory {
    PlayerItem items[8];int count,cursor;
    PlayerItem* GetFirstItem(){cursor=0;return count?&items[0]:0;}
    PlayerItem* GetNextItem(){return ++cursor<count?&items[cursor]:0;}
};
struct PlayerMock {int m_nIndex,last,count;struct {int m_bIsActive;} m_cAI;Inventory m_ItemList;int ApplyUseItem(int idx,ItemPos){last=idx;++count;return 1;}} Player[1];
struct NpcMock {int m_CurrentLife,m_CurrentLifeMax,m_CurrentMana,m_CurrentManaMax,x,y;void GetMpsPos(int* a,int* b){*a=x;*b=y;}} Npc[2];
static unsigned int tick=1000;
unsigned int IR_GetCurrentTime(){return tick;}
int PhongThanAutoPoolUse(int item,ItemPos){Player[0].last=item;++Player[0].count;return 1;}
#include "../Sources/Core/Src/PhongThanAutoRecovery.inl"
int main(){
    Player[0].m_nIndex=1;Npc[1].m_CurrentLife=40;Npc[1].m_CurrentLifeMax=81;Npc[1].m_CurrentMana=20;Npc[1].m_CurrentManaMax=90;
    Inventory& bag=Player[0].m_ItemList;bag.count=4;
    for(int i=0;i<4;++i){bag.items[i].nIdx=i;bag.items[i].nPlace=pos_equiproom;Item[i].detail=0;Item[i].genre=item_medicine;Item[i].effect=153;}
    Item[0].genre=8;Item[1].effect=154;bag.items[2].nPlace=9;
    PhongThanSetRecoveryRow(0,0,-48);PhongThanRecoveryTick();
    if(Player[0].last!=3||Player[0].count!=1)return 1;
    PhongThanRecoveryTick();if(Player[0].count!=1)return 2;
    tick+=1000;Npc[1].m_CurrentLife=48;PhongThanRecoveryTick();if(Player[0].count!=1)return 3;
    Npc[1].m_CurrentLife=30;Npc[1].m_CurrentLifeMax=0;PhongThanRecoveryTick();if(Player[0].count!=1)return 4;
    Npc[1].m_CurrentLifeMax=81;PhongThanSetRecoveryRow(0,0,0);PhongThanRecoveryTick();if(Player[0].count!=1)return 5;
    PhongThanSetRecoveryRow(3,0,-25);PhongThanRecoveryTick();if(Player[0].last!=1||Player[0].count!=2)return 6;
    tick+=1000;PhongThanSetRecoveryRow(3,0,0);PhongThanSetRecoveryRow(0,0,60);PhongThanRecoveryTick();if(Player[0].last!=3||Player[0].count!=3)return 7;
    PhongThanSetRecoveryRow(0,0,0);PhongThanSetRecoveryRow(-1,0,80);tick+=1000;PhongThanRecoveryTick();if(Player[0].count!=3)return 8;
    tick+=1000;bag.count=5;bag.items[4].nIdx=4;bag.items[4].nPlace=pos_equiproom;Item[4].genre=item_medicine;Item[4].detail=2;Item[4].effect=153;
    PhongThanSetRecoveryRow(0,0,-60);PhongThanSetRecoveryRow(2,2,-35);PhongThanRecoveryTick();
    if(Player[0].last!=4||Player[0].count!=4)return 9;
    Npc[1].x=100;Npc[1].y=200;PhongThanSetAutoMode(8,100,true);Npc[1].x=250;
    if(!PhongThanAutoOutsideRange(1))return 10;Npc[1].x=160;if(PhongThanAutoOutsideRange(1))return 11;
    PhongThanSetAutoMode(0,100,false);if(PhongThanAutoOutsideRange(1))return 12;
    PhongThanSetRecoveryRow(2,2,0);PhongThanSetRecoveryRow(0,32768+28,-60);tick+=1000;
    Item[0].detail=28;Item[0].part=3;PhongThanRecoveryTick();
    if(Player[0].last!=0||Player[0].count!=5)return 13;
    tick+=1000;Item[0].part=4;PhongThanRecoveryTick();if(Player[0].count!=5)return 14;
    PhongThanSetAutoPreview(true);int ax,ay,radius;
    if(!PhongThanGetAutoRange(ax,ay,radius))return 15;
    PhongThanSetAutoPreview(false);if(PhongThanGetAutoRange(ax,ay,radius))return 16;
    puts("PASS: native medicine genre/effect/detail, location, HP/MP, threshold priority, throttle, disable/max-zero, anchored combat range.");return 0;
}
