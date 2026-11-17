#ifndef PHONGTHAN_PK_TEXT_H
#define PHONGTHAN_PK_TEXT_H
// Authored compatibility/test feedback, TCVN3 bytes expressed in ASCII source.
// Not claimed to be original PT server notifications.
#define PT_PK_OK "X\270c nh\313n"
#define PT_PK_CANCEL "H\361y b\341"
#define PT_PK_NOVICE "B\271n ch\255a tho\270t T\251n th\361."
#define PT_PK_BLOCKED "Kh\253ng th\323 \256\346i phe PK trong tr\271ng th\270i hi\326n t\271i."
inline const char* PtPkStateText(int camp)
{
    switch(camp) {
    case 8: return "PK: Phe t\371 do.";
    case 4: return "PK: Phe l\267nh \256\336a - v\265ng.";
    case 3: return "PK: Phe l\267nh \256\336a - xanh d\255\254ng.";
    case 2: return "PK: Phe l\267nh \256\336a - h\345ng.";
    case 1: return "PK: Phe l\267nh \256\336a - n\251u.";
    case 7: return "\247\267 t\276t PK.";
    }
    return PT_PK_BLOCKED;
}
#endif
