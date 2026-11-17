#ifndef PHONGTHAN_FORGED_NAME_H
#define PHONGTHAN_FORGED_NAME_H
#include <string.h>
// Exact affix bytes from the active original equipment-refinement table.
// This recognizes an existing canonical name only; it does not create stats,
// rename plain templates, or infer rarity from stars/durability/sale price.
inline bool PhongThanHasForgedName(const char* name) {
    if(!name)return false;while(*name=='#' || *name=='$')++name;
    static const char* prefixes[]={
        "\x4C\x69\x6E\x68\x20\x4B\x68\xDD",
        "\x4E\x67\x68\xDC\x61\x20\x4B\x68\xDD",
        "\x51\x75\x79\x20\x4E\x67\x75\x79\xAA\x6E",
        "\x50\x68\xF4\x63\x20\x6D\x61",
        "\x48\xB5\x6F\x20\x4B\x69\xD6\x74",
        "\x50\x68\xB8\x20\x51\x75\xA9\x6E",
        "\x54\x68\x69\xAA\x6E\x20\x54\xF8",
        "\x3C\x43\x68\x69\xD5\x6E\x3E\x20\x50\x68\xB8\x20\x51\x75\xA9\x6E",
        "\x3C\x4C\x69\xD6\x74\x3E\x20\x50\x68\xB8\x20\x51\x75\xA9\x6E"};
    for(unsigned i=0;i<sizeof(prefixes)/sizeof(prefixes[0]);++i) {
        unsigned n=strlen(prefixes[i]);
        if(!strncmp(name,prefixes[i],n) && name[n]==' ' && name[n+1])return true;
    }
    return false;
}
#endif
