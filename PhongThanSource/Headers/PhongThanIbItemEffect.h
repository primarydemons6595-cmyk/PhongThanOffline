#ifndef PHONGTHAN_IB_ITEM_EFFECT_H
#define PHONGTHAN_IB_ITEM_EFFECT_H

// ibitem.txt uses a small PT-specific effect-ID domain. These two IDs are
// documented by the authoritative item rows as direct HP/inner-energy doses;
// they are not IDs 153/154 from the global magic-attribute registry.
enum PHONGTHAN_IB_EFFECT_ID
{
    PHONGTHAN_IB_EFFECT_LIFE_POTION = 153,
    PHONGTHAN_IB_EFFECT_MANA_POTION = 154
};

inline int PhongThanAdaptIbEffectId(int rawId)
{
    if (rawId == PHONGTHAN_IB_EFFECT_LIFE_POTION)
        return 148; // magic_lifepotion_v
    if (rawId == PHONGTHAN_IB_EFFECT_MANA_POTION)
        return 149; // magic_manapotion_v
    return rawId;
}

#endif
