#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include "KWin32.h"
#include "KLuaScript.h"
#include "GameDataDef.h"
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
#include "../../Headers/PhongThanBotObjectives.h"
#include "../../Headers/PhongThanBotNpcVisit.h"
enum {Capacity=1};
#define PTBOT_STORY_CATALOG_ONLY
#include "../../Sources/Core/Src/PhongThanBotStory.inl"
static int writes=0;
static int Forbidden(Lua_State*){++writes;return 0;}
static int Stub(Lua_State*){return 0;}
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);assert(argc>1);
 for(int file=1;file<argc;++file){KLuaScript script;assert(script.Init());TLua_Funcs api[]={{"Include",Stub},{"SetTask",Forbidden},{"QuestExchange",Forbidden},{"AddOwnExp",Forbidden}};assert(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));assert(script.Load(argv[file]));
  int top=lua_gettop(script.m_LuaState),before=botStoryActions.size();BotReadStory(file,&script,0);BotReadStory(file,&script,1);assert(lua_gettop(script.m_LuaState)==top);if((int)botStoryActions.size()==before)printf("INFO no table action for NPC script %s\n",argv[file]);assert(!writes);
  for(int i=before;i<(int)botStoryActions.size();++i){const BotStoryAction& a=botStoryActions[i];assert(a.script==(DWORD)file&&a.task>0&&a.task<5000&&a.step>0&&a.profession>=0&&a.profession<=2);}
 }
 KLuaScript bad;assert(bad.Init());const char* payload="PTG_ROLE='test';PTG_STEPS={{role='test',level=1,task=1,from=0,to=1,needs={{999,0,0,0,0,0,1}}}}";assert(!Lua_ExecuteBuffer(bad.m_LuaState,payload,strlen(payload),"invalid needs"));int before=botStoryActions.size();BotReadStory(999,&bad,0);assert((int)botStoryActions.size()==before);
 assert(PTBotObjectiveScore(PTBOT_STORY,0,0,0,10)<PTBotObjectiveScore(PTBOT_TRAIN,0,0,0,10));assert(PTBotObjectiveScore(PTBOT_BOSS,-1,0,0,1)<0);assert(PTBotObjectiveScore(PTBOT_BOSS,0,101,0,1)<0);assert(PTBotObjectiveScore(PTBOT_EVENT,0,0,0,10,true)<PTBotObjectiveScore(PTBOT_EVENT,0,0,0,10));
 printf("PASS authored story catalog: %d actions, %d kill rules, %d NPC scripts; no game mutations; invalid requirements rejected; activity priorities bounded.\n",botStoryActions.size(),botStoryKills.size(),argc-1);return 0;
}
