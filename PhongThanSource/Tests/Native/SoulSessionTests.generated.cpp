
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#define _SERVER
#include "../../Headers/PhongThanSoulState.h"
#include "../../Headers/PhongThanCompose.h"
enum {item_equip=0,pos_builditem=7,SAVE_REQUEST=1,MAX_PLAYER=2,MAX_NPC=3,MAX_SUBWORLD=2,MAX_ITEM=32,MAX_PLAYER_ITEM=32};
struct KItem {int genre,detail,part,level,stack,serial,param,marker;
 struct Lock {bool IsLock(){return false;}}lock;Lock* GetLock(){return &lock;}bool IsTemp(){return false;}int GetExpireTime(){return 0;}int GetID(){return serial;}int GetGenre()const{return genre;}int GetDetailType()const{return detail;}int GetParticularType()const{return part;}int GetSeries()const{return 0;}int GetLevel()const{return level;}int GetStackNum()const{return stack;}
 int GetParam()const{return param;}void SetParam(int v){param=v;}int GetWidth(){return 1;}int GetHeight(){return 1;}void SetStackNum(int n){stack=n;}
}Item[32];
struct ItemPos{int nPlace,nX,nY;};
struct Bag{struct Pos {int nPlace,nX;}m_Items[32];int build[9];int GetBuildItem(int s){return build[s];}int FindSame(int n){return owned[n]?n:0;}bool owned[32],room,pay;int money,failedRemove,failedAdd;
 int GetEquipmentMoney(){return money;}
 bool SearchPosition(int,int,ItemPos* p,bool){p->nPlace=1;p->nX=p->nY=0;return room;}
 bool Remove(int id){if(!owned[id]||id==failedRemove)return false;owned[id]=false;return true;}
 int Add(int id,int place,int,int,bool){if(place!=pos_builditem&&id==failedAdd)return 0;owned[id]=true;return id;}
};
struct TestPlayer {Bag m_ItemList;int m_uMustSave,m_nIndex,m_nNetConnectIdx;DWORD m_dwID;bool m_PlayerDBLoad,trading,locked;bool GetLockState(){return locked;}bool CheckTrading(){return trading;}bool Pay(int amount){if(!m_ItemList.pay||m_ItemList.money<amount)return false;m_ItemList.money-=amount;return true;}}Player[2];
struct TestItems{void Remove(int n){Item[n].serial=0;}}ItemSet;
void g_DebugLog(const char*,int,int){}
int g_Random(int){return 0;}
#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"
#include "../../Sources/Core/Src/PhongThanSoulRegistry.inl"
#undef _SERVER
// Original tuple: quantity.genre.detail.particular.series.level.
// Lists, ranges and *<exclusions> retain their authored meaning.
inline bool PtSocketNumber(const char*& p,int& n){n=0;while(*p==' '||*p=='\t')++p;if(*p<'0'||*p>'9')return false;do{int d=*p++-'0';if(n>(2147483647-d)/10)return false;n=n*10+d;}while(*p>='0'&&*p<='9');while(*p==' '||*p=='\t')++p;return true;}
inline bool PtSocketSelector(const char* text,int value){
 if(!strcmp(text,"*"))return true;bool negate=text[0]=='*'&&text[1]=='<';const char* p=text+(negate?2:0);char end=negate?'>':*p=='['?']':0;if(!negate&&end)++p;
 bool found=false;do{int lo=0,hi=0;if(!PtSocketNumber(p,lo))return false;hi=lo;if(*p=='-'){++p;if(!PtSocketNumber(p,hi)||hi<lo)return false;}if(value>=lo&&value<=hi)found=true;if(!end)return !*p&&found;if(*p==end)return !p[1]&&(negate?!found:found);if(*p++!=',')return false;}while(*p);return false;
}
inline bool PtSocketTuple(const char* text,KItem& item){
 if(!strcmp(text,"*"))return true;int values[6]={item.GetStackNum(),item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetSeries(),item.GetLevel()};const char* p=text;
 for(int i=0;i<6;++i){char part[128];int n=0;while(*p&&*p!='.'){if(n>=127)return false;part[n++]=*p++;}part[n]=0;if(!PtSocketSelector(part,values[i]))return false;if(!*p)return i>=2;++p;}return !*p;
}

