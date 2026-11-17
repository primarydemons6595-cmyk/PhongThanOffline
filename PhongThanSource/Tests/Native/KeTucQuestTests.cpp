// Real Engine/Lua4; original warehouse prefix plus reconstructed extension.
#define main OriginalQuestTestMain
#include "OriginalNpcQuestTests.cpp"
#undef main
static int nowDay=20000,randomValue=100,lastNote=-99;
static int DayClock(Lua_State*L){return Result(L,nowDay*86400);}
static int ByteGet(Lua_State*L){return Result(L,((unsigned)N(L,1)>>((N(L,2)-1)*8))&255);}
static int ByteSet(Lua_State*L){unsigned shift=(N(L,2)-1)*8;return Result(L,((unsigned)N(L,1)&~(255U<<shift))|((N(L,3)&255U)<<shift));}
static int FixedRandom(Lua_State*L){return Result(L,randomValue);}
static int NoteRecord(Lua_State*L){if(N(L,1)==51)lastNote=N(L,2);return 0;}
static int RuntimeWorld(Lua_State*L){return Result(L,1002+player.playerType);}
static int ContentWorld(Lua_State*L){return Result(L,2+player.playerType);}
static int Say(Lua_State*L){for(int i=3;i<=Lua_GetTopIndex(L);++i){const char*s=Lua_ValueToString(L,i);if(s&&strrchr(s,'/'))Offer(L,strrchr(s,'/')+1);}return 0;}
static bool Death(int target,int team=0){return scripts[THUKHO].CallFunction("PTK_Death",0,"dd",target,team)!=0;}
static bool Take(int type,int level){player.playerType=type;player.level=level;return Call(THUKHO,"ptk_take1");}
static bool Complete(int target){int need=(player.task[993]>>8&255)*10;for(int i=0;i<need;++i)CHECK(Death(target));CHECK(Call(THUKHO,"ptk_finish"));return true;}
int main(int argc,char**argv){
    if(argc!=2)return 2;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    if(!SetCurrentDirectoryA(argv[1]))return 3;g_SetRootPath(NULL);g_SetFilePath("\\");
    TLua_Funcs api[]={{"GetTask",GetTask},{"SetTask",SetTask},{"GetByte",ByteGet},{"SetByte",ByteSet},
        {"HaveNormalItem",Have},{"AddNormalItem",Add},{"QuestExchange",Exchange},{"GetLevel",Level},{"GetPlayerType",Type},
        {"LocalSystemTime",DayClock},{"random",FixedRandom},{"TaskNote",NoteRecord},{"AddOwnExp",Exp},
        {"Msg2Player",Stub},{"CloseDialog",Stub},{"SayTask",SayTask},{"Talk",Talk},{"MsgBox",Box},{"Say",Say},{"GetExtPoint",Zero},
        {"GetWorldPos",RuntimeWorld},{"GetContentWorldPos",ContentWorld},{"SubWorldIdx2ContentID",ContentWorld}};
    int targets[]={0,1,2};
    for(int type=0;type<3;++type){
        char filename[80];sprintf(filename,"\\warehouse\\%d.lua",1002+type);
        CHECK(scripts[THUKHO].Init());CHECK(scripts[THUKHO].RegisterFunctions(api,sizeof(api)/sizeof(api[0])));CHECK(scripts[THUKHO].Load(filename));
        Reset("Kế Tục three factions: accept/kill/return");player.playerType=type;player.level=1;nowDay=20000;randomValue=100;
        CHECK(Call(THUKHO,"main"));CHECK(Offered("ptk_open"));CHECK(Call(THUKHO,"ptk_open",true));CHECK(Offered("ptk_take1"));
        CHECK(Call(THUKHO,"ptk_take1",true));CHECK((player.task[993]&255)==targets[type]+1);CHECK((player.task[993]>>8&255)==1);
        CHECK(player.task[984]==1&&CountItem(1,0,1,1)==1&&lastNote==0);
        CHECK(Call(THUKHO,"ptk_take1"));CHECK(player.task[984]==1&&CountItem(1,0,1,1)==1);
        CHECK(Call(THUKHO,"ptk_finish"));CHECK(player.exp==0&&player.task[993]!=0);
        CHECK(Death(100));CHECK(player.task[994]==0);
        player.playerType=(type+1)%3;CHECK(Death(targets[type]));CHECK(player.task[994]==0);CHECK(Call(THUKHO,"ptk_finish"));CHECK(player.exp==0);player.playerType=type;
        CHECK(Complete(targets[type]));CHECK(player.exp==100&&player.task[993]==0&&player.task[994]==0&&lastNote==-1);
        CHECK(Call(THUKHO,"ptk_finish"));CHECK(player.exp==100);
        Reset("daily limit, reload, midnight carryover");player.playerType=type;player.level=20;nowDay=20000;
        for(int n=0;n<5;++n){CHECK(Take(type,20));CHECK(player.task[984]==n+1);CHECK(Complete(targets[type]));}
        CHECK(player.exp==500);CHECK(Take(type,20));CHECK(player.task[993]==0&&player.task[984]==5);
        ++nowDay;CHECK(Take(type,20));CHECK(player.task[984]==1);CHECK(Death(targets[type]));CHECK(player.task[994]==1);
        ++nowDay;CHECK(Call(THUKHO,"main"));CHECK(player.task[994]==1&&player.task[984]==1&&player.task[993]!=0);
        CHECK(Complete(targets[type]));CHECK(player.task[984]==0&&player.task[973]==nowDay);
        Reset("full bag acceptance is atomic");player.playerType=type;player.level=1;player.freeCells=0;
        CHECK(Take(type,1));CHECK(player.task[993]==0&&player.task[984]==0&&player.grants==0);
        Reset("level bands, loot and capped progress");player.playerType=type;nowDay=20000;
        int levels[]={1,10,11,20,21,40,41,60,61,80,81,100,101};int counts[]={10,10,20,20,40,40,80,80,150,150,250,250,400};
        for(int band=0;band<13;++band){player.task[993]=0;player.task[984]=0;player.freeCells=100;CHECK(Take(type,levels[band]));CHECK((player.task[993]>>8&255)*10==counts[band]);}
        player.task[993]=1*256+targets[type]+1;player.task[994]=9;randomValue=1;int gifts=player.grants;
        CHECK(Death(targets[type],6));CHECK(player.task[994]==10&&player.grants==gifts+1);
        CHECK(Death(targets[type],6));CHECK(player.task[994]==10&&player.grants==gifts+1);
        scripts[THUKHO].Exit();
    }
    printf("PASS KE_TUC_LUA4 cases=%d assertions=%d factions=3 runtime_writes=NONE\n",cases,assertions);return 0;
}
