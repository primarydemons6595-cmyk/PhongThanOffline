#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "../../Headers/PhongThanCharacter.h"
#include "../../Headers/PhongThanRealmQuestText.h"
enum {MAX_PLAYER=4,MAX_NPC=5,MAX_TEAM_MEMBER=1,TASKVALUE_PT_REALM_SIDE=4868,TASKVALUE_PT_REALM_LEVEL=4869,TASKVALUE_PT_TITLE_QUALIFY_BEGIN=4820,UI_NEWSINFO=0,NEWSMESSAGE_NORMAL=0,PHONGTHAN_SCRIPT_SHOW=1};
struct Task{int values[5000];char title[16];int GetSaveVal(int n){return values[n];}void SetSaveVal(int,const char* s,int){strcpy(title,s);}};
struct PlayerType{char AccountName[32],Name[32];Task m_cTask;struct{int m_nFlag,m_nID;}m_cTeam;int m_nIndex;}Player[MAX_PLAYER];
struct NpcType{int m_SubWorldIndex;}Npc[MAX_NPC];
struct Npcs{int GetDistance(int,int){return 1;}}NpcSet;
struct KPhongThanScriptAction{int View,OptionCount,Operation,ServerOwned,ContentLength;char Content[600];};
unsigned long GetTitleQualifyWord(int p,int){return strtoul(Player[p].m_cTask.title,0,16);}
int news=0;void PhongThanBroadcastScriptAction(KPhongThanScriptAction&,bool){++news;}
int PhongThanQuestTeamMember(int,int i){return i==1?1:2;}
void g_StrCpyLen(char* to,const char* from,int count){strncpy(to,from,count-1);to[count-1]=0;}
void g_DebugLog(const char*){}
#include "../../Sources/Core/Src/PhongThanRealmWorld.inl"
int main(int argc,char**argv){
 assert(argc==2&&strstr(argv[1],"realm_equipment_20261027")&&SetCurrentDirectoryA(argv[1]));CreateDirectoryA("CharacterStore",0);DeleteFileA("CharacterStore\\realm_world.dat");
 using namespace PtRealmWorldRuntime;assert(Load()&&state.Phase==0);state.Streak=3;state.Phase=1;assert(Save());
 strcpy(Player[1].AccountName,"same-account");strcpy(Player[1].Name,"RoleA");strcpy(Player[2].AccountName,"same-account");strcpy(Player[2].Name,"RoleB");
 assert(CompleteReputation(1,1)&&state.Reputation==1);assert(CompleteReputation(1,1)&&state.Reputation==1);assert(CompleteReputation(2,1)&&state.Reputation==2);assert(LastReceipt(1)==1&&LastReceipt(2)==1);
 loaded=good=false;receipts.clear();assert(Load()&&state.Reputation==2&&LastReceipt(1)==1);
 assert(MoveFileA(path,"CharacterStore\\realm_world.hold"));assert(CreateDirectoryA(path,0));assert(!CompleteReputation(1,2)&&state.Reputation==2&&LastReceipt(1)==1);assert(RemoveDirectoryA(path));assert(MoveFileA("CharacterStore\\realm_world.hold",path));
 loaded=good=false;receipts.clear();assert(Load()&&state.Reputation==2&&LastReceipt(1)==1);
 state.Phase=2;state.Reputation=1500;assert(Save());Player[1].m_cTask.values[4879]=0;Dragon(0,1,3);assert(state.FinalMask==0);
 Player[1].m_cTask.values[4879]=Player[2].m_cTask.values[4879]=1;Player[1].m_nIndex=1;Player[2].m_nIndex=2;Player[1].m_cTeam.m_nFlag=1;
 Dragon(0,1,3);Dragon(1,1,3);Dragon(2,1,3);assert(state.Phase==3&&news==1&&(receipts[ReceiptIndex(1)].Flags&1)&&(receipts[ReceiptIndex(2)].Flags&1));
 loaded=good=false;receipts.clear();assert(Load()&&state.Phase==3);GrantTitle(1,1);assert(GetTitleQualifyWord(1,0)==2);
 HANDLE file=CreateFileA(path,GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);assert(file!=INVALID_HANDLE_VALUE);SetFilePointer(file,sizeof(FileHeader)-1,0,FILE_BEGIN);DWORD wrote=0;BYTE corrupt=0x7f;assert(WriteFile(file,&corrupt,1,&wrote,0)&&wrote==1);CloseHandle(file);
 loaded=good=false;assert(!Load()&&!PhongThanRealmDestination(1,1073)&&!CompleteReputation(1,2));assert(DeleteFileA(path));
 puts("PASS actual durable world store: atomic write rollback, per-role receipts, duplicate delivery, restart, final registration/team title, checksum fail-closed");return 0;
}
