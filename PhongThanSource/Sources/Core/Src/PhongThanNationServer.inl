#include "PhongThanNation.h"
#include "PhongThanTerritory.h"
#include "PhongThanNationStore.h"
static PTNationStore s_PtNation;
static const char* s_PtNationFile=".\\CharacterStore\\nations.dat";
static unsigned int s_PtNationRights[6];
static bool s_PtNationRightsReady=false;
struct PTNationSession {BYTE Ticket[PHONGTHAN_SESSION_TICKET_SIZE];DWORD LastRequest,LastMutation;};
struct PTNationSync {BYTE Ticket[PHONGTHAN_SESSION_TICKET_SIZE];DWORD PlayerId,Revision;bool Valid;};
static PTNationSync s_PtNationSync[MAX_PLAYER];
static PTNationSession s_PtNationSessions[MAX_PLAYER];
static bool PTNationLive(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&(Player[p].m_nNetConnectIdx>=0||(Player[p].m_bIsBot&&Player[p].m_PlayerDBLoad))&&
    Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&!Player[p].m_bExchangeServer;}
static bool PTNationReady(){
    if(!s_PtNation.Load(s_PtNationFile))return false;
    if(s_PtNationRightsReady)return true;
    KTabFile roles;if(!roles.Load("\\settings\\tongsetting\\tongdutys.txt"))return false;
    memset(s_PtNationRights,0,sizeof(s_PtNationRights));
    int found=0;unsigned int seen=0;for(int row=2;row<=roles.GetHeight();++row){int id=0;char rights[128];
        roles.GetInteger(row,"DutyID",0,&id);roles.GetString(row,"RightSet","",rights,sizeof(rights));
        if(id<1||id>5||(seen&(1u<<id)))return false;seen|=1u<<id;
        for(char* s=rights;*s;){int value=atoi(s);if(value<1||value>18)return false;s_PtNationRights[id]|=1u<<(value-1);
            char* next=strchr(s,'&');if(!next)break;s=next+1;}
        ++found;
    }s_PtNationRightsReady=found==5;return s_PtNationRightsReady;
}
static const char* PTNationPlayerName(int p){return Npc[Player[p].m_nIndex].Name;}
static int PTNationSelf(int p){return s_PtNation.Member(PTNationPlayerName(p));}
static int PTNationOnline(const char* name){for(int p=1;p<MAX_PLAYER;++p)if(PTNationLive(p)&&!strcmp(PTNationPlayerName(p),name))return p;return 0;}
static bool PTNationRight(int member,int right){return member>=0&&s_PtNation.State.Members[member].Duty>=1&&
    s_PtNation.State.Members[member].Duty<=5&&(s_PtNationRights[s_PtNation.State.Members[member].Duty]&(1u<<(right-1)));}
static bool PTNationNearFounder(int p){
    int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex,x=0,y=0;
    if(world<0||world>=MAX_SUBWORLD||SubWorld[world].m_SubWorldID!=1001)return false;
    Npc[actor].GetMpsPos(&x,&y);return abs(x-52352)<=800&&abs(y-102656)<=800;
}
static int PTNationCount(unsigned int id){int n=0;for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m)if(s_PtNation.State.Members[m].NationId==id&&s_PtNation.State.Members[m].Duty>0)++n;return n;}
// The cumulative journal is committed with nation state before touching cash.
// Its applied counter and money share one serialized character snapshot, so a
// crash before/after the character save replays the difference exactly once.
static __int64 PTNationApplied(int p){return (__int64)(unsigned int)Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_NATION_WALLET_LOW)|
    ((__int64)Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_NATION_WALLET_HIGH)<<32);}
