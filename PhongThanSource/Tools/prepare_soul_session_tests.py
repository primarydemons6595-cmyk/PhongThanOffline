"""Production Soul service validation/replay fixture; no live user state."""
from pathlib import Path
import runpy
ROOT=Path(__file__).resolve().parents[2]/'PhongThanSource'
runpy.run_path(str(ROOT/'Tools/prepare_soul_tests.py'))
s=(ROOT/'Tests/Native/SoulRuntimeTests.generated.cpp').read_text('ascii').split('int main(){')[0]
s=s.replace('SAVE_REQUEST=1};','SAVE_REQUEST=1,MAX_PLAYER=2,MAX_NPC=3,MAX_SUBWORLD=2,MAX_ITEM=32,MAX_PLAYER_ITEM=32};')
s=s.replace('int GetGenre()const', 'struct Lock {bool IsLock(){return false;}}lock;Lock* GetLock(){return &lock;}bool IsTemp(){return false;}int GetExpireTime(){return 0;}int GetID(){return serial;}int GetGenre()const')
s=s.replace('struct Bag{','struct Bag{struct Pos {int nPlace,nX;}m_Items[32];int build[9];int GetBuildItem(int s){return build[s];}int FindSame(int n){return owned[n]?n:0;}')
s=s.replace('struct TestPlayer {Bag m_ItemList;int m_uMustSave;', 'struct TestPlayer {Bag m_ItemList;int m_uMustSave,m_nIndex,m_nNetConnectIdx;DWORD m_dwID;bool m_PlayerDBLoad,trading,locked;bool GetLockState(){return locked;}bool CheckTrading(){return trading;}')
s+=r'''
#include "../../Headers/PhongThanPlayerProtocol.h"
struct Actor {DWORD m_dwID;int m_CurrentLife,m_RegionIndex,m_SubWorldIndex;}Npc[MAX_NPC];
struct World {int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
struct FakeServer {PHONGTHAN_COMPOSE_RESULT reply;PHONGTHAN_PLAYER_EVENT event;int replies;
 int SendData(int,const void* data,int size){assert(size==sizeof(event));memcpy(&event,data,size);return 0;}
 void PackDataToClient(int,BYTE* data,int size){assert(size==sizeof(reply));memcpy(&reply,data,size);++replies;}
}server,*g_pServer=&server;
DWORD tick=1000;DWORD SoulTestTick(){return tick;}DWORD KSG_GetCurSec(){return 1000;}
#define GetTickCount SoulTestTick
'''
service=(ROOT/'Sources/Core/Src/PhongThanSoulCompose.inl').read_text('utf8').split('#ifdef _SERVER\nnamespace PtSoulService {')[1]
s+='namespace PtSoulService {'+service.rsplit('#endif',1)[0]
s+=r'''
static void Init(PHONGTHAN_COMPOSE_COMMAND& command){
 Inputs(ptSoulRecipes[0],command.Items?(int*)command.Items:0,command.Quantities);int indexes[9],selected[9];Inputs(ptSoulRecipes[0],indexes,selected);
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
'''
s=s.replace('Inputs(ptSoulRecipes[0],command.Items?(int*)command.Items:0,command.Quantities);','')
(ROOT/'Tests/Native/SoulSessionTests.generated.cpp').write_text(s,'ascii')