#include "../../Sources/Core/Src/PhongThanSoulCompose.inl"
static void Reset(){ZeroMemory(Item,sizeof(Item));ZeroMemory(Player,sizeof(Player));Player[1].m_ItemList.money=100000000;Player[1].m_ItemList.room=Player[1].m_ItemList.pay=true;}
static void Seed(int i,int genre,int detail,int part,int level,int count){Item[i].genre=genre;Item[i].detail=detail;Item[i].part=part;Item[i].level=level;Item[i].stack=count;Item[i].serial=100+i;Player[1].m_ItemList.owned[i]=true;}
static void Inputs(const PTSoulRecipe& recipe,int* indexes,int* selected){
 Reset();ZeroMemory(indexes,9*sizeof(int));ZeroMemory(selected,9*sizeof(int));int detail=strstr(recipe.equipment,"[0]")?0:strstr(recipe.equipment,"[10]")?10:2;
 Seed(1,0,detail,detail?0:9,1,1);Item[1].param=0xc17f0012;Item[1].marker=777;indexes[0]=1;selected[0]=1;
 for(int m=0;m<8;++m)if(recipe.materials[m][0]){int quantity=0,genre=0,id=0;assert(sscanf(recipe.materials[m],"%d.%d.%d",&quantity,&genre,&id)==3);Seed(m+2,genre,id,0,0,quantity+3);indexes[m+1]=m+2;selected[m+1]=quantity+3;}
}

#include "../../Headers/PhongThanPlayerProtocol.h"
struct Actor {DWORD m_dwID;int m_CurrentLife,m_RegionIndex,m_SubWorldIndex;}Npc[MAX_NPC];
struct World {int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
struct FakeServer {PHONGTHAN_COMPOSE_RESULT reply;PHONGTHAN_PLAYER_EVENT event;int replies;
 int SendData(int,const void* data,int size){assert(size==sizeof(event));memcpy(&event,data,size);return 0;}
 void PackDataToClient(int,BYTE* data,int size){assert(size==sizeof(reply));memcpy(&reply,data,size);++replies;}
}server,*g_pServer=&server;
DWORD tick=1000;DWORD SoulTestTick(){return tick;}DWORD KSG_GetCurSec(){return 1000;}
#define GetTickCount SoulTestTick
namespace PtSoulService {
struct Session {DWORD Owner,Actor,Nonce,Last,Until,Next,Stone;int Map,Mode,Result,Recipe;};
static Session sessions[MAX_PLAYER];static DWORD serial=0;
static bool Actor(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].m_CurrentLife>0&&Npc[Player[p].m_nIndex].m_RegionIndex>=0&&!Player[p].GetLockState()&&!Player[p].CheckTrading();}
static bool Valid(int p){if(!Actor(p))return false;Session& s=sessions[p];int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;return w>=0&&w<MAX_SUBWORLD&&s.Owner==Player[p].m_dwID&&s.Actor==Npc[a].m_dwID&&s.Map==SubWorld[w].m_SubWorldID&&(int)(GetTickCount()-s.Until)<0;}
static void Reply(int p,int result,int recipe,DWORD nonce){Session& s=sessions[p];PHONGTHAN_COMPOSE_RESULT reply;ZeroMemory(&reply,sizeof(reply));PhongThanInitializeWireHeader(&reply.Header,PT_SOUL_COMPOSE_REPLY,sizeof(reply),PHONGTHAN_WIRE_FLAG_RESPONSE,nonce);reply.MapId=s.Map;reply.EntityId=s.Actor;reply.Result=result;reply.RecipeId=recipe;g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,(BYTE*)&reply,sizeof(reply));}
}
int PhongThanSoulOpen(int p,int mode,unsigned long stone){
 if(!PtSoulService::Actor(p)||(mode!=8&&mode!=9)||(mode==9&&!stone))return 0;int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD)return 0;
 PtSoulService::Session& s=PtSoulService::sessions[p];ZeroMemory(&s,sizeof(s));s.Owner=Player[p].m_dwID;s.Actor=Npc[a].m_dwID;s.Map=SubWorld[w].m_SubWorldID;s.Mode=mode;s.Stone=stone;s.Nonce=(++PtSoulService::serial+GetTickCount())&0x7fffffff;if(!s.Nonce)s.Nonce=1;s.Until=GetTickCount()+120000;
 PHONGTHAN_PLAYER_EVENT event;ZeroMemory(&event,sizeof(event));PhongThanInitializeWireHeader(&event.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(event),PHONGTHAN_WIRE_FLAG_RESPONSE,s.Nonce);event.MapId=s.Map;event.EntityId=s.Actor;event.Operation=PHONGTHAN_PLAYER_ENCHASE_PANEL;event.Value=mode;
 return SUCCEEDED(g_pServer->SendData(Player[p].m_nNetConnectIdx,&event,sizeof(event)))?1:0;
}
static void PhongThanSoulComposeCommand(int p,const PHONGTHAN_COMPOSE_COMMAND* command){
 if(!PtSoulService::Valid(p))return;PtSoulService::Session& s=PtSoulService::sessions[p];if(command->MapId!=(unsigned)s.Map)return;
 if(command->Header.Sequence==s.Last){PtSoulService::Reply(p,s.Result,s.Recipe,s.Nonce);return;}
 if(command->Header.Sequence!=s.Nonce){PtSoulService::Reply(p,PT_COMPOSE_STALE,0,s.Nonce);return;}
 DWORD now=GetTickCount();if(s.Next&&(int)(now-s.Next)<0)return;s.Next=now+300;
 int indexes[9],selected[9];bool valid=true,stone=!s.Stone;
 for(int slot=0;slot<9;++slot){indexes[slot]=Player[p].m_ItemList.GetBuildItem(slot);selected[slot]=0;
  int i=indexes[slot];if(!i){if(command->Items[slot])valid=false;continue;}if(i<=0||i>=MAX_ITEM){valid=false;continue;}
  int at=Player[p].m_ItemList.FindSame(i);if(!at||at>=MAX_PLAYER_ITEM||Player[p].m_ItemList.m_Items[at].nPlace!=pos_builditem||Player[p].m_ItemList.m_Items[at].nX!=slot)valid=false;
  KItem& item=Item[i];int quantity=command->Quantities[slot],count=item.GetStackNum();
  if(!item.GetID()||item.GetID()!=command->Items[slot]||count<1||count>10000||quantity<-1||quantity>count||item.GetLock()->IsLock()||item.IsTemp()||(item.GetExpireTime()>0&&(unsigned)item.GetExpireTime()<=KSG_GetCurSec()))valid=false;
  selected[slot]=quantity==-1?0:quantity?quantity:count;if(selected[slot]>0&&item.GetID()==s.Stone&&PtSoulRules::Stone(item))stone=true;
 }
 int recipe=0,money=0;int result=valid&&stone?PtSoulCompose(p,indexes,selected,s.Mode,recipe,money,false):PT_COMPOSE_STALE;
 s.Last=s.Nonce;if(++s.Nonce==0)s.Nonce=1;s.Result=result;s.Recipe=recipe;s.Until=now+120000;PtSoulService::Reply(p,result,recipe,s.Nonce);
}

