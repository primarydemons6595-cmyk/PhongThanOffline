#ifndef PHONGTHAN_ISLAND_CONTRACT_H
#define PHONGTHAN_ISLAND_CONTRACT_H
// 5360..5372 reserved for island progression; 5373 realm EXP, 5380..5389 offline, 5390 nation receipt. PK owns 4762..4764;
// life recipe books use 5200..5326 and master credit uses5350.
#include "PhongThanPkPunishment.h"
namespace PtIslandTask {
enum {Lure=5360,Receipt=5361,DailyEnd=5362,DailyKills=5363,DailyToolReceipt=5364,
 TienCredit=5365,MaCredit=5366,HuntX=5367,HuntY=5368,Pass=5369,
 QuynhFame=5370,QuynhReceipt=5371,DailyMigration=5372,End=5372};
}
typedef char PtIslandSlotsMustNotOverlapPk[(PtIslandTask::Lure>PtPkPunishment::Seconds&&PtIslandTask::End<5400)?1:-1];
inline bool PtIslandCombatMap(int map){return map==1054||map==1055||map==1056||map==1065;}
#endif
