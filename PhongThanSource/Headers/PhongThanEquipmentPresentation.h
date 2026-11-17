#ifndef PHONGTHAN_EQUIPMENT_PRESENTATION_H
#define PHONGTHAN_EQUIPMENT_PRESENTATION_H
#include <string.h>
#include <stdio.h>
#include <limits.h>

// Reserved system inline-picture range, below Text.h's 512 custom boundary.
// Original yellow strip: 15 stars, pitch 14, width 208; +12 uses 166 pixels.
#define PT_UPGRADE_PIC_BASE 480
// FaceDetail occupies IDs 1..95. Reserve 416..475 for 5 qualities x 12
// levels; keep 481..492 as legacy yellow IDs. No custom-picture decoding.
#define PT_COLORED_STAR_PIC_BASE 416
inline int PhongThanStarPic(int quality,int level) {
    if (quality<0 || quality>5 || level<1 || level>12) return 0;
    if (quality==5)return 396+level-1; // purple, outside legacy upgrade IDs
    return PT_COLORED_STAR_PIC_BASE + quality*12 + level-1;
}
inline int PhongThanStarCount(int pic) {
    if (pic>=396 && pic<408)return pic-396+1;
    if (pic>=416 && pic<476) return (pic-416)%12+1;
    return pic>480 && pic<=492 ? pic-480 : 0;
}
inline int PhongThanStarQuality(int pic) {
    if (pic>=396 && pic<408)return 5;
    return pic>=416 && pic<476 ? (pic-416)/12 : 2;
}

inline int PhongThanStarWidth(int count) { return count==12?172:count*14-2; }
inline bool PhongThanBigItemPath(const char* icon,bool female,char* out,int capacity)
{
    if(!icon || !out || capacity<1)return false;
    int len=(int)strlen(icon);
    const char* suffix=female?"_big_female.spr":"_big_male.spr";
    if(len<4 || _stricmp(icon+len-4,".spr") || len-4+(int)strlen(suffix)>=capacity)return false;
    memcpy(out,icon,len-4);strcpy(out+len-4,suffix);return true;
}
inline bool PhongThanSumKnownItemPower(int base,int magic,int upgrade,int& total)
{
    total=0;
    if(base<0 || magic<0 || upgrade<0 || base>INT_MAX-magic || base+magic>INT_MAX-upgrade)return false;
    total=base+magic+upgrade;return true;
}
#define PT_VNG_STAR_STRIP "\\spr\\ui4\\tips\\\xD0\xC7\xD0\xC7\xBB\xC6\xC9\xAB.spr"
#define PT_VNG_STAR_FRAME12 "\\spr\\ui4\\tips\\\xB1\xDF\xBF\xF2" "12.spr"
#define PT_VNG_EQUIP_FX_ROOT "\\spr\\ui4\\\xB5\xC0\xBE\xDF\xC0\xB8\\"
inline const char* PhongThanStarStrip(int quality) {
    if(quality==5)return "\\spr\\ui4\\tips\\\xD0\xC7\xD0\xC7\xD7\xCF\xC9\xAB.spr";
    static const char* paths[] = {"\x5C\x73\x70\x72\x5C\x75\x69\x34\x5C\x74\x69\x70\x73\x5C\xD0\xC7\xD0\xC7\xB0\xD7\xC9\xAB\x2E\x73\x70\x72","\x5C\x73\x70\x72\x5C\x75\x69\x34\x5C\x74\x69\x70\x73\x5C\xD0\xC7\xD0\xC7\xC0\xB6\xC9\xAB\x2E\x73\x70\x72","\x5C\x73\x70\x72\x5C\x75\x69\x34\x5C\x74\x69\x70\x73\x5C\xD0\xC7\xD0\xC7\xBB\xC6\xC9\xAB\x2E\x73\x70\x72","\x5C\x73\x70\x72\x5C\x75\x69\x34\x5C\x74\x69\x70\x73\x5C\xD0\xC7\xD0\xC7\xB3\xC8\xC9\xAB\x2E\x73\x70\x72","\x5C\x73\x70\x72\x5C\x75\x69\x34\x5C\x74\x69\x70\x73\x5C\xD0\xC7\xD0\xC7\xC2\xCC\xC9\xAB\x2E\x73\x70\x72"};
    return paths[quality>=0 && quality<=4 ? quality : 0];
}

#endif
