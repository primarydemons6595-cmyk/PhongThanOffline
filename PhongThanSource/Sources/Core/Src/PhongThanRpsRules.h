#ifndef PHONGTHAN_RPS_RULES_H
#define PHONGTHAN_RPS_RULES_H
#define PT_GDCNI_RPS_ROUND 0x50545250
// Original Game.exe: BtnForfex=0, BtnStone=1, BtnCloth=2.
inline bool PtRpsValid(int choice) { return choice >= 0 && choice <= 2; }
inline int PtRpsResult(int self, int other)
{
    if (!PtRpsValid(self) || !PtRpsValid(other)) return -2;
    if (self == other) return 0;
    return (self + 2) % 3 == other ? 1 : -1;
}
#endif
