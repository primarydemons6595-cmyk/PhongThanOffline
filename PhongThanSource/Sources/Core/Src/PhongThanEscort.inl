#include "PhongThanEscort.h"
#include "PhongThanEscortRules.h"
#include <time.h>
#include "PhongThanQuestPayout.inl"
namespace PtEscort {
// Offset 0 phase (0 idle,1 moving,2 delivered,3 decoy),1 day,2 runs,3 deadline,
// 4..6 cart map/MPS,7..15 pinned item tuple/count + XP/money/credit.
struct Session {DWORD owner,entity;int npc,lastWorld,lastX,lastY,absent,player,rider;DWORD visualAt,visualCart;float walkBefore,runBefore;Session(){memset(this,0,sizeof(*this));}};
static Session sessions[MAX_PLAYER][3];
static void Migrate(int p) {
    if(Player[p].m_pStatusLoadPlayerInfo)return;
    if(Player[p].m_cTask.GetSaveVal(4500))return;
    for(int k=1;k<=2;++k) {
        int old=k==1?4701:4725,phase=Player[p].m_cTask.GetSaveVal(old);
        int deadline=Player[p].m_cTask.GetSaveVal(old+3),day=Player[p].m_cTask.GetSaveVal(old+1);
        // Starter career/progress values cannot satisfy this escort signature.
        if((phase==1 || phase==2) && deadline>1700000000 && day>19000 &&
            Player[p].m_cTask.GetSaveVal(old+4)>=1001 && Player[p].m_cTask.GetSaveVal(old+5)>0 &&
            Player[p].m_cTask.GetSaveVal(PtEscortBase(k))==0) {
            for(int i=0;i<=20;++i)Player[p].m_cTask.SetSaveVal(PtEscortBase(k)+i,Player[p].m_cTask.GetSaveVal(old+i),TRUE);
        }
    }
    Player[p].m_cTask.SetSaveVal(4500,1,TRUE);
}
static int Get(int p,int k,int offset){Migrate(p);return Player[p].m_cTask.GetSaveVal(PtEscortBase(k)+offset);}
static void Set(int p,int k,int offset,int value){Player[p].m_cTask.SetSaveVal(PtEscortBase(k)+offset,value,TRUE);}
static bool Live(int p){return PhongThanIsLivePlayer(p)&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC;}
static bool Active(int p,int kind){int phase=Get(p,kind,0);return phase==1 || (kind==1 && phase==3);}
static bool Cart(const Session& s){return s.npc>0&&s.npc<MAX_NPC&&Npc[s.npc].m_dwID==s.entity&&Npc[s.npc].m_SubWorldIndex>=0&&Npc[s.npc].m_RegionIndex>=0&&Npc[s.npc].IsAlive()&&Npc[s.npc].m_CurrentLife>0;}
static void DriverVisual(Session& s,DWORD cart) {
#ifndef PT_ESCORT_TEST
    if(!Live(s.player))return;
    DWORD tick=GetTickCount();if(s.visualAt && s.visualCart==cart && (DWORD)(tick-s.visualAt)<1000)return;
    s.visualAt=tick;s.visualCart=cart;
    int actor=Player[s.player].m_nIndex;
    PHONGTHAN_PLAYER_EVENT packet;memset(&packet,0,sizeof(packet));
    PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    packet.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;packet.EntityId=Npc[actor].m_dwID;
    packet.Operation=PHONGTHAN_PLAYER_ESCORT_RIDER;packet.Value=(int)cart;
    PHONGTHAN_PLAYER_EVENT ownership=packet;ownership.Operation=PHONGTHAN_PLAYER_ESCORT_OWNER;ownership.EntityId=s.entity;ownership.Value=(int)Npc[actor].m_dwID;
    int cartWorld=s.npc>0 && s.npc<MAX_NPC && Npc[s.npc].m_dwID==s.entity?Npc[s.npc].m_SubWorldIndex:-1;
    if(cartWorld>=0 && cartWorld<MAX_SUBWORLD)ownership.MapId=SubWorld[cartWorld].m_SubWorldID;
    for(int p=1;p<MAX_PLAYER;++p)if(Live(p)) {
        int audienceWorld=Npc[Player[p].m_nIndex].m_SubWorldIndex;
        if(audienceWorld==Npc[actor].m_SubWorldIndex)g_pServer->SendData(Player[p].m_nNetConnectIdx,&packet,sizeof(packet));
        if(cartWorld>=0 && audienceWorld==cartWorld)g_pServer->SendData(Player[p].m_nNetConnectIdx,&ownership,sizeof(ownership));
    }
#endif
}
static void ExitDriver(Session& s) {
    if(s.rider && Live(s.player) && Player[s.player].m_dwID==s.owner) {
        int actor=Player[s.player].m_nIndex;
        Npc[actor].m_CurrentWalkSpeed=s.walkBefore;Npc[actor].m_CurrentRunSpeed=s.runBefore;
#ifndef PT_ESCORT_TEST
        Player[s.player].UpdataCurData();
#endif
        DriverVisual(s,0);
    }
    bool wasRider=s.rider!=0;s.rider=0;
#ifndef PT_ESCORT_TEST
    if(wasRider && Live(s.player) && Player[s.player].m_dwID==s.owner) {
        int actor=Player[s.player].m_nIndex;
        Npc[actor].SendSyncData(0,TRUE);
        Npc[actor].SendSyncData(Player[s.player].m_nNetConnectIdx,FALSE);
        PhongThanSummonEscortPause(actor,FALSE);
    }
#endif
}
static void Remove(Session& s){ExitDriver(s);if(s.npc>0&&s.npc<MAX_NPC&&Npc[s.npc].m_dwID==s.entity&&!Npc[s.npc].IsPlayer())PhongThanRemoveNpc(s.npc);s=Session();}
static int Map(int n){int w=Npc[n].m_SubWorldIndex;return w>=0&&w<MAX_SUBWORLD?SubWorld[w].m_SubWorldID:0;}
static bool WineAnchor(int p,bool giver){
 if(!Live(p))return false;int npc=Player[p].m_nLastNpcIndex,actor=Player[p].m_nIndex;
 if(npc<=0||npc>=MAX_NPC||Npc[npc].m_Kind!=kind_dialoger||Map(npc)!=(giver?1044:1046)||Map(actor)!=Map(npc))return false;
 int x,y,px,py;Npc[npc].GetMpsPos(&x,&y);Npc[actor].GetMpsPos(&px,&py);
 return PtEscortNear(x,y,(giver?236:237)*256,(giver?192:187)*512,768)&&PtEscortNear(x,y,px,py,512);
}
static bool Anchor(int p,int kind,bool giver){
    if(kind==3)return WineAnchor(p,giver);
    if(!Live(p))return false;int n=Player[p].m_nLastNpcIndex;
    if(n<=0||n>=MAX_NPC||!Npc[n].m_dwID||Npc[n].m_Kind!=kind_dialoger)return false;
    int world=giver?(kind==1?1015:1019):(kind==1?1019:1021);
    int x=giver?(kind==1?191*256:1464*32):(kind==1?164*256:233*256);
    int y=giver?(kind==1?212*512:3239*32):(kind==1?196*512:198*512);
    int nx,ny,px,py;Npc[n].GetMpsPos(&nx,&ny);Npc[Player[p].m_nIndex].GetMpsPos(&px,&py);
    return Map(n)==world&&Map(Player[p].m_nIndex)==world&&PtEscortNear(nx,ny,x,y,650)&&PtEscortNear(px,py,nx,ny,512);
}
static void SaveCart(int p,int kind,Session& s){if(!Cart(s))return;int x,y;Npc[s.npc].GetMpsPos(&x,&y);Set(p,kind,4,Map(s.npc));Set(p,kind,5,x);Set(p,kind,6,y);}
static int Spawn(int p,int kind,int map,int x,int y){
    int w=g_SubWorldSet.SearchWorld(map);if(w<0)return 0;
    int npc=0;
    // Prefer the requested depot coordinate, then the nearest free collision
    // cell within four cells. Never disable collision to stack carts together.
    for(int distance=0;distance<=(kind==1?32:0) && !npc;++distance)
    for(int dy=-4;dy<=4 && !npc;++dy)
    for(int dx=-4;dx<=4 && !npc;++dx) {
        if(dx*dx+dy*dy!=distance)continue;
        int px=x+dx*32,py=y+dy*32;
#ifndef PT_ESCORT_TEST
        int region,mx,my,ox,oy;
        SubWorld[w].Mps2Map(px,py,&region,&mx,&my,&ox,&oy);
        if(region<0 || region>=SubWorld[w].m_nTotalRegion ||
           SubWorld[w].m_Region[region].GetBarrierMin(mx,my,ox,oy,kind==1?FALSE:TRUE,TRUE)!=Obstacle_NULL)continue;
#endif
        npc=PhongThanSpawnNpc(kind==1?556:kind==3?648:363,kind==1?1:30,w,px,py,kind==1?FALSE:TRUE);
    }
    if(npc<=0)return 0;
    Npc[npc].m_Kind=kind_normal;Npc[npc].m_AiMode=0;
#ifndef PT_ESCORT_TEST
    Npc[npc].m_DialogRadius=256;
#endif
    Npc[npc].m_DeathScriptID=0;
    Npc[npc].m_CurrentCamp=Npc[Player[p].m_nIndex].m_CurrentCamp;
    if(Npc[npc].m_CurrentLifeMax<=0){Npc[npc].m_CurrentLifeMax=kind==1?100000:150000;Npc[npc].m_LifeMax=Npc[npc].m_CurrentLifeMax;}
    Npc[npc].m_CurrentLife=Npc[npc].m_CurrentLifeMax;
    if(Npc[npc].m_WalkSpeed<=0)Npc[npc].m_WalkSpeed=8;
    Npc[npc].m_RunSpeed=Npc[npc].m_WalkSpeed;
    Npc[npc].m_CurrentWalkSpeed=(float)Npc[npc].m_WalkSpeed;Npc[npc].m_CurrentRunSpeed=(float)Npc[npc].m_WalkSpeed;
    const char* script="\\script\\phongthan\\escort\\cart.lua";
    strcpy(Npc[npc].ActionScript,script);Npc[npc].m_ActionScriptID=g_FileName2Id((char*)script);
    g_StrCpyLen(Npc[npc].Name,kind==1?"Xe L\255\254ng":kind==3?"Xe Chuyen Ruou":"Xe Ti\252u",sizeof(Npc[npc].Name));
    Session& s=sessions[p][kind-1];s=Session();s.owner=Player[p].m_dwID;s.player=p;s.npc=npc;s.entity=Npc[npc].m_dwID;
    int actor=Player[p].m_nIndex;s.lastWorld=Map(actor);Npc[actor].GetMpsPos(&s.lastX,&s.lastY);SaveCart(p,kind,s);
    return npc;
}
static int Value(int kind,int field,int fallback){if(kind==3)return fallback;int id=2000+kind;return PhongThanEventOwns(id)?PhongThanEventValue(id,field):fallback;}
struct AcceptContext {int player,kind,runs,day,duration;int reward[9];};
static bool AcceptMove(void* opaque){
    AcceptContext& c=*(AcceptContext*)opaque;int n=Player[c.player].m_nIndex,x,y;Npc[n].GetMpsPos(&x,&y);
    if(c.kind==1){if(!Spawn(c.player,c.kind,1015,193*256,210*512))return false;}
    // The actor occupies its own collision cell. Spawn beside it, never inside it.
    static const int dx[8]={96,96,0,-96,-96,-96,0,96};
    static const int dy[8]={0,64,64,64,0,-64,-64,-64};
    if(c.kind!=1){bool spawned=false;for(int at=0;at<8&&!spawned;++at)spawned=Spawn(c.player,c.kind,Map(n),x+dx[at],y+dy[at])>0;
    if(!spawned)return false;}
    Set(c.player,c.kind,17,0);Set(c.player,c.kind,16,0);Set(c.player,c.kind,1,c.day);Set(c.player,c.kind,2,c.runs+1);Set(c.player,c.kind,3,(int)time(NULL)+c.duration);
    for(int i=0;i<9;++i)Set(c.player,c.kind,7+i,c.reward[i]);
    return true;
}
static void Pink(int p) {
    int actor=Player[p].m_nIndex;
#ifndef PT_ESCORT_TEST
    Player[p].m_cPK.SetLockPKState(enumPKWar,Player[p].m_cPK.GetLockPKState());
    Npc[actor].SetCamp(2);Npc[actor].ChangeCurrentCamp(2);
#else
    Npc[actor].m_CurrentCamp=2;
#endif
    Session& s=sessions[p][0];if(Cart(s))Npc[s.npc].m_CurrentCamp=2;
    DriverVisual(s,0);
}
static int AcceptFake(int p) {
    if(!Anchor(p,1,true))return -2;
    if(Get(p,1,0)||Get(p,2,0))return -10;
    if(Npc[Player[p].m_nIndex].m_Level<Value(1,10,55))return -7;
    if(Value(1,22,1)&&PhongThanNationDuty(p)<=0)return -8;
    if(Player[p].GetLockState()||Player[p].CheckTrading())return 0;
    if(!Player[p].Pay(40000))return -12;
    if(!Spawn(p,1,1015,193*256,210*512)) {
        if(!Player[p].Earn(40000))g_DebugLog("[Escort] decoy fee rollback player=%d",p);
        return -13;
    }
    Set(p,1,0,3);Set(p,1,3,0);Pink(p);return 1;
}
static int Accept(int p,int kind){
    if(kind==3)return 0;
    if(!Anchor(p,kind,true))return -2;
    int eventId=2000+kind;if(PhongThanEventOwns(eventId)&&!PhongThanEventAccepting(eventId))return -3;
    int day=PtEscortDay((long)time(NULL)),runs=Get(p,kind,1)==day?Get(p,kind,2):0;
    if(Get(p,kind,0)||Get(p,3-kind,0))return -10;
    if(Npc[Player[p].m_nIndex].m_Level<Value(kind,10,kind==1?55:30))return -7;
    if(Value(kind,22,kind==1?1:0)&&PhongThanNationDuty(p)<=0)return -8;
    if(runs>=Value(kind,11,20))return -9;
    if(Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)<Value(kind,23,kind==1?0:10))return -11;
    if(!PtEscortCanAccept(Get(p,kind,0),Get(p,3-kind,0),runs,Value(kind,11,20),Npc[Player[p].m_nIndex].m_Level,
        Value(kind,10,kind==1?55:30),Value(kind,22,kind==1?1:0)!=0,PhongThanNationDuty(p)>0,
        Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT),Value(kind,23,kind==1?0:10)))return -4;
    AcceptContext c;c.player=p;c.kind=kind;c.runs=runs;c.day=day;c.duration=Value(kind,2,1800);
    // These defaults are evidenced by VNG's completion screenshot and original
    // 162B1F5B.lua. No new random reward table is manufactured here.
    int defaults[9]={kind==1?8:3,kind==1?267:83,0,0,0,1,kind==1?100000:0,kind==1?0:500000,kind==1?0:10};
    for(int i=0;i<9;++i)c.reward[i]=Value(kind,13+i,defaults[i]);
    PhongThanQuestItemTuple reward;
    if(!PhongThanDecodeQuestItemTuple(c.reward[0],c.reward[1],c.reward[2],c.reward[3],c.reward[4],0,reward)||c.reward[5]<0||c.reward[5]>64)return -2;
    PhongThanQuestItemTuple ticket;int count=runs>=Value(kind,12,kind==1?1:20)?Value(kind,8,kind==1?1:0):0;
    if(!PhongThanDecodeQuestItemTuple(Value(kind,3,8),Value(kind,4,268),Value(kind,5,2),Value(kind,6,0),Value(kind,7,0),0,ticket))return -2;
    int result=PhongThanEventTicketPayment(p,ticket,count,PtEscortBase(kind),0,1,AcceptMove,&c);
    if(result==1 && kind==1)Pink(p);return result;
}
static int Payout(int p,int kind,int phase){
    if(Get(p,kind,0)!=phase)return 0;
    PhongThanQuestExchangeRow row;int count=Get(p,kind,12),rows=count?1:0;
    if(count<0||count>64||!PhongThanDecodeQuestItemTuple(Get(p,kind,7),Get(p,kind,8),Get(p,kind,9),Get(p,kind,10),Get(p,kind,11),0,row.item))return -2;
    row.count=count;
    if(kind==1 && phase==2) {
        if(!PhongThanKimBaiAdjust(p,count))return 0;
        // Completion at Phong Lam grants only XP, money and the Kim Bai counter.
        // Random material rewards belong exclusively to the separate Ly Tinh exchange.
        int result=PhongThanQuestPayout(p,PtEscortBase(kind),phase,0,NULL,0,
            Get(p,kind,16)?0:Get(p,kind,13),Get(p,kind,14),0);
        if(result==1)Set(p,kind,17,0); // retire a legacy pending random token
        if(result!=1)PhongThanKimBaiAdjust(p,-count);
        return result;
    }
    return PhongThanQuestPayout(p,PtEscortBase(kind),phase,0,&row,rows,(kind==1 && phase==2 && Get(p,kind,16))?0:Get(p,kind,13),Get(p,kind,14),Get(p,kind,15));
}
static int Deliver(int p,int kind){
    if(!Anchor(p,kind,false)||Get(p,kind,0)!=1)return -2;
    Session& s=sessions[p][kind-1];if(s.owner!=Player[p].m_dwID||!Cart(s)||Get(p,kind,3)<=(long)time(NULL))return -5;
    int a=Player[p].m_nIndex,x,y,cx,cy;Npc[a].GetMpsPos(&x,&y);Npc[s.npc].GetMpsPos(&cx,&cy);
    if(Map(a)!=Map(s.npc)||!PtEscortNear(x,y,cx,cy,512))return -6;
    if(kind==1){
        // Delivery records completion; Phong Lam grants XP, materials and marker together.
        int paid=PhongThanQuestPayout(p,PtEscortBase(kind),1,2,NULL,0,0,0,0);
        if(paid==1)Remove(s);return paid;
    }
    int result=Payout(p,kind,1);if(result==1)Remove(s);return result;
}
static bool TransferCart(Session& s,int world,int x,int y) {
    int target=g_SubWorldSet.SearchWorld(world);if(target<0 || !Cart(s))return false;
#ifdef PT_ESCORT_TEST
    Npc[s.npc].m_SubWorldIndex=target;Npc[s.npc].x=x;Npc[s.npc].y=y;return true;
#else
    KNpc& cart=Npc[s.npc];int region,mx,my,ox,oy;
    static const int dx[8]={96,96,0,-96,-96,-96,0,96},dy[8]={0,64,64,64,0,-64,-64,-64};
    for(int i=0;i<(s.rider?1:8);++i){int px=x+(s.rider?0:dx[i]),py=y+(s.rider?0:dy[i]);
        SubWorld[target].Mps2Map(px,py,&region,&mx,&my,&ox,&oy);
        if(region<0 || region>=SubWorld[target].m_nTotalRegion)continue;
        if(s.rider){if(SubWorld[target].m_Region[region].GetBarrierMin(mx,my,ox,oy,FALSE,TRUE)!=Obstacle_NULL)continue;}
        else if(SubWorld[target].TestBarrier(px,py))continue;
        bool changedWorld=target!=cart.m_SubWorldIndex;
        PHONGTHAN_ENTITY_REFERENCE packet;
        PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_WORLD_ENTITY_REMOVE,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
        packet.MapId=Map(s.npc);packet.EntityId=s.entity;if(changedWorld)cart.SendDataToNearRegion(&packet,sizeof(packet));
        KRegion& old=SubWorld[cart.m_SubWorldIndex].m_Region[cart.m_RegionIndex];old.RemoveNpc(s.npc);old.DecRef(cart.m_MapX,cart.m_MapY,obj_npc);
        cart.m_SubWorldIndex=target;cart.m_RegionIndex=region;cart.m_MapX=mx;cart.m_MapY=my;cart.m_OffX=ox;cart.m_OffY=oy;cart.m_MapZ=0;cart.m_OriginX=px;cart.m_OriginY=py;
        SubWorld[target].m_Region[region].AddNpc(s.npc);SubWorld[target].m_Region[region].AddRef(mx,my,obj_npc);
        cart.SendCommand(do_stand);
        if(changedWorld)cart.SendSyncData(0,TRUE);
        else {
            PHONGTHAN_ENTITY_POSITION position;memset(&position,0,sizeof(position));
            PhongThanInitializeWireHeader(&position.Header,PHONGTHAN_MSG_WORLD_ENTITY_POSITION,sizeof(position),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
            position.MapId=world;position.EntityId=s.entity;position.Mode=PHONGTHAN_POSITION_TRACK;
            position.Action=PHONGTHAN_ACTION_STAND;position.X=px;position.Y=py;
            cart.SendDataToNearRegion(&position,sizeof(position));
        }
        return true;
    }
    return false;
#endif
}
static void Update(int p,int kind,long now){
    Session& s=sessions[p][kind-1];
    if(s.owner&&(!Live(p)||s.owner!=Player[p].m_dwID)){Remove(s);return;}
    if(!Live(p)||!Active(p,kind)){if(s.owner)Remove(s);return;}
    if(Get(p,kind,0)==1 && Get(p,kind,3)<=now){Set(p,kind,0,0);Remove(s);return;}
    if(!s.owner){Spawn(p,kind,Get(p,kind,4),Get(p,kind,5),Get(p,kind,6));return;}
    if(!Cart(s)){Set(p,kind,0,0);Remove(s);return;}
    int a=Player[p].m_nIndex,world=Map(a),cw=Map(s.npc),x,y,cx,cy;Npc[a].GetMpsPos(&x,&y);Npc[s.npc].GetMpsPos(&cx,&cy);
    if(s.rider && Npc[a].m_CurrentLife<=0)ExitDriver(s);
    if(kind==1){if(Npc[a].m_CurrentCamp!=2)Pink(p);DriverVisual(s,s.rider?s.entity:0);}
    if(kind==3)DriverVisual(s,s.rider?s.entity:0);
    bool closeEnough=world==cw&&PtEscortNear(x,y,cx,cy,768);
    if(kind==3&&!s.rider&&world!=cw){if(++s.absent>=120){Set(p,kind,0,0);Remove(s);}return;}
    if((kind!=1 || s.rider) && world!=cw&&(kind==1 || (s.lastWorld==cw&&PtEscortNear(s.lastX,s.lastY,cx,cy,512)))){
        if(TransferCart(s,world,x,y)&&Map(s.npc)==world){closeEnough=true;cx=x;cy=y;}
    }
    if(!closeEnough){if(kind!=1 && ++s.absent>=120){Set(p,kind,0,0);Remove(s);return;}}
    else {
        s.absent=0;
        if(kind!=1 || s.rider) {
            if(kind==1||(kind==3&&s.rider)){Npc[a].m_CurrentWalkSpeed=(float)Npc[s.npc].m_WalkSpeed;Npc[a].m_CurrentRunSpeed=(float)Npc[s.npc].m_WalkSpeed;}
            if((kind==1||kind==3) && s.rider){if(x!=cx || y!=cy)TransferCart(s,world,x,y);}
            else if(!PtEscortNear(x,y,cx,cy,32))Npc[s.npc].SendCommand(do_walk,x,y);
        } else Npc[s.npc].SendCommand(do_stand);
        SaveCart(p,kind,s);
    }
    s.lastWorld=world;s.lastX=x;s.lastY=y;
}
}
int PhongThanEscortRelation(int first,int second) {
    if(first<=0 || first>=MAX_NPC || second<=0 || second>=MAX_NPC || !Npc[first].IsPlayer())return -1;
    for(int p=1;p<MAX_PLAYER;++p) {
        PtEscort::Session& s=PtEscort::sessions[p][0];
        if(s.owner!=Player[p].m_dwID || !PtEscort::Cart(s) || !PtEscort::Active(p,1) ||
           Npc[first].m_SubWorldIndex!=Npc[second].m_SubWorldIndex)continue;
        // Area attacks see one target: the cargo, not cargo plus driver.
        if(s.rider && Player[p].m_nIndex==second)return relation_none;
        if(s.npc==second)return Player[p].m_nIndex==first?relation_dialog:relation_enemy;
    }
    return -1;
}
int PhongThanEscortCargoKind(int npc,int* owner){
 if(owner)*owner=0;if(npc<=0||npc>=MAX_NPC)return 0;
 for(int player=1;player<MAX_PLAYER;++player)for(int kind=1;kind<=2;++kind){PtEscort::Session& session=PtEscort::sessions[player][kind-1];if(session.npc==npc&&session.entity==Npc[npc].m_dwID&&session.owner==Player[player].m_dwID&&PtEscort::Active(player,kind)&&PtEscort::Cart(session)){if(owner)*owner=player;return kind;}}return 0;
}
int PhongThanEscortCombatTarget(int npc){
    if(npc<=0 || npc>=MAX_NPC || !Npc[npc].IsPlayer())return npc;
    int cart=PhongThanEscortDrivenCart(Npc[npc].GetPlayerIdx());
    return cart>0?cart:npc;
}
bool PhongThanEscortPkLocked(int player) {
    return PtEscort::Live(player) && PtEscort::Active(player,1);
}
bool PhongThanEscortMounted(int player) {
    if(!PtEscort::Live(player))return false;
    PtEscort::Session& s=PtEscort::sessions[player][0];
    return s.owner==Player[player].m_dwID && s.rider && PtEscort::Cart(s) && PtEscort::Active(player,1);
}
bool PhongThanEscortCartKilled(int npc,int robber) {
    if(npc<=0 || npc>=MAX_NPC)return false;
    for(int p=1;p<MAX_PLAYER;++p) {
        PtEscort::Session& s=PtEscort::sessions[p][0];
        if(s.npc!=npc || s.entity!=Npc[npc].m_dwID || s.owner!=Player[p].m_dwID || !PtEscort::Active(p,1))continue;
        bool real=PtEscort::Get(p,1,0)==1;PtEscort::Set(p,1,0,0);
        if(real && robber!=p && PtEscort::Live(robber)) {
            int pending=PtEscort::Get(robber,1,19);
            if(pending>=0 && pending<=MAX_INT-250000)PtEscort::Set(robber,1,19,pending+250000);
        }
        PtEscort::Remove(s);return true;
    }
    return false;
}
int LuaEventEscortRide(Lua_State* L) {
    int p=GetPlayerIndex(L),result=0;bool enter=(int)Lua_ValueToNumber(L,1)!=0;
    if(PtEscort::Live(p)) {
        PtEscort::Session& s=PtEscort::sessions[p][0];int actor=Player[p].m_nIndex;
        int x,y,cx,cy;Npc[actor].GetMpsPos(&x,&y);
        if(s.owner==Player[p].m_dwID && PtEscort::Active(p,1) && PtEscort::Cart(s) &&
           Player[p].m_nLastNpcIndex==s.npc && PtEscort::Map(actor)==PtEscort::Map(s.npc)) {
            Npc[s.npc].GetMpsPos(&cx,&cy);
            if(PtEscortNear(x,y,cx,cy,512)) {
                if(!enter){PtEscort::ExitDriver(s);Npc[s.npc].SendCommand(do_stand);result=1;}
                else if(s.rider){result=1;}
                else if(Npc[actor].m_CurrentLife>0) {
                    s.walkBefore=Npc[actor].m_CurrentWalkSpeed;s.runBefore=Npc[actor].m_CurrentRunSpeed;s.rider=1;
                    Npc[actor].m_CurrentWalkSpeed=(float)Npc[s.npc].m_WalkSpeed;Npc[actor].m_CurrentRunSpeed=(float)Npc[s.npc].m_WalkSpeed;
#ifndef PT_ESCORT_TEST
                    Npc[actor].m_nPeopleIdx=0;Npc[actor].m_nObjectIdx=0;
                    Npc[actor].m_NowCommand.CmdKind=do_stand;
                    Npc[actor].SendCommand(do_stand);
                    PhongThanSummonEscortPause(actor,TRUE);
                    Npc[actor].SendSyncData(0,TRUE);
                    Npc[actor].SendSyncData(Player[p].m_nNetConnectIdx,FALSE);
#endif
                    PtEscort::DriverVisual(s,s.entity);result=1;
                }
            }
        }
    }
    g_DebugLog("[EscortRide] player=%d enter=%d result=%d",p,enter?1:0,result);
    Lua_PushNumber(L,result);return 1;
}
int LuaDailyWineRide(Lua_State* L) {
    int p=GetPlayerIndex(L),result=0;bool enter=(int)Lua_ValueToNumber(L,1)!=0;
    if(PtEscort::Live(p)) {
        PtEscort::Session& s=PtEscort::sessions[p][2];int actor=Player[p].m_nIndex;
        int x,y,cx,cy;Npc[actor].GetMpsPos(&x,&y);
        if(s.owner==Player[p].m_dwID && PtEscort::Active(p,3) && PtEscort::Cart(s) &&
           Player[p].m_nLastNpcIndex==s.npc && PtEscort::Map(actor)==PtEscort::Map(s.npc)) {
            Npc[s.npc].GetMpsPos(&cx,&cy);
            if(PtEscortNear(x,y,cx,cy,512)) {
                if(!enter){PtEscort::ExitDriver(s);Npc[s.npc].SendCommand(do_stand);result=1;}
                else if(s.rider){result=1;}
                else if(Npc[actor].m_CurrentLife>0) {
                    s.walkBefore=Npc[actor].m_CurrentWalkSpeed;s.runBefore=Npc[actor].m_CurrentRunSpeed;s.rider=1;
                    Npc[actor].m_CurrentWalkSpeed=(float)Npc[s.npc].m_WalkSpeed;Npc[actor].m_CurrentRunSpeed=(float)Npc[s.npc].m_WalkSpeed;
#ifndef PT_ESCORT_TEST
                    Npc[actor].m_nPeopleIdx=0;Npc[actor].m_nObjectIdx=0;
                    Npc[actor].m_NowCommand.CmdKind=do_stand;
                    Npc[actor].SendCommand(do_stand);
                    PhongThanSummonEscortPause(actor,TRUE);
                    Npc[actor].SendSyncData(0,TRUE);
                    Npc[actor].SendSyncData(Player[p].m_nNetConnectIdx,FALSE);
#endif
                    PtEscort::DriverVisual(s,s.entity);result=1;
                }
            }
        }
    }
    g_DebugLog("[WineRide] player=%d enter=%d result=%d",p,enter?1:0,result);
    Lua_PushNumber(L,result);return 1;
}
static void PhongThanEscortJournal(int p) {
#ifndef PT_ESCORT_TEST
    static DWORD owners[MAX_PLAYER];static int phases[MAX_PLAYER];
    if(!PtEscort::Live(p)){owners[p]=0;phases[p]=0;return;}
    int phase=PtEscort::Get(p,1,0);if(phase!=1 && phase!=2)phase=0;
    bool fresh=owners[p]!=Player[p].m_dwID;
    if(!fresh && phases[p]==phase)return;
    const char* text=phase==1?"VÀn L≠¨ng: ß≠a xe Æ’n <color=green>Tr≠¨ng Qu’ Ph≠¨ng<color>, Tuy÷t Long L‹nh (164/196). Nh p xe Æ” l™n ho∆c xuËng xe. ThÍi hπn 30 phÛt.":phase==2?"VÀn L≠¨ng: ß∑ giao l≠¨ng th∂o. V“ g∆p <color=green>Phong L©m<color> Î Mπnh T©n (191/212), ch‰n Hoµn thµnh VÀn L≠¨ng Æ” nhÀn th≠Îng.":"VÀn L≠¨ng: Chuy’n Æ∑ k’t thÛc.";
    if(phase || (!fresh && phases[p]))SendPhongThanTaskRecord(p,4101,phase,phase?(fresh || !phases[p]?0:1):2,text);
    owners[p]=Player[p].m_dwID;phases[p]=phase;
#endif
}
void PhongThanEscortTick(){
    static DWORD last=0;DWORD tick=GetTickCount();if((DWORD)(tick-last)<250)return;last=tick;
    long now=(long)time(NULL);for(int p=1;p<MAX_PLAYER;++p) {
        for(int kind=1;kind<=3;++kind)PtEscort::Update(p,kind,now);
        PhongThanEscortJournal(p);
        int pending=PtEscort::Get(p,1,19);
        if(PtEscort::Live(p) && pending>0)PhongThanQuestPayout(p,PtEscortBase(1)+19,pending,0,NULL,0,0,pending,0);
    }
}
int LuaEventEscortAction(Lua_State* L){
    int result=-2,p=GetPlayerIndex(L);int kind=(int)Lua_ValueToNumber(L,1),operation=(int)Lua_ValueToNumber(L,2);
    if(PtEscort::Live(p)&&(kind==1||kind==2)){
        if(operation==0)result=PtEscort::Get(p,kind,0);
        else if(operation==5&&kind==1)result=PtEscort::AcceptFake(p);
        else if(operation==1)result=PtEscort::Accept(p,kind);
        else if(operation==2)result=PtEscort::Deliver(p,kind);
        else if(operation==3&&kind==1&&PtEscort::Anchor(p,kind,true))result=PtEscort::Payout(p,kind,2);
        else if(operation==4&&PtEscort::Active(p,kind)&&(PtEscort::Anchor(p,kind,true)||PtEscort::Anchor(p,kind,false))){PtEscort::Set(p,kind,0,0);PtEscort::Remove(PtEscort::sessions[p][kind-1]);result=1;}
    }
    Lua_PushNumber(L,result);return 1;
}
#include "PhongThanEscortRedeem.inl"
int LuaEventEscortInfo(Lua_State* L){
    int p=GetPlayerIndex(L),kind=(int)Lua_ValueToNumber(L,1);
    if(!PtEscort::Live(p)||(kind!=1&&kind!=2)){for(int i=0;i<6;++i)Lua_PushNumber(L,0);return 6;}
    PtEscort::Session& s=PtEscort::sessions[p][kind-1];int left=PtEscort::Get(p,kind,3)-(long)time(NULL);
    Lua_PushNumber(L,PtEscort::Get(p,kind,0));Lua_PushNumber(L,left>0?left:0);Lua_PushNumber(L,PtEscort::Cart(s)?s.npc:0);
    Lua_PushNumber(L,PtEscort::Get(p,kind,1)==PtEscortDay((long)time(NULL))?PtEscort::Get(p,kind,2):0);Lua_PushNumber(L,PtEscort::Value(kind,11,20));Lua_PushNumber(L,s.rider?1:0);return 6;
}

