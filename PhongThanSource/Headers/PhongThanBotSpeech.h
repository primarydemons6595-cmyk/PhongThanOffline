#ifndef PT_BOT_SPEECH_H
#define PT_BOT_SPEECH_H
inline int PTBotSpeechVariant(unsigned long seed,int previous,int count){if(count<2)return 0;if(previous<0||previous>=count)return seed%count;int pick=seed%(count-1);return pick>=previous?pick+1:pick;}
#endif
