// Headless reference simulation. Not a CoreServer integration test.
// Damage composition below calls production helpers; movement, AI, resistance
// and AOE are explicit normalized test fixtures (see Docs/THREE_FACTION_SMOKE.md).
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Headers/PhongThanCombatState.h"
#include "../../Headers/PhongThanDamageShield.h"
#include "../../Headers/PhongThanFactionBalance.h"

enum { GiapSi, DaoSi, DiNhan, PetCount=3, MaxTicks=600 };
static const char* Names[]={"Giap Si","Dao Si","Di Nhan"};
struct Rng {
    unsigned int state;
    Rng(unsigned int seed):state(seed){}
    unsigned int Next(){state=1664525U*state+1013904223U;return state;}
    int Range(int a,int b){return a+(int)(Next()%(unsigned int)(b-a+1));}
};
struct Pet {int hp,x,attack;};
struct Actor {
    int faction,hp,maxHp,mana,physical,internal,physicalResist,magicResist;
    int x,speed,range,period,next,shield;
    Pet pets[PetCount];
};
struct Options {bool aoe,blocking,giapHp,giapSpeed;Options():aoe(true),blocking(true),giapHp(true),giapSpeed(true){}};
struct Stats {
    int wins,losses,draws,aoeHits,petKills,blocked,ownerHits,petHits,firstMelee,giapLife;
    Stats(){memset(this,0,sizeof(*this));}
};
static int failures=0;
static void Check(bool ok,const char* message){
    if(!ok){++failures;fprintf(stderr,"FAIL: %s\n",message);}
}
static int Abs(int n){return n<0?-n:n;}
// Optional runtime-policy sensitivity; defaults preserve the reference fixture.
static bool modelPvp=false,modelPetPvp=false;
static int modelPvpPercent=100;
static Actor MakeActor(int faction,int x,Rng& rng,const Options& options){
    Actor a;memset(&a,0,sizeof(a));a.faction=faction;a.x=x;
    // Equal progression/gear budget, intentionally different class strengths.
    a.maxHp=faction==GiapSi?10000:faction==DaoSi?6500:8000;
    if(faction==GiapSi&&!options.giapHp)a.maxHp=6500;
    a.hp=a.maxHp*rng.Range(90,110)/100;
    a.maxHp=a.hp;a.mana=30000;
    a.physical=faction==GiapSi?1300:180;
    a.internal=faction==DaoSi?1550:faction==DiNhan?300:0;
    a.physicalResist=faction==GiapSi?75:faction==DaoSi?0:20;
    a.magicResist=faction==GiapSi?40:25;
    a.speed=faction==GiapSi?45:0;
    if(faction==GiapSi&&!options.giapSpeed)a.speed=10;
    a.range=faction==GiapSi?40:420;
    a.period=faction==DaoSi?3:2;
    for(int i=0;i<PetCount;++i){
        a.pets[i].hp=faction==DiNhan?4000:0;
        a.pets[i].x=x+(x>0?-80:80);
        a.pets[i].attack=600;
    }
    return a;
}
static int RawHit(Actor& a,Rng& rng){
    int low=0,high=0;
    if(a.faction==GiapSi){
        PhongThanComposeOriginalPhysics(a.physical*85/100,a.physical*115/100,
            0,0,0,10,11,1,0,a.maxHp,0,0,&low,&high);
    }else{
        int skill[3]={a.internal*85/100,0,a.internal*115/100};
        int item[3]={0,0,0},out[3];
        PhongThanComposeOriginalElement(1,1,skill,item,0,0,a.mana,0,0,0,0,out);
        low=out[0];high=out[2];
    }
    return rng.Range(low,high);
}
static void Hurt(int& hp,int raw,int resist,int& shield,int attacker,int victim,bool petVictim=false){
    // Fixture subset of KNpc::CalcDamage: finite armor first, then resistance.
    PhongThanAbsorbDamage(raw,shield);
    if(resist>75)resist=75;if(resist< -75)resist=-75;
    int damage=raw*(100-resist)/100;
    if(modelPvp && (!petVictim || modelPetPvp))damage=PhongThanScaleCombatDamage(damage,modelPvpPercent);
    damage=PhongThanFactionDamage(damage,attacker,victim);
    if(damage>0){hp-=damage;if(hp<0)hp=0;}
}
static int NearestPet(Actor& target,int x){
    int nearest=-1;
    for(int i=0;i<PetCount;++i)if(target.pets[i].hp>0 &&
        (nearest<0 || Abs(target.pets[i].x-x)<Abs(target.pets[nearest].x-x)))nearest=i;
    return nearest;
}
static void Act(Actor& a,Actor& b,int tick,Rng& rng,const Options& options,Stats& s){
    if(a.hp<=0||b.hp<=0)return;
    int pet=NearestPet(b,a.x);
    bool blocked=options.blocking&&a.faction==GiapSi&&pet>=0&&
        Abs(b.pets[pet].x-a.x)<=a.range;
    if(blocked)++s.blocked;
    int targetX=blocked?b.pets[pet].x:b.x;
    if(Abs(targetX-a.x)>a.range){
        int distance=Abs(targetX-a.x)-a.range;
        int step=distance<a.speed?distance:a.speed;
        a.x+=targetX>a.x?step:-step;
        return;
    }
    if(tick<a.next)return;
    a.next=tick+a.period;
    if(a.faction!=GiapSi){if(a.mana<50)return;a.mana-=50;}
    int raw=RawHit(a,rng),zero=0;
    if(blocked){
        Hurt(b.pets[pet].hp,raw,20,zero,a.faction,b.faction,true);
        if(!b.pets[pet].hp)++s.petKills;
        ++s.petHits;return;
    }
    // AOE selects a live pet cluster, otherwise its owner. Enemy pets are
    // independent damage recipients, never a multiplier on owner damage.
    if(a.faction==DaoSi&&options.aoe){
        int center=pet>=0?b.pets[pet].x:b.x;
        for(int i=0;i<PetCount;++i)if(b.pets[i].hp>0&&Abs(b.pets[i].x-center)<=120){
            Hurt(b.pets[i].hp,raw,20,zero,a.faction,b.faction,true);++s.aoeHits;
            if(!b.pets[i].hp)++s.petKills;
        }
        if(Abs(b.x-center)>120)return;
    }
    Hurt(b.hp,raw,a.faction==GiapSi?b.physicalResist:b.magicResist,b.shield,a.faction,b.faction);
    if(a.faction==GiapSi){++s.ownerHits;if(!s.firstMelee)s.firstMelee=tick+1;}
}
static void PetsAct(Actor& owner,Actor& enemy,int tick,Rng& rng){
    if(owner.hp<=0||enemy.hp<=0)return;
    for(int i=0;i<PetCount;++i){Pet& p=owner.pets[i];if(p.hp<=0)continue;
        int d=Abs(p.x-enemy.x);
        if(d>40){int step=d-40<30?d-40:30;p.x+=p.x<enemy.x?step:-step;}
        else if(tick%2==0)Hurt(enemy.hp,p.attack*rng.Range(85,115)/100,enemy.physicalResist,enemy.shield,owner.faction,enemy.faction);
    }
}
static Stats Duel(int first,int second,unsigned int seed,const Options& options){
    Rng rng(seed);Actor a=MakeActor(first,0,rng,options),b=MakeActor(second,400,rng,options);Stats s;
    for(int tick=0;tick<MaxTicks&&a.hp>0&&b.hp>0;++tick){
        // Seeded initiative avoids a permanent first-argument advantage.
        if(rng.Next()&0x10000U){Act(a,b,tick,rng,options,s);Act(b,a,tick,rng,options,s);}
        else{Act(b,a,tick,rng,options,s);Act(a,b,tick,rng,options,s);}
        PetsAct(a,b,tick,rng);PetsAct(b,a,tick,rng);
    }
    if(a.hp>0&&b.hp==0)s.wins=1;
    else if(b.hp>0&&a.hp==0)s.losses=1;
    else s.draws=1; // Timeout/double KO is never counted as a win.
    s.giapLife=a.faction==GiapSi?a.hp:b.faction==GiapSi?b.hp:0;
    return s;
}
static Stats Run(int first,int second,int count,unsigned int seed,const Options& options){
    Stats sum;
    for(int n=0;n<count;++n){Stats s=Duel(first,second,seed+(unsigned int)n*7919U,options);
        sum.wins+=s.wins;sum.losses+=s.losses;sum.draws+=s.draws;
        sum.aoeHits+=s.aoeHits;sum.petKills+=s.petKills;sum.blocked+=s.blocked;
        sum.ownerHits+=s.ownerHits;sum.petHits+=s.petHits;sum.firstMelee+=s.firstMelee;
        sum.giapLife+=s.giapLife;
    }
    return sum;
}
int main(int argc,char** argv){
    unsigned int seed=20261027U;Options options;
    for(int arg=1;arg<argc;++arg){
        if(!strcmp(argv[arg],"--disable-aoe"))options.aoe=false;
        else if(!strcmp(argv[arg],"--disable-blocking"))options.blocking=false;
        else if(!strcmp(argv[arg],"--seed")&&arg+1<argc){
            char* end=0;unsigned long n=strtoul(argv[++arg],&end,10);
            if(!argv[arg][0]||*end){fprintf(stderr,"Invalid seed\n");return 2;}seed=(unsigned int)n;
        }else{fprintf(stderr,"Usage: ThreeFactionSmoke [--seed N] [--disable-aoe] [--disable-blocking]\n");return 2;}
    }
    const int count=300;Stats results[3];
    puts("REFERENCE MODEL: production damage composition; fixture AI/AOE/collision/stats. 300 battles per pair (900 total).");
    printf("seed=%u; aoe=%d; blocking=%d\n",seed,options.aoe,options.blocking);
    for(int pair=0;pair<3;++pair){int other=(pair+1)%3;
        results[pair]=Run(pair,other,count,seed,options);Stats& s=results[pair];
        printf("%s vs %s: %d/%d = %.2f%%; losses=%d draws=%d aoeHits=%d petKills=%d blockedTicks=%d\n",
            Names[pair],Names[other],s.wins,count,s.wins*100.0/count,s.losses,s.draws,s.aoeHits,s.petKills,s.blocked);
        Check(s.wins+s.losses+s.draws==count,"battle accounting");
        Check(s.wins*100>55*count,"expected counter must win strictly >55% (>=166/300)");
        Check(s.draws==0,"no unresolved battles in balanced fixture");
    }
    Check(results[0].firstMelee>0,"Giap Si must close distance and hit Dao Si");
    Options equalHp=options;equalHp.giapHp=false;
    Stats absentHp=Run(GiapSi,DaoSi,count,seed,equalHp);
    Options slow=options;slow.giapSpeed=false;
    Stats absentSpeed=Run(GiapSi,DaoSi,count,seed,slow);
    printf("Giap ablation: equal HP wins=%d/%d; slow approach wins=%d/%d\n",absentHp.wins,count,absentSpeed.wins,count);
    Check(results[0].giapLife>absentHp.giapLife,"removing Giap Si HP advantage must reduce surviving HP");
    Check(results[0].wins>absentSpeed.wins,"slowing Giap Si approach must reduce wins");
    Check(results[1].aoeHits>0&&results[1].petKills>0,"Dao Si AOE must damage and clear pets");
    Check(results[2].blocked>0&&results[2].petHits>0,"pets must block melee and divert single-target attacks");
    Options noAoe=options;noAoe.aoe=false;
    Stats absentAoe=Run(DaoSi,DiNhan,count,seed,noAoe);
    Options noBlock=options;noBlock.blocking=false;
    Stats absentBlock=Run(DiNhan,GiapSi,count,seed,noBlock);
    printf("Ablation: Dao wins without AOE=%d/%d; Di Nhan wins without blocking=%d/%d; Giap owner hits blocked=%d unblocked=%d\n",
        absentAoe.wins,count,absentBlock.wins,count,results[2].ownerHits,absentBlock.ownerHits);
    Check(results[1].wins>absentAoe.wins,"removing AOE must hurt Dao Si win rate");
    Check(results[2].wins>absentBlock.wins,"removing blocking must hurt Di Nhan win rate");
    Check(results[2].ownerHits<absentBlock.ownerHits,"blocking must reduce Giap Si attacks on owner");
    printf("%s: %d failure(s)\n",failures?"FAIL":"PASS",failures);
    return failures?1:0;
}