static bool PTNationRecoverWallet(int p){
    // Never reconcile cash/tasks before the complete role snapshot is loaded.
    if(Player[p].m_pStatusLoadPlayerInfo)return false;
    int w=s_PtNation.Wallet(PTNationPlayerName(p));__int64 total=w<0?0:s_PtNation.State.Wallets[w].Total;
    __int64 delta=total-PTNationApplied(p);if(!delta)return true;
    __int64 cash=Player[p].m_ItemList.GetEquipmentMoney();
    if(delta< -cash||delta>2000000000-cash)return false;
    bool ok=delta<0?Player[p].m_ItemList.DecMoney((int)-delta)!=0:Player[p].m_ItemList.AddMoney(room_equipment,(int)delta)!=0;
    if(!ok)return false;
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_NATION_WALLET_LOW,(int)(unsigned int)total,TRUE);
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_NATION_WALLET_HIGH,(int)(total>>32),TRUE);
    return Player[p].Save()!=0;
}
void PhongThanNationSyncPlayer(int p){
    if(!PTNationLive(p)||Player[p].m_pStatusLoadPlayerInfo||!PTNationReady())return;
    PTNationSync& cache=s_PtNationSync[p];
    if(cache.Valid&&cache.PlayerId==Player[p].m_dwID&&cache.Revision==s_PtNation.State.Revision&&
       !memcmp(cache.Ticket,Player[p].m_SessionTicket,sizeof(cache.Ticket)))return;
    bool fresh=!cache.Valid||cache.PlayerId!=Player[p].m_dwID||memcmp(cache.Ticket,Player[p].m_SessionTicket,sizeof(cache.Ticket));
    if(fresh&&!PhongThanTerritoryFinishDonation(p,true)){
        int member=PTNationSelf(p);if(member>=0&&s_PtNation.State.Contributions[member].PendingAfter)return;
    }
    if(!PTNationRecoverWallet(p))return;
    int member=PTNationSelf(p),nation=member>=0?s_PtNation.Nation(s_PtNation.State.Members[member].NationId):-1;
    KPlayerTong& presentation=Player[p].m_cTong;int actor=Player[p].m_nIndex;
    bool joined=member>=0&&nation>=0&&s_PtNation.State.Members[member].Duty>0;
    // These existing fields are the character/snapshot ABI only. No old guild methods.
    presentation.m_nFlag=joined?1:0;presentation.m_nFigure=joined?s_PtNation.State.Members[member].Duty:0;
    presentation.m_dwTongNameID=joined?s_PtNation.State.Nations[nation].Id:0;
    presentation.m_szName[0]=0;presentation.m_szMasterName[0]=0;
    presentation.m_szAgname[0]=0;presentation.m_nTongNationalEmblem=0;
    presentation.m_dwMoney=joined?s_PtNation.State.Nations[nation].Money:0;
    presentation.m_dwMemberNum=joined?PTNationCount(presentation.m_dwTongNameID):0;
    presentation.m_btLevel=joined?(s_PtNation.State.Development[nation].CityLevel?s_PtNation.State.Development[nation].CityLevel:1):0;
    cache.Valid=true;cache.PlayerId=Player[p].m_dwID;cache.Revision=s_PtNation.State.Revision;
    memcpy(cache.Ticket,Player[p].m_SessionTicket,sizeof(cache.Ticket));
    if(joined){lstrcpyn(presentation.m_szName,s_PtNation.State.Nations[nation].Name,sizeof(presentation.m_szName));
        lstrcpyn(presentation.m_szMasterName,s_PtNation.State.Nations[nation].Leader,sizeof(presentation.m_szMasterName));}
}
int PhongThanNationDuty(int p){if(!PTNationLive(p)||!PTNationReady())return 0;int m=PTNationSelf(p);return m<0?0:s_PtNation.State.Members[m].Duty;}
int PhongThanNationPending(int p){if(!PTNationLive(p)||!PTNationReady())return 0;int m=PTNationSelf(p);return m>=0&&!s_PtNation.State.Members[m].Duty;}
#include "PhongThanTerritoryCore.inl"
#include "PhongThanTerritoryAdmin.inl"
static void PTNationSend(int p,const PTNationRequest& q,int result,const char* message){
    if(!PTNationLive(p)||Player[p].m_bIsBot)return;
    PTNationResponse out;memset(&out,0,sizeof(out));out.Family=PT_NATION_FAMILY;out.Protocol=2;out.Version=1;
    out.Operation=q.Operation;out.RequestId=q.RequestId;out.Result=result;out.Page=q.Page;
    lstrcpyn(out.Message,message,sizeof(out.Message));
    bool ready=PTNationReady();
    int own=ready?PTNationSelf(p):-1;if(own>=0){out.SelfNationId=s_PtNation.State.Members[own].NationId;out.Duty=s_PtNation.State.Members[own].Duty;out.Rights=s_PtNationRights[out.Duty];}
    unsigned int id=q.NationId?q.NationId:out.SelfNationId;int n=ready?s_PtNation.Nation(id):-1;
    if(n>=0){const PTNationRecord& country=s_PtNation.State.Nations[n];out.NationId=country.Id;out.Money=country.Money;out.MemberCount=PTNationCount(country.Id);
        lstrcpyn(out.Name,country.Name,sizeof(out.Name));lstrcpyn(out.Leader,country.Leader,sizeof(out.Leader));lstrcpyn(out.Notice,country.Notice,sizeof(out.Notice));}
    bool list=q.Operation==PT_NATION_LIST;int begin=q.Page*PT_NATION_ROWS;
    // Applications are visible only to the same nation's recruiters.
    bool applications=own>=0&&out.SelfNationId==id&&PTNationRight(own,5);
    int total=0;
    for(int i=0;ready&&i<(list?PT_NATION_CAPACITY:PT_NATION_MEMBER_CAPACITY);++i){
        PTNationRow row;memset(&row,0,sizeof(row));
        if(list){const PTNationRecord& c=s_PtNation.State.Nations[i];if(!c.Id)continue;
            row.Id=c.Id;row.Level=(unsigned short)PTNationCount(c.Id);lstrcpyn(row.Name,c.Name,sizeof(row.Name));lstrcpyn(row.Leader,c.Leader,sizeof(row.Leader));}
        else {const PTNationMember& m=s_PtNation.State.Members[i];if(!id||m.NationId!=id||(!m.Duty&&!applications))continue;
            if(q.Value==-1&&m.Duty)continue;
            if(q.Value==1&&m.Duty!=PT_NATION_LORD&&m.Duty!=PT_NATION_DEPUTY)continue;
            if(q.Value==2&&m.Duty!=PT_NATION_MINISTER)continue;
            if(q.Value==4&&(m.Duty==0||m.Duty<PT_NATION_CITIZEN))continue;
            row.Id=m.NationId;row.Duty=(BYTE)m.Duty;lstrcpyn(row.Name,m.Name,sizeof(row.Name));int online=PTNationOnline(m.Name);
            row.Online=online?1:0;row.Level=online?(unsigned short)Npc[Player[online].m_nIndex].m_Level:0;}
        if(total>=begin&&out.Count<PT_NATION_ROWS)out.Rows[out.Count++]=row;++total;
    }out.Total=total;
    char packet[sizeof(tagExtendProtoHeader)+sizeof(out)];tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;
    memset(h,0,sizeof(*h));h->ProtocolType=s2c_extend;h->wLength=sizeof(packet)-1;memcpy(packet+sizeof(*h),&out,sizeof(out));
    g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,packet,sizeof(packet));
}
static int PTNationMutateBase(int p,const PTNationRequest& q,const char** error){
    *error="Kh\253ng th\323 th\371c hi\326n thao t\270c n\265y.";
    const char* me=PTNationPlayerName(p);int member=PTNationSelf(p);
    unsigned int own=member>=0?s_PtNation.State.Members[member].NationId:0;
    int nation=s_PtNation.Nation(own),target=q.Target[0]?s_PtNation.Member(q.Target):-1;
    int duty=member>=0?s_PtNation.State.Members[member].Duty:0;
    if(q.Operation==PT_NATION_CREATE){
        if(member>=0){*error="B\271n \256\267 c\343 qu\350c gia ho\306c \256ang xin gia nh\313p.";return 0;}
        if(!PTNationNearFounder(p)){*error="H\267y \256\325n g\306p C\254 X\255\254ng t\271i Phong Th\307n \247\265i \256\323 ki\325n l\313p.";return 0;}
        if(Npc[Player[p].m_nIndex].m_Level<PT_NATION_CREATE_LEVEL){*error="C\307n \256\271t c\312p 60 \256\323 ki\325n l\313p qu\350c gia.";return 0;}
        int length=strlen(q.Text);if(length<2||length>16||q.Text[0]==' '||q.Text[length-1]==' '||strchr(q.Text,'<')||strchr(q.Text,'>')){*error="T\252n qu\350c gia ph\266i t\365 2 \256\325n 16 k\375 t\371, kh\253ng c\343 m\267 m\265u.";return 0;}
        for(int i=0;i<PT_NATION_CAPACITY;++i)if(s_PtNation.State.Nations[i].Id&&!lstrcmpi(s_PtNation.State.Nations[i].Name,q.Text)){*error="T\252n qu\350c gia n\265y \256\267 \256\255\356c s\366 d\364ng.";return 0;}
        int country=s_PtNation.FreeNation(),slot=s_PtNation.FreeMember();if(country<0||slot<0||s_PtNation.State.NextId==0x7fffffff)return 0;
        int money=GetPrivateProfileInt("MoneyREQ","Money",500000,".\\settings\\tongsetting\\createtong.ini");
        int credit=GetPrivateProfileInt("BaseREQ","CreatorCredit",500,".\\settings\\tongsetting\\createtong.ini");
        if(money<0||credit<0||Player[p].m_ItemList.GetEquipmentMoney()<money||Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)<credit){*error="Ch\255a \256\361 500 danh v\344ng v\265 50 v\271n \256\323 ki\325n l\313p qu\350c gia.";return 0;}
        PTNationRecord& c=s_PtNation.State.Nations[country];memset(&c,0,sizeof(c));c.Id=s_PtNation.State.NextId++;
        lstrcpyn(c.Name,q.Text,sizeof(c.Name));lstrcpyn(c.Leader,me,sizeof(c.Leader));
        PTNationMember& m=s_PtNation.State.Members[slot];memset(&m,0,sizeof(m));m.NationId=c.Id;m.Duty=PT_NATION_LORD;lstrcpyn(m.Name,me,sizeof(m.Name));return money+1;
    }
    if(q.Operation==PT_NATION_APPLY){
        if(member>=0){*error="B\271n \256\267 c\343 qu\350c gia ho\306c \256ang xin gia nh\313p.";return 0;}
        // F10 admission works remotely; creation alone requires the founder NPC.
        if(Npc[Player[p].m_nIndex].m_Level<PT_NATION_JOIN_LEVEL){*error="C\307n \256\271t c\312p 12 \256\323 xin gia nh\313p qu\350c gia.";return 0;}
        if(s_PtNation.Nation(q.NationId)<0)return 0;int free=s_PtNation.FreeMember();if(free<0)return 0;
        PTNationMember& application=s_PtNation.State.Members[free];memset(&application,0,sizeof(application));application.NationId=q.NationId;lstrcpyn(application.Name,me,sizeof(application.Name));return -1;
    }
    if(member<0||nation<0||q.NationId!=own)return 0;
    if(q.Operation==PT_NATION_CANCEL){if(duty)return 0;memset(&s_PtNation.State.Members[member],0,sizeof(PTNationMember));return -1;}
    if(q.Operation==PT_NATION_LEAVE){if(!duty||duty==PT_NATION_LORD){*error="L\267nh ch\363a c\307n nh\255\352ng quy\322n ho\306c gi\266i t\270n qu\350c gia tr\255\355c.";return 0;}memset(&s_PtNation.State.Members[member],0,sizeof(PTNationMember));return -1;}
    if(q.Operation==PT_NATION_DISSOLVE){if(!PTNationRight(member,1))return 0;
        for(int a=0;a<PT_NATION_MEMBER_CAPACITY;++a)if(s_PtNation.State.Members[a].NationId==own)memset(&s_PtNation.State.Members[a],0,sizeof(PTNationMember));
        memset(&s_PtNation.State.Nations[nation],0,sizeof(PTNationRecord));return -1;}
    if(q.Operation==PT_NATION_ANNOUNCE){if(!PTNationRight(member,3))return 0;lstrcpyn(s_PtNation.State.Nations[nation].Notice,q.Text,256);return -1;}
    if(q.Operation==PT_NATION_DEPOSIT||q.Operation==PT_NATION_WITHDRAW){
        int amount=q.Value;PTNationRecord& c=s_PtNation.State.Nations[nation];
        if(amount<=0||amount>2000000000)return 0;
        if(q.Operation==PT_NATION_DEPOSIT){if(!PTNationRight(member,7)||Player[p].m_ItemList.GetEquipmentMoney()<amount||c.Money>2000000000-amount)return 0;c.Money+=amount;return amount+1;}
        if(!PTNationRight(member,8)||c.Money<amount||Player[p].m_ItemList.GetEquipmentMoney()>2000000000-amount)return 0;c.Money-=amount;return -amount-1;
    }
    if(target<0||target==member||s_PtNation.State.Members[target].NationId!=own)return 0;
    PTNationMember& person=s_PtNation.State.Members[target];
    if(q.Operation==PT_NATION_ACCEPT||q.Operation==PT_NATION_REJECT){if(!PTNationRight(member,5)||person.Duty)return 0;
        if(q.Operation==PT_NATION_ACCEPT)person.Duty=PT_NATION_CITIZEN;else memset(&person,0,sizeof(person));return -1;}
    if(q.Operation==PT_NATION_KICK){if(!PTNationRight(member,6)||!person.Duty||person.Duty<=duty)return 0;memset(&person,0,sizeof(person));return -1;}
    if(q.Operation==PT_NATION_APPOINT){if(!PTNationRight(member,4)||person.Duty<=duty||q.Value<=duty||q.Value<2||q.Value>5)return 0;person.Duty=q.Value;return -1;}
    if(q.Operation==PT_NATION_TRANSFER){if(duty!=PT_NATION_LORD||!person.Duty)return 0;
        s_PtNation.State.Members[member].Duty=PT_NATION_DEPUTY;person.Duty=PT_NATION_LORD;lstrcpyn(s_PtNation.State.Nations[nation].Leader,person.Name,32);return -1;}
    return 0;
}
static int PTNationMutate(int p,const PTNationRequest& q,const char** error){
    int member=PTNationSelf(p),nation=member>=0?s_PtNation.Nation(s_PtNation.State.Members[member].NationId):-1;
    if(member>=0&&s_PtNation.State.Contributions[member].PendingAfter){*error="Contribution transaction pending.";return 0;}
    int target=q.Target[0]?s_PtNation.Member(q.Target):-1;
    if(target>=0&&s_PtNation.State.Contributions[target].PendingAfter){*error="Member contribution transaction pending.";return 0;}
    if(nation>=0&&(q.Operation==PT_NATION_DEPOSIT||q.Operation==PT_NATION_WITHDRAW))for(int pending=0;pending<PT_NATION_MEMBER_CAPACITY;++pending)if(s_PtNation.State.Contributions[pending].NationId==s_PtNation.State.Nations[nation].Id&&s_PtNation.State.Contributions[pending].PendingAfter){*error="Nation transaction pending.";return 0;}
    if(q.Operation==PT_NATION_DISSOLVE&&nation>=0){
        unsigned int own=s_PtNation.State.Nations[nation].Id;
        for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m)if(s_PtNation.State.Contributions[m].NationId==own&&s_PtNation.State.Contributions[m].PendingAfter){*error="Contribution transaction pending.";return 0;}
        int map=s_PtNation.State.Development[nation].CityMapId;
        for(int w=0;w<MAX_SUBWORLD;++w)if(map&&SubWorld[w].m_SubWorldID==map&&PTTerritory::Preserve(w)){*error="Leave the city before dissolving the nation.";return 0;}
    }
    int result=PTNationMutateBase(p,q,error);if(!result)return 0;
    for(int n=0;n<PT_NATION_CAPACITY;++n)if(s_PtNation.State.Development[n].NationId!=s_PtNation.State.Nations[n].Id)memset(&s_PtNation.State.Development[n],0,sizeof(PTNationDevelopment));
    for(int book=0;book<PT_NATION_CAPACITY;++book)if(s_PtNation.State.Books[book].NationId!=s_PtNation.State.Nations[book].Id)memset(&s_PtNation.State.Books[book],0,sizeof(PTNationBook));
    for(int m=0;m<PT_NATION_MEMBER_CAPACITY;++m){PTTerritoryContribution& c=s_PtNation.State.Contributions[m];const PTNationMember& person=s_PtNation.State.Members[m];if(c.NationId!=person.NationId||strcmp(c.Name,person.Name))memset(&c,0,sizeof(c));}
    return result;
}
static int PTNationExecute(int p,const PTNationRequest& q){
    if(!PTNationLive(p))return 0;
    if(!PTNationReady()){PTNationSend(p,q,0,"Ch\255a th\323 \256\344c d\367 li\326u qu\350c gia. Vui l\337ng th\366 l\271i sau.");return 0;}
    if(q.Operation==PT_NATION_GET||q.Operation==PT_NATION_LIST){PTNationSend(p,q,1,"");return 1;}
    if(!PTNationRecoverWallet(p)){PTNationSend(p,q,0,"C\307n ho\265n t\312t \256\345ng b\351 ng\251n l\255\356ng tr\255\355c khi thao t\270c.");return 0;}
    if(Player[p].CheckTrading()||Player[p].GetLockState()){PTNationSend(p,q,0,"H\267y k\325t th\363c giao d\336ch tr\255\355c.");return 0;}
    PTNationState* previous=new PTNationState;memcpy(previous,&s_PtNation.State,sizeof(*previous));
    const char* error=NULL;int payment=PTNationMutate(p,q,&error);
    if(!payment){memcpy(&s_PtNation.State,previous,sizeof(*previous));delete previous;PTNationSend(p,q,0,error);return 0;}
    if(payment!= -1){
        int w=s_PtNation.Wallet(PTNationPlayerName(p));if(w<0)w=s_PtNation.FreeWallet();
        if(w<0){memcpy(&s_PtNation.State,previous,sizeof(*previous));delete previous;PTNationSend(p,q,0,"Ch\255a th\323 ghi nh\313n giao d\336ch ng\251n qu\374. Vui l\337ng th\366 l\271i sau.");return 0;}
        PTNationWallet& wallet=s_PtNation.State.Wallets[w];lstrcpyn(wallet.Name,PTNationPlayerName(p),sizeof(wallet.Name));
        wallet.Total+=payment>0?-(__int64)(payment-1):-(__int64)(payment+1);
    }
    ++s_PtNation.State.Revision;
    if(!s_PtNation.Save(s_PtNationFile)){memcpy(&s_PtNation.State,previous,sizeof(*previous));delete previous;PTNationSend(p,q,0,"Ch\255a l\255u \256\255\356c qu\350c gia. Thao t\270c ch\255a \256\255\356c \270p d\364ng.");return 0;}
    delete previous;
    if(!PTNationRecoverWallet(p)){PTNationSend(p,q,0,"Giao d\336ch \256ang ch\352 \256\345ng b\351. H\267y \256\250ng nh\313p l\271i \256\323 kh\253i ph\364c.");return 0;}
    for(int online=1;online<MAX_PLAYER;++online)if(PTNationLive(online))PhongThanNationSyncPlayer(online);
    PTNationRequest refresh=q;refresh.Page=0;refresh.NationId=0;refresh.Operation=PT_NATION_GET;PTNationSend(p,refresh,1,"Thao t\270c th\265nh c\253ng.");
    return 1;
}
#include "PhongThanBotNationAdapter.inl"
static bool PhongThanProcessNation(int p,const char* data,int size){
    if(!data||size<(int)sizeof(tagExtendProtoHeader)+1||((BYTE*)data)[0]!=c2s_extend)return false;
    const tagExtendProtoHeader* header=(const tagExtendProtoHeader*)data;
    const PTNationRequest* q=(const PTNationRequest*)(data+sizeof(*header));if(q->Family!=PT_NATION_FAMILY)return false;
    if(!PTNationLive(p)||header->wLength+1!=size||!PTNationRequestValid(q,size-sizeof(*header)))return true;
    PTNationSession& session=s_PtNationSessions[p];
    if(memcmp(session.Ticket,Player[p].m_SessionTicket,sizeof(session.Ticket))){memset(&session,0,sizeof(session));memcpy(session.Ticket,Player[p].m_SessionTicket,sizeof(session.Ticket));}
    if(q->RequestId<=session.LastRequest)return true;session.LastRequest=q->RequestId;
    if(q->Operation>PT_NATION_LIST){DWORD now=GetTickCount();if(session.LastMutation&&now-session.LastMutation<400)return true;session.LastMutation=now;}
    FILE* trace=fopen("native_nation_server.log","a");if(trace){fprintf(trace,"request op=%d ready=%d\n",q->Operation,PTNationReady());fclose(trace);}
    PTNationExecute(p,*q);return true;
}
void PhongThanNationOpenNpc(int p,int operation){
    if(!PTNationLive(p)||!PTNationReady()||!PTNationNearFounder(p))return;
    PTNationRequest q;memset(&q,0,sizeof(q));q.Operation=operation==PT_NATION_LIST?PT_NATION_LIST:(operation==PT_NATION_CREATE?PT_NATION_CREATE:PT_NATION_GET);
    PTNationSend(p,q,1,"");
}
int PhongThanNationLeave(int p){if(!PTNationLive(p)||!PTNationReady())return 0;int m=PTNationSelf(p);if(m<0)return 0;
    PTNationRequest q;memset(&q,0,sizeof(q));q.NationId=s_PtNation.State.Members[m].NationId;q.Operation=s_PtNation.State.Members[m].Duty?PT_NATION_LEAVE:PT_NATION_CANCEL;return PTNationExecute(p,q);}
