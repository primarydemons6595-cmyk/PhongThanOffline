// Runs production accept/cart lifecycle/delivery/payout code, not a second
// implementation. Only engine boundaries (movement, inventory, persistence)
// are replaced with deterministic host objects.
#define PT_ESCORT_TEST
#define main QuestExchangeContractMain
#include "QuestExchangeTests.cpp"
#undef main
#include <vector>
enum {MAX_SUBWORLD=4,kind_normal=0,kind_player=1,kind_dialoger=3,do_stand=0,do_walk=1,relation_none=0,relation_dialog=3,relation_enemy=4,TASKVALUE_PT_CREDIT=4820};
#define MAX_INT 0x7fffffff
struct World {int m_SubWorldID;} SubWorld[MAX_SUBWORLD];
int WorldSet::SearchWorld(int map){for(int i=0;i<MAX_SUBWORLD;++i)if(SubWorld[i].m_SubWorldID==map)return i;return -1;}
struct FakeNpc {
    DWORD m_dwID,m_DeathScriptID,m_ActionScriptID;int m_SubWorldIndex,m_RegionIndex,m_Kind,m_AiMode,m_CurrentCamp,m_CurrentLife,m_CurrentLifeMax,m_LifeMax,m_WalkSpeed,m_RunSpeed,m_Level,x,y;
    char Name[32],ActionScript[260];bool dead;float m_CurrentWalkSpeed,m_CurrentRunSpeed;int commands,m_NpcSettingIdx;
    int GetPlayerIdx(){return m_Kind==kind_player?(int)m_dwID-200:0;}
    bool IsAlive(){return !dead;}bool IsPlayer(){return m_Kind==kind_player;}
    void GetMpsPos(int* px,int* py){*px=x;*py=y;}
    bool ChangeWorld(int map,int px,int py){int w=g_SubWorldSet.SearchWorld(map);if(w<0)return false;m_SubWorldIndex=w;x=px;y=py;return true;}
    void SendCommand(int command,int px=0,int py=0){if(command==do_walk){x=px;y=py;}++commands;}
} Npc[MAX_NPC];
static DWORD entitySerial=1000;static bool spawnFails,managed,accepting=true;static int rewardMoneyOverride=-1;
static bool PhongThanIsLivePlayer(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0;}
static void PhongThanRemoveNpc(int n){ZeroMemory(&Npc[n],sizeof(Npc[n]));}
static int PhongThanSpawnNpc(int,int,int world,int x,int y,BOOL){if(spawnFails)return 0;for(int n=4;n<MAX_NPC;++n)if(!Npc[n].m_dwID){Npc[n].m_dwID=++entitySerial;Npc[n].m_SubWorldIndex=world;Npc[n].m_RegionIndex=0;Npc[n].x=x;Npc[n].y=y;return n;}return 0;}
static void g_StrCpyLen(char* to,const char* from,int size){strncpy(to,from,size);to[size-1]=0;}
static DWORD g_FileName2Id(char*){return 1;}
static bool PhongThanEventOwns(int){return managed;}
static bool PhongThanEventAccepting(int){return accepting;}
static int PhongThanEventValue(int id,int field){int kind=id-2000;
    int defaults[25]={0,300,1800,8,268,2,0,0,kind==1?1:0,0,kind==1?55:30,20,kind==1?1:20,kind==1?8:3,kind==1?267:83,0,0,0,1,kind==1?100000:0,kind==1?0:500000,kind==1?0:10,kind==1?1:0,kind==1?0:10,0};
    if(field==20&&rewardMoneyOverride>=0)return rewardMoneyOverride;return defaults[field];}
