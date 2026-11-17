#ifndef KPHONGTHANAPPEARANCE_H
#define KPHONGTHANAPPEARANCE_H

#include "KTabFile.h"
#include "GameDataDef.h"

class KPhongThanAppearance
{
private:
	KTabFile m_MeleeWeapon;
	KTabFile m_RangeWeapon;
	KTabFile m_Armor;
	KTabFile m_Helm;
	KTabFile m_Horse;
	KTabFile m_HorseVerified;
	BOOL m_bWeaponLoaded;
	BOOL m_bArmorLoaded;
	BOOL m_bHorseLoaded;

	BOOL ResolveTablePart(KTabFile& Table, int nEquipId, int nResourceOffset,
		PHONGTHAN_VISUAL_PART* pVisual);
	BOOL ResolveOriginalTierPart(int nDetailType, int nParticularType,
		int nLevel, PHONGTHAN_VISUAL_PART* pVisual, int realmEquipment=0, int realmSide=0);

public:
	BOOL Init();
	BOOL ResolveWeapon(int nDetail, int nEquipId, PHONGTHAN_VISUAL_PART* pVisual);
	BOOL ResolveArmor(int nDetailType, int nParticularType, int nLevel,
		int nEquipId, PHONGTHAN_VISUAL_PART* pVisual, int realmEquipment=0, int realmSide=0);
	BOOL ResolveHelm(int nDetailType, int nParticularType, int nLevel,
		int nEquipId, PHONGTHAN_VISUAL_PART* pVisual, int realmEquipment=0, int realmSide=0);
	BOOL ResolveHorse(int nEquipId, PHONGTHAN_VISUAL_PART* pVisual);
	BOOL ResolvePhiPhong(int nDetailType, int nParticularType, int nLevel,
		PHONGTHAN_VISUAL_PART* pVisual, int realmEquipment=0, int realmSide=0);
	void SetDefault(PHONGTHAN_APPEARANCE* pAppearance);
};

extern KPhongThanAppearance g_PhongThanAppearance;

#endif
