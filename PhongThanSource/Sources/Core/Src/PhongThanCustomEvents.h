#ifndef PT_CUSTOM_EVENTS_H
#define PT_CUSTOM_EVENTS_H
#include <string>
#include <vector>
#include "PhongThanCustomEventSchedule.h"
struct PTCustomItem {int genre,detail,part,level,series,count;PTCustomItem():genre(0),detail(0),part(0),level(0),series(0),count(1){}};
struct PTCustomGoods {int id,silver;bool barter;std::string name;PTCustomItem item,payment;PTCustomGoods():id(0),silver(0),barter(false){}};
struct PTCustomRecipe {int eventId,id,cook,burn,silver,useExperience,useSilver,randomSilver,batchMask;bool hasTool,hasUseResources,hasBatchOptions;PTCustomItem tool,output;std::vector<PTCustomItem> needs,rewards;std::string name,script,useScript,waitText,readyText,burntText,useText;PTCustomRecipe():eventId(0),id(0),cook(0),burn(0),silver(0),useExperience(0),useSilver(0),randomSilver(0),batchMask(7),hasTool(false),hasUseResources(false),hasBatchOptions(false){}};
inline bool PTCustomScaleRecipe(const PTCustomRecipe& source,int quantity,PTCustomRecipe& target){
 target=source;if(!quantity)return true;
 int bit=quantity==1?1:quantity==10?2:quantity==100?4:0;
 if(!bit||!(source.batchMask&bit)||source.output.count<=0)return false;
 for(unsigned int i=0;i<target.needs.size();++i){
  int n=source.needs[i].count*quantity;if(n%source.output.count||n/source.output.count<1||n/source.output.count>10000)return false;
  target.needs[i].count=n/source.output.count;
 }
 // Currency rounds upwards; item quantities must divide exactly.
 double fee=(double)source.silver*quantity/source.output.count;if(fee>1000000000)return false;
 target.silver=(int)fee;if(target.silver<fee)++target.silver;target.output.count=quantity;return true;
}
struct PTCustomNpc {int id,tid,map,x,y;bool shop,recipes;std::string name,script;PTCustomNpc():id(0),tid(0),map(0),x(0),y(0),shop(false),recipes(false){}};
struct PTCustomEvent {int id,toolNpc,shopTheme,toolIdleSeconds;bool enabled,configuredEnabled,hasShopTheme,hasToolIdle;PTCustomEventSchedule schedule;long revision;std::string name,toolScript,toolName;std::vector<PTCustomNpc> npcs;std::vector<PTCustomGoods> goods;std::vector<PTCustomRecipe> recipes;PTCustomEvent():id(0),toolNpc(261),shopTheme(1),toolIdleSeconds(300),enabled(false),configuredEnabled(false),hasShopTheme(false),hasToolIdle(false),revision(0){}};
#ifdef _SERVER
void PhongThanCustomEventsTick();
void PhongThanCustomPotsTick();
const PTCustomEvent* PhongThanCustomEvent(int id);
const PTCustomRecipe* PhongThanCustomRecipe(int id);
bool PhongThanCustomNpcAccess(int player,int eventId,int npcId,int feature);
int PhongThanCustomUseItem(int player,int index);
struct lua_State;int LuaCustomEvent(lua_State*);int LuaCustomEventConsume(lua_State*);
#endif
#endif
