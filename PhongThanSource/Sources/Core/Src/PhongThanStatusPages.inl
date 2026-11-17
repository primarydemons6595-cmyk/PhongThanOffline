#include "../../../Headers/PhongThanStatusPages.h"
#include "PhongThanLifeRuntime.h"
#include "../../../Headers/PhongThanSoulState.h"
#ifndef _SERVER
static PT_STATUS_PAGES_STATE ptStatusPages;
static DWORD ptStatusPagesRequested=0,ptStatusPagesReceived=0;
static DWORD ptStatusPagesSequence=0,ptStatusPagesPending=0;
static PT_STATUS_PAGES_REQUEST ptStatusPagesLastAction;
int PhongThanPetAttributeBonus(int player,int index){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(player!=CLIENT_PLAYER_INDEX||actor<=0||actor>=MAX_NPC||ptStatusPages.ActorId!=Npc[actor].m_dwID||ptStatusPages.MapId!=(unsigned int)SubWorld[0].m_SubWorldID)return 0;
 return PtPetAttributeBonus(ptStatusPages.Pet,index);
}
int PhongThanPetSkillBonus(int player,int skill){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(player!=CLIENT_PLAYER_INDEX||actor<=0||actor>=MAX_NPC||ptStatusPages.ActorId!=Npc[actor].m_dwID||ptStatusPages.MapId!=(unsigned int)SubWorld[0].m_SubWorldID)return 0;
 return PtPetSkillBonus(ptStatusPages.Pet,skill);
}
int PhongThanStatusPagesAction(const PT_STATUS_PAGES_REQUEST* source){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC||!g_pClient)return 0;
 PT_STATUS_PAGES_REQUEST q;ZeroMemory(&q,sizeof(q));if(source)q=*source;
 bool retry=source==&ptStatusPagesLastAction&&ptStatusPagesPending;
 if(retry&&(q.ActorId!=Npc[actor].m_dwID||q.MapId!=(unsigned int)SubWorld[0].m_SubWorldID)){ZeroMemory(&q,sizeof(q));retry=false;ptStatusPagesPending=0;}
 PhongThanInitializeWireHeader(&q.Header,PT_STATUS_PAGES_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,retry?ptStatusPagesPending:++ptStatusPagesSequence);
 q.ActorId=Npc[actor].m_dwID;q.MapId=SubWorld[0].m_SubWorldID;
 if(q.Operation!=PT_STATUS_READ){ptStatusPagesPending=q.Header.Sequence;ptStatusPagesLastAction=q;}
 g_pClient->SendPackToServer(&q,sizeof(q));ptStatusPagesRequested=GetTickCount();return 1;
}
int PhongThanGetStatusPages(PT_STATUS_PAGES_STATE* state){
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC)return 0;
 if(GetTickCount()-ptStatusPagesRequested>2000)PhongThanStatusPagesAction(ptStatusPagesPending?&ptStatusPagesLastAction:0);
 if(ptStatusPagesPending)return 0;
 if(ptStatusPages.ActorId!=Npc[actor].m_dwID||ptStatusPages.MapId!=(unsigned int)SubWorld[0].m_SubWorldID||GetTickCount()-ptStatusPagesReceived>6000)return 0;
 if(state)*state=ptStatusPages;return 1;
}
#else
#include "../../../Headers/PhongThanAccountStorage.h"
#include "PhongThanSummon.h"
struct PT_PROFILE_FILE {DWORD Magic,Version;char Account[32],Role[32];PT_PERSONAL_PROFILE Profile;DWORD Checksum;};
static DWORD PtProfileHash(const void* bytes,int count){const BYTE*p=(const BYTE*)bytes;DWORD h=2166136261U;for(int i=0;i<count;++i)h=(h^p[i])*16777619U;return h;}
static bool PtProfilePath(int player,std::string& path){
 std::string directory,name;if(!PtAccountDirectory("CharacterStore",Player[player].AccountName,directory,true)||!PtRoleFileName(Player[player].Name,name))return false;
 path=directory+"\\"+name+".profile";return path.size()<MAX_PATH-5;
}
static bool PtProfileRead(int player,PT_PERSONAL_PROFILE& profile){
 ZeroMemory(&profile,sizeof(profile));std::string path;if(!PtProfilePath(player,path))return false;
 HANDLE f=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(f==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_NOT_FOUND;
 PT_PROFILE_FILE data;DWORD got=0;bool ok=GetFileSize(f,0)==sizeof(data)&&ReadFile(f,&data,sizeof(data),&got,0)&&got==sizeof(data);CloseHandle(f);
 if(!ok||data.Magic!=0x46525054||data.Version!=1||data.Account[31]||data.Role[31]||
    _stricmp(data.Account,Player[player].AccountName)||_stricmp(data.Role,Player[player].Name)||
    data.Checksum!=PtProfileHash(&data,sizeof(data)-4)||!PtStatusValidProfile(data.Profile))return false;
 profile=data.Profile;return true;
}
bool PhongThanReadPublicProfile(int player,PT_PERSONAL_PROFILE* out){
 if(!out||player<=0||player>=MAX_PLAYER)return false;return PtProfileRead(player,*out);
}
static bool PtProfileWrite(int player,const PT_PERSONAL_PROFILE& profile){
 if(!PtStatusValidProfile(profile))return false;std::string path;if(!PtProfilePath(player,path))return false;
 PT_PROFILE_FILE data;ZeroMemory(&data,sizeof(data));data.Magic=0x46525054;data.Version=1;
 lstrcpyn(data.Account,Player[player].AccountName,32);lstrcpyn(data.Role,Player[player].Name,32);data.Profile=profile;
 data.Checksum=PtProfileHash(&data,sizeof(data)-4);std::string temp=path+".tmp";
 HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
 DWORD wrote=0;bool ok=WriteFile(f,&data,sizeof(data),&wrote,0)&&wrote==sizeof(data)&&FlushFileBuffers(f);CloseHandle(f);
 if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
static void PhongThanStatusPagesRequest(int player,const PT_STATUS_PAGES_REQUEST& q){
 if(player<=0||player>=MAX_PLAYER)return;int actor=Player[player].m_nIndex;
 if(actor<=0||actor>=MAX_NPC||Npc[actor].m_dwID!=q.ActorId||!Player[player].m_PlayerDBLoad)return;
 int world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD||q.MapId!=(unsigned int)SubWorld[world].m_SubWorldID)return;
 static PT_STATUS_PAGES_STATE applied[MAX_PLAYER];static DWORD appliedOwner[MAX_PLAYER];
 if(q.Operation!=PT_STATUS_READ&&q.Header.Sequence&&appliedOwner[player]==Player[player].m_dwID&&
    applied[player].ActorId==q.ActorId&&applied[player].MapId==q.MapId&&applied[player].Header.Sequence==q.Header.Sequence){
  g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx,(BYTE*)&applied[player],sizeof(applied[player]));return;
 }
 static DWORD last[MAX_PLAYER];DWORD now=GetTickCount();if(now-last[player]<250)return;last[player]=now;
 PT_STATUS_PAGES_STATE state;ZeroMemory(&state,sizeof(state));state.ActorId=q.ActorId;state.MapId=q.MapId;
 PhongThanInitializeWireHeader(&state.Header,PT_STATUS_PAGES_RESPONSE,sizeof(state),PHONGTHAN_WIRE_FLAG_RESPONSE,q.Header.Sequence);
 if(!PtProfileRead(player,state.Profile))state.Result=-2;
 for(int word=0;word<16;++word){const char* text=Player[player].m_cTask.GetSaveStr(TASKVALUE_PT_TITLE_QUALIFY_BEGIN+word);state.Titles[word]=text?strtoul(text,0,16):0;}
 state.CurrentTitle=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CURRENT_TITLE);
 state.PetResult=PhongThanPetRead(player,&state.Pet)?1:-2;
 if(q.Operation!=PT_STATUS_READ){
  if(Player[player].CheckTrading()||Player[player].GetLockState())state.Result=-3;
  else if(q.Operation==PT_STATUS_CANCEL_ITEM_BUFF){
   extern bool PhongThanCancelItemBuff(int,int);state.Result=PhongThanCancelItemBuff(player,q.Value)?1:-1;
  }else if(q.Operation==PT_STATUS_SAVE_PROFILE){
   if(state.Result==-2||!PtProfileWrite(player,q.Profile))state.Result=-2;
   else{state.Profile=q.Profile;state.Result=1;}
  }else if(q.Operation==PT_STATUS_SELECT_TITLE||q.Operation==PT_STATUS_HIDE_TITLE){
   int title=q.Operation==PT_STATUS_HIDE_TITLE?0:q.Value;
   if(!PtStatusHasTitle(state.Titles,title))state.Result=-4;
   else{Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_CURRENT_TITLE,title,TRUE);Npc[actor].SetRank(title);state.CurrentTitle=title;state.Result=1;}
  }else if(q.Operation==PT_STATUS_LIFE_OPEN){state.Result=PhongThanLifeOpenMenu(player,q.Value)?1:-1;
  }else if(q.Operation==PT_STATUS_SOUL_OPEN){state.Result=q.Value==8?PhongThanSoulOpen(player,8):0;
  }else if(q.Operation==PT_STATUS_LIFE_MAKE){state.Result=PhongThanLifeMake(player,q.Value)?1:-1;
  }else if(q.Operation==PT_STATUS_SUMMON_LOCK){state.Result=PhongThanSummonSetLock(actor,q.Value)?1:-1;
  }else if(((q.Operation>=PT_PET_RENAME&&q.Operation<=PT_PET_SALE_OPTIONS)||q.Operation==PT_PET_PROFILE_OPTIONS)&&PtStatusSafeText(q.Profile.Memo,sizeof(q.Profile.Memo))){
   state.Result=PhongThanPetAction(player,q.Operation,q.Value,q.Profile.Memo);
   state.PetResult=PhongThanPetRead(player,&state.Pet)?1:-2;
  }else state.Result=-1;
 }
 state.SummonLocked=PhongThanSummonGetLock(actor);
 if(q.Operation!=PT_STATUS_READ&&q.Header.Sequence){applied[player]=state;appliedOwner[player]=Player[player].m_dwID;}
 g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx,(BYTE*)&state,sizeof(state));
}
#endif