static int ptEscortTrapDepth[MAX_PLAYER];
void PhongThanEscortTrapScope(int p,int delta){
    if(p>0 && p<MAX_PLAYER){ptEscortTrapDepth[p]+=delta;if(ptEscortTrapDepth[p]<0)ptEscortTrapDepth[p]=0;}
}
bool PhongThanEscortMayChangeMap(int p){
    return !PhongThanEscortMounted(p) || (p>0 && p<MAX_PLAYER && ptEscortTrapDepth[p]>0);
}
int PhongThanEscortDrivenCart(int p){
    return PhongThanEscortMounted(p)?PtEscort::sessions[p][0].npc:0;
}

int PhongThanEscortSnapshotTemplate(int p,int ordinary){
    int cart=PhongThanEscortDrivenCart(p);
    return cart>0?Npc[cart].m_NpcSettingIdx:ordinary;
}

int LuaDailyWineAction(Lua_State* L){
 int p=GetPlayerIndex(L),op=(int)Lua_ValueToNumber(L,1),result=0;
 if(PtEscort::Live(p)){
  using namespace PtEscort;int kind=3,phase=Get(p,kind,0),day=PtEscortDay((long)time(NULL)),runs=Get(p,kind,1)==day?Get(p,kind,2):0;
  if(phase&&Get(p,3,3)<(int)time(NULL)){Set(p,3,0,0);Remove(sessions[p][2]);phase=0;}
  if(op==0)result=phase;
  else if(op==1&&Anchor(p,3,true)&&phase==0&&Npc[Player[p].m_nIndex].m_Level>=95&&runs<9&&!Get(p,1,0)&&!Get(p,2,0)){
   PhongThanQuestExchangeRow need;need.count=1;PhongThanDecodeQuestItemTuple(3,79,0,0,0,0,need.item);
   if(PhongThanQuestExchange(p,4460,0,1,&need,1,NULL,0)){
    Set(p,3,18,0);Set(p,3,1,day);Set(p,3,2,runs+1);Set(p,3,3,(int)time(NULL)+1200);
    int actor=Player[p].m_nIndex,x,y;Npc[actor].GetMpsPos(&x,&y);result=Spawn(p,3,1044,x+96,y)>0?1:0;
    if(!result){PhongThanQuestExchangeRow back=need;PhongThanQuestPayout(p,4460,1,0,&back,1,0,0,0);Set(p,3,2,runs);}
   }
  }
  else if(op==2&&Anchor(p,3,false)&&phase==1&&Get(p,3,18)==1){Session& session=sessions[p][2];int actor=Player[p].m_nIndex,x,y,cx,cy;Npc[actor].GetMpsPos(&x,&y);
   if(Cart(session)&&Map(session.npc)==1046&&Get(p,3,3)>=(int)time(NULL)){Npc[session.npc].GetMpsPos(&cx,&cy);if(PtEscortNear(x,y,cx,cy,512)){Set(p,3,0,2);Remove(session);result=1;}}
  }
  else if((op==3||op==4)&&Anchor(p,3,false)&&phase==2&&Get(p,3,3)>=(int)time(NULL)){
   PhongThanQuestExchangeRow item;item.count=1;PhongThanDecodeQuestItemTuple(3,175,0,0,0,0,item.item);
   result=PhongThanQuestPayout(p,4460,2,0,op==4?&item:NULL,op==4?1:0,op==3?Npc[Player[p].m_nIndex].m_Level*10000:0,0,0);
   if(result==1){int receipt=Player[p].m_cTask.GetSaveVal(4378)+1;Player[p].m_cTask.SetSaveVal(4378,receipt,TRUE);PtDaily::Queue(p);Player[p].m_cTask.SetSaveVal(PtDaily::LastReceipt+8,receipt,TRUE);PtDaily::Flush(p);}
  }
  else if(op==5&&(Anchor(p,3,true)||Anchor(p,3,false))&&phase){Set(p,3,0,0);Remove(sessions[p][2]);result=1;}
 }Lua_PushNumber(L,result);return 1;
}
void PhongThanDailyWineKill(int p,int map){if(PtEscort::Live(p)&&map==1046&&PtEscort::Get(p,3,0)==1&&PtEscort::Get(p,3,3)>=(int)time(NULL)&&PtEscort::Cart(PtEscort::sessions[p][2])&&!PtEscort::sessions[p][2].rider){int x,y,cx,cy;Npc[Player[p].m_nIndex].GetMpsPos(&x,&y);Npc[PtEscort::sessions[p][2].npc].GetMpsPos(&cx,&cy);if(PtEscort::Map(PtEscort::sessions[p][2].npc)==map&&PtEscortNear(x,y,cx,cy,1024))PtEscort::Set(p,3,18,1);}}
