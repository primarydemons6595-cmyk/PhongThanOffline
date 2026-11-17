#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef int BOOL;
typedef unsigned long DWORD;
#define TRUE 1
#define FALSE 0
#define KPHONGTHAN_PROFESSION_H
#include "../../Sources/Core/Src/KPhongThanProfessionSkills.h"
#include "../../Headers/PhongThanSkillLearning.h"
enum {PHONGTHAN_PROFESSION_COUNT=4,camp_begin=0,MAX_SKILL=3000,MAX_SKILLLEVEL=64};
struct KProfession {
    struct Record {int m_nId,m_nCamp;char m_szKey[32],m_szName[64];}m_Profession[4];
    BOOL Init();BOOL IsValid(int n){return n>=0&&n<4;}
}g_Profession;
#include "ProfessionNamesProduction.inl"
struct PHONGTHAN_CHARACTER_SKILL_RECORD {int SkillId,Level,Value;};
struct PHONGTHAN_CHARACTER_STATE_HEADER {unsigned int FightSkillCount;};
static PHONGTHAN_CHARACTER_SKILL_RECORD records[8];
static const PHONGTHAN_CHARACTER_SKILL_RECORD* PhongThanCharacterFightSkills(const PHONGTHAN_CHARACTER_STATE_HEADER*){return records;}
struct List {
    int levels[3000],experience[3000];
    void Clear(){memset(this,0,sizeof(*this));}
    int FindSame(int id){return levels[id]?id:0;}
    void Add(int id,int level,int value){levels[id]=level;experience[id]=value;}
};
struct Actor {List m_SkillList;}Npc[2];
struct Profession {int n;int GetProfession(){return n;}};
struct KPlayer {int m_nIndex;Profession m_cProfession;
    int LoadPlayerFightSkillList(const PHONGTHAN_CHARACTER_STATE_HEADER*,unsigned int&);
};
#include "LoginSkillsProduction.inl"
int main(){
    assert(g_Profession.Init());
    const char* names[]={"\x47\x69\xB8\x70\x20\x53\xDC","\xA7\xB9\x6F\x20\x53\xDC","\x44\xDE\x20\x4E\x68\xA9\x6E","\x56\xF2\x20\x53\xDC"};
    for(int n=0;n<4;++n)assert(!strcmp(names[n],g_Profession.m_Profession[n].m_szName));
    KPlayer player;player.m_nIndex=1;player.m_cProfession.n=3;
    PHONGTHAN_CHARACTER_STATE_HEADER state;state.FightSkillCount=4;
    records[0].SkillId=1696;records[0].Level=7;records[0].Value=12345;
    records[1].SkillId=1791;records[1].Level=4;records[1].Value=678;
    records[2].SkillId=27;records[2].Level=10;records[2].Value=0;
    records[3].SkillId=450;records[3].Level=10;records[3].Value=0;
    Npc[1].m_SkillList.levels[28]=10;
    unsigned int param=0;
    assert(player.LoadPlayerFightSkillList(&state,param)==1&&param==4);
    assert(Npc[1].m_SkillList.levels[1696]==7&&Npc[1].m_SkillList.experience[1696]==12345);
    assert(Npc[1].m_SkillList.levels[1791]==4&&Npc[1].m_SkillList.levels[1691]==1);
    assert(!Npc[1].m_SkillList.levels[27]&&!Npc[1].m_SkillList.levels[28]&&!Npc[1].m_SkillList.levels[450]);
    int count;const int* ids=PhongThanVuSiDisplaySkills(count);assert(count==20);
    for(int i=0;i<count;++i){assert(PhongThanProfessionOwnsSkill(3,ids[i]));for(int j=0;j<i;++j)assert(ids[j]!=ids[i]);}
    state.FightSkillCount=0;param=0;assert(player.LoadPlayerFightSkillList(&state,param)==1);
    assert(Npc[1].m_SkillList.levels[1691]==1&&!Npc[1].m_SkillList.levels[1696]);
    player.m_cProfession.n=4;param=0;assert(player.LoadPlayerFightSkillList(&state,param)==-1);
    puts("PASS actual production Vu Si login: retained skill levels/XP, stale/foreign skills removed, sparse F5 registry, empty save, invalid profession, Vietnamese names.");
    return 0;
}
