// Sensitivity assessment, NOT a full GameServer PvP integration test.
// Player HP/gear/AI remain the documented normalized reference fixture.
// One summon uses real template407 values for owner90/rank10, unlike the old
// three-pet fixture. Never tune production or assert win rates to pass this.
#include "../../Headers/PhongThanFactionBalance.h"
static bool counterEnabled=true;
static int ModelFactionDamage(int damage,int a,int b) {
    return counterEnabled?PhongThanFactionDamage(damage,a,b):damage;
}
#define PhongThanFactionDamage ModelFactionDamage
#define main ThreeFactionReferenceMain
#include "ThreeFactionSmoke.cpp"
#undef main
#undef PhongThanFactionDamage
#define MAX_PERCENT 100
#define MAX_HIT_PERCENT 95
typedef int BOOL;
#define TRUE 1
#define FALSE 0
static Rng* hitRng=0;
static BOOL g_RandPercent(int p){return hitRng->Range(0,99)<p;}
struct HitProbe {BOOL CheckHitTarget(int,int,int=0);};
#include "SummonHitCheck.inl"
struct SummonFixture {int hp,ar,damage,range,cost,recast,attackFrames;};
struct OneResult {
    int wins,losses,draws,casts,hits,misses;
    OneResult(){memset(this,0,sizeof(*this));}
};
static Actor RuntimeActor(int faction,int x,Rng& rng,const Options& options,const SummonFixture& pet) {
    Actor a=MakeActor(faction,x,rng,options);
    for(int i=0;i<PetCount;++i)a.pets[i].hp=0;
    if(faction==DiNhan){a.pets[0].hp=pet.hp;a.pets[0].attack=pet.damage;a.mana-=pet.cost;}
    return a;
}
static void OnePetAct(Actor& owner,Actor& enemy,int tick,Rng& rng,const SummonFixture& f,
    int& recastAt,bool enabled,OneResult& result) {
    if(!enabled||owner.faction!=DiNhan||owner.hp<=0||enemy.hp<=0)return;
    Pet& pet=owner.pets[0];
    if(pet.hp<=0 && tick>=recastAt && owner.mana>=f.cost) {
        pet.hp=f.hp;pet.x=owner.x+(owner.x>enemy.x?-80:80);pet.attack=f.damage;
        owner.mana-=f.cost;recastAt=tick+f.recast;++result.casts;
        return; // Summon takes an action; no immediate attack in this beat.
    }
    if(pet.hp<=0)return;
    int distance=Abs(pet.x-enemy.x);
    if(distance>f.range){int step=distance-f.range<30?distance-f.range:30;pet.x+=pet.x<enemy.x?step:-step;return;}
    // Player dodge is a shared scenario variable, not recovered live gear.
    hitRng=&rng;HitProbe probe;
    int shots=(tick+1)*9/f.attackFrames-tick*9/f.attackFrames;
    for(int shot=0;shot<shots&&enemy.hp>0;++shot)if(probe.CheckHitTarget(f.ar,2000)) {
        int before=enemy.hp;Hurt(enemy.hp,pet.attack,enemy.physicalResist,enemy.shield,owner.faction,enemy.faction);
        if(enemy.hp<before)++result.hits;
    }else ++result.misses;
}
static OneResult OneDuel(int first,int second,unsigned int seed,const SummonFixture& pet,bool summons) {
    Rng rng(seed);Options options;
    Actor a=RuntimeActor(first,0,rng,options,pet),b=RuntimeActor(second,400,rng,options,pet);
    if(!summons){a.pets[0].hp=b.pets[0].hp=0;if(first==DiNhan)a.mana+=pet.cost;if(second==DiNhan)b.mana+=pet.cost;}
    int recastA=pet.recast,recastB=pet.recast;OneResult result;
    if(summons && (first==DiNhan||second==DiNhan))result.casts=1;
    Stats ignored;
    for(int tick=0;tick<MaxTicks&&a.hp>0&&b.hp>0;++tick) {
        if(rng.Next()&0x10000U){Act(a,b,tick,rng,options,ignored);Act(b,a,tick,rng,options,ignored);}
        else{Act(b,a,tick,rng,options,ignored);Act(a,b,tick,rng,options,ignored);}
        OnePetAct(a,b,tick,rng,pet,recastA,summons,result);
        OnePetAct(b,a,tick,rng,pet,recastB,summons,result);
    }
    if(a.hp>0&&b.hp==0)result.wins=1;
    else if(b.hp>0&&a.hp==0)result.losses=1;
    else result.draws=1;
    return result;
}
int main(int argc,char** argv) {
    if(argc!=9){fprintf(stderr,"Usage: OneSummonSmoke HP AR Damage Range Cost RecastBeats PvpPercent AttackFrames\n");return 2;}
    SummonFixture pet={atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]),atoi(argv[6]),atoi(argv[8])};
    if(pet.hp<=0||pet.ar<=0||pet.damage<=0||pet.range<=0||pet.cost<=0||pet.recast<=0||pet.attackFrames<1)return 2;
    modelPvp=true;modelPvpPercent=atoi(argv[7]);if(modelPvpPercent<1||modelPvpPercent>100)return 2;
    printf("ASSESSMENT: normalized player gear/AI; one ranged summon HP=%d AR=%d rawDamagePerHit=%d range=%d mana=%d recast=%d half-second beats; animation=%d frames at18fps; one connecting projectile/cast; player dodge=2000.\n",pet.hp,pet.ar,pet.damage,pet.range,pet.cost,pet.recast,pet.attackFrames);
    const unsigned int seeds[]={20261030U,42U,12345U};const int count=300;
    int pairs[3][2]={{GiapSi,DaoSi},{DaoSi,DiNhan},{DiNhan,GiapSi}};
    for(int mode=0;mode<4;++mode) {
        counterEnabled=mode!=1;bool summons=mode!=2;
        modelPetPvp=mode!=3;
        printf("MODE counter=%d summon=%d pet_pvp_scaled=%d pvp_percent=%d%s\n",counterEnabled,summons,modelPetPvp,modelPvpPercent,mode==3?" PREVIOUS_ASYMMETRIC_POLICY":" CURRENT_POLICY");
        for(int pair=0;pair<3;++pair) {
            OneResult total;
            for(int seedIndex=0;seedIndex<3;++seedIndex)for(int side=0;side<2;++side)for(int n=0;n<count;++n) {
                int first=pairs[pair][side],second=pairs[pair][1-side];
                OneResult r=OneDuel(first,second,seeds[seedIndex]+n*7919U,pet,summons);
                total.wins+=side?r.losses:r.wins;total.losses+=side?r.wins:r.losses;
                total.draws+=r.draws;total.casts+=r.casts;total.hits+=r.hits;total.misses+=r.misses;
            }
            if(total.wins+total.losses+total.draws!=1800)return 1;
            if(summons && pair>0 && total.casts<1800)return 1;
            printf("RESULT %s -> %s wins=%d losses=%d draws=%d total=1800 rate=%.2f%% casts=%d petHits=%d petMisses=%d\n",
                Names[pairs[pair][0]],Names[pairs[pair][1]],total.wins,total.losses,total.draws,
                total.wins*100.0/1800,total.casts,total.hits,total.misses);
        }
    }
    puts("PASS mechanics/accounting; win rates are observations, not a PvP-balance certificate.");
    return 0;
}
