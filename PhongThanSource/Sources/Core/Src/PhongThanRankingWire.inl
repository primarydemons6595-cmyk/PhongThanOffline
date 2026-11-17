#include "../../../Headers/PhongThanRanking.h"
#ifndef _SERVER
static PTRankingPage ptRankingPage;
static PTRankingQuery ptRankingLast;
static DWORD ptRankingSequence=0,ptRankingRequested=0,ptRankingReceived=0;
int PhongThanRankingQuery(const PTRankingQuery* source){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!source||!g_pClient||a<=0||a>=MAX_NPC||!PTRankingValidQuery(source->Category,source->Page))return 0;
 PTRankingQuery q=*source;
 PhongThanInitializeWireHeader(&q.Header,PT_RANK_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,++ptRankingSequence);
 q.ActorId=Npc[a].m_dwID;q.MapId=SubWorld[0].m_SubWorldID;
 ptRankingLast=q;ptRankingRequested=GetTickCount();ptRankingReceived=0;
 g_pClient->SendPackToServer(&q,sizeof(q));return 1;
}
int PhongThanRankingGet(PTRankingPage* result){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!result||a<=0||a>=MAX_NPC)return 0;
 if(ptRankingLast.ActorId!=Npc[a].m_dwID||ptRankingLast.MapId!=(DWORD)SubWorld[0].m_SubWorldID)return 0;
 if(!ptRankingReceived&&GetTickCount()-ptRankingRequested>2500)PhongThanRankingQuery(&ptRankingLast);
 if(!ptRankingReceived||ptRankingPage.ActorId!=Npc[a].m_dwID||ptRankingPage.Header.Sequence!=ptRankingSequence)return 0;
 *result=ptRankingPage;return 1;
}
static void PhongThanRankingReceive(const PTRankingPage& page){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(a>0&&a<MAX_NPC&&PTRankingValidPage(page)&&page.ActorId==Npc[a].m_dwID&&page.MapId==(DWORD)SubWorld[0].m_SubWorldID&&
 page.Header.Sequence==ptRankingSequence&&page.Category==ptRankingLast.Category&&page.Page==ptRankingLast.Page){ptRankingPage=page;ptRankingReceived=GetTickCount();}
}
#else
static void PhongThanRankingRequest(int p,const PTRankingQuery& q){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||!PTRankingValidQuery(q.Category,q.Page))return;
 int a=Player[p].m_nIndex;if(a<=0||a>=MAX_NPC||Npc[a].m_dwID!=q.ActorId)return;
 int w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD||q.MapId!=(DWORD)SubWorld[w].m_SubWorldID)return;
 static DWORD last[MAX_PLAYER];DWORD now=GetTickCount();if(now-last[p]<200)return;last[p]=now;
 PTRankingPage out;ZeroMemory(&out,sizeof(out));out.ActorId=q.ActorId;out.MapId=q.MapId;out.Category=q.Category;out.Page=q.Page;
 PhongThanInitializeWireHeader(&out.Header,PT_RANK_REPLY,sizeof(out),PHONGTHAN_WIRE_FLAG_RESPONSE,q.Header.Sequence);
 if(!PhongThanRankingPage(q.Category,q.Page,out))out.Result=-1;
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&out,sizeof(out));
}
#endif
