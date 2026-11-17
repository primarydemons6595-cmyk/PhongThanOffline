#ifndef PT_CHAT_CHANNELS_H
#define PT_CHAT_CHANNELS_H
enum { PT_CHAT_TRADE=0x40000001,PT_CHAT_AREA,PT_CHAT_NATION,PT_CHAT_FRIENDS,PT_CHAT_WORLD,PT_CHAT_TONG,PT_CHAT_CHANNEL_COUNT=6 };
inline bool PTBotPublicChannelAllowed(int level,int minimum,unsigned long channel){return (channel==PT_CHAT_AREA&&level>=1)||(level>=minimum&&(channel==PT_CHAT_WORLD||channel==PT_CHAT_TRADE));}
#endif
