#ifndef PHONGTHAN_SPRITE_ANCHOR_H
#define PHONGTHAN_SPRITE_ANCHOR_H
#include <string.h>

// VNG human composites and passerby NPCs share the 510-square authoring
// canvas. An absent SPR centre is not a SwordOnline (160,192) anchor.
inline bool PhongThanUsesActorCanvas(const char* path, int width, int height)
{
    return path && width == 510 && height == 510 &&
        (strstr(path,"npcres\\human\\") || strstr(path,"npcres\\passerby\\"));
}
// Normal PT monsters use the same 510-square body canvas but live under
// npcres\monster (and sibling NPC directories).  This is intentionally a
// body-rendering predicate: name/HP remains in its existing visual-height
// domain and must not be moved merely because the body reference changes.
inline bool PhongThanUsesNpcBodyCanvas(const char* path, int width, int height)
{
    return path && width == 510 && height == 510 &&
        strstr(path, "npcres\\") != NULL;
}
inline int PhongThanActorReferenceY(const char* path, int cx, int cy)
{
    const bool human = path && strstr(path,"npcres\\human\\");
    return (!human && (cx || cy)) ? cy : 293;
}
inline void PhongThanActorReference(const char* path, int cx, int cy, int& x, int& y)
{
    const bool human = path && strstr(path,"npcres\\human\\");
    x -= (!human && (cx || cy)) ? cx : 255;
    y -= PhongThanActorReferenceY(path, cx, cy);
}
#endif
