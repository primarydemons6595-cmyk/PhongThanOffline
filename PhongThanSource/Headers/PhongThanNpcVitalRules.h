#ifndef PHONGTHAN_NPC_VITAL_RULES_H
#define PHONGTHAN_NPC_VITAL_RULES_H

inline int PhongThanClampLife(int currentLife, int maxLife)
{
	if (maxLife <= 0 || currentLife <= 0)
		return 0;
	return currentLife > maxLife ? maxLife : currentLife;
}

inline int PhongThanLifePercent(int currentLife, int maxLife, int percentScale)
{
	if (maxLife <= 0 || percentScale <= 0)
		return 0;
	const int clampedLife = PhongThanClampLife(currentLife, maxLife);
	return (int)(((__int64)clampedLife * percentScale) / maxLife);
}

inline int PhongThanApplyNpcLifeRegen(int currentLife, int maxLife,
	int fixedRegen, int regenPercent, int percentScale, int eligible)
{
	if (!eligible || currentLife <= 0 || maxLife <= 0 || percentScale <= 0)
		return PhongThanClampLife(currentLife, maxLife);

	__int64 regen = fixedRegen;
	regen += ((__int64)fixedRegen * regenPercent) / percentScale;
	__int64 nextLife = (__int64)currentLife + regen;
	if (nextLife <= 0)
		return 0;
	if (nextLife >= maxLife)
		return maxLife;
	return (int)nextLife;
}

inline int PhongThanVisualAnchorHeight(int reportedHeight, int fallbackHeight)
{
	return reportedHeight > 0 ? reportedHeight : fallbackHeight;
}

inline int PhongThanSpriteVisualAnchorHeight(int logicalHeight,
	int spriteHeight, int referenceSpotY, int fallbackHeight)
{
	int visualHeight = referenceSpotY;
	if (visualHeight <= 0 || visualHeight > spriteHeight)
		visualHeight = spriteHeight;
	if (visualHeight <= 0)
		return PhongThanVisualAnchorHeight(logicalHeight, fallbackHeight);
	return logicalHeight + visualHeight;
}

inline void PhongThanProjectWorldAnchor(int worldX, int worldY, int worldZ,
	int focusX, int focusY, int viewWidth, int viewHeight,
	int& screenX, int& screenY)
{
	screenX = worldX - focusX + viewWidth / 2;
	screenY = (worldY - focusY) / 2 + viewHeight / 2 -
		((worldZ * 887) >> 10);
}

#endif
