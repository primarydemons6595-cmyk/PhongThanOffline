
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanStall.h"
#include "../../Headers/PhongThanStallMessages.h"
enum {MAX_PLAYER=8,MAX_NPC=8,MAX_SUBWORLD=2,do_none=0,do_stand=1,do_sit=2,do_death=3,PLAYER_MENU_STATE_NORMAL=0,
 enumMSG_ID_FUNCTION_FOBIDDENED=1,enumMSG_ID_TRADE_BUZY=2,s2c_msgshow=3,s2c_playertradefail=4,s2c_playertradeaction=5,MESSAGE_SYSTEM_ANNOUCE_HEAD=0};
static int syncs=0,requests=0,closes=0;static bool allowed=true;static int priced=1;
struct PLAYER_TRADE_START_COMMAND {BYTE ProtocolType;char Name[32];};
struct PLAYER_TRADE_START_FAIL_SYNC {BYTE ProtocolType;};
struct PLAYER_TRADE_ACTION_SYNC {BYTE ProtocolType;DWORD dwID;BOOL bOpen;};
struct SHOW_MSG_SYNC {BYTE ProtocolType;int m_wMsgID,m_wLength;LPVOID ptr;};
struct Server {void PackDataToClient(int,const void*,int){++requests;}}server;static Server* g_pServer=&server;
struct KNpc {int m_Doing,m_bRideHorse,m_SubWorldIndex,m_RegionIndex;void SendCommand(int c){m_Doing=c;}void SendSyncData(int,BOOL){++syncs;}};static KNpc Npc[MAX_NPC];
struct Shop {int nTrade,nDest;char cName[32];void Release(){nTrade=nDest=0;cName[0]=0;}};
struct KPlayer {int m_nIndex,m_nPlayerIndex,m_nNetConnectIdx,m_dwID,m_bForbidTrade;bool locked,busy;Shop m_PTrade;struct{int m_nState;}m_cMenuState;
 bool CheckTrading(){return busy||m_PTrade.nTrade;}bool GetLockState(){return locked;}int GetTradeCount(){return priced;}void SendSystemMessage(int,const char*){}void TradeStart(BYTE*);void SendTradeCancel();};static KPlayer Player[MAX_PLAYER];
