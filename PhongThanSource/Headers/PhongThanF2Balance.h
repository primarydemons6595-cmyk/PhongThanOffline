#ifndef PHONGTHAN_F2_BALANCE_H
#define PHONGTHAN_F2_BALANCE_H
enum { PT_F2_BALANCE_GET=0x50544632,PT_F2_CURRENCY_GET=0x50544633 };
struct PTF2BalanceQuery { int Type,Genre,Detail,Particular,Level,Series; };
struct PTF2Currency { PTF2BalanceQuery Query; char Label[20];int Enabled; };
enum { PT_F2_MONEY_SLOT1=0x50544641,PT_F2_MONEY_SLOT2=0x50544642 };
const PTF2Currency* PhongThanF2CurrencyConfig(int slot);
int PhongThanF2CurrencyCount(int player,int slot);
int PhongThanF2WalletCount(int player,const PTF2BalanceQuery& q);
#endif
