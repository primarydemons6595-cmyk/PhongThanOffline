#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "KEngine.h"
#include "KTabFile.h"
#include "KPakFile.h"
#include "KIniFile.h"
#include "KLuaScript.h"
#include "../../Headers/PhongThanNpcStatMath.h"
#include "../../Headers/PhongThanSummonStats.h"
#include "../../Headers/PhongThanCombatState.h"
#include "../../Sources/Core/Src/PhongThanSummon.h"
KTabFile g_NpcSetting;
#include "SummonNpcStatLoader.inl"
#define MAX_PERCENT 100
#define MAX_HIT_PERCENT 95
static int hitChance=0;
static BOOL g_RandPercent(int p){hitChance=p;return p>50;}
struct HitProbe {BOOL CheckHitTarget(int,int,int=0);};
#include "SummonHitCheck.inl"
struct Actor {
    int m_Level,m_LifeMax,m_AttackRating,m_Defend,m_ExDefend;
    int m_CurrentLifeMax,m_CurrentAttackRating,attackRank,m_CurrentAttackSpeed,m_CurrentCastSpeed;
    struct {int nValue[3];}m_PhysicsDamage;
    void RestoreNpcBaseInfo(){m_CurrentLifeMax=m_LifeMax;m_CurrentAttackRating=m_AttackRating;m_CurrentAttackSpeed=m_CurrentCastSpeed=0;}
};
struct Reader {
    KLuaScript* script;int row;
    Reader(KLuaScript* s,int npc):script(s),row(npc+2){}
    int Read(int level,const char* style,const char* col){return GetNpcKeyData(script,row,level,style,col);}
};
struct BadReader {int Read(int,const char*,const char*){return -1;}};
int main(int argc,char** argv) {
    assert((argc==2||argc==3) && SetCurrentDirectoryA(argv[1]));g_SetRootPath(argv[1]);g_SetFilePath("\\");
    assert(g_NpcSetting.Load("\\settings\\phongthan\\Npcs.txt"));
    FILE* f=fopen("script/npclevelscript/standard.lua","rb");assert(f);
    fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);unsigned char* data=new unsigned char[size];
    assert(fread(data,1,size,f)==size);fclose(f);
    KLuaScript script;assert(script.Init() && script.LoadBuffer(data,size) && script.ExecuteCode());delete[] data;
    int ids[]={359,360,361,362,403,404,405,406,407};
    for(int i=0;i<9;++i)for(int rank=1;rank<=10;++rank) {
        Actor pet;memset(&pet,0,sizeof(pet));pet.attackRank=rank;Reader reader(&script,ids[i]);
        assert(PhongThanApplySummonCombatLevel(pet,90,reader,rank));
        assert(pet.m_Level==90 && pet.attackRank==rank);
        assert(pet.m_CurrentLifeMax==reader.Read(90,"Life","LifeParam"));
        Actor before=pet;assert(!PhongThanApplySummonCombatLevel(pet,0,reader,rank));assert(!memcmp(&before,&pet,sizeof(pet)));
    }
    Reader reader(&script,407);Actor cloud;memset(&cloud,0,sizeof(cloud));cloud.attackRank=10;
    assert(reader.Read(10,"Life","LifeParam")==500);
    assert(reader.Read(10,"AR","ARParam")==1150);
    assert(PhongThanApplySummonCombatLevel(cloud,90,reader,10));
    assert(cloud.m_CurrentLifeMax==1700 && cloud.m_CurrentAttackRating==4750 && cloud.attackRank==10);
    assert(cloud.m_CurrentAttackSpeed==90&&cloud.m_CurrentCastSpeed==90);
    Actor before=cloud;BadReader bad;
    assert(!PhongThanApplySummonCombatLevel(cloud,90,bad,10));assert(!memcmp(&before,&cloud,sizeof(cloud)));
    assert(PhongThanApplySummonCombatLevel(cloud,200,reader,10));assert(cloud.attackRank==10 && cloud.m_CurrentLifeMax==3350);
    assert(PhongThanApplySummonCombatLevel(cloud,90,reader,10));
    Reader boss(&script,86);int dodge=boss.Read(30,"Defense","DefenseParam");assert(dodge==110);
    HitProbe probe;probe.CheckHitTarget(1150,dodge);int oldChance=hitChance;
    probe.CheckHitTarget(cloud.m_CurrentAttackRating,dodge);int newChance=hitChance;
    assert(oldChance==91 && newChance==97); // Exact existing engine rule, not forced 100%.
    KTabFile skills;assert(skills.Load("\\settings\\skills.txt"));int attackRow=0;
    for(int row=2;row<=skills.GetHeight();++row){int id=0;skills.GetInteger(row,"SkillId",0,&id);if(id==145)attackRow=row;}
    assert(attackRow);char attackScript[256];skills.GetString(attackRow,"LvlSetScript","",attackScript,sizeof(attackScript));
    KPakFile sf;assert(sf.Open(attackScript));int attackSize=sf.Size();unsigned char* attackData=new unsigned char[attackSize];
    assert(sf.Read(attackData,attackSize)==(DWORD)attackSize);sf.Close();
    KLuaScript attackLua;assert(attackLua.Init()&&attackLua.LoadBuffer(attackData,attackSize)&&attackLua.ExecuteCode());delete[] attackData;
    int top=0;attackLua.SafeCallBegin(&top);
    assert(attackLua.CallFunction("GetSkillLevelData",1,"ssd","physicsenhance_p","",10));
    const char* enhancement=Lua_ValueToString(attackLua.m_LuaState,Lua_GetTopIndex(attackLua.m_LuaState));assert(enhancement);
    int physicalPercent=atoi(enhancement);assert(physicalPercent==150);attackLua.SafeCallEnd(top);
    int low=0,high=0;
    PhongThanComposeOriginalPhysics(cloud.m_PhysicsDamage.nValue[0],cloud.m_PhysicsDamage.nValue[2],
        1,physicalPercent,0,1,2,1,0,cloud.m_CurrentLifeMax,0,0,&low,&high);
    assert(low==2250 && high==2250);
    assert(low-boss.Read(30,"ExDefense","ExDefenseParam")/2==2162);
    printf("PASS real NPC table + Lua4: 9 summons x 10 ranks; owner90 rank10 HP500->1700 AR1150->4750; CuuAnh30 hit=%d->%d damage=2162 per physical hit (unbuffed)\n",oldChance,newChance);
    if(argc==3) {
        KTabFile summons;assert(summons.Load("\\settings\\summonskill.txt"));int summonRow=0;
        for(int r=2;r<=summons.GetHeight();++r){int id=0;summons.GetInteger(r,"id",0,&id);if(id==458)summonRow=r;}
        assert(summonRow);int base=0,param=0,delay=0,range=0;
        summons.GetInteger(summonRow,"costvalue",0,&base);summons.GetInteger(summonRow,"costparam",0,&param);
        summons.GetInteger(summonRow,"casttime",0,&delay);skills.GetInteger(attackRow,"AttackRadius",0,&range);
        int animation=20;g_NpcSetting.GetInteger(409,"AttackSpeed",20,&animation);
        int frames=animation*100/(100+cloud.m_CurrentAttackSpeed);frames-=frames%2;if(frames<=0)frames=1;
        assert(frames==6);
        int pkRate=20;KIniFile ini;BOOL pkPresent=ini.Load("\\settings\\npc\\PKSet.ini");
        if(pkPresent)ini.GetInteger("PK","Rate",20,&pkRate);
        FILE* fixture=fopen(argv[2],"wb");assert(fixture);
        fprintf(fixture,"{\"OwnerLevel\":90,\"Rank\":10,\"HP\":%d,\"AR\":%d,\"Damage\":%d,\"Range\":%d,\"Cost\":%d,\"RecastBeats\":%d,\"PvpPercent\":%d,\"AttackFrames\":%d,\"PkFilePresent\":%s}\n",
            cloud.m_CurrentLifeMax,cloud.m_CurrentAttackRating,low,range,PhongThanSummonCost(base,param,10),delay*2/18,pkRate,frames,pkPresent?"true":"false");fclose(fixture);
    }
    return 0;
}
