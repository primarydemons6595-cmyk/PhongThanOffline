#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanBankSecurity.h"
#define _SERVER
#define TRUE 1
enum {MAX_PLAYER=3,MAX_NPC=4,MAX_SUBWORLD=1,TASKVALUE_BASEDATA_PASSWORD=202};
struct Task {char password[16];const char* GetSaveStr(int){return password;}};
struct Bag{int hand;int Hand(){return hand;}};
struct Trade{int nTrade;};struct MoveLock{int bLock;};
struct KPlayer{int m_PlayerDBLoad,m_nIndex,m_nNetConnectIdx,m_nPaceBarTime,trading,locked,saves;DWORD m_dwID;Task m_cTask;Bag m_ItemList;Trade m_PTrade;MoveLock move;
 int CheckTrading(){return trading;}int GetLockState(){return locked;}MoveLock* GetLockMove(){return &move;}
 void SetLockState(int value){locked=value;}void SetSavePw(char* text,int);bool Save(){++saves;return true;}
}Player[MAX_PLAYER];
struct NpcStub{int m_SubWorldIndex;DWORD m_dwID;}Npc[MAX_NPC];struct World{int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
#include "../../Sources/Core/Src/PhongThanBankLockState.inl"
void KPlayer::SetSavePw(char* text,int){strcpy(m_cTask.password,text);PhongThanBankSetLocked(1,true);}
struct SOME_BOX_SYNC{BYTE ProtocolType;BOOL bEquipEx;int nBoxIndex;};enum{s2c_opensomebox=88};
static int opened;
static PTBankSecurityState reply;static int replies,notices;static bool atTreasury=true;static DWORD ticks=10000;
struct Server{void PackDataToClient(int,void* data,int size){if(size==sizeof(SOME_BOX_SYNC))++opened;else if(size==sizeof(reply)){memcpy(&reply,data,size);++replies;}}}server,*g_pServer=&server;
static bool PhongThanBankNearTreasury(int){return atTreasury;}static void PTF4Notice(int,const char*){++notices;}
#define GetTickCount() ticks
#include "../../Sources/Core/Src/PhongThanBankSecurity.inl"
#define CHECK(c) do{if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static PTBankSecurityRequest Query(int action,const char* old="",const char* value="",const char* confirm=""){
 PTBankSecurityRequest q;memset(&q,0,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PT_BANK_SECURITY_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);
 q.MapId=1002;q.EntityId=123;q.Action=action;strcpy(q.Old,old);strcpy(q.Password,value);strcpy(q.Confirm,confirm);ticks+=2000;PTBankSecurityProcess(1,q);return q;
}
int main(){Player[1].m_PlayerDBLoad=1;Player[1].m_nIndex=1;Player[1].m_dwID=99;Npc[1].m_dwID=123;SubWorld[0].m_SubWorldID=1002;
 PTBankSecurityRequest q=Query(0);CHECK(reply.Result==0&&!reply.Configured&&!reply.Locked&&PTValidateBankSecurity(&q,sizeof(q)));
 Query(1,"","abc","wrong");CHECK(reply.Result==-2&&!reply.Configured);
 Query(1,"","abc","abc");CHECK(reply.Result==1&&reply.Configured&&reply.Locked&&Player[1].saves==1);
 Query(1,"","overwrite","overwrite");CHECK(reply.Result==-2&&!strcmp(Player[1].m_cTask.password,"abc"));
 Query(2,"wrong");CHECK(reply.Result==-2&&PhongThanBankLocked(1)&&!Player[1].locked);
 Query(2,"abc");CHECK(!PhongThanBankLocked(1)&&!Player[1].locked&&opened==1);
 Query(2,"wrong");CHECK(reply.Result==-2); // prior successful unlock cannot authorise a bad password
 Query(4,"bad","new","new");CHECK(reply.Result==-2&&!strcmp(Player[1].m_cTask.password,"abc"));
 Query(4,"abc","new","new");CHECK(reply.Result==1&&reply.Locked&&!strcmp(Player[1].m_cTask.password,"new"));
 Query(2,"new");CHECK(!reply.Locked&&!Player[1].locked);
 Query(3);CHECK(reply.Result==1&&reply.Locked&&!Player[1].locked);
 Query(2,"new");CHECK(!PhongThanBankLocked(1));Npc[1].m_dwID=124;CHECK(PhongThanBankLocked(1));Npc[1].m_dwID=123;
 CHECK(!Player[1].locked); // vault operations never mutate character protection
 atTreasury=false;Query(0);CHECK(reply.Result==-1&&notices);atTreasury=true;
 q=Query(0);int before=replies;q.EntityId=124;PTBankSecurityProcess(1,q);CHECK(replies==before);
 q=Query(0);q.Old[15]='x';CHECK(!PTValidateBankSecurity(&q,sizeof(q)));q.Old[15]=0;q.Reserved[1]=1;CHECK(!PTValidateBankSecurity(&q,sizeof(q)));
 puts("PASS vault-only password, close/relogin gate and actual open response; bank password: create/confirm/unlock/change/lock, invalid credentials, no overwrite, real treasury scope, packet bounds");return 0;
}
