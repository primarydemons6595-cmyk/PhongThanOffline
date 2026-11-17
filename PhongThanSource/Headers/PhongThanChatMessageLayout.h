#ifndef PHONGTHAN_CHAT_MESSAGE_LAYOUT_H
#define PHONGTHAN_CHAT_MESSAGE_LAYOUT_H
inline int PTChatMessageFont(int requested,int base){return requested>=12&&requested<=20?requested:base;}
inline int PTChatLineSlots(int lines,int font,int base){return (lines*(font+1)+base)/(base+1);}
struct PTChatVisibleLines {
 int slots,skip,count,offset;
 PTChatVisibleLines(int messageSlots,int actualLines,int slotSkip,int available,int font,int base){
  slots=messageSlots-slotSkip;if(slots>available)slots=available;if(slots<0)slots=0;
  int pixels=slotSkip*(base+1);skip=(pixels+font)/(font+1);offset=skip*(font+1)-pixels;
  count=(slots*(base+1)-offset)/(font+1);if(count>actualLines-skip)count=actualLines-skip;if(count<0)count=0;
 }
};
#endif
