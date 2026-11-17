#define main OriginalQuestTestMain
#include "OriginalNpcQuestTests.cpp"
#undef main
#include "../../../Headers/PhongThanNpcQuestMark.h"
#include "../../../Headers/PhongThanTaskJournal.h"
#include "../../../Headers/PhongThanUiProtocol.h"
#include "../../Sources/Core/Src/PhongThanScriptWire.h"
static DWORD questTick=10000;
static DWORD QuestTestClock(){return questTick;}
#define GetTickCount QuestTestClock
#include "../../Sources/Core/Src/PhongThanNpcQuestMarks.inl"
#undef GetTickCount
static int markerDay=20000;
static int MarkerClock(Lua_State*L){return Result(L,markerDay*86400);}
static int MarkerByte(Lua_State*L){return Result(L,((unsigned)N(L,1)>>((N(L,2)-1)*8))&255);}
static int MarkerWorld(Lua_State*L){return Result(L,1002+player.playerType);}
static int MarkerContentWorld(Lua_State*L){return Result(L,2+player.playerType);}
static int ItemCountAny(Lua_State*L){return Result(L,CountItem(4,N(L,1)));}
static bool ReadMark(KLuaScript& script,int& stage,int& level)
{
    PlayerState before=player;int top=Lua_GetTopIndex(script.m_LuaState);
    CHECK(script.CallFunction("GetPlayerTaskState",2,""));
    CHECK(Lua_GetTopIndex(script.m_LuaState)==top+2);
    stage=N(script.m_LuaState,-2);level=N(script.m_LuaState,-1);
    Lua_SetTopIndex(script.m_LuaState,top);
    CHECK(stage>=0&&stage<=3);CHECK(level>=0&&level<=100);
    CHECK(!memcmp(&before,&player,sizeof(before))); // no grants, state mutations or money
    return true;
}
static bool QuestCallbackChecks(const char* root)
{
    char manifest[MAX_PATH];sprintf(manifest,"%s\\callback_test_paths.txt",root);
    FILE* f=fopen(manifest,"rt");CHECK(f!=NULL);char line[512];int loaded=0;
    TLua_Funcs api[]={{"GetTask",GetTask},{"SetTask",SetTask},{"HaveNormalItem",Have},
        {"HaveEventItem",EventHave},{"GetItemCount",ItemCountAny},{"GetLevel",Level},{"GetPlayerType",Type},
        {"GetCamp",Camp},{"LocalSystemTime",MarkerClock},{"GetByte",MarkerByte},{"IsMaster",Zero},
        {"GetWorldPos",MarkerWorld},{"GetContentWorldPos",MarkerContentWorld},{"SubWorldIdx2ContentID",MarkerContentWorld}};
    while(fgets(line,sizeof(line),f))
    {
        int len=strlen(line);while(len&&(line[len-1]=='\r'||line[len-1]=='\n'))line[--len]=0;
        if(!len)continue;
        KLuaScript script;CHECK(script.Init());CHECK(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
        CHECK(script.Load(line));++loaded;
        for(int type=0;type<3;++type)for(int variant=0;variant<5;++variant)
        {
            Reset("all bound marker callbacks are read-only");player.playerType=type;player.level=variant==0?1:variant==1?3:variant==2?7:variant==3?12:35;
            if(variant>=2){for(int task=0;task<60;++task)player.task[task]=variant-1;
                Seed(3,10,10);Seed(3,12,10);Seed(3,13,10);Seed(4,26,1);}
            int stage,level;CHECK(ReadMark(script,stage,level));
        }
        if(strstr(line,"warehouse")){}
        script.Exit();
    }
    fclose(f);CHECK(loaded==31);
    return true;
}
static bool JournalAndWireChecks()
{
    Reset("journal full IDs, reward vs return, safe dynamic values");
    CHECK(PhongThanTaskRecordId(15,"Task 51 - step 2: return")==51);
    CHECK(PhongThanTaskRecordId(30,"Task 894 - step 5")==894);
    CHECK(PhongThanTaskRecordId(7,"original exact text")==7);
    CHECK(PhongThanTaskRecordId(7,"Task 51 - step 2")==7);
    CHECK(PhongThanTaskRewardStep(8,8));CHECK(!PhongThanTaskRewardStep(8,7));
    CHECK(!PhongThanTaskRewardStep(42,8));CHECK(!PhongThanTaskRewardStep(852,1));
    CHECK(!PhongThanTaskRewardStep(51,2));CHECK(PhongThanTaskRewardStep(7,10));
    char text[512];PhongThanFormatTaskStep(text,sizeof(text),"find %s / %d","Task 12 - step 0: material, 10");CHECK(!strcmp(text,"find material / 10"));
    PhongThanFormatTaskStep(text,sizeof(text),"find %s","Task 12 - step 0: %n");CHECK(!strcmp(text,"find %n"));
    char shortBuffer[5];PhongThanFormatTaskStep(shortBuffer,sizeof(shortBuffer),"%s","Task 12 - step 0: abcdef");CHECK(!strcmp(shortBuffer,"abcd"));
    CHECK(PhongThanQuestMarkVariant(0,0,20)==-1);CHECK(PhongThanQuestMarkVariant(1,10,15)==0);
    CHECK(PhongThanQuestMarkVariant(1,10,16)==1);CHECK(PhongThanQuestMarkVariant(2,10,100)==2);
    CHECK(PhongThanQuestMarkVariant(3,10,15)==3);CHECK(PhongThanQuestMarkVariant(3,10,16)==4);
    KPhongThanScriptAction action;memset(&action,0,sizeof(action));action.Operation=PHONGTHAN_SCRIPT_SHOW;
    action.View=UI_NOTEINFO;action.ServerOwned=1;action.MapId=1002;action.NumberArgument=PT_NPC_QUEST_MARK_NOTE;
    strcpy(action.Key,PT_NPC_QUEST_MARK_KEY);strcpy(action.Content,"123:2;124:3;");action.ContentLength=strlen(action.Content);
    unsigned char packet[8192];unsigned int size=PhongThanBuildScriptPacket(action,packet,sizeof(packet));CHECK(size>0);
    CHECK(PhongThanValidateUiAction(packet,size));KPhongThanScriptAction decoded;
    CHECK(PhongThanReadScriptPacket(packet,size,&decoded));CHECK(decoded.NumberArgument==PT_NPC_QUEST_MARK_NOTE);
    CHECK(!strcmp(decoded.Key,action.Key)&&!strcmp(decoded.Content,action.Content));CHECK(decoded.MapId==1002);
    CHECK(PhongThanReceiveNpcQuestMarks(decoded));CHECK(PhongThanGetNpcQuestMark(123,1002)==2);
    CHECK(PhongThanGetNpcQuestMark(124,1002)==3);CHECK(PhongThanGetNpcQuestMark(123,1003)==-1);
    strcpy(decoded.Content,"123:1;123:3;");decoded.ContentLength=strlen(decoded.Content);
    CHECK(PhongThanReceiveNpcQuestMarks(decoded));CHECK(PhongThanGetNpcQuestMark(123,1002)==2);
    strcpy(decoded.Content,"123:9;");decoded.ContentLength=strlen(decoded.Content);
    CHECK(PhongThanReceiveNpcQuestMarks(decoded));CHECK(PhongThanGetNpcQuestMark(123,1002)==2);
    questTick+=6001;CHECK(PhongThanGetNpcQuestMark(123,1002)==-1);
    decoded.Content[0]=0;decoded.ContentLength=0;CHECK(PhongThanReceiveNpcQuestMarks(decoded));CHECK(s_PTQuestMarkCount==0);
    return true;
}
int main(int argc,char**argv)
{
    if(argc!=2)return 2;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    if(!SetCurrentDirectoryA(argv[1]))return 3;g_SetRootPath(NULL);g_SetFilePath("\\");
    if(!QuestCallbackChecks(argv[1]) || !JournalAndWireChecks())return 4;
    printf("PASS QUEST_MARK_UI callbacks=31 assertions=%d cases=%d runtime_writes=NONE\n",assertions,cases);return 0;
}
