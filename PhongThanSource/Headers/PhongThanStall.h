#ifndef PHONGTHAN_STALL_H
#define PHONGTHAN_STALL_H
enum { PT_STALL_MAX_LICENSES=49, PT_STALL_CUSTOM_FIRST=9500, PT_STALL_CUSTOM_COUNT=32,
       PT_STALL_SALES_TASK=5351, PT_STALL_TURNOVER_TASK=5352, PT_STALL_SAVED_TASK=5353,
       PT_GDI_STALL_STATUS=0x5054534c };
struct PTStallStatus {int Sales,Turnover,Level,NextSales;};
// Named request opens; empty request stops. Retrying either request is harmless.
enum {PT_STALL_NOOP=0,PT_STALL_OPEN=1,PT_STALL_CLOSE=2};
inline int PTStallIntent(bool active,bool named){return named?(active?PT_STALL_NOOP:PT_STALL_OPEN):(active?PT_STALL_CLOSE:PT_STALL_NOOP);}
inline bool PTStallCanOpenControl(bool active,bool idle,bool pricing){return active||idle||pricing;}
// The exact original IB identities; never treat a similarly numbered skill as a permit.
inline bool PTStallBuff(int id){return (id>=38&&id<=45)||(id>=125&&id<=132)||id==186||(id>=9500&&id<9532);}
inline int PTStallOriginalMap(int detail){
 static const int maps[8]={1021,1020,1052,1001,1067,1068,1069,1070};
 if(detail>=38&&detail<=45)return maps[detail-38];
 if(detail>=125&&detail<=132)return maps[detail-125];return 0;
}
inline int PTStallOriginalSeconds(int detail){return detail>=125&&detail<=132?518400:172800;}
inline int PTStallDefaultMode(int map){
 if(map>=1002&&map<=1004)return 2;
 if(map==1001||map==1020||map==1021||map==1052||(map>=1067&&map<=1070))return 1;
 return 0;
}
inline int PTStallLevel(int sales){int level=1;while(level<10&&sales>=level*level*10)++level;return level;}
inline unsigned long PTStallResumeExpiry(unsigned long end,unsigned long saved,unsigned long now){return saved&&end>saved&&now>=saved?now+(end-saved):end;}
#ifdef _SERVER
bool PhongThanStallAllowed(int player);
int PhongThanUseStallLicense(int player,int item);
void PhongThanStallTick(int player);
void PhongThanStallPersist(int player);
bool PhongThanStallPurchase(int buyer,int seller,int item);
#endif
#endif