struct {int m_SubWorldIndex;}SubWorld[MAX_SUBWORLD];
struct KLuaScript {int m_LuaState;void SetGlobalName(const char*){}void SafeCallBegin(int*){}void SafeCallEnd(int){}void CallFunction(const char*,int,const char*){}}script;
static void* g_GetScript(const char*){return &script;}static void Lua_PushNumber(int,int){}static int Lua_GetTopIndex(int){return 0;}static int Lua_ValueToNumber(int,int){return 1;}
static const char* SCRIPT_PROTOCOL_FILE="stub",*SCRIPT_PLAYERINDEX="stub",*SCRIPT_PLAYERID="stub",*SCRIPT_SUBWORLDINDEX="stub";
static void g_StrCpyLen(char* dest,const char* text,int size){lstrcpynA(dest,text,size);}
bool PhongThanStallAllowed(int){return allowed;}
void KPlayer::TradeStart(BYTE* pProtocol)
{
    if(!pProtocol || m_nIndex<=0 || m_nIndex>=MAX_NPC)return;
    const PLAYER_TRADE_START_COMMAND* request=(const PLAYER_TRADE_START_COMMAND*)pProtocol;
    if(!memchr(request->Name,0,sizeof(request->Name)))return;
    // Explicit, idempotent requests: a delayed retry must not reopen a closed stall.
    int intent=PTStallIntent(m_PTrade.nTrade!=0,request->Name[0]!=0);
    if(intent==PT_STALL_CLOSE){SendTradeCancel();return;}
    if(intent!=PT_STALL_OPEN||CheckTrading()||GetLockState())return;
    if(!PhongThanStallAllowed(m_nPlayerIndex)){
        SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptStallDenied);return;
    }
	if (m_bForbidTrade)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_FUNCTION_FOBIDDENED;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}
	if (!m_PTrade.nTrade && (
		Npc[m_nIndex].m_bRideHorse || m_cMenuState.m_nState != PLAYER_MENU_STATE_NORMAL))
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_TRADE_BUZY;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		g_pServer->PackDataToClient(m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return;
	}

	KLuaScript* pScript = (KLuaScript*)g_GetScript(SCRIPT_PROTOCOL_FILE);

	if (pScript)
	{
		Lua_PushNumber(pScript->m_LuaState, m_nPlayerIndex);
		pScript->SetGlobalName(SCRIPT_PLAYERINDEX);

		Lua_PushNumber(pScript->m_LuaState, m_dwID);
		pScript->SetGlobalName(SCRIPT_PLAYERID);

		Lua_PushNumber(pScript->m_LuaState, Npc[Player[m_nPlayerIndex].m_nIndex].m_SubWorldIndex);
		pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);

		int nTopIndex = 0;
		int nReturn = 0;
		pScript->SafeCallBegin(&nTopIndex);
		pScript->CallFunction("PermitTrade", 1, "");
		nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
		nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
		pScript->SafeCallEnd(nTopIndex);

		if(!nReturn) return;
	}

	PLAYER_TRADE_START_COMMAND *pInfo = (PLAYER_TRADE_START_COMMAND *)pProtocol;
	Player[m_nPlayerIndex].m_PTrade.nTrade = !Player[m_nPlayerIndex].m_PTrade.nTrade;

	if (m_PTrade.nTrade)
	{
		if (GetTradeCount())
		{
			g_StrCpyLen(m_PTrade.cName,pInfo->Name,sizeof(m_PTrade.cName));
            Npc[m_nIndex].SendCommand(do_none);
            Npc[m_nIndex].SendCommand(do_sit);
            Npc[m_nIndex].SendSyncData(0,TRUE);
		}
		else
		{
			m_PTrade.Release();

			PLAYER_TRADE_START_FAIL_SYNC sync;
			sync.ProtocolType = s2c_playertradefail;
			g_pServer->PackDataToClient(m_nNetConnectIdx, &sync, sizeof(PLAYER_TRADE_START_FAIL_SYNC));
		}
	}
	else
	{
		Player[m_nPlayerIndex].SendTradeCancel();
	}
}
void KPlayer::SendTradeCancel()
{
    const bool wasSelling=m_PTrade.nTrade!=0;
	Player[m_nPlayerIndex].m_PTrade.Release();
    if(wasSelling&&m_nIndex>0&&m_nIndex<MAX_NPC&&Npc[m_nIndex].m_Doing!=do_death)Npc[m_nIndex].SendCommand(do_stand);
	for (int i = 1; i < MAX_PLAYER; i ++)
	{
		if (Player[i].m_PTrade.nDest == m_nPlayerIndex)
		{
			PLAYER_TRADE_ACTION_SYNC	sSync;
			sSync.ProtocolType = s2c_playertradeaction;
			sSync.dwID = 0;
			sSync.bOpen = FALSE;
			if (Player[i].m_nNetConnectIdx != -1)
				g_pServer->PackDataToClient(Player[i].m_nNetConnectIdx, (BYTE*)&sSync, sizeof(PLAYER_TRADE_ACTION_SYNC));
		}
	}
    if(wasSelling&&m_nIndex>0&&m_nIndex<MAX_NPC&&Npc[m_nIndex].m_SubWorldIndex>=0&&Npc[m_nIndex].m_SubWorldIndex<MAX_SUBWORLD&&Npc[m_nIndex].m_RegionIndex>=0)Npc[m_nIndex].SendSyncData(0,TRUE);
}
enum {c2s_playerbuyitem=100,c2s_playereatitem=101,c2s_playersellitem=102,c2s_playerthrowawayitem=103,c2s_playertradebuy=104,c2s_playertradestart=105,c2s_storemoney=106,c2s_tradeapplystart=107,c2s_tradeapplystateopen=108,c2s_withdrawamoney=109};
static bool DispatcherBlocks(int nIndex,BYTE byProtocol){return (Player[nIndex].GetLockState() && 
				(byProtocol == c2s_tradeapplystateopen
				|| byProtocol == c2s_tradeapplystart
				|| byProtocol == c2s_playerbuyitem
				|| byProtocol == c2s_playersellitem
				|| byProtocol == c2s_storemoney
				|| byProtocol == c2s_withdrawamoney // rut tien;
				|| (byProtocol == c2s_playertradestart && !Player[nIndex].m_PTrade.nTrade)
				|| byProtocol == c2s_playertradebuy
				/*|| byProtocol == c2s_playereatitem*/
				|| byProtocol == c2s_playerthrowawayitem)
				);}

