#ifndef KPHONGTHAN_PROFESSION_SKILLS_H
#define KPHONGTHAN_PROFESSION_SKILLS_H

#include "KProfession.h"
#include "PhongThanSummon.h"
#include "PhongThanEventHandbookSkills.inl"

// Canonical player-skill ownership from the VNG Skills.txt registry.
enum KPHONGTHAN_PROFESSION_SKILL_CONSTANT
{
	PHONGTHAN_COMMON_SKILL_FIRST = 1,
	PHONGTHAN_COMMON_SKILL_LAST = 2,
	PHONGTHAN_DAOSHI_SKILL_FIRST = 3,
	PHONGTHAN_DAOSHI_SKILL_LAST = 26,
	PHONGTHAN_JIASHI_SKILL_FIRST = 27,
	PHONGTHAN_JIASHI_SKILL_LAST = 42,
	PHONGTHAN_YIREN_SKILL_FIRST = 43,
	PHONGTHAN_YIREN_SKILL_LAST = 51,
	PHONGTHAN_PROFESSION_SKILL_LEVEL = 1
};

inline BOOL PhongThanGetProfessionSkillRange(int nProfession,
	int* pnFirst, int* pnLast)
{
	if (!pnFirst || !pnLast)
		return FALSE;
	switch (nProfession)
	{
	case 0: // Giap Si
		*pnFirst = PHONGTHAN_JIASHI_SKILL_FIRST;
		*pnLast = PHONGTHAN_JIASHI_SKILL_LAST;
		return TRUE;
	case 1: // Dao Si
		*pnFirst = PHONGTHAN_DAOSHI_SKILL_FIRST;
		*pnLast = PHONGTHAN_DAOSHI_SKILL_LAST;
		return TRUE;
	case 2: // Di Nhan
		*pnFirst = PHONGTHAN_YIREN_SKILL_FIRST;
		*pnLast = PHONGTHAN_YIREN_SKILL_LAST;
		return TRUE;
	}
	return FALSE;
}

inline BOOL PhongThanIsProfessionSkill(int nSkillId)
{
    if(nSkillId==1691)return TRUE;
	for(int i=0;i<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++i)
		if(kPtHandbookSkills[i].profession==3 && kPtHandbookSkills[i].id==nSkillId)return TRUE;
	return PhongThanIsSummon(nSkillId) ||
		(nSkillId >= PHONGTHAN_DAOSHI_SKILL_FIRST &&
		nSkillId <= PHONGTHAN_YIREN_SKILL_LAST);
}

inline BOOL PhongThanProfessionOwnsSkill(int nProfession, int nSkillId)
{
    if(nSkillId==1691)return nProfession==3;
	for(int i=0;i<sizeof(kPtHandbookSkills)/sizeof(kPtHandbookSkills[0]);++i)
		if(kPtHandbookSkills[i].profession==3 && kPtHandbookSkills[i].id==nSkillId)return nProfession==3;
	int nFirst = 0;
	int nLast = 0;
	if (PhongThanIsSummon(nSkillId)) return nProfession == 2;
	return PhongThanGetProfessionSkillRange(nProfession, &nFirst, &nLast) &&
		nSkillId >= nFirst && nSkillId <= nLast;
}

inline const int* PhongThanVuSiDisplaySkills(int& count)
{
    static const int ids[]={1696,1701,1706,1711,1716,1721,1726,1731,1736,1741,
        1746,1751,1756,1761,1766,1771,1776,1781,1786,1791};
    count=sizeof(ids)/sizeof(ids[0]);return ids;
}

#endif
