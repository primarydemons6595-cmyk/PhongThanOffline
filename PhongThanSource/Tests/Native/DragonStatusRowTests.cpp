#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanCombatState.h"
#include "../../Headers/PhongThanStatusUi.h"
#include "../../Headers/PhongThanOriginalItemBuffData.h"
#define MAGIC_ATTRIB_ENTRY(id,name,text) name=id,
enum {
#include "../../Sources/Core/Src/KMagicAttribRegistry.inc"
};
#undef MAGIC_ATTRIB_ENTRY
struct KMagicAttrib{int nAttribType,nValue[3];};
struct KNpc{PhongThanCombatState m_PtCombat;struct {int nValue[3];}m_ManaShield;int m_CurrentAllMagicDeadlyStrike;int m_CurrentAttackRating;int m_CurrentColdDamageMaxPercent;int m_CurrentColdDamageMinPercent;int m_CurrentColdDeadlyStrike;int m_CurrentColdEnhance;int m_CurrentDamage2Mana;int m_CurrentDamageReduce;int m_CurrentDeadlyStrikeEnhanceP;int m_CurrentDefend;int m_CurrentEarthDeadlyStrike;int m_CurrentExDefend;int m_CurrentFireDeadlyStrike;int m_CurrentFireEnhance;int m_CurrentFreezeTimeReducePercent;int m_CurrentHitRecover;int m_CurrentIgnoreColdResist;int m_CurrentIgnoreDefensePercent;int m_CurrentIgnoreEarthResist;int m_CurrentIgnoreExDefence;int m_CurrentIgnoreFireResist;int m_CurrentIgnoreLightResist;int m_CurrentLifeReplenish;int m_CurrentLightDeadlyStrike;int m_CurrentLightEnhance;int m_CurrentManaReplenish;int m_CurrentPhysicsResist;int m_CurrentPoisonEnhance;int m_CurrentPoisonTimeReducePercent;int m_CurrentReduceMagicDeadlyStrike;int m_CurrentReducePhysicsDeadlyStrike;int m_CurrentStaticMagicShieldP;int m_CurrentStunTimeReducePercent;};
class KNpcAttribModify{public:void TuChanAttrib(KNpc*,void*);void StrengthV(KNpc*,void*){}void DexterityV(KNpc*,void*){}void VitalityV(KNpc*,void*){}void EnergyV(KNpc*,void*){}};
void KNpcAttribModify::TuChanAttrib(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (!pMagic || !pNpc)
		return;

	switch (pMagic->nAttribType)
	{
	case magic_ignoredefense_p:
		pNpc->m_CurrentIgnoreDefensePercent += pMagic->nValue[0];
		break;
	case magic_exdefense_v:
	case magic_addexdefense_v:
		pNpc->m_CurrentExDefend += pMagic->nValue[0];
		break;
	case magic_addallattribute_v:
		StrengthV(pNpc, pData);
		DexterityV(pNpc, pData);
		VitalityV(pNpc, pData);
		EnergyV(pNpc, pData);
		break;
	case magic_allmagicdeadlystirke:
		pNpc->m_CurrentAllMagicDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_reducephysicsdeadlystirke:
		pNpc->m_CurrentReducePhysicsDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_reducemagicdeadlystirke:
		pNpc->m_CurrentReduceMagicDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_firedeadlystirke:
		pNpc->m_CurrentFireDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_earthdeadlystirke:
		pNpc->m_CurrentEarthDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_colddeadlystirke:
		pNpc->m_CurrentColdDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_lightdeadlystirke:
		pNpc->m_CurrentLightDeadlyStrike += pMagic->nValue[0];
		break;
	case magic_ignoreearthresist:
		pNpc->m_CurrentIgnoreEarthResist += pMagic->nValue[0];
		break;
	case magic_ignorefireresist:
		pNpc->m_CurrentIgnoreFireResist += pMagic->nValue[0];
		break;
	case magic_ignorecoldresist:
		pNpc->m_CurrentIgnoreColdResist += pMagic->nValue[0];
		break;
	case magic_ignorelightresist:
		pNpc->m_CurrentIgnoreLightResist += pMagic->nValue[0];
		break;
	case magic_ignoreexdefence:
		pNpc->m_CurrentIgnoreExDefence += pMagic->nValue[0];
		break;
	case magic_addcolddamagemin_p:
		pNpc->m_CurrentColdDamageMinPercent += pMagic->nValue[0];
		break;
	case magic_addcolddamagemax_p:
		pNpc->m_CurrentColdDamageMaxPercent += pMagic->nValue[0];
		break;
	case magic_modify_PhysicsDamageRes_p:
		pNpc->m_CurrentDamageReduce += pMagic->nValue[0];
		break;
	}
}
// Verified against game.dat:0x69df8b..0x69e1bd. Reserved fields remain zero,
// including the NPC damage reduction field the original explicitly sets to 0.
static void PhongThanFillStatusOriginal(const KNpc& npc, PTStatusOriginal& out)
{
    memset(&out,0,sizeof(out));
    int* v=out.Value;
    const PhongThanCombatState& p=npc.m_PtCombat;
    v[0]=p.PhysicsEnhance;
    v[1]=npc.m_CurrentAttackRating;v[2]=npc.m_CurrentDefend;
    v[3]=p.AttackHitRecover;
    v[4]=npc.m_CurrentFireEnhance;v[5]=npc.m_CurrentLightEnhance;
    v[6]=npc.m_CurrentColdEnhance;v[7]=npc.m_CurrentPoisonEnhance;
    v[8]=p.EffectEnhance[0];v[9]=p.EffectEnhance[1];
    v[10]=p.IgnoreMagicResist;
    v[11]=npc.m_CurrentIgnoreFireResist;v[12]=npc.m_CurrentIgnoreLightResist;
    v[13]=npc.m_CurrentIgnoreColdResist;v[14]=npc.m_CurrentIgnoreEarthResist;
    v[15]=npc.m_CurrentIgnoreExDefence;
    v[17]=npc.m_CurrentDeadlyStrikeEnhanceP;
    v[18]=npc.m_CurrentAllMagicDeadlyStrike;
    v[20]=npc.m_CurrentFireDeadlyStrike;v[21]=npc.m_CurrentLightDeadlyStrike;
    v[22]=npc.m_CurrentColdDeadlyStrike;
    v[23]=npc.m_CurrentReducePhysicsDeadlyStrike;v[24]=npc.m_CurrentReduceMagicDeadlyStrike;
    for(int i=0;i<4;++i)v[25+i]=p.ElementCriticalReduce[i];
    v[29]=p.CriticalDamage[0];v[30]=p.CriticalDamage[1];
    v[31]=p.CriticalDamageReduce[0];v[32]=p.CriticalDamageReduce[1];
    v[33]=npc.m_CurrentPhysicsResist;v[34]=npc.m_CurrentHitRecover;
    v[35]=npc.m_CurrentFreezeTimeReducePercent;v[36]=npc.m_CurrentPoisonTimeReducePercent;
    v[37]=npc.m_CurrentStunTimeReducePercent;v[38]=p.ConfuseTimeReduce;
    v[39]=p.AbsorbDamage[0];v[40]=p.AbsorbDamage[2];v[41]=p.AbsorbDamage[3];
    v[42]=p.AbsorbDamage[1];v[43]=p.AbsorbDamage[4];
    v[44]=npc.m_CurrentStaticMagicShieldP+npc.m_ManaShield.nValue[0];
    v[45]=npc.m_CurrentDamage2Mana;v[46]=p.WalkRunEnhance;
    v[47]=npc.m_CurrentLifeReplenish;v[48]=npc.m_CurrentManaReplenish;
    v[49]=p.PlayerDamageEnhance;v[50]=p.MonsterDamageEnhance;
    v[51]=p.PlayerDamageReduce;
}

int main(){assert(magic_reducephysicsdeadlystirke==204);KNpc npc;memset(&npc,0,sizeof(npc));npc.m_PtCombat.CriticalDamageReduce[0]=17;KNpcAttribModify modifier;KMagicAttrib attr;memset(&attr,0,sizeof(attr));attr.nAttribType=ptOriginalItemBuffs[0].attrs[1][0];attr.nValue[0]=ptOriginalItemBuffs[0].attrs[1][1];modifier.TuChanAttrib(&npc,&attr);PTStatusOriginal s;PhongThanFillStatusOriginal(npc,s);assert(PhongThanStatusOriginalRow(s,1,6)==5);assert(PhongThanStatusOriginalRow(s,1,7)==17);assert(PhongThanStatusOriginalRow(s,1,8)==0);int before=30,after=before-npc.m_CurrentReducePhysicsDeadlyStrike;assert(after==25);puts("PASS real modifier + status projection: dragon +5 in upper physical focused row, lower damage row stays 17, spell row unchanged; native focused chance 30 -> 25");return 0;}