static void Init(PHONGTHAN_COMPOSE_COMMAND& command){
 int indexes[9],selected[9];Inputs(ptSoulRecipes[0],indexes,selected);
 for(int i=0;i<9;++i){Player[1].m_ItemList.build[i]=indexes[i];int id=indexes[i];if(id){Player[1].m_ItemList.m_Items[id].nPlace=pos_builditem;Player[1].m_ItemList.m_Items[id].nX=i;}command.Items[i]=id?Item[id].serial:0;command.Quantities[i]=selected[i];}
 Player[1].m_nIndex=1;Player[1].m_PlayerDBLoad=true;Player[1].m_dwID=50;Npc[1].m_dwID=51;Npc[1].m_CurrentLife=100;Npc[1].m_RegionIndex=0;Npc[1].m_SubWorldIndex=0;SubWorld[0].m_SubWorldID=1021;
 assert(PhongThanSoulOpen(1,8)==1);command.MapId=1021;command.Header.Sequence=PtSoulService::sessions[1].Nonce;
}
int main(){
 CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);PHONGTHAN_COMPOSE_COMMAND command;ZeroMemory(&command,sizeof(command));Init(command);
 // Preview trims partial stacks; copy only quantities into authoritative request.
 int indexes[9],selected[9];for(int i=0;i<9;++i){indexes[i]=Player[1].m_ItemList.build[i];selected[i]=command.Quantities[i];}int recipe,money;assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);memcpy(command.Quantities,selected,sizeof(selected));
 PhongThanSoulComposeCommand(1,&command);assert(server.reply.Result==PT_COMPOSE_OK);int balance=Player[1].m_ItemList.money,param=Item[1].param,quantity=Item[2].stack;
 PhongThanSoulComposeCommand(1,&command);assert(server.reply.Result==PT_COMPOSE_OK&&Player[1].m_ItemList.money==balance&&Item[1].param==param&&Item[2].stack==quantity);
 tick+=500;Init(command);command.Items[1]++;PhongThanSoulComposeCommand(1,&command);assert(server.reply.Result==PT_COMPOSE_STALE&&Item[1].param==(int)0xc17f0012&&Player[1].m_ItemList.money==100000000);
 tick+=500;Init(command);int replies=server.replies;Player[1].m_dwID++;PhongThanSoulComposeCommand(1,&command);assert(server.replies==replies&&Player[1].m_ItemList.money==100000000);
 tick+=500;Init(command);Player[1].trading=true;replies=server.replies;PhongThanSoulComposeCommand(1,&command);assert(server.replies==replies);
 tick+=500;Init(command);tick+=120001;replies=server.replies;PhongThanSoulComposeCommand(1,&command);assert(server.replies==replies);
 tick+=500;Init(command);command.MapId++;replies=server.replies;PhongThanSoulComposeCommand(1,&command);assert(server.replies==replies);
 tick+=500;Init(command);command.Header.Sequence++;PhongThanSoulComposeCommand(1,&command);assert(server.reply.Result==PT_COMPOSE_STALE&&Player[1].m_ItemList.money==100000000);
 puts("PASS production soul service: exact serial/build slot, nonce, duplicate commit no double charge, stale identity/map, expired session, trade lock");return 0;
}
