#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum { ACTION_CHAT, ACTION_MAKEFRIEND, ACTION_TRADE, ACTION_JOINTEAM,
    ACTION_INVITETEAM, ACTION_FOLLOW, ACTION_VIEWITEM, ACTION_BLACKLIST,
    ACTION_TONG, ACTION_JOINTONG, ACTION_JOINTONG_REPLY, ACTION_REVENGE, ACTION_TRADE_FOLKGAME, ACTION_CONTEXT_FACE };
enum { UIS_S_IDLE=0, WM_RBUTTONDOWN=10, WM_RBUTTONDBLCLK=11, WM_LBUTTONDOWN=12,
    MK_CONTROL=8, MK_SHIFT=4, VK_MENU=18, relation_none=1, relation_ally=4,
    relation_enemy=8, kind_player=1, PT_PLAYER_TARGET_SELECT=99, MK_RBUTTON=2 };
#define LOWORD(x) ((unsigned short)(x))
#define HIWORD(x) ((unsigned short)((unsigned int)(x)>>16))
struct KUiPlayerItem { char Name[32]; unsigned int uId; int nIndex,nData,nTeamID; };
static int alt=0,opened=0,dispatched=0,lastAction=-1,lastX=0,lastY=0;
static KUiPlayerItem lastPlayer;
int GetKeyState(int) { return alt ? 0x8000 : 0; }
void g_DebugLog(const char*,...) {}
struct UiBase { int status; int GetStatus() { return status; } } g_UiBase;
struct KUiPlayerBar {
    static bool IsSelfName(char* name) { return strcmp(name,"self")==0; }
};
struct Core {
    bool held,found,currentFound;
    int kind,currentKind,finds,resolves;
    KUiPlayerItem target,current;
    bool CheckMouse() { return held; }
    void OperationRequest(int,int,int) {}
    bool FindSelectNPC(int x,int y,int mask,bool select,void* out,int& type,bool named) {
        if(mask!=(relation_none|relation_ally|relation_enemy) || select || !named) abort();
        ++finds; lastX=x; lastY=y; type=kind; *(KUiPlayerItem*)out=target; return found;
    }
    bool FindSpecialNPC(char*,void* out,int& type) {
        ++resolves; type=currentKind; *(KUiPlayerItem*)out=current; return currentFound;
    }
} core,*g_pCoreShell=&core;
void PopUpContextPeopleMenu(const KUiPlayerItem& player,int,int) { ++opened; lastPlayer=player; }
void ProcessPeople(KUiPlayerItem* player,int action) { ++dispatched;lastPlayer=*player;lastAction=action; }
void PhongThanRemotePeopleBegin(const KUiPlayerItem&,int) {}
#include "../../Sources/GameClient/Ui/UiCase/UiPlayerContextCompat.inl"
static int checks=0;
void Check(bool condition,const char* name) {
    ++checks; if(!condition) { printf("FAIL: %s\n",name);exit(1); }
}
void Reset() {
    memset(&core,0,sizeof(core)); g_pCoreShell=&core; alt=opened=dispatched=0;
    g_UiBase.status=0; core.kind=core.currentKind=kind_player;
    core.found=core.currentFound=true;
    strcpy(core.target.Name,"other"); core.target.nIndex=12;core.target.uId=44;
    core.current=core.target;
}
int main() {
    Reset(); Check(PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL,(220<<16)|123)
        && opened==1 && lastX==123 && lastY==220,"Ctrl-right opens once at cursor");
    Reset(); Check(!PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,0,0) && !opened,"plain right skill unchanged");
    Reset(); Check(!PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_RBUTTON,0) && !opened && !core.finds,"real plain right packet bypasses player menu hit testing");
    Reset(); Check(!PhongThanHandlePlayerContextChord(WM_RBUTTONDBLCLK,MK_RBUTTON,0) && !opened,"plain double right remains combat input");
    Reset(); Check(PhongThanHandlePlayerContextChord(WM_RBUTTONDBLCLK,MK_CONTROL|MK_RBUTTON,0) && opened==1,"Ctrl-double-right opens player menu");
    Reset(); Check(!PhongThanHandlePlayerContextChord(WM_LBUTTONDOWN,MK_CONTROL,0) && !opened,"Ctrl-left unchanged");
    Reset(); Check(!PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL|MK_SHIFT,0),"Ctrl-shift binding preserved");
    Reset(); alt=1; Check(!PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL,0),"Alt binding preserved");
    Reset(); core.found=false;Check(PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL,0)&&!opened,"empty ground consumed without combat fallback");
    Reset();core.held=true;Check(PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL,0)&&!opened&&!core.finds,"held item never discarded");
    Reset();g_pCoreShell=0;Check(PhongThanHandlePlayerContextChord(WM_RBUTTONDOWN,MK_CONTROL,0)&&!opened,"null core safe");
    Reset();g_UiBase.status=1;Check(!PhongThanOpenPlayerContextMenu(1,2)&&!core.finds,"modal state respected");
    Reset();core.kind=2;Check(!PhongThanOpenPlayerContextMenu(1,2)&&!opened,"monster not a player menu");
    Reset();strcpy(core.target.Name,"self");Check(!PhongThanOpenPlayerContextMenu(1,2),"self rejected");
    Reset();core.target.uId=0;Check(!PhongThanOpenPlayerContextMenu(1,2),"missing identity rejected");
    Reset();core.target.nIndex=-1;Check(!PhongThanOpenPlayerContextMenu(1,2),"invalid index rejected");
    Reset();memset(core.target.Name,'A',32);PhongThanOpenPlayerContextMenu(1,2);Check(lastPlayer.Name[31]==0,"target name terminated");
    Reset();PhongThanProcessPlayerMenuAction(&core.target,ACTION_VIEWITEM);
    Check(dispatched==1&&lastAction==ACTION_VIEWITEM&&core.resolves==1,"view uses existing operation after identity check");
    Reset();core.current.uId=55;PhongThanProcessPlayerMenuAction(&core.target,ACTION_TRADE);Check(!dispatched,"reused NPC slot cannot trade with wrong player");
    Reset();core.currentFound=false;PhongThanProcessPlayerMenuAction(&core.target,ACTION_FOLLOW);Check(!dispatched,"departed target rejected");
    Reset();core.current.nData=3;PhongThanProcessPlayerMenuAction(&core.target,ACTION_TRADE);Check(dispatched==1&&lastPlayer.nData==3,"current target trade state forwarded");
    Reset();core.currentFound=false;PhongThanProcessPlayerMenuAction(&core.target,ACTION_CHAT);Check(dispatched==1&&!core.resolves,"name-only chat remains usable");
    Reset();PhongThanProcessPlayerMenuAction(&core.target,-1);PhongThanProcessPlayerMenuAction(&core.target,20);Check(!dispatched,"unknown action rejected");
    Reset();PhongThanProcessPlayerMenuAction(&core.target,ACTION_TRADE_FOLKGAME);
    Check(dispatched==1&&core.resolves==1&&lastAction==ACTION_TRADE_FOLKGAME,"RPS checks live target identity");
    Reset();core.held=true;PhongThanProcessPlayerMenuAction(&core.target,ACTION_TRADE);Check(!dispatched,"held item blocks action dispatch");
    Check(strcmp(PhongThanPeopleActionName(ACTION_TRADE),"Giao d\xDE" "ch")==0,"TCVN3 trade label");
    for(int i=ACTION_CHAT;i<=ACTION_CONTEXT_FACE;++i) {
        const char* name=PhongThanPeopleActionName(i);
        Check(name[0]&&strlen(name)<32&&!strchr(name,'?')&&!strstr(name,"\xEF\xBF\xBD"),"valid bounded label without Unicode replacement");
    }
    printf("PASS: %d player context checks\n",checks);return 0;
}