static int kimBai[MAX_PLAYER];
static int PhongThanKimBaiCount(int p){return kimBai[p];}
static bool PhongThanKimBaiAdjust(int p,int delta){if(kimBai[p]+delta<0 || kimBai[p]+delta>100)return false;kimBai[p]+=delta;return true;}
static int randomValues[3],randomCursor;
static int EscortTestRandom(int bound){int v=randomValues[randomCursor++%3];return v%bound;}
#define g_Random EscortTestRandom
static int PhongThanNationDuty(int p){return Player[p].m_cTong.m_nFlag;}
#include "../../Sources/Core/Src/PhongThanEscort.inl"
#undef g_Random
static int checks;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %s line=%d\n",#x,__LINE__);return 1;}}while(0)
static void Init(){
    ZeroMemory(kimBai,sizeof(kimBai));ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(PtEscort::sessions,sizeof(PtEscort::sessions));
    ItemSet.allocated=0;ItemSet.failAt=0;spawnFails=false;managed=false;accepting=true;rewardMoneyOverride=-1;
    SubWorld[0].m_SubWorldID=1015;SubWorld[1].m_SubWorldID=1019;SubWorld[2].m_SubWorldID=1021;SubWorld[3].m_SubWorldID=1001;randomCursor=0;ZeroMemory(randomValues,sizeof(randomValues));
    for(int p=1;p<MAX_PLAYER;++p){Player[p].m_dwID=100+p;Player[p].m_nIndex=p;Player[p].m_nLastNpcIndex=3;Player[p].m_cTong.m_nFlag=1;Player[p].m_ItemList.capacity=20;Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_CREDIT,20,TRUE);Npc[p].m_dwID=200+p;Npc[p].m_Kind=kind_player;Npc[p].m_Level=60;Npc[p].m_CurrentLife=1000;Npc[p].m_CurrentWalkSpeed=10;Npc[p].m_CurrentRunSpeed=20;}
    Npc[3].m_dwID=300;Npc[3].m_Kind=kind_dialoger;
}
static void At(int p,int kind,bool giver){
    int map=giver?(kind==1?1015:1019):(kind==1?1019:1021),x=giver?(kind==1?191*256:1464*32):(kind==1?164*256:233*256),y=giver?(kind==1?212*512:3239*32):(kind==1?196*512:198*512);
    Npc[p].ChangeWorld(map,x,y);Npc[3].ChangeWorld(map,x,y);Player[p].m_nLastNpcIndex=3;
}
static void Arrive(int p,int kind){At(p,kind,false);PtEscort::Session& s=PtEscort::sessions[p][kind-1];Npc[s.npc].ChangeWorld(PtEscort::Map(p),Npc[p].x,Npc[p].y);}
static bool menuClose;
static int Dialog(Lua_State* L){for(int i=1;i<=Lua_GetTopIndex(L);++i)if(Lua_IsString(L,i)&&strstr(Lua_ValueToString(L,i),"/pt_escort_close"))menuClose=true;return 0;}
static int Noop(Lua_State*){return 0;}
static bool LoadScript(KLuaScript& script,const char* path){
    FILE* f=fopen(path,"rb");if(!f){printf("MISSING %s\n",path);return false;}fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);std::vector<unsigned char> data(size);fread(&data[0],1,size,f);fclose(f);
    TLua_Funcs api[]={{"EventEscortRedeem",LuaEventEscortRedeem},{"EventEscortKimBai",LuaEventEscortKimBai},{"EventEscortRide",LuaEventEscortRide},{"EventEscortAction",LuaEventEscortAction},{"EventEscortInfo",LuaEventEscortInfo},{"Say",Dialog},{"CloseDialog",Noop}};
    return script.Init()&&script.RegisterFunctions(api,sizeof(api)/sizeof(api[0]))&&script.LoadBuffer(&data[0],size)&&script.ExecuteCode();
}
int main(int argc,char** argv){
    Init();At(1,1,true);CHECK(PtEscort::Accept(1,1)==1);CHECK(PtEscort::Get(1,1,0)==1&&PtEscort::Get(1,1,2)==1);CHECK(PtEscort::Accept(1,1)<0);
    CHECK(PtEscort::Deliver(1,1)<0);Arrive(1,1);CHECK(PtEscort::Deliver(2,1)<0);CHECK(PtEscort::Deliver(1,1)==1);CHECK(PtEscort::Get(1,1,0)==2&&Player[1].xp==0);
    CHECK(PtEscort::Payout(1,1,1)==0);At(1,1,true);Player[1].m_ItemList.capacity=0;kimBai[1]=100;CHECK(PtEscort::Payout(1,1,2)==0&&PtEscort::Get(1,1,0)==2&&Player[1].xp==0);kimBai[1]=0;
    Player[1].m_ItemList.capacity=0;PtEscort::Set(1,1,14,250000);PtEscort::Set(1,1,15,999);PtEscort::Set(1,1,17,9);
    int randomBefore=randomCursor,itemsBefore=Player[1].m_ItemList.Count(),creditBefore=Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
    CHECK(PtEscort::Payout(1,1,2)==1);CHECK(Player[1].xp==100000&&kimBai[1]==1);
    CHECK(Player[1].cash==250000&&Player[1].m_ItemList.Count()==itemsBefore&&randomCursor==randomBefore);
    CHECK(Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)==creditBefore&&PtEscort::Get(1,1,17)==0);
    CHECK(!PtEscort::Payout(1,1,2));CHECK(Player[1].xp==100000&&Player[1].cash==250000&&kimBai[1]==1);
    Player[1].m_ItemList.capacity=20;
    CHECK(PtEscort::Accept(1,1)==-1&&PtEscort::Get(1,1,2)==1);int item=ItemSet.Add(8,0,0,0,268,2,NULL,1);CHECK(item>0);Player[1].m_ItemList.Add(item,pos_equiproom,1,0,false);
    spawnFails=true;CHECK(PtEscort::Accept(1,1)==0&&Item[item].id&&PtEscort::Get(1,1,2)==1);spawnFails=false;CHECK(PtEscort::Accept(1,1)==1&&!Item[item].id&&PtEscort::Get(1,1,2)==2);
    Init();At(1,2,true);managed=true;rewardMoneyOverride=600000;CHECK(PtEscort::Accept(1,2)==1);rewardMoneyOverride=900000;Arrive(1,2);Player[1].m_ItemList.capacity=0;CHECK(PtEscort::Deliver(1,2)==0&&Player[1].cash==0&&PtEscort::Get(1,2,0)==1);
    Player[1].m_ItemList.capacity=20;accepting=false;CHECK(PtEscort::Deliver(1,2)==1);CHECK(Player[1].cash==600000&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)==30);CHECK(PtEscort::Deliver(1,2)<0&&Player[1].cash==600000);
    At(1,2,true);CHECK(PtEscort::Accept(1,2)==-3);accepting=true;CHECK(PtEscort::Accept(1,2)==1);int cart=PtEscort::sessions[1][1].npc;Npc[cart].dead=true;PtEscort::Update(1,2,(long)time(NULL));CHECK(PtEscort::Get(1,2,0)==0&&Player[1].cash==600000);
    Init();At(1,1,true);CHECK(PtEscort::Accept(1,1)==1);cart=PtEscort::sessions[1][0].npc;DWORD old=Npc[cart].m_dwID;PtEscort::SaveCart(1,1,PtEscort::sessions[1][0]);PtEscort::Remove(PtEscort::sessions[1][0]);PtEscort::Update(1,1,(long)time(NULL));CHECK(PtEscort::Cart(PtEscort::sessions[1][0])&&PtEscort::sessions[1][0].entity!=old);
    cart=PtEscort::sessions[1][0].npc;Npc[cart].m_dwID=99999;PtEscort::Update(1,1,(long)time(NULL));CHECK(Npc[cart].m_dwID==99999&&PtEscort::Get(1,1,0)==0);
    Init();At(1,1,true);CHECK(PtEscort::Accept(1,1)==1);PtEscort::Set(1,1,3,(int)time(NULL)-1);PtEscort::Update(1,1,(long)time(NULL));CHECK(PtEscort::Get(1,1,0)==0&&Player[1].xp==0);
    CHECK(PtEscortDay(1700000000)==(1700000000+25200)/86400);CHECK(PtEscortBase(1)+23<PtEscortBase(2)&&PtEscortBase(2)+23<4800);
    // Stationary without a driver, even if owner changes maps outside the cart.
    Init();At(1,1,true);CHECK(PtEscort::Accept(1,1)==1);cart=PtEscort::sessions[1][0].npc;
    int originalX=Npc[cart].x;Npc[1].x+=200;PtEscort::Update(1,1,(long)time(NULL));CHECK(Npc[cart].x==originalX);
    CHECK(PhongThanEscortRelation(1,cart)==relation_dialog);Npc[2].m_SubWorldIndex=Npc[cart].m_SubWorldIndex;
    CHECK(PhongThanEscortRelation(2,cart)==relation_enemy);
    CHECK(PhongThanEscortRelation(1,0)==-1);
    CHECK(PhongThanEscortCartKilled(cart,2));CHECK(PtEscort::Get(1,1,0)==0 && PtEscort::Get(2,1,19)==250000);
    CHECK(!PhongThanEscortCartKilled(cart,2));CHECK(PtEscort::Get(2,1,19)==250000);
    CHECK(PhongThanQuestPayout(2,PtEscortBase(1)+19,250000,0,NULL,0,0,250000,0)==1);CHECK(Player[2].cash==250000);
    CHECK(PhongThanQuestPayout(2,PtEscortBase(1)+19,250000,0,NULL,0,0,250000,0)==0 && Player[2].cash==250000);
    CHECK(PtEscort::Deliver(2,1)<0 && PtEscort::Deliver(1,1)<0);
    // Every pinned material/bonus outcome; no double spend on stale CAS.
    for(int token=1;token<=16;++token) {
        Init();Npc[1].ChangeWorld(1001,209*256,206*512);Npc[3].ChangeWorld(1001,209*256,206*512);
        kimBai[1]=2;PtEscort::Set(1,1,20,token);
        CHECK(PtEscort::Redeem(1)==token && kimBai[1]==1 && PtEscort::Get(1,1,20)==0);
        CHECK(Player[1].m_ItemList.Count()==(token>8?2:1));
        CHECK(PhongThanQuestPayout(1,PtEscortBase(1)+20,token,0,NULL,0,0,0,0)==0);
    }
    Init();Npc[1].ChangeWorld(1001,209*256,206*512);Npc[3].ChangeWorld(1001,209*256,206*512);
    CHECK(PtEscort::Redeem(1)==-1 && randomCursor==0);
    kimBai[1]=1;PtEscort::Set(1,1,20,9);Player[1].m_ItemList.capacity=1;
    CHECK(PtEscort::Redeem(1)==0 && kimBai[1]==1 && PtEscort::Get(1,1,20)==9 && Player[1].m_ItemList.Count()==0);
    Player[1].m_ItemList.capacity=2;Player[1].trading=true;
    CHECK(PtEscort::Redeem(1)==0 && kimBai[1]==1 && PtEscort::Get(1,1,20)==9);Player[1].trading=false;
    ItemSet.failAt=ItemSet.allocated+2;
    CHECK(PtEscort::Redeem(1)==0 && kimBai[1]==1 && PtEscort::Get(1,1,20)==9 && Player[1].m_ItemList.Count()==0);
    ItemSet.failAt=0;CHECK(PtEscort::Redeem(1)==9 && kimBai[1]==0 && Player[1].m_ItemList.Count()==2);
    CHECK(PtEscort::Redeem(1)==-1 && Player[1].m_ItemList.Count()==2);
    Npc[1].x+=1000;kimBai[1]=1;CHECK(PtEscort::Redeem(1)==-2 && kimBai[1]==1);
    if(argc==2){
        char path[1024];KLuaScript vlGiver,vlDestination,vtGiver,vtDestination,cartScript,redeemScript;
        sprintf(path,"%s/script/phongthan/npc_restore/1015_01.lua",argv[1]);CHECK(LoadScript(vlGiver,path));
        sprintf(path,"%s/script/phongthan/escort/van_luong_deliver.lua",argv[1]);CHECK(LoadScript(vlDestination,path));
        sprintf(path,"%s/script/phongthan/escort/van_tieu_giver.lua",argv[1]);CHECK(LoadScript(vtGiver,path));
        sprintf(path,"%s/script/phongthan/escort/van_tieu_deliver.lua",argv[1]);CHECK(LoadScript(vtDestination,path));
        sprintf(path,"%s/script/phongthan/escort/cart.lua",argv[1]);CHECK(LoadScript(cartScript,path));
        sprintf(path,"%s/script/phongthan/escort/kim_bai_redeem.lua",argv[1]);CHECK(LoadScript(redeemScript,path));
        Init();Npc[1].ChangeWorld(1001,209*256,206*512);Npc[3].ChangeWorld(1001,209*256,206*512);kimBai[1]=1;
        CHECK(redeemScript.CallFunction("main",0,""));CHECK(redeemScript.CallFunction("pt_redeem",0,"") && kimBai[1]==0 && Player[1].m_ItemList.Count()==2);
        Init();At(1,1,true);CHECK(PtEscort::Accept(1,1)==1);cart=PtEscort::sessions[1][0].npc;
        Player[1].m_nLastNpcIndex=cart;CHECK(cartScript.CallFunction("pt_cart_enter",0,""));CHECK(PhongThanEscortMounted(1));
        CHECK(Npc[1].m_CurrentRunSpeed==Npc[cart].m_WalkSpeed);
        Npc[1].x+=200;PtEscort::Update(1,1,(long)time(NULL));CHECK(Npc[cart].x==Npc[1].x);
        CHECK(cartScript.CallFunction("pt_cart_leave",0,""));CHECK(!PhongThanEscortMounted(1));CHECK(Npc[1].m_CurrentRunSpeed==20);
        originalX=Npc[cart].x;Npc[1].x+=200;PtEscort::Update(1,1,(long)time(NULL));CHECK(Npc[cart].x==originalX);
        Init();At(1,1,true);menuClose=false;CHECK(vlGiver.CallFunction("main",0,"")&&menuClose);CHECK(vlGiver.CallFunction("pt_escort_take",0,""));CHECK(PtEscort::Get(1,1,0)==1);
        Arrive(1,1);CHECK(vlDestination.CallFunction("pt_escort_deliver",0,"")&&PtEscort::Get(1,1,0)==2);At(1,1,true);
        CHECK(vlGiver.CallFunction("pt_escort_claim",0,"")&&Player[1].xp==100000&&kimBai[1]==1);
        CHECK(vlGiver.CallFunction("pt_escort_claim",0,"")&&Player[1].xp==100000&&kimBai[1]==1);
        Init();At(1,2,true);menuClose=false;CHECK(vtGiver.CallFunction("main",0,"")&&menuClose);CHECK(vtGiver.CallFunction("pt_escort_take",0,""));CHECK(PtEscort::Get(1,2,0)==1);
        Arrive(1,2);CHECK(vtDestination.CallFunction("pt_escort_deliver",0,"")&&Player[1].cash==500000&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)==30);
        CHECK(vtDestination.CallFunction("pt_escort_deliver",0,"")&&Player[1].cash==500000);
    }
    printf("PASS ESCORT_QUEST checks=%d production_accept_delivery_payout_cart_lifecycle Lua4_payloads=%d\n",checks,argc==2?6:0);return 0;
}
