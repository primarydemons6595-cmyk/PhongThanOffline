#ifndef PHONGTHAN_EQUIPMENT_WORLD_PRESENTATION_H
#define PHONGTHAN_EQUIPMENT_WORLD_PRESENTATION_H

// Authoritative PT state-graphic rows from
// settings/npcres/zhuang_tai_tu_xing_dui_zhao_biao.txt.
// Rows 39-42 are the four original PT profession variants of the weapon +12
// effect. They are mutually exclusive. Rows 35-38 are the sex-specific 3/5
// piece fully-upgraded set effects.
static const unsigned char PHONGTHAN_WEAPON_EFFECT_NONE = 0;
static const int PHONGTHAN_EQUIPMENT_EFFECT_CAPACITY = 4;

// Owned compatibility encoding, not an original PT state ID: bit7 marks
// the dim tier ONLY for ordinary equipment effects26..28. Skill state IDs
// are not tagged. User-confirmed profession colors: yellow/purple/cyan.
inline unsigned char PhongThanResolveOrdinarySetWorldEffect(int pieces, int profession)
{
	if (pieces < 3 || profession < 0 || profession > 2) return 0;
	return (unsigned char)((26 + profession) | (pieces < 5 ? 0x80 : 0));
}

inline unsigned char PhongThanEquipmentGraphicState(unsigned char effect)
{
	if (effect >= 154 && effect <= 156) return (unsigned char)(effect & 0x7f);
	return effect;
}

inline int PhongThanOrdinarySetAlpha(int state, const unsigned char* effects, int count)
{
	if (!effects || state < 26 || state > 28) return 255;
	int alpha = 255;
	for (int i = 0; i < count; ++i)
	{
		if (effects[i] == state) return 255;
		if (effects[i] >= 154 && effects[i] <= 156 &&
			PhongThanEquipmentGraphicState(effects[i]) == state) alpha = 128;
	}
	return alpha;
}

inline unsigned char PhongThanResolveWeaponWorldEffect(int upgradeLevel,
	int profession)
{
	if (upgradeLevel < 12 || profession < 0 || profession > 3)
		return PHONGTHAN_WEAPON_EFFECT_NONE;
	// User-selected loose presentation: the existing Giap Si +12 effect for all professions.
	return 39;
}

inline unsigned char PhongThanResolveSetWorldEffect(int equippedPieces,
	int allPiecesUpgrade12, int female)
{
	if (!allPiecesUpgrade12 || equippedPieces < 3)
		return 0;
	if (equippedPieces >= 5)
		return (unsigned char)(female ? 37 : 38);
	return (unsigned char)(female ? 35 : 36);
}

inline int PhongThanEquipmentWorldEffectValid(unsigned char effect)
{
	return effect == PHONGTHAN_WEAPON_EFFECT_NONE ||
		(effect >= 26 && effect <= 28) || (effect >= 154 && effect <= 156) ||
		(effect >= 35 && effect <= 42) || effect == 44 || effect == 45 ||
        (effect >= 62 && effect <= 64) || effect == 72 || effect == 73;
}

// Loose activation policy: keep the existing 3/5-piece thresholds. State IDs
// and sprite colors come from the original table; this is not a claim that
// all original server activation rules have been recovered.
struct PT_EQUIPMENT_GLOW_PIECE {
    int SetId, Group, Quality, Realm, Tier, Upgrade;
};
struct PT_EQUIPMENT_GLOW_RESULT {
    int SetId, Group, Family, Pieces, Tier, AllUpgrade12;
    unsigned char UpgradeEffect, BaseEffect;
};
inline int PhongThanGlowFamily(const PT_EQUIPMENT_GLOW_PIECE& item) {
    if(item.Tier<1 || item.Upgrade<0)return 0;
    if(item.Realm)return 4;
    if(item.Quality==5)return 3; // Explicit Tinh Quan equipment family.
    if(item.Quality==3)return 2; // Persisted forge recipe/canonical orange name.
    return item.SetId>0 && item.Group>0 ? 1 : 0;
}
inline int PhongThanSameGlowSet(const PT_EQUIPMENT_GLOW_PIECE& a,
    const PT_EQUIPMENT_GLOW_PIECE& b) {
    const int family=PhongThanGlowFamily(a);
    if(!family || family!=PhongThanGlowFamily(b) || a.Realm!=b.Realm)return 0;
    if(a.SetId>0 || b.SetId>0)
        return a.SetId>0 && a.Group>0 && a.SetId==b.SetId && a.Group==b.Group;
    // Original Tinh Quan and forged templates can have no SetID. Keep these
    // bounded to the same family/tier; never combine them with white/blue gear.
    return family>=2 && a.Tier==b.Tier;
}
inline unsigned char PhongThanResolveFamilyGlow(int family,int pieces,
    int profession) {
    if(pieces<3)return 0;
    if(family==4)return profession>=0 && profession<3 ? (unsigned char)(62+profession) : 0;
    if(family==3)return 45;
    if(family==2)return pieces>=5 ? 45 : 44;
    return family==1 ? PhongThanResolveOrdinarySetWorldEffect(pieces,profession) : 0;
}
inline void PhongThanSelectEquipmentGlow(const PT_EQUIPMENT_GLOW_PIECE* items,
    int count,int profession,int female,PT_EQUIPMENT_GLOW_RESULT& result) {
    result.SetId=result.Group=result.Family=result.Pieces=result.Tier=result.AllUpgrade12=0;
    result.UpgradeEffect=result.BaseEffect=0;
    if(!items || count<0 || count>32)return;
    for(int i=0;i<count;++i) {
        const int family=PhongThanGlowFamily(items[i]);
        if(!family)continue;
        int pieces=0,all12=1;
        for(int j=0;j<count;++j)if(PhongThanSameGlowSet(items[i],items[j])) {
            ++pieces;if(items[j].Upgrade<12)all12=0;
        }
        if(pieces<3)continue;
        // Evaluate every set, including a complete set after an unrelated
        // first equipped item. Ties are stable when slot order changes.
        if(pieces<result.Pieces || (pieces==result.Pieces && family<result.Family))continue;
        if(pieces==result.Pieces && family==result.Family) {
            if(all12<result.AllUpgrade12)continue;
            if(all12==result.AllUpgrade12 && (items[i].SetId>result.SetId ||
                (items[i].SetId==result.SetId && items[i].Tier>=result.Tier)))continue;
        }
        result.SetId=items[i].SetId;result.Group=items[i].Group;result.Family=family;
        result.Pieces=pieces;result.Tier=items[i].Tier;result.AllUpgrade12=all12;
        result.UpgradeEffect=all12 ? (family==4 ? (unsigned char)(female?72:73) :
            PhongThanResolveSetWorldEffect(pieces,1,female)) : 0;
        result.BaseEffect=result.UpgradeEffect ? 0 : PhongThanResolveFamilyGlow(family,pieces,profession);
    }
}

inline int PhongThanMergeEquipmentWorldEffect(unsigned char* states, int capacity,
	unsigned char effect)
{
	int i;
	if (!states || capacity <= 0 || effect == PHONGTHAN_WEAPON_EFFECT_NONE ||
		!PhongThanEquipmentWorldEffectValid(effect))
		return 0;
	effect = PhongThanEquipmentGraphicState(effect);
	for (i = 0; i < capacity; ++i)
	{
		if (states[i] == effect)
			return 1;
		if (states[i] == 0)
		{
			states[i] = effect;
			return 1;
		}
	}
	return 0;
}

#endif
