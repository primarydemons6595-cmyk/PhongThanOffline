#ifndef PHONGTHAN_TRAN_NGUYEN_TASKS_H
#define PHONGTHAN_TRAN_NGUYEN_TASKS_H
// 4550..4572 and 4600..4684 already belong to Giap Si/shared/VTT quests.
// 4701..4745 are also read by the legacy escort migration. Reserve this
// compact, audited range exclusively for Tran Nguyen; never copy the old
// overlapping values into it.
namespace PtTranTask {
enum {Phase=4746,Mask=4747,Sequence=4748,Slot=4749,Day=4750,Runs=4751,
 Tier3=4752,Credit=4753,Books=4758,BookWords=4,End=4761};
// Stable mapping for the 120 unique books used by the 135 classic recipes.
// Groups 0..4 contain six books each; groups 5..14 contain nine each.
inline int BookIndex(int book){int group=book/100,n=book%100;
 if(book>0&&group<5&&n>=1&&n<=6)return group*6+n-1;
 if(group>=5&&group<=14&&n>=1&&n<=9)return 30+(group-5)*9+n-1;
 return -1;
}
}
#endif