int PhongThanNationDissolve(int p){if(!PTNationLive(p)||!PTNationReady())return 0;int m=PTNationSelf(p);if(m<0||s_PtNation.State.Members[m].Duty!=1)return 0;
    PTNationRequest q;memset(&q,0,sizeof(q));q.NationId=s_PtNation.State.Members[m].NationId;q.Operation=PT_NATION_DISSOLVE;return PTNationExecute(p,q);}

#include "../../../Headers/PhongThanRanking.h"
#include <vector>
static int PTNationWealthCompare(const void* left,const void* right){
 const PTRankingRow& a=*(const PTRankingRow*)left;const PTRankingRow& b=*(const PTRankingRow*)right;
 if(a.Money!=b.Money)return a.Money>b.Money?-1:1;return strcmp(a.Name,b.Name);
}
bool PhongThanNationWealthRank(PTRankingPage* out){
 if(!out||!PTNationReady())return false;std::vector<PTRankingRow> rows;
 for(int n=0;n<PT_NATION_CAPACITY;++n){const PTNationRecord& country=s_PtNation.State.Nations[n];if(!country.Id)continue;
  PTRankingRow r;memset(&r,0,sizeof(r));lstrcpyn(r.Name,country.Name,32);lstrcpyn(r.Nation,country.Leader,32);
  r.Money=country.Money;r.Level=PTNationCount(country.Id);r.Power=-1;r.Honor=-1;rows.push_back(r);}
 if(!rows.empty())qsort(&rows[0],rows.size(),sizeof(PTRankingRow),PTNationWealthCompare);
 out->Total=min(10,(int)rows.size());out->Count=0;out->Result=1;
 for(int i=out->Page*PT_RANK_ROWS;i<out->Total&&out->Count<PT_RANK_ROWS;++i)out->Rows[out->Count++]=rows[i];return true;
}