enum {UIS_S_IDLE=0,UIS_S_TRADE_SETPRICE=4};
struct Core {bool active;int GetTradeState(){return active?1:0;}}core;static Core* g_pCoreShell=&core;
struct UiBase {int status;int GetStatus(){return status;}void SetStatus(int s){status=s;}}g_UiBase;
struct KUiItem {static void OnNpcTradeMode(bool){}};
class KPtStall {public:static KPtStall* self;int lastRefresh;bool visible;bool Initialize(){lastRefresh=0;visible=false;return true;}void Breathe(){}void Hide(){visible=false;}void Show(){visible=true;}void BringToTop(){}
    void Close() {
        Hide();if(g_UiBase.GetStatus()==UIS_S_TRADE_SETPRICE)g_UiBase.SetStatus(UIS_S_IDLE);
        KUiItem::OnNpcTradeMode(false);
    }
    static void Open() {
        if(!g_pCoreShell)return;
        if(!self){self=new KPtStall;if(!self->Initialize()){delete self;self=0;return;}}
        if(!g_pCoreShell->GetTradeState())g_UiBase.SetStatus(UIS_S_TRADE_SETPRICE);
        KUiItem::OnNpcTradeMode(true);self->lastRefresh=0;self->Breathe();self->Show();self->BringToTop();
    }
};KPtStall* KPtStall::self=0;
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));syncs=requests=0;allowed=true;priced=1;Player[1].m_nIndex=1;Player[1].m_nPlayerIndex=1;Npc[1].m_SubWorldIndex=0;Npc[1].m_RegionIndex=0;}
int main(){
 PLAYER_TRADE_START_COMMAND start,stop;ZeroMemory(&start,sizeof(start));ZeroMemory(&stop,sizeof(stop));strcpy(start.Name,"Shop");
 for(int cycle=0;cycle<100;++cycle){Reset();Player[1].TradeStart((BYTE*)&start);assert(Player[1].m_PTrade.nTrade&&Npc[1].m_Doing==do_sit&&syncs==1);
  Player[1].TradeStart((BYTE*)&start);assert(Player[1].m_PTrade.nTrade&&syncs==1);
  core.active=true;g_UiBase.status=UIS_S_IDLE;KPtStall::Open();assert(KPtStall::self->visible);KPtStall::self->Close();assert(!KPtStall::self->visible&&Player[1].m_PTrade.nTrade);
  assert(PTStallCanOpenControl(true,false,false));KPtStall::Open();assert(KPtStall::self->visible);
  Player[1].locked=true;Player[1].busy=true;allowed=false;Player[1].m_bForbidTrade=1;assert(!DispatcherBlocks(1,c2s_playertradestart));Player[1].TradeStart((BYTE*)&stop);assert(!Player[1].m_PTrade.nTrade&&Npc[1].m_Doing==do_stand&&syncs==2);
  Player[1].TradeStart((BYTE*)&stop);assert(!Player[1].m_PTrade.nTrade&&syncs==2);assert(DispatcherBlocks(1,c2s_playertradestart));assert(DispatcherBlocks(1,c2s_storemoney));
  Player[1].locked=Player[1].busy=false;Player[1].m_bForbidTrade=0;allowed=true;Player[1].TradeStart((BYTE*)&start);assert(Player[1].m_PTrade.nTrade&&syncs==3);Player[1].TradeStart((BYTE*)&stop);assert(!Player[1].m_PTrade.nTrade&&syncs==4);
 }
 Reset();priced=0;Player[1].TradeStart((BYTE*)&start);assert(!Player[1].m_PTrade.nTrade);
 Reset();allowed=false;Player[1].TradeStart((BYTE*)&start);assert(!Player[1].m_PTrade.nTrade);
 assert(!PTStallCanOpenControl(false,false,false));assert(PTStallCanOpenControl(false,true,false));assert(PTStallCanOpenControl(false,false,true));
 puts("PASS 100 production open-hide-reopen-stop-reopen cycles; duplicate requests, locked/busy stop, immediate snapshot sync, empty stock and regular trade/bank lock preservation");return 0;
}
