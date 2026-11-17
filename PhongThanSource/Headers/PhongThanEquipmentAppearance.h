#ifndef PHONGTHAN_EQUIPMENT_APPEARANCE_H
#define PHONGTHAN_EQUIPMENT_APPEARANCE_H

// Canonical original PT basic equipment tier -> player-part resource row.
// Rows 0 and 6 are unequipped bodies (human and ascended respectively).
// Rows 1..5 are ordinary outfits; 7/8 are ascended outfits. Never select 6
// merely because the item is level 10: that renders an unequipped body.
inline int PhongThanResolveBasicPartResource(int detailType,
	int particularType, int level)
{
	if ((detailType != 2 && detailType != 7 && detailType != 9) ||
		particularType < 0 || particularType > 11 || level < 1 || level > 10)
		return -1;
	return (level - 1) / 2 + 1;
}

// The equipment bank is determined by actual requiregdflag/requiregdlevel
// requirements. Series 25..30 also occur on ordinary Tinh Quan equipment;
// neither that marker nor the actor's realm may turn ordinary gear into
// an ascended outfit.
inline int PhongThanResolveEquipmentPartResource(int detail,int particular,
    int level,int realmEquipment,int realmSide)
{
    if((detail!=2&&detail!=7&&detail!=9)||particular<0||level<1||level>10)
        return -1;
    if(realmEquipment && (realmSide==1||realmSide==2))
        return detail==9?7:6+realmSide;
    // Expanded non-ascended quality families share the highest ordinary
    // outfit. Item registry validation owns whether a tuple exists.
    if(particular>11)return 5;
    return PhongThanResolveBasicPartResource(detail,particular,level);
}

#endif
