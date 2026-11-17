#ifndef PHONGTHAN_PK_CONTRACT_H
#define PHONGTHAN_PK_CONTRACT_H
// Original client 790EAA/790ECC: opcode A1, raw camp, exactly two bytes.
// Resource 4A200C53 rows are 8,4,3,2,1,7. No legacy JX enum alias.
enum { PT_PK_OPCODE = 0xA1, PT_PK_PACKET_SIZE = 2 };
inline int PtPkSelectable(int camp)
{ return (camp >= 1 && camp <= 4) || camp == 7 || camp == 8; }
inline int PtPkKnownCamp(int camp)
{ return camp == 0 || PtPkSelectable(camp); }
inline int PtPkMenuCamp(int row)
{ static const int camps[6] = {8,4,3,2,1,7}; return row >= 0 && row < 6 ? camps[row] : -1; }
inline int PtPkMenuRow(int camp)
{ for (int i=0; i<6; ++i) if (PtPkMenuCamp(i)==camp) return i; return -1; }
inline int PtPkValidPacket(const unsigned char* data, unsigned int size)
{ return data && size == PT_PK_PACKET_SIZE && data[0] == PT_PK_OPCODE && PtPkSelectable(data[1]); }
// Original player/player base table 767E5A..767FCA. Numeric relation values
// are common to the current source and original client: none=1, ally=4, enemy=8.
inline int PtPkBaseRelation(int a, int b)
{
    if (!PtPkKnownCamp(a) || !PtPkKnownCamp(b)) return -1;
    if (!a || !b) return 4;
    if (a == 8 || b == 8) return 8;
    if (a == b) return 4;
    if (a == 7 || b == 7) return 1;
    return 8;
}
#endif
