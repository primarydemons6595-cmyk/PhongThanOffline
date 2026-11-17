// Execute unmodified production function bodies with isolated engine services.
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define _SERVER
#define _ASSERT assert
typedef int BOOL;
typedef unsigned char BYTE;
#define TRUE 1
#define FALSE 0
enum {
#define MAGIC_ATTRIB_ENTRY(id,symbol,key) symbol=id,
#include "../../Sources/Core/Src/KMagicAttribRegistry.inc"
#undef MAGIC_ATTRIB_ENTRY
    magic_require_translife=1000,magic_require_fortune_value,
    TASKVALUE_PT_REALM_SIDE=1,TASKVALUE_PT_REALM_LEVEL=2,
    REGION_GRID_WIDTH=8,REGION_GRID_HEIGHT=8,
    Obstacle_NULL=0,Obstacle_Normal=1,Obstacle_JumpFly=2,
    Obstacle_LT=3,Obstacle_RT=4,Obstacle_LB=5,Obstacle_RB=6
};
struct KMagicAttrib {int nAttribType;int nValue[3];};
struct Actor {
    int m_SubWorldIndex,m_Level,m_Series,m_nSex,m_byTranslife,hp,team;
} Npc[8];
struct Task {int GetSaveVal(int){return 0;}};
struct Profession {int m_nProfession;};
struct Owner {
    int m_nIndex,m_nCurStrength,m_nCurDexterity,m_nCurVitality,m_nCurEngergy;
    Task m_cTask;Profession m_cProfession;
} Player[2];
struct KItemList {
    int m_PlayerIdx;
    int EquipmentRealm(){return 0;}
    int GetPlayerFortune(){return 0;}
    BOOL EnoughAttrib(void*);
};
struct KRegion {
    int m_Obstacle[8][8],m_nWidth;
    BYTE refs[64];BYTE* m_pNpcRef;
    int cells[8][8];
    BYTE GetBarrierMin(int,int,int,int,BOOL,BOOL);
    int FindNpc(int x,int y,int launcher,int relation){
        int n=cells[x][y];return n&&Npc[n].team!=Npc[launcher].team?n:0;
    }
};
static bool PhongThanVttBarrierAt(KRegion*,int,int){return false;}
struct World {KRegion m_Region[1];} SubWorld[1];
struct Clock {int GetGameTime(){return 100;}} g_SubWorldSet;
struct KMissle {
    int m_ulDamageInterval,m_ulNextCalDamageTime,m_nHitCount,hits;
    int ProcessCollision(int,int,int,int,int,int);
    BOOL GetOffsetAxis(int,int,int x,int y,int dx,int dy,int& r,int& ox,int& oy){
        r=0;ox=x+dx;oy=y+dy;return ox>=0&&ox<8&&oy>=0&&oy<8;
    }
    void ProcessDamage(int n){Npc[n].hp-=1000;++hits;}
};
#include "CollisionProduction.inl"
#include "BarrierProduction.inl"
#include "RequirementsProduction.inl"
#include "../../Headers/PhongThanEquipmentAppearance.h"
struct PHONGTHAN_VISUAL_PART {int nResourceId,nPaletteId,bVisible;};
struct KPhongThanAppearance {
    BOOL ResolveOriginalTierPart(int,int,int,PHONGTHAN_VISUAL_PART*,int=0,int=0);
    BOOL ResolvePhiPhong(int,int,int,PHONGTHAN_VISUAL_PART*,int=0,int=0);
};
#define ZeroMemory(p,n) memset(p,0,n)
#include "TierProduction.inl"
#include "CapeProduction.inl"
#define KPHONGTHAN_PROFESSION_H
typedef unsigned long DWORD;
#include "../../Sources/Core/Src/KPhongThanProfessionSkills.h"
int main(){
    KRegion& r=SubWorld[0].m_Region[0];memset(&r,0,sizeof(r));
    r.m_nWidth=8;r.m_pNpcRef=r.refs;
    Npc[1].team=1;
    for(int i=2;i<=4;++i){Npc[i].team=2;Npc[i].hp=1000;r.cells[2+i-2][3]=i;}
    Npc[5].team=1;Npc[5].hp=1000;r.cells[3][4]=5;
    Npc[6].team=2;Npc[6].hp=1000;r.cells[7][7]=6;
    KMissle aoe;memset(&aoe,0,sizeof(aoe));
    assert(aoe.ProcessCollision(1,0,3,3,4,2)==3);
    assert(aoe.hits==3&&Npc[2].hp==0&&Npc[3].hp==0&&Npc[4].hp==0);
    assert(Npc[5].hp==1000&&Npc[6].hp==1000);
    aoe.hits=0;aoe.m_nHitCount=1;
    assert(aoe.ProcessCollision(1,0,3,3,4,2)==1&&aoe.hits==1);
    assert(aoe.ProcessCollision(1,0,3,3,0,2)==0);
    r.refs[3*8+3]=3;
    assert(r.GetBarrierMin(3,3,0,0,TRUE,TRUE)==Obstacle_JumpFly);
    assert(r.GetBarrierMin(3,3,0,0,FALSE,FALSE)==Obstacle_NULL);
    r.refs[3*8+3]=0;
    assert(r.GetBarrierMin(3,3,0,0,TRUE,TRUE)==Obstacle_NULL);
    puts("PASS production AOE: three enemy pet recipients, ally/outside exclusion, hit-count limit; NPC occupancy blocks movement.");
    Player[1].m_nIndex=1;Player[1].m_cProfession.m_nProfession=1;
    Npc[1].m_Series=1;Npc[1].m_Level=100;
    KItemList bag;bag.m_PlayerIdx=1;
    KMagicAttrib req;memset(&req,0,sizeof(req));
    req.nAttribType=magic_requirelevel;req.nValue[0]=98;assert(bag.EnoughAttrib(&req));
    req.nAttribType=magic_requireseries;req.nValue[0]=1;assert(bag.EnoughAttrib(&req));
    Npc[1].m_Series=0;assert(!bag.EnoughAttrib(&req));
    assert(PhongThanResolveBasicPartResource(9,1,10)==5);
    puts("PASS production audit: Dao ordinary tier-10 cape requirements pass at level 100.");
    if(PhongThanResolveBasicPartResource(9,13,10)<0)
        puts("KNOWN GAP: expanded cape appearance is unmapped by the basic tier resolver.");
    KPhongThanAppearance appearance;PHONGTHAN_VISUAL_PART visual;
    assert(appearance.ResolvePhiPhong(9,1,10,&visual)&&visual.nResourceId==5);
    assert(appearance.ResolvePhiPhong(9,52,10,&visual,1,2)&&visual.nResourceId==7);
    puts("PASS fixed production cape resolver: ordinary tier10=5; explicit realm cape=7.");
    int count=0;const int* display=PhongThanVuSiDisplaySkills(count);
    assert(count==20&&display[0]==1696&&display[19]==1791);
    puts("LIMIT: region lookup, PK relation, and damage recipient are fixtures; this does not execute full missile/AI/server login.");
    return 0;
}
