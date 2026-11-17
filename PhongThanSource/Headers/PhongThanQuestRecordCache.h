#ifndef PHONGTHAN_QUEST_RECORD_CACHE_H
#define PHONGTHAN_QUEST_RECORD_CACHE_H
#include <vector>
#include <string>
class PhongThanQuestRecordCache {
 struct Entry {int id,step,action;std::string text;};
 std::vector<Entry> entries;unsigned int actor;int connection;
public:
 PhongThanQuestRecordCache():actor(0),connection(-1){}
 bool Changed(unsigned int entity,int socket,int id,int step,int action,const char* text){
  if(actor!=entity||connection!=socket){entries.clear();actor=entity;connection=socket;}
  for(unsigned int i=0;i<entries.size();++i)if(entries[i].id==id){Entry& e=entries[i];
   if(e.step==step&&e.action==action&&e.text==text)return false;
   e.step=step;e.action=action;e.text=text;return true;}
  if(entries.size()>=128)entries.erase(entries.begin());
  Entry e;e.id=id;e.step=step;e.action=action;e.text=text;entries.push_back(e);return true;
 }
};
#endif
