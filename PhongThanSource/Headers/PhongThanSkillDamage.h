#ifndef PHONGTHAN_SKILL_DAMAGE_H
#define PHONGTHAN_SKILL_DAMAGE_H
#include <string.h>
#include <limits.h>

// The combat receiver consumes 17 fixed slots, including absent attributes.
// AddBaseDamage is authoritative Skills.txt metadata, independent of animation
// and of whether the skill also applies a debuff.
inline void PhongThanPrepareSkillDamage(KMagicAttrib* output,
    const KMagicAttrib* input, int count, int addBaseDamage,
    int physicsEnhanceType, int physicsDamageType)
{
    memcpy(output, input, sizeof(KMagicAttrib) * count);
    if (addBaseDamage && count > 9 &&
        output[9].nAttribType != physicsEnhanceType &&
        output[9].nAttribType != physicsDamageType)
    {
        memset(&output[9], 0, sizeof(KMagicAttrib));
        output[9].nAttribType = physicsEnhanceType;
    }
}

// Fixed slots 9..13 are physics, cold, fire, lightning and earth damage.
// Status durations, attack rating and the unused legacy magic slot are not
// additional damage. Keep this view tied to the descriptors sent to the hit
// receiver, rather than implementing a second set of skill formulas for F3.
inline void PhongThanSumHitDamage(const KMagicAttrib* damage, int count,
    int* minimum, int* maximum)
{
    if (!minimum || !maximum) return;
    *minimum = 0;
    *maximum = 0;
    if (!damage || count < 14) return;
    for (int i = 9; i <= 13; ++i)
    {
        int low = damage[i].nValue[0] > 0 ? damage[i].nValue[0] : 0;
        int high = damage[i].nValue[2] > 0 ? damage[i].nValue[2] : 0;
        *minimum = low > INT_MAX - *minimum ? INT_MAX : *minimum + low;
        *maximum = high > INT_MAX - *maximum ? INT_MAX : *maximum + high;
    }
}
#endif
