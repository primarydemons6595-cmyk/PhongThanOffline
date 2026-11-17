#define PT_REMOTE_TIMEOUT "\x4B\x68\xAB\x6E\x67\x20\x6E\x68\xCB\x6E\x20\xAE\xAD\xEE\x63\x20\x70\x68\xB6\x6E\x20\x68\xE5\x69\x20\x74\xF5\x20\x6D\xB8\x79\x20\x63\x68\xF1\x2E"
#define PT_REMOTE_OFFLINE_VIEW "\x4E\x67\xAD\xEA\x69\x20\x63\x68\xAC\x69\x20\xAE\xB7\x20\x6F\x66\x66\x6C\x69\x6E\x65\x2C\x20\x6B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x78\x65\x6D\x20\x74\x72\x61\x6E\x67\x20\x62\xDE\x2E"
#define PT_REMOTE_OFFLINE_TEAM "\x4E\x67\xAD\xEA\x69\x20\x63\x68\xAC\x69\x20\xAE\xB7\x20\x6F\x66\x66\x6C\x69\x6E\x65\x2C\x20\x6B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x67\x69\x61\x20\x6E\x68\xCB\x70\x20\x68\x6F\xC6\x63\x20\x6D\xEA\x69\x20\x76\xB5\x6F\x20\x74\xE6\x20\xAE\xE9\x69\x2E"
#define PT_REMOTE_OFFLINE "\x4E\x67\xAD\xEA\x69\x20\x63\x68\xAC\x69\x20\xAE\xB7\x20\x6F\x66\x66\x6C\x69\x6E\x65\x2C\x20\x6B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x74\x68\xF9\x63\x20\x68\x69\xD6\x6E\x20\x74\x68\x61\x6F\x20\x74\xB8\x63\x2E"
#define PT_REMOTE_TEAM_FAILED "\x4B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x67\x69\x61\x20\x6E\x68\xCB\x70\x20\x68\x6F\xC6\x63\x20\x6D\xEA\x69\x20\x76\xB5\x6F\x20\x74\xE6\x20\xAE\xE9\x69\x2E\x20\x48\xB7\x79\x20\x6B\x69\xD3\x6D\x20\x74\x72\x61\x20\xAE\xE9\x69\x20\x74\x72\xAD\xEB\x6E\x67\x2C\x20\x63\x68\xE7\x20\x74\x72\xE8\x6E\x67\x20\x76\xB5\x20\x74\x72\xB9\x6E\x67\x20\x74\x68\xB8\x69\x20\x6E\x68\xCB\x6E\x20\x74\x68\xB5\x6E\x68\x20\x76\x69\xAA\x6E\x2E"
#define PT_REMOTE_FAILED "\x4B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x74\x68\xF9\x63\x20\x68\x69\xD6\x6E\x20\x74\x68\x61\x6F\x20\x74\xB8\x63\x20\x6C\xF3\x63\x20\x6E\xB5\x79\x2E"
#define PT_REMOTE_TOO_FAR "\x4E\x67\xAD\xEA\x69\x20\x63\x68\xAC\x69\x20\x6B\x68\xAB\x6E\x67\x20\xEB\x20\x67\xC7\x6E\x2C\x20\x6B\x68\xAB\x6E\x67\x20\x74\x68\xD3\x20\x74\x68\x65\x6F\x20\x73\x61\x75\x2E"
#include "../../../../Headers/PhongThanRemotePeople.h"
static bool s_PtRemotePending=false;
static DWORD s_PtRemoteStarted=0;
static PT_PEOPLE_COMMAND s_PtRemoteCommand;
static void PhongThanRemotePeopleBegin(const KUiPlayerItem& player,int action){
 if(!g_pCoreShell||!PtPeopleActionAllowed(action)||!PtPeopleNameValid(player.Name))return;
 PT_PEOPLE_COMMAND q;ZeroMemory(&q,sizeof(q));q.Action=action;q.TargetId=player.uId;lstrcpyn(q.Name,player.Name,32);
 if(g_pCoreShell->TeamOperation(PT_PEOPLE_ACTION,(unsigned int)&q,sizeof(q))){s_PtRemoteCommand=q;s_PtRemoteStarted=GetTickCount();s_PtRemotePending=true;}
}
void PhongThanStartWhisper(const char* name){
 if(!PtPeopleNameValid(name)||KUiPlayerBar::IsSelfName((char*)name))return;
 KUiPlayerItem player;ZeroMemory(&player,sizeof(player));lstrcpyn(player.Name,name,32);PhongThanRemotePeopleBegin(player,ACTION_CHAT);
}
void PhongThanOpenRemotePeopleMenu(const char* name,int x,int y){
 if(!g_pCoreShell||!PtPeopleNameValid(name)||KUiPlayerBar::IsSelfName((char*)name)||g_UiBase.GetStatus()!=UIS_S_IDLE||g_pCoreShell->CheckMouse())return;
 KUiPlayerItem player;ZeroMemory(&player,sizeof(player));lstrcpyn(player.Name,name,32);player.nIndex=-2;player.nTeamID=-1;
 PopUpContextPeopleMenu(player,x,y);
}
static void PhongThanRemotePeopleTick(){
 if(!s_PtRemotePending||!g_pCoreShell)return;
 PT_PEOPLE_RESULT r;
 if(!g_pCoreShell->TeamOperation(PT_PEOPLE_GET,(unsigned int)&r,sizeof(r))){
  if(GetTickCount()-s_PtRemoteStarted>5000){s_PtRemotePending=false;UIMessageBox(PT_REMOTE_TIMEOUT);}
  return;
 }
 if(r.Action!=s_PtRemoteCommand.Action||strcmp(r.Name,s_PtRemoteCommand.Name))return;
 s_PtRemotePending=false;
 if(!r.Result){UIMessageBox(r.Action==6?PT_REMOTE_OFFLINE_VIEW:(r.Action==3||r.Action==4)?PT_REMOTE_OFFLINE_TEAM:PT_REMOTE_OFFLINE);return;}
 if(r.Result<0){UIMessageBox((r.Action==3||r.Action==4)?PT_REMOTE_TEAM_FAILED:PT_REMOTE_FAILED);return;}
 if(r.Result==2)return; // Server already performed view/join/invite.
 KUiPlayerItem player;ZeroMemory(&player,sizeof(player));lstrcpyn(player.Name,r.Name,32);player.uId=r.TargetId;player.nIndex=-1;player.nData=r.MenuState;player.nTeamID=r.TeamId;
 if(r.Action==ACTION_FOLLOW){
  int kind=-1;KUiPlayerItem nearPlayer;ZeroMemory(&nearPlayer,sizeof(nearPlayer));
  if(!g_pCoreShell->FindSpecialNPC(r.Name,&nearPlayer,kind)||kind!=kind_player||nearPlayer.nIndex<=0||nearPlayer.uId!=r.TargetId){UIMessageBox(PT_REMOTE_TOO_FAR);return;}
  player=nearPlayer;
 }
 ProcessPeople(&player,r.Action);
}
