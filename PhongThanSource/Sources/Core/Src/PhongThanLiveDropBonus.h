#ifndef PT_LIVE_DROP_BONUS_H
#define PT_LIVE_DROP_BONUS_H
#include "LegacyDropExecutionPolicy.h"
inline bool PhongThanAppendLiveDropBonus(const LegacyDrop::Execution::Plan& plan,int rate,unsigned long seed,int maxItems,LegacyDrop::Execution::Result& result,std::string& error){
 if(rate<=1000)return true;
 if(rate>10000||maxItems<1){error="invalid_campaign_drop_rate";return false;}
 LegacyDrop::Random bonus(seed^0x6a09e667UL);int extra=rate/1000-1;if((int)bonus.Below(1000)<rate%1000)++extra;
 for(int roll=0;roll<extra;++roll){LegacyDrop::Execution::Result more;if(!plan.Draw(bonus,more,error,0))return false;
  for(unsigned i=0;i<more.items.size()&&result.items.size()<(unsigned)maxItems;++i)result.items.push_back(more.items[i]);}
 return true;
}
#endif
