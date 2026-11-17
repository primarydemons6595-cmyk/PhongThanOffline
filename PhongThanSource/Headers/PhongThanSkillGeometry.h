#ifndef PHONGTHAN_SKILL_GEOMETRY_H
#define PHONGTHAN_SKILL_GEOMETRY_H

inline bool PhongThanAnchorMissileAtTarget(bool base, bool targetOnly,
    int count, bool stationary, int target)
{
    return base && targetOnly && count == 1 && stationary && target > 0;
}

// Fly events can launch a spread at exactly the followed target's position.
// In that case its parent direction remains meaningful; distance does not.
inline void PhongThanSpreadFactors(int dx, int dy, int distance,
    int directionX, int directionY, int& factorX, int& factorY)
{
    if (distance > 0)
    {
        factorX = dx * 1024 / distance;
        factorY = dy * 1024 / distance;
    }
    else
    {
        factorX = directionX;
        factorY = directionY;
    }
}
#endif
