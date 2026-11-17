#include "PhongThanCustomEvents.h"
extern BOOL PhongThanReloadScriptSafe(const char*);
namespace PtCustomEvents {
static std::map<int,PTCustomEvent> events;
struct Spawn {int index,eventId,npcId;DWORD entity;Spawn():index(0),eventId(0),npcId(0),entity(0){}};
static std::vector<Spawn> spawns;static long revision=0;static unsigned long next=0;static std::string error;
static bool Integer(const std::string& s,int& value,int low=0,int high=2000000000){long v=0;if(!PtEvents::Number(s,v,low,high))return false;value=v;return true;}
static bool Item(const std::string& s,PTCustomItem& item){std::vector<std::string> f=PtEvents::Split(s,',');bool valid=f.size()==5&&Integer(f[0],item.genre,0,8)&&Integer(f[1],item.detail,0,100000)&&Integer(f[2],item.part,0,100000)&&Integer(f[3],item.level,0,1000)&&Integer(f[4],item.series,0,1000);if(valid&&(item.genre==3||item.genre==4||item.genre==5))item.part=0;return valid;}
static bool Label(const std::string& hex,std::string& label){char text[256];if(!PtEvents::Hex(hex,text,sizeof(text)))return false;label=text;return !label.empty();}
static bool Script(const std::string& path,int eventId,long version,std::string& result){char prefix[120];sprintf(prefix,"script\\phongthan\\custom_events\\e%d\\v%ld\\",eventId,version);
 if(path.find(prefix)!=0||path.size()>95||path.size()<4||path.substr(path.size()-4)!=".lua"||path.find("..")!=std::string::npos)return false;
 for(unsigned int i=0;i<path.size();++i)if(!((path[i]>='a'&&path[i]<='z')||(path[i]>='0'&&path[i]<='9')||path[i]=='_'||path[i]=='\\'||path[i]=='.'))return false;result="\\"+path;return true;
}
static void Remove(Spawn& s){if(s.index<=0||s.index>=MAX_NPC||Npc[s.index].m_dwID!=s.entity||Npc[s.index].IsPlayer()){s=Spawn();return;}
 KNpc& n=Npc[s.index];int w=n.m_SubWorldIndex,r=n.m_RegionIndex,x=n.m_MapX,y=n.m_MapY;NpcSet.Remove(s.index);if(w>=0&&w<MAX_SUBWORLD&&r>=0&&r<SubWorld[w].m_nTotalRegion){SubWorld[w].m_Region[r].RemoveNpc(s.index);SubWorld[w].m_Region[r].DecRef(x,y,obj_npc);}s=Spawn();
}
static bool Load(const std::string& bytes){std::vector<std::string> lines=PtEvents::Split(bytes,'\n');if(lines.empty())return false;std::vector<std::string> h=PtEvents::Split(lines[0],'\t');int version=0;
 if(h.size()!=3||h[0]!="PT_CUSTOM_EVENTS"||h[1]!="1"||!Integer(h[2],version,1))return false;if(version==revision)return true;
 std::map<int,PTCustomEvent> candidate;std::set<int> recipeIds;int npcCount=0;
 for(unsigned int line=1;line<lines.size();++line){if(lines[line].empty())continue;std::vector<std::string> f=PtEvents::Split(lines[line],'\t');int id=0,key=0;if(f.size()<3||!Integer(f[1],id,1,65535))return false;
  if(f[0]=="E"){int enabled=0,rev=0;if((f.size()!=5&&f.size()!=10)||candidate.size()>=64||candidate.find(id)!=candidate.end()||!Integer(f[2],enabled,0,1)||!Integer(f[3],rev,1))return false;PTCustomEvent e;e.id=id;e.configuredEnabled=enabled!=0;e.revision=rev;if(!Label(f[4],e.name))return false;
   if(f.size()==10){int yearly=0;if(!Integer(f[5],e.schedule.startDay,10101,99991231)||!Integer(f[6],e.schedule.startMinute,0,1439)||!Integer(f[7],e.schedule.endDay,10101,99991231)||!Integer(f[8],e.schedule.endMinute,0,1439)||!Integer(f[9],yearly,0,1))return false;e.schedule.yearly=yearly!=0;if(!e.schedule.Valid())return false;}
   e.enabled=e.configuredEnabled&&e.schedule.ActiveNow(time(NULL));candidate[id]=e;continue;}
  if(candidate.find(id)==candidate.end()||!Integer(f[2],key,f[0]=="P"?0:1,0xFFFFFF))return false;PTCustomEvent& e=candidate[id];
  if(f[0]=="D"){if(f.size()!=4||key!=1||e.hasToolIdle||!Integer(f[3],e.toolIdleSeconds,30,86400))return false;e.hasToolIdle=true;
  }else if(f[0]=="S"){if(f.size()!=4||key!=1||e.hasShopTheme||!Integer(f[3],e.shopTheme,0,4))return false;e.hasShopTheme=true;
  }else if(f[0]=="N"){PTCustomNpc n;n.id=key;int shop=0,recipes=0;if(f.size()!=11||++npcCount>256||!Integer(f[3],n.tid,0,100000)||!Integer(f[4],n.map,1,65535)||!Integer(f[5],n.x,1,1000000)||!Integer(f[6],n.y,1,1000000)||!Integer(f[7],shop,0,1)||!Integer(f[8],recipes,0,1)||!Label(f[9],n.name)||n.name.size()>=32||!Script(f[10],id,e.revision,n.script))return false;
   for(unsigned int x=0;x<e.npcs.size();++x)if(e.npcs[x].id==key)return false;n.shop=shop!=0;n.recipes=recipes!=0;e.npcs.push_back(n);
  }else if(f[0]=="G"){PTCustomGoods g;g.id=key;if((f.size()!=6&&f.size()!=7)||e.goods.size()>=19||!Integer(f[3],g.silver,0,1000000000)||!Item(f[4],g.item)||!Integer(f[5],g.item.count,1,64))return false;for(unsigned int x=0;x<e.goods.size();++x)if(e.goods[x].id==key)return false;if(f.size()==7&&!Label(f[6],g.name))return false;e.goods.push_back(g);
  }else if(f[0]=="B"){PTCustomGoods* goods=0;for(unsigned int x=0;x<e.goods.size();++x)if(e.goods[x].id==key)goods=&e.goods[x];if(f.size()!=5||!goods||goods->barter||goods->silver||!Item(f[3],goods->payment)||!Integer(f[4],goods->payment.count,1,1000000000))return false;goods->barter=true;
  }else if(f[0]=="R"){PTCustomRecipe r;r.id=key;r.eventId=id;if(f.size()!=12||e.recipes.size()>=19||!recipeIds.insert(key).second||!Integer(f[3],r.cook,0,604800)||!Integer(f[4],r.burn,1,604800)||!Integer(f[5],r.silver,0,1000000000)||!Item(f[7],r.output)||!Integer(f[8],r.output.count,1,64)||!Label(f[9],r.name)||!Script(f[10],id,e.revision,r.script)||!Script(f[11],id,e.revision,r.useScript))return false;
   if(f[6]!="-"){r.hasTool=true;if(!Item(f[6],r.tool))return false;}if(r.output.genre!=3&&r.output.genre!=4&&r.output.genre!=6&&r.output.genre!=8)return false;e.recipes.push_back(r);
  }else if(f[0]=="P"){if(f.size()!=6||!e.toolScript.empty()||!Script(f[3],id,e.revision,e.toolScript)||!Integer(f[4],e.toolNpc,1,65535)||!Label(f[5],e.toolName)||e.toolName.size()>31)return false;
  }else if(f[0]=="T"){if(f.size()!=5)return false;PTCustomRecipe* recipe=0;for(unsigned int x=0;x<e.recipes.size();++x)if(e.recipes[x].id==key)recipe=&e.recipes[x];if(!recipe)return false;char text[1201];if(!PtEvents::Hex(f[4],text,sizeof(text)))return false;
   if(f[3]=="wait")recipe->waitText=text;else if(f[3]=="ready")recipe->readyText=text;else if(f[3]=="burnt")recipe->burntText=text;else if(f[3]=="use")recipe->useText=text;else return false;
  }else if(f[0]=="V"){
   PTCustomRecipe* recipe=0;for(unsigned int x=0;x<e.recipes.size();++x)if(e.recipes[x].id==key)recipe=&e.recipes[x];
   if(f.size()!=4||!recipe||recipe->hasBatchOptions||!Integer(f[3],recipe->batchMask,1,7))return false;recipe->hasBatchOptions=true;
  }else if(f[0]=="U"){PTCustomRecipe* recipe=0;for(unsigned int x=0;x<e.recipes.size();++x)if(e.recipes[x].id==key)recipe=&e.recipes[x];if(f.size()!=6||!recipe||recipe->hasUseResources||!Integer(f[3],recipe->useExperience,0,1000000000)||!Integer(f[4],recipe->useSilver,0,1000000000)||!Integer(f[5],recipe->randomSilver,0,1000000000))return false;recipe->hasUseResources=true;
  }else if(f[0]=="I"||f[0]=="W"){if(f.size()!=5)return false;PTCustomRecipe* recipe=0;for(unsigned int x=0;x<e.recipes.size();++x)if(e.recipes[x].id==key)recipe=&e.recipes[x];if(!recipe)return false;PTCustomItem item;if(!Item(f[3],item)||!Integer(f[4],item.count,1,1000))return false;std::vector<PTCustomItem>& target=f[0]=="I"?recipe->needs:recipe->rewards;if(target.size()>=(f[0]=="W"?1000:12))return false;target.push_back(item);
  }else return false;
 }
 // Load new versioned scripts before swapping any active NPC bindings.
 for(std::map<int,PTCustomEvent>::iterator it=candidate.begin();it!=candidate.end();++it){PTCustomEvent& e=it->second;
  for(unsigned int r=0;r<e.recipes.size();++r)if(e.recipes[r].needs.empty()||!PhongThanReloadScriptSafe(e.recipes[r].script.c_str())||!PhongThanReloadScriptSafe(e.recipes[r].useScript.c_str()))return false;
  if(!e.toolScript.empty()&&!PhongThanReloadScriptSafe(e.toolScript.c_str()))return false;
  if(e.configuredEnabled)for(unsigned int n=0;n<e.npcs.size();++n)if(!PhongThanReloadScriptSafe(e.npcs[n].script.c_str()))return false;
 }
 for(unsigned int old=0;old<spawns.size();++old)Remove(spawns[old]);spawns.clear();events.swap(candidate);revision=version;return true;
}
static void Status(){char data[512];sprintf(data,"{\"Version\":1,\"Pid\":%lu,\"Utc\":%lu,\"Revision\":%ld,\"Npcs\":%u,\"Error\":\"%s\"}",GetCurrentProcessId(),(unsigned long)time(NULL),revision,(unsigned)spawns.size(),error.c_str());CreateDirectoryA("EventService\\CustomEvents",0);PtEvents::Write("EventService\\CustomEvents\\status.json",data);}
}
const PTCustomEvent* PhongThanCustomEvent(int id){std::map<int,PTCustomEvent>::iterator i=PtCustomEvents::events.find(id);return i==PtCustomEvents::events.end()?0:&i->second;}
const PTCustomRecipe* PhongThanCustomRecipe(int id){for(std::map<int,PTCustomEvent>::iterator e=PtCustomEvents::events.begin();e!=PtCustomEvents::events.end();++e)for(unsigned int r=0;r<e->second.recipes.size();++r)if(e->second.recipes[r].id==id)return &e->second.recipes[r];return 0;}
bool PhongThanCustomNpcAccess(int p,int eventId,int npcId,int feature){using namespace PtCustomEvents;const PTCustomEvent* e=PhongThanCustomEvent(eventId);if(!e||!e->enabled||p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].CheckTrading()||Player[p].GetLockState())return false;
 int actor=Player[p].m_nIndex;if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsAlive())return false;
 for(unsigned int n=0;n<e->npcs.size();++n){const PTCustomNpc& def=e->npcs[n];if(def.id!=npcId||(feature==1&&!def.shop)||(feature==2&&!def.recipes))continue;
  for(unsigned int j=0;j<spawns.size();++j){Spawn& s=spawns[j];if(s.eventId!=eventId||s.npcId!=npcId||s.index<=0||s.index>=MAX_NPC||Npc[s.index].m_dwID!=s.entity)continue;
   return Player[p].m_nLastNpcIndex==s.index&&Npc[actor].m_SubWorldIndex==Npc[s.index].m_SubWorldIndex&&NpcSet.GetDistance(actor,s.index)<=384;
  }
 }return false;
}
void PhongThanCustomEventsTick(){using namespace PtCustomEvents;unsigned long now=(unsigned long)time(NULL);if(now<next)return;next=now+1;std::string bytes;
 if(!PtEvents::Read("EventService\\CustomEvents\\current.tsv",bytes,16777216)){Status();return;}if(!Load(bytes)){error="configuration_or_generated_script_invalid";Status();return;}error="";
 for(std::map<int,PTCustomEvent>::iterator it=events.begin();it!=events.end();++it){PTCustomEvent& e=it->second;e.enabled=e.configuredEnabled&&e.schedule.ActiveNow((time_t)now);if(!e.enabled){for(unsigned int j=0;j<spawns.size();){if(spawns[j].eventId==e.id){Remove(spawns[j]);spawns.erase(spawns.begin()+j);}else ++j;}continue;}
  for(unsigned int n=0;n<e.npcs.size();++n){PTCustomNpc& def=e.npcs[n];bool exists=false;for(unsigned int j=0;j<spawns.size();++j)if(spawns[j].eventId==e.id&&spawns[j].npcId==def.id){exists=true;break;}if(exists)continue;
   int world=g_SubWorldSet.SearchWorld(def.map);if(world<0){error="map_not_loaded";continue;}if(SubWorld[world].GetBarrier(def.x,def.y)!=0){error="npc_position_blocked";continue;}
   int index=NpcSet.Add(MAKELONG(1,def.tid),world,def.x,def.y,TRUE);if(index<=0||index>=MAX_NPC){error="npc_spawn_failed";continue;}KNpc& npc=Npc[index];npc.m_Kind=kind_dialoger;npc.SetFightMode(0);npc.SetCurrentCamp(camp_free);lstrcpynA(npc.Name,def.name.c_str(),sizeof(npc.Name));lstrcpynA(npc.ActionScript,def.script.c_str(),sizeof(npc.ActionScript));npc.m_ActionScriptID=g_FileName2Id(npc.ActionScript);npc.m_DeathScriptID=npc.m_TimerScriptID=npc.m_DropScriptID=0;
   Spawn spawned;spawned.index=index;spawned.entity=npc.m_dwID;spawned.eventId=e.id;spawned.npcId=def.id;spawns.push_back(spawned);
  }
 }PhongThanCustomPotsTick();Status();
}
