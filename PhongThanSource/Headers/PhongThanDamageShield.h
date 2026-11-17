#ifndef PHONGTHAN_DAMAGE_SHIELD_H
#define PHONGTHAN_DAMAGE_SHIELD_H
// Consume the incoming damage before replacing it with its unabsorbed part.
inline void PhongThanAbsorbDamage(int& damage,int& shield)
{
    if(damage<=0 || shield<=0)return;
    const int absorbed=damage<shield?damage:shield;
    shield-=absorbed;
    damage-=absorbed;
}
#endif
