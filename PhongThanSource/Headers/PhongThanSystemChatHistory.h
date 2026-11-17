#ifndef PHONGTHAN_SYSTEM_CHAT_HISTORY_H
#define PHONGTHAN_SYSTEM_CHAT_HISTORY_H
// Classify by transport/resource, never by the displayed sender or text.
// Copy the final encoded message so formatting and item metadata survive.
template<class List,class Item>
bool PtKeepSystemChat(List& history,bool explicitSystem,bool publicSink,
 int channel,int gmChannel,int resource,int systemResource,
 const char* text,int length,Item* item){
 if(!explicitSystem&&!(publicSink&&((gmChannel>=0&&channel==gmChannel)||(systemResource>=0&&resource==systemResource))))return false;
 return history.AddOneMessage(text,length,0,-1,item)!=0;
}
#endif
