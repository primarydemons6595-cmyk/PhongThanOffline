#define main PublicChatOriginalMain
#include "PublicChatTests.cpp"
#undef main
#include "KNpc.h"
#include "KIniFile.h"
#include "Text.h"
#undef MAX_SUBWORLD
#define MAX_SUBWORLD 8
struct TestWorld {int m_SubWorldID;} TestSubWorld[MAX_SUBWORLD];
#define SubWorld TestSubWorld
#define KNpc TestNpc
#define GetTickCount TestTick
#include "../../Sources/Core/Src/PhongThanPvpNotice.inl"
#undef GetTickCount
#undef KNpc
int main(){
 Setup();now=1000000;
 for(int p=1;p<=3;++p){Player[p].m_PlayerDBLoad=true;Player[p].m_cTong.m_nFlag=1;Player[p].m_cTong.m_dwTongNameID=p==2?200:100;Npc[p].m_nPlayerIdx=p;Npc[p].m_FightMode=1;}
 CHECK(Query("\\O100")==PT_CHAT_TONG);CHECK(Query("\\O200")==0);CHECK(Query("\\O")==PT_CHAT_NATION);
 for(int member=1;member<=3;++member)CHECK(Subscribe(member,PT_CHAT_TONG,true));
 server.Clear();CHECK(Say(1,PT_CHAT_TONG));CHECK(server.count==2&&server.connections[0]==1&&server.connections[1]==3);
 Player[3].m_cTong.m_nFlag=0;now+=10000;server.Clear();CHECK(Say(1,PT_CHAT_TONG));CHECK(server.count==1);Player[3].m_cTong.m_nFlag=1;
 Npc[2].m_SubWorldIndex=1;SubWorld[1].m_SubWorldID=1001;
 CHECK(Subscribe(1,PT_CHAT_AREA,true));CHECK(Subscribe(2,PT_CHAT_AREA,true));
 server.Clear();PhongThanPvpNotice(1,2);CHECK(server.count==5);
 int guild=0,area=0;
 for(int i=0;i<server.count;++i){CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(server.packets[i]+sizeof(tagExtendProtoHeader));CHECK(msg->sentlen>0&&msg->sentlen<=255);if(msg->channelid==PT_CHAT_TONG)++guild;else if(msg->channelid==PT_CHAT_AREA)++area;else CHECK(false);}
 CHECK(guild==3&&area==2);server.Clear();PhongThanPvpNotice(1,2);CHECK(server.count==0);
 now+=3001;Player[1].m_cPK.aim=2;PhongThanPvpNotice(1,2);CHECK(server.count==0);Player[1].m_cPK.aim=0;
 Player[2].m_cPK.spar=1;PhongThanPvpNotice(1,2);CHECK(server.count==0);Player[2].m_cPK.spar=0;
 Npc[1].m_nCurPKPunishState=enumDEATH_MODE_PKBATTLE_PUNISH;PhongThanPvpNotice(1,2);CHECK(server.count==0);Npc[1].m_nCurPKPunishState=0;
 Player[2].m_cTong.m_dwTongNameID=100;PhongThanPvpNotice(1,2);CHECK(server.count==5);server.Clear();now+=3001;Player[2].m_cTong.m_dwTongNameID=200;
 Player[2].m_bIsBot=true;PhongThanPvpNotice(1,2);CHECK(server.count==0);Player[2].m_bIsBot=false;
 Player[3].m_cTong.m_dwTongNameID=300;PhongThanPvpNotice(1,2);CHECK(server.count==4);
 // Notifications are hooked only in authoritative player death, not damage.
 puts("PASS guild chat identity/membership, guild+local death packet routing, cooldown, shared-guild deduplication, duel/battle/bot exclusions");return 0;
}
