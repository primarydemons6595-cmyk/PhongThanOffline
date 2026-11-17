#ifndef PHONGTHAN_PURPLE_EQUIPMENT_H
#define PHONGTHAN_PURPLE_EQUIPMENT_H
#include <string.h>
// Authored Tinh Quan equipment prefix in original TCVN3 item tables.
// Material/event names containing these words are not equipment rarity.
inline bool PhongThanIsTinhQuanEquipment(const char* name) {
    if(!name)return false;
    while(*name=='#' || *name=='$')++name;
    // Purple is explicit in the original armor/belt/boot/helm/pendant
    // records. A horse or weapon merely named Tinh Quan is not purple.
    if(strncmp(name,"<c=pk>",6))return false;
    name+=6;
    static const char prefix[]="Tinh Qu\xA9" "n";
    const unsigned len=sizeof(prefix)-1;
    return !strncmp(name,prefix,len) && (name[len]=='*' || name[len]==' ') && name[len+1];
}
#define PT_PURPLE_EQUIPMENT_COLOR 0xBD28FF
#endif
