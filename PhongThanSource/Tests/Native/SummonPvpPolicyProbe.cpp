#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../Headers/PhongThanFactionBalance.h"
enum {damage_physics,damage_magic};
struct Actor {int m_Series;} Npc[4];
struct NpcSetMock {int m_nPKDamageRate;} NpcSet;
struct VictimProbe {
    bool player;
    bool IsPlayer(){return player;}
    int Apply(int attacker,int victim) {
        int attackOwner=1,victimOwner=2,nType=damage_physics,nDamage=1000;
        bool factionPvp=true,bReturn=false;
        Npc[1].m_Series=attacker;Npc[2].m_Series=victim;
#include "SummonPvpPolicy.inl"
        return nDamage;
    }
};
int main(int argc,char** argv) {
    if(argc!=2)return 2;NpcSet.m_nPKDamageRate=atoi(argv[1]);
    VictimProbe human,pet;human.player=true;pet.player=false;
    for(int a=0;a<3;++a)for(int b=0;b<3;++b) {
        int h=human.Apply(a,b),p=pet.Apply(a,b);
        printf("PRODUCTION_POLICY attacker=%d victim=%d playerDamage=%d petDamage=%d\n",a,b,h,p);
        assert(h==PhongThanFactionDamage(PhongThanScaleCombatDamage(1000,NpcSet.m_nPKDamageRate),a,b));
        assert(p==h);
    }
    puts("PASS: all nine pairs apply the same PvP base to owner-credited pets and players.");
    return 0;
}
