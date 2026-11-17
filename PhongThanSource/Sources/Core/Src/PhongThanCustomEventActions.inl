// ScriptFuns.cpp: normal inventory rollback/CAS owns all event payments.
#include "PhongThanCustomEvents.h"
#include "KRandom.h"
#include "../../../Headers/PhongThanEventShop.h"
#include "../../../Headers/PhongThanPotProgress.h"
namespace PtCustomActions {
enum { Task=5348,CookFirst=5300,CookSlots=32,Tag=0x2E000000,ToolTag=0x2D000000,Mask=0xFF000000 };
static int usePlayer=0,useItem=0,useResult=0,recipeResult=0;
static bool Ready(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsAlive()&&!Player[p].CheckTrading()&&!Player[p].GetLockState();}
static bool MapItem(const PTCustomItem& item,PhongThanQuestExchangeRow& row){row.count=item.count;return PhongThanDecodeQuestItemTuple(item.genre,item.detail,item.part,item.level,item.series,0,row.item);}
static int Token(int p){int old=Player[p].m_cTask.GetSaveVal(Task);return old>=0&&old<2000000000?old+1:0;}
static bool HasTool(int p,const PTCustomItem& tool){PhongThanQuestExchangeRow row;if(!MapItem(tool,row))return false;for(PlayerItem* i=Player[p].m_ItemList.GetFirstItem();i;i=Player[p].m_ItemList.GetNextItem())if(i->nPlace==pos_equiproom||i->nPlace==pos_equiproomex||i->nPlace==pos_ptbag2||i->nPlace==pos_ptbag3||i->nPlace==pos_immediacy){int n=i->nIdx;if(n>0&&n<MAX_ITEM&&Item[n].GetID()&&!Item[n].GetLock()->IsLock()&&PhongThanQuestItemMatches(Item[n],row.item)&&Item[n].GetStackNum()>0)return true;}return false;}
static bool Same(const PTCustomItem& a,const PTCustomItem& b){return a.genre==b.genre&&a.detail==b.detail&&a.part==b.part&&a.level==b.level&&a.series==b.series;}
static bool CookingTool(const PTCustomEvent& e,const PTCustomItem& item){for(unsigned int r=0;r<e.recipes.size();++r)if(e.recipes[r].hasTool&&Same(e.recipes[r].tool,item))return true;return false;}
static bool OwnsTool(int p,const PTCustomEvent& e){if(!e.enabled||e.toolScript.empty())return false;
 for(PlayerItem* i=Player[p].m_ItemList.GetFirstItem();i;i=Player[p].m_ItemList.GetNextItem())if(i->nPlace==pos_equiproom||i->nPlace==pos_equiproomex||i->nPlace==pos_ptbag2||i->nPlace==pos_ptbag3||i->nPlace==pos_immediacy){int n=i->nIdx;if(n>0&&n<MAX_ITEM&&Item[n].GetID()&&Item[n].GetStackNum()>0&&!Item[n].GetLock()->IsLock()&&Item[n].m_CommonAttrib.nParam==(ToolTag|e.id))return true;}return false;
}
static void Tell(int p,const char* text){KPhongThanScriptAction a;ZeroMemory(&a,sizeof(a));a.Operation=PHONGTHAN_SCRIPT_SHOW;a.View=UI_TOPMESSAGE;a.ServerOwned=1;lstrcpynA(a.Content,text,sizeof(a.Content));a.ContentLength=strlen(a.Content);Player[p].DoScriptAction(&a);}
static bool JobPath(int p,int eventId,int recipe,char* path){const PHONGTHAN_CHARACTER_STATE_HEADER* state=(const PHONGTHAN_CHARACTER_STATE_HEADER*)Player[p].m_SaveBuffer;if(!state||!PhongThanValidateCharacterState(state,state->StateSize))return false;
 char key[180];int at=0;for(int part=0;part<2;++part){const char* name=part?Player[p].Name:Player[p].AccountName;for(int i=0;name[i]&&i<32;++i)at+=sprintf(key+at,"%02x",(unsigned char)name[i]);key[at++]='-';}sprintf(key+at,"%lu",state->RoleTime);
 CreateDirectoryA("EventService\\CustomEvents",0);CreateDirectoryA("EventService\\CustomEvents\\jobs",0);char relative[400];sprintf(relative,"EventService\\CustomEvents\\jobs\\%s-%d-%d.ini",key,eventId,recipe);return GetFullPathNameA(relative,400,path,0)>0;
}
static std::string Format(std::string text,const PTCustomRecipe& recipe,unsigned long seconds);
#include "PhongThanCustomEventPots.inl"
static bool WriteJob(const char* path,int token,unsigned long ready,unsigned long burn,int claimed,const PTCustomItem& output){char b[512];sprintf(b,"[Cook]\nToken=%d\nReady=%lu\nBurn=%lu\nClaim=%d\nOutput=%d,%d,%d,%d,%d,%d\n",token,ready,burn,claimed,output.genre,output.detail,output.part,output.level,output.series,output.count);
 char next[400];sprintf(next,"%s.next",path);HANDLE f=CreateFileA(next,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;DWORD written=0;bool ok=WriteFile(f,b,strlen(b),&written,0)&&written==strlen(b)&&FlushFileBuffers(f);CloseHandle(f);if(ok)ok=MoveFileExA(next,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;if(!ok)DeleteFileA(next);WritePrivateProfileStringA(NULL,NULL,NULL,path);return ok;
}
static bool ActiveJob(int p,const char* path){int recipe=GetPrivateProfileIntA("Cook","Token",0,path),slot=GetPrivateProfileIntA("Cook","Claim",0,path);return recipe>0&&slot>=CookFirst&&slot<CookFirst+CookSlots&&Player[p].m_cTask.GetSaveVal(slot)==recipe;}
static int Exchange(int p,const std::vector<PTCustomItem>& needs,const std::vector<PTCustomItem>& rewards,int silver,const PTCustomRecipe* produced=0,int exact=0,int task=Task,int expected=-1,int nextValue=-1,int tag=Tag,int silverReward=0,int experienceReward=0){if(!Ready(p)||needs.size()>12||rewards.size()>12||silver<0||silverReward<0||experienceReward<0||Player[p].m_ItemList.GetEquipmentMoney()<silver||Player[p].m_ItemList.GetEquipmentMoney()-silver>2147483647-silverReward)return 0;
 int next=nextValue>=0?nextValue:Token(p),before=expected>=0?expected:next-1;if(nextValue<0&&!next)return 0;PhongThanQuestExchangeRow input[12],output[12];unsigned int i;for(i=0;i<needs.size();++i)if(!MapItem(needs[i],input[i]))return 0;for(i=0;i<rewards.size();++i)if(!MapItem(rewards[i],output[i]))return 0;
 int prototypeId=0;KItem* prototype=0;if(produced){const PTCustomItem& o=produced->output;prototypeId=CreateVngNormalItem(o.genre,o.detail,o.part,o.level,o.series,0);if(prototypeId<=0)return 0;prototype=&Item[prototypeId];prototype->m_CommonAttrib.nParam=tag|produced->id;}
 if(silver&&!Player[p].m_ItemList.CostMoney(silver)){if(prototypeId)ItemSet.Remove(prototypeId);return 0;}
 if(silverReward&&!Player[p].Earn(silverReward)){if(silver)Player[p].Earn(silver);if(prototypeId)ItemSet.Remove(prototypeId);return 0;}
 int ok=PhongThanQuestExchange(p,task,before,next,input,(int)needs.size(),output,(int)rewards.size(),true,prototype,true,exact);
 if(prototypeId)ItemSet.Remove(prototypeId);if(!ok&&silverReward)Player[p].m_ItemList.CostMoney(silverReward);if(!ok&&silver)Player[p].Earn(silver);if(ok){if(experienceReward)Player[p].DirectAddExp(experienceReward);Player[p].Save();}return ok;
}
static int Buy(int p,const PTCustomEvent& e,const PTCustomGoods& goods){std::vector<PTCustomItem> needs,rewards;rewards.push_back(goods.item);if(goods.barter)needs.push_back(goods.payment);PTCustomRecipe tool;tool.output=goods.item;tool.id=e.id;return Exchange(p,needs,rewards,goods.silver,CookingTool(e,goods.item)?&tool:0,0,Task,-1,-1,ToolTag)?2:-2;}
 #include "PhongThanEventShopServer.inl"
// Each use draws one entry. Its count is the quantity awarded for that draw.
static bool HasUseReward(const PTCustomRecipe& recipe){return !recipe.rewards.empty()||recipe.useExperience>0||recipe.useSilver>0||recipe.randomSilver>0;}
static int Consume(int p,const PTCustomRecipe& recipe,int index){if(!HasUseReward(recipe)||recipe.rewards.size()>1000||recipe.useExperience<0||recipe.useSilver<0||recipe.randomSilver<0)return 0;PTCustomItem source=recipe.output;source.count=1;std::vector<PTCustomItem> needs,rewards;needs.push_back(source);int money=recipe.useSilver;unsigned int choices=recipe.rewards.size()+(recipe.randomSilver>0?1:0);if(choices){unsigned int draw=g_Random(choices);if(draw<recipe.rewards.size())rewards.push_back(recipe.rewards[draw]);else{if(money>2147483647-recipe.randomSilver)return 0;money+=recipe.randomSilver;}}return Exchange(p,needs,rewards,0,0,index,Task,-1,-1,Tag,money,recipe.useExperience);}
static void ExpireJobs(int p){unsigned long now=(unsigned long)time(NULL);for(int slot=CookFirst;slot<CookFirst+CookSlots;++slot){int id=Player[p].m_cTask.GetSaveVal(slot);const PTCustomRecipe* r=PhongThanCustomRecipe(id);char path[400];if(!r||!JobPath(p,r->eventId,id,path)||!ActiveJob(p,path))continue;unsigned long burn=GetPrivateProfileIntA("Cook","Burn",0,path);if(burn&&now>=burn){std::vector<PTCustomItem> empty;if(Exchange(p,empty,empty,0,0,0,slot,id,0))FinishPot(p,r->eventId,id);}}}
static int Start(int p,const PTCustomRecipe& recipe){ExpireJobs(p);const PTCustomEvent* e=PhongThanCustomEvent(recipe.eventId);Pot* pot=recipe.hasTool?OwnedPot(p,recipe.eventId):0;if(recipe.hasTool&&(!e||!pot||!ToolAccess(p,*e)||!HasTool(p,recipe.tool)))return -2;if(pot&&pot->recipe)return -4;
 std::vector<PTCustomItem> needs=recipe.needs;if(recipe.hasTool){PTCustomItem tool=recipe.tool;tool.count=1;needs.push_back(tool);}
 if(!recipe.cook){std::vector<PTCustomItem> output;output.push_back(recipe.output);if(!Exchange(p,needs,output,recipe.silver,&recipe))return -2;if(pot){pot->recipe=recipe.id;FinishPot(p,recipe.eventId,recipe.id);}return 2;}
 char path[400];if(!JobPath(p,recipe.eventId,recipe.id,path))return -2;if(ActiveJob(p,path))return -4;
 int slot=0;for(int i=CookFirst;i<CookFirst+CookSlots;++i){int active=Player[p].m_cTask.GetSaveVal(i);if(active&&!PhongThanCustomRecipe(active)){Player[p].m_cTask.SetSaveVal(i,0,TRUE);active=0;}if(!active){slot=i;break;}}
 unsigned long now=(unsigned long)time(NULL);if(!slot||!WriteJob(path,recipe.id,now+recipe.cook,now+recipe.cook+recipe.burn,slot,recipe.output))return -2;
 Pot before;if(pot){before=*pot;pot->recipe=recipe.id;pot->started=now;pot->ready=now+recipe.cook;pot->burn=now+recipe.cook+recipe.burn;if(!SavePot(*pot)){*pot=before;return -2;}}
 std::vector<PTCustomItem> empty;if(!Exchange(p,needs,empty,recipe.silver,0,0,slot,0,recipe.id)){if(pot){*pot=before;SavePot(*pot);}return -2;}return 1;
}
static std::string Format(std::string text,const PTCustomRecipe& recipe,unsigned long seconds){char n[32],burn[32];sprintf(n,"%lu",seconds);sprintf(burn,"%d",recipe.burn);const char* keys[]={"{name}","{seconds}","{burn_seconds}"};const char* values[]={recipe.name.c_str(),n,burn};for(int k=0;k<3;++k){unsigned int at=0;while((at=text.find(keys[k],at))!=std::string::npos){text.replace(at,strlen(keys[k]),values[k]);at+=strlen(values[k]);}}return text;}
static int Claim(int p,const PTCustomRecipe& recipe,bool status){Pot* pot=recipe.hasTool?OwnedPot(p,recipe.eventId):0;if(recipe.hasTool&&(!pot||pot->recipe!=recipe.id))return -2;char path[400];if(!JobPath(p,recipe.eventId,recipe.id,path)||!ActiveJob(p,path))return -2;
 unsigned long now=(unsigned long)time(NULL),ready=GetPrivateProfileIntA("Cook","Ready",0,path),burn=GetPrivateProfileIntA("Cook","Burn",0,path);int token=GetPrivateProfileIntA("Cook","Token",0,path);
 if(status){char text[240];std::string custom;if(now<ready){sprintf(text,"%s: con %lu giay de chin. Nhan trong %d giay sau khi chin.",recipe.name.c_str(),ready-now,recipe.burn);custom=Format(recipe.waitText,recipe,ready-now);}else if(now<burn){sprintf(text,"%s: da chin, con %lu giay truoc khi chay.",recipe.name.c_str(),burn-now);custom=Format(recipe.readyText,recipe,burn-now);}else{sprintf(text,"%s: da chay. Chon nhan/xu ly de bat dau me moi.",recipe.name.c_str());custom=Format(recipe.burntText,recipe,0);}Tell(p,custom.empty()?text:custom.c_str());return 0;}
 if(now<ready)return -3;
 char raw[180];GetPrivateProfileStringA("Cook","Output","",raw,sizeof(raw),path);PTCustomRecipe snapshot=recipe;if(sscanf(raw,"%d,%d,%d,%d,%d,%d",&snapshot.output.genre,&snapshot.output.detail,&snapshot.output.part,&snapshot.output.level,&snapshot.output.series,&snapshot.output.count)!=6||snapshot.output.count<1||snapshot.output.count>100)return -2;
 int slot=GetPrivateProfileIntA("Cook","Claim",0,path);if(slot<CookFirst||slot>=CookFirst+CookSlots)return -2;
 std::vector<PTCustomItem> empty,output;if(now<burn)output.push_back(snapshot.output);if(!Exchange(p,empty,output,0,now<burn?&snapshot:0,0,slot,recipe.id,0))return -2;if(recipe.hasTool)FinishPot(p,recipe.eventId,recipe.id);return now<burn?3:4;
}
}
void PhongThanEventShopBuy(int p,const PTEventShopBuy& request){if((request.EventId&0x40000000UL)&&PTEventShopExternal())PTEventShopExternal()(p,request);else PtCustomActions::ShopRequest(p,request);}
int LuaCustomEvent(Lua_State* L){using namespace PtCustomActions;int argc=Lua_GetTopIndex(L);int p=GetPlayerIndex(L),eventId=(int)Lua_ValueToNumber(L,1),npc=(int)Lua_ValueToNumber(L,2),op=(int)Lua_ValueToNumber(L,3),key=(int)Lua_ValueToNumber(L,4),result=-1;
 int quantity=argc>=5?(int)Lua_ValueToNumber(L,5):0;
 const PTCustomEvent* e=PhongThanCustomEvent(eventId);int feature=(op==1||op==7)?1:op>=2?2:0;
 if(Ready(p)&&e&&(npc==0?(op!=1&&ToolAccess(p,*e)):PhongThanCustomNpcAccess(p,eventId,npc,feature))){
  if(op==6){
   Pot* pot=npc==0?OwnedPot(p,eventId):0;int recipeId=npc==0?(pot?pot->recipe:0):key,phase=0,seconds=0;
   const PTCustomRecipe* active=PhongThanCustomRecipe(recipeId);
   if(!active||active->eventId!=eventId||(npc!=0&&active->hasTool))recipeId=0;
   if(recipeId){char path[400];if(JobPath(p,eventId,recipeId,path)&&ActiveJob(p,path)){
    unsigned long now=(unsigned long)time(NULL),ready=GetPrivateProfileIntA("Cook","Ready",0,path),burn=GetPrivateProfileIntA("Cook","Burn",0,path);
    phase=now<ready?1:now<burn?2:3;seconds=phase==1?ready-now:phase==2?burn-now:0;
   }}
   Lua_PushNumber(L,recipeId);Lua_PushNumber(L,phase);Lua_PushNumber(L,seconds);return 3;
  }
  if(op==0)result=1;
  else if(op==7&&npc>0)result=OpenShop(p,*e,npc);
  else if(op==1){for(unsigned int i=0;i<e->goods.size();++i)if(e->goods[i].id==key){result=Buy(p,*e,e->goods[i]);break;}}
  else{const PTCustomRecipe* recipe=PhongThanCustomRecipe(key);if(recipe&&recipe->eventId==eventId&&(!recipe->hasTool||npc==0)){
   if(op==2){recipeResult=-2;char main[]="main";Player[p].ExecuteScript2((char*)recipe->script.c_str(),main,npc,quantity);result=recipeResult;}
   else if(op==3){PTCustomRecipe scaled;recipeResult=result=PTCustomScaleRecipe(*recipe,quantity,scaled)?Start(p,scaled):-5;}
   else if(op==4)result=Claim(p,*recipe,false);else if(op==5)result=Claim(p,*recipe,true);
  }}
 }Lua_PushNumber(L,result);return 1;
}
int LuaCustomEventConsume(Lua_State* L){using namespace PtCustomActions;int p=GetPlayerIndex(L),event=(int)Lua_ValueToNumber(L,1),id=(int)Lua_ValueToNumber(L,2);const PTCustomRecipe* recipe=PhongThanCustomRecipe(id);int result=0;
 if(p==usePlayer&&useItem>0&&useItem<MAX_ITEM&&Ready(p)&&recipe&&recipe->eventId==event&&HasUseReward(*recipe)&&Item[useItem].m_CommonAttrib.nParam==(Tag|id)&&Player[p].m_ItemList.FindSame(useItem)>0){
  result=Consume(p,*recipe,useItem);
 }if(result&&recipe&&!recipe->useText.empty())Tell(p,recipe->useText.c_str());useResult=result;Lua_PushNumber(L,result);return 1;
}
int PhongThanCustomUseItem(int p,int index){using namespace PtCustomActions;if(index<=0||index>=MAX_ITEM)return -1;int type=Item[index].m_CommonAttrib.nParam&Mask;if(type!=Tag&&type!=ToolTag)return -1;
 if(!Ready(p)||usePlayer||Item[index].GetLock()->IsLock())return 0;int id=Item[index].m_CommonAttrib.nParam&0xFFFFFF;
 if(type==ToolTag){const PTCustomEvent* e=PhongThanCustomEvent(id);return e&&OwnsTool(p,*e)&&PlacePot(p,*e)?1:0;}
 const PTCustomRecipe* recipe=PhongThanCustomRecipe(id);if(!recipe||!HasUseReward(*recipe))return 0;
 usePlayer=p;useItem=index;useResult=0;char main[]="main";Player[p].ExecuteScript(g_FileName2Id((char*)recipe->useScript.c_str()),main,0);int result=useResult;usePlayer=useItem=useResult=0;return result;
}
void PhongThanCustomPotsTick(){PtCustomActions::TickPots();}
