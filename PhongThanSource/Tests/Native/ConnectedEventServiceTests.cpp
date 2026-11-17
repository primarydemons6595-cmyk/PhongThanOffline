// Exercise recovered Lua with the production event API implementations. Player
// persistence/currency/inventory boundaries are explicit in-memory test hosts.
#define main EventApiContractMain
#include "EventLuaCompatTests.cpp"
#undef main
#include <vector>
static int tasks[5000],buffs[512],cash=1000000,experience=0,credit=0,level=60,bag=0,morph=-1;
static int runtimeWorld=1002,randomOutcome=100,capturedRoll=-1;
static int HostRoulette(Lua_State*L){capturedRoll=(int)Lua_ValueToNumber(L,1);return LuaRouletteCompat(L);}
static int HostGetTask(Lua_State*L){int i=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,i>=0&&i<5000?tasks[i]:0);return 1;}
static int HostSetTask(Lua_State*L){int i=(int)Lua_ValueToNumber(L,1);if(i>=0&&i<5000)tasks[i]=(int)Lua_ValueToNumber(L,2);return 0;}
static int HostZero(Lua_State*L){Lua_PushNumber(L,0);return 1;}
static int HostNoop(Lua_State*){return 0;}
static int HostWorld(Lua_State*L){Lua_PushNumber(L,runtimeWorld);return 1;}
static int HostContentWorld(Lua_State*L){Lua_PushNumber(L,runtimeWorld-1000);return 1;}
static int HostName(Lua_State*L){Lua_PushString(L,"ServiceTest");return 1;}
static int HostLevel(Lua_State*L){Lua_PushNumber(L,level);return 1;}
static int HostCash(Lua_State*L){Lua_PushNumber(L,cash);return 1;}
static int HostPay(Lua_State*L){int n=(int)Lua_ValueToNumber(L,1);int ok=n>0&&cash>=n;if(ok)cash-=n;Lua_PushNumber(L,ok);return 1;}
static int HostEarn(Lua_State*L){cash+=(int)Lua_ValueToNumber(L,1);return 0;}
static int HostExp(Lua_State*L){experience+=(int)Lua_ValueToNumber(L,1);return 0;}
static int HostCredit(Lua_State*L){credit+=(int)Lua_ValueToNumber(L,1);return 0;}
static int HostTime(Lua_State*L){Lua_PushNumber(L,200000);return 1;}
static int HostBuff(Lua_State*L){int i=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,i>=0&&i<512?buffs[i]:0);return 1;}
static int HostBuffCount(Lua_State*L){int count=0;for(int i=0;i<512;++i)if(buffs[i])++count;Lua_PushNumber(L,count);return 1;}
static int HostAddBuff(Lua_State*L){int i=(int)Lua_ValueToNumber(L,1);if(i>=0&&i<512)buffs[i]=1;Lua_PushNumber(L,1);return 1;}
static int HostRemoveBuff(Lua_State*L){int i=(int)Lua_ValueToNumber(L,1);if(i>=0&&i<512)buffs[i]=0;return 0;}
static int HostCreature(Lua_State*L){Lua_PushNumber(L,0);Lua_PushNumber(L,-1);return 2;}
static int HostMorph(Lua_State*L){morph=(int)Lua_ValueToNumber(L,1);return 0;}
static int HostGetMorph(Lua_State*L){Lua_PushNumber(L,morph);return 1;}
static int HostHaveItem(Lua_State*L){Lua_PushNumber(L,(int)Lua_ValueToNumber(L,2)==137?bag:0);return 1;}
static int HostFindItem(Lua_State*L){Lua_PushNumber(L,bag>0?137:0);return 1;}
static int HostCostItem(Lua_State*L){int ok=bag>0&&(int)Lua_ValueToNumber(L,1)==137;if(ok)--bag;Lua_PushNumber(L,ok);return 1;}
static int HostRandom(Lua_State*L){int a=(int)Lua_ValueToNumber(L,1),b=(int)Lua_ValueToNumber(L,2);Lua_PushNumber(L,a==1&&b==100?randomOutcome:a);return 1;}
static int HostClose(Lua_State*){Player[1].m_UiDialogToken=0;Player[1].m_UiDialogView=0;return 0;}
static int Hook(DWORD,char *callback,int value){return testScript->CallFunction(callback,0,"d",value)?1:0;}
struct Service {KLuaScript *script;int world,shop;};
static void Select(Service &service){testScript=service.script;runtimeWorld=service.world;Npc[1].m_SubWorldIndex=service.shop?service.shop-1:5;SubWorld[Npc[1].m_SubWorldIndex].m_SubWorldID=service.world;}
static int Input(Service &service,const char *callback,int quantity){
    Select(service);if(!service.script->CallFunction((char*)callback,0,""))return 0;
    PHONGTHAN_UI_NUMBER_REQUEST request;memset(&request,0,sizeof(request));request.MapId=service.world;request.DialogToken=Player[1].m_UiDialogToken;request.Value=quantity;
    if(!request.DialogToken)return 0;
    const int before=executed;
    if(!PhongThanEventNumberInput(Player[1],request)||executed!=before+1)return 0;
    if(PhongThanEventNumberInput(Player[1],request)||executed!=before+1)return 0;
    return 1;
}
int main(int argc,char **argv){
    if(argc!=2)return 2;FILE *file=fopen(argv[1],"rb");CHECK(file!=NULL);
    TLua_Funcs api[]={
        {"GetWorldPos",HostWorld},{"GetContentWorldPos",HostContentWorld},{"SubWorldIdx2ContentID",HostContentWorld},
        {"GetTask",HostGetTask},{"SetTask",HostSetTask},{"GetLevel",HostLevel},{"GetName",HostName},{"GetCash",HostCash},
        {"Pay",HostPay},{"Earn",HostEarn},{"AddOwnExp",HostExp},{"AddCredit",HostCredit},{"SystemTime",HostTime},
        {"HaveIBBuff",HostBuff},{"GetIBBuffCount",HostBuffCount},{"AddIBBuff",HostAddBuff},{"RemoveIBBuff",HostRemoveBuff},
        {"GetCreatureInfo",HostCreature},{"SetCreatureType",HostNoop},{"PolyMorph",HostMorph},{"GetMorphType",HostGetMorph},
        {"GetTGuardIndexByPlayerName",HostZero},{"HaveNormalItem",HostHaveItem},{"FindAValidIBItem",HostFindItem},{"CostIBItem",HostCostItem},
        {"GetSex",HostZero},{"SetDeathScript",HostNoop},{"AddNormalItem",HostZero},
        {"CloseDialog",HostClose},{"Say",HostNoop},{"SayTask",HostNoop},{"Talk",HostNoop},{"MsgBox",HostNoop},
        {"Msg2Player",HostNoop},{"TopMessage",HostNoop},{"AddGlobalCountNews",HostNoop},{"random",HostRandom},
        {"GetGoodsName",LuaGetGoodsNameCompat},{"GetGoodsWeight",LuaGetGoodsWeightCompat},{"GetGoodsNum",LuaGetGoodsNumCompat},
        {"GetGoodsPurchasePrice",LuaGetGoodsPurchasePriceCompat},{"GetGoodsSalePrice",LuaGetGoodsSalePriceCompat},
        {"ChangeGoodsNum",LuaChangeGoodsNumCompat},{"PlayerSellGoods",LuaPlayerSellGoodsCompat},
        {"InputDialog",LuaInputDialogCompat},{"Roulette",HostRoulette},{"ConsumeRouletteResult",LuaConsumeRouletteResultCompat}};
    Service shops[5];std::vector<Service> astrologers;
    char path[512];int world,shop,count=0;
    while(fscanf(file,"%511s %d %d",path,&world,&shop)==3){
        FILE *source=fopen(path,"rb");CHECK(source!=NULL);fseek(source,0,SEEK_END);int size=ftell(source);rewind(source);
        std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,source)==(unsigned)size);fclose(source);
        Service service;service.script=new KLuaScript;service.world=world;service.shop=shop;
        CHECK(service.script->Init());CHECK(service.script->RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
        CHECK(service.script->LoadBuffer(&bytes[0],size));CHECK(service.script->ExecuteCode());
        if(shop>0&&shop<=5)shops[shop-1]=service;else astrologers.push_back(service);++count;
    }
    fclose(file);CHECK(count==7 && astrologers.size()==2);
    currentPlayer=1;Player[1].index=1;Player[1].m_nIndex=1;Player[1].m_dwID=101;Player[1].m_SelUiScriptId=99;
    Npc[1].m_dwID=201;executeHook=Hook;
    Select(shops[0]);CHECK(shops[0].script->CallFunction("main",0,""));
    CHECK(shops[0].script->CallFunction("camel_level1",0,""));CHECK(cash==999000 && tasks[60]==1 && tasks[332]==1 && buffs[160]==1 && morph==364);
    int purchase=g_PhongThanCommerce.Purchase(1,1,(unsigned long)time(NULL)),before=cash;
    CHECK(Input(shops[0],"buyin1",10));CHECK(tasks[110]==1 && tasks[111]==10 && tasks[120]==10 && cash==before-purchase*10);
    int sale=g_PhongThanCommerce.Sale(2,1,(unsigned long)time(NULL));before=cash;
    CHECK(Input(shops[1],"sellout1",10));CHECK(tasks[110]==0 && tasks[111]==0 && tasks[120]==0 && cash==before+sale*10);
    // Every restored merchant generates a reachable request, accepts its goods,
    // pays the original reward once, and cannot pay at a different merchant.
    for(int i=0;i<5;++i){
        Select(shops[i]);CHECK(shops[i].script->CallFunction("RandomTask",0,""));
        const int good=tasks[334],quantity=tasks[335],destination=tasks[333];CHECK(destination==i+1 && good>=1 && good<=15 && quantity>0);
        const int supplier=(good-1)/3;CHECK(supplier!=i && supplier<5);
        char callback[20];sprintf(callback,"buyin%d",(good-1)%3+1);CHECK(Input(shops[supplier],callback,quantity));
        before=cash;int oldExp=experience;CHECK(shops[supplier].script->CallFunction("complete_task",0,""));CHECK(cash==before && experience==oldExp && tasks[333]==destination);
        Select(shops[i]);CHECK(shops[i].script->CallFunction("complete_task",0,""));CHECK(tasks[333]==0 && tasks[334]==0 && tasks[335]==0 && tasks[120]==0 && cash==before+35000 && experience==oldExp+15000 && tasks[766]==i+1);
        before=cash;oldExp=experience;CHECK(shops[i].script->CallFunction("complete_task",0,""));CHECK(cash==before && experience==oldExp);
    }
    // Extra daily delivery consumes exactly one bag only on a valid request.
    Select(shops[0]);CHECK(shops[0].script->CallFunction("paoshangrenwu",0,""));CHECK(tasks[333]==0);
    bag=1;CHECK(shops[0].script->CallFunction("paoshangrenwu",0,""));CHECK(tasks[333]==1 && bag==0);
    CHECK(shops[0].script->CallFunction("yes_4",0,""));CHECK(tasks[60]==0 && tasks[120]==0 && tasks[110]==0 && tasks[111]==0 && morph==-1);
    // Actual original roulette rewards, all 12 outcomes, protected completion.
    const int draws[12]={1,11,20,29,38,49,56,63,73,83,90,100};
    for(unsigned int a=0;a<astrologers.size();++a)for(int outcome=0;outcome<12;++outcome){
        memset(tasks,0,sizeof(tasks));memset(buffs,0,sizeof(buffs));Select(astrologers[a]);randomOutcome=draws[outcome];before=experience;
        level=19;capturedRoll=-1;
        CHECK(astrologers[a].script->CallFunction("yes_3",0,""));CHECK(capturedRoll==-1&&tasks[73]==0&&tasks[4910]==0);
        level=20;
        CHECK(astrologers[a].script->CallFunction("rolling",0,""));
        CHECK(capturedRoll==outcome && tasks[72]==outcome && tasks[4910]==1 && experience==before);
        PHONGTHAN_UI_NUMBER_REQUEST response;memset(&response,0,sizeof(response));response.MapId=runtimeWorld;response.DialogToken=Player[1].m_UiDialogToken;response.Value=1;
        g_PhongThanRoulettePending[1].created-=180000;
        response.Value=-1;CHECK(PhongThanEventNumberInput(Player[1],response));CHECK(tasks[4910]==1&&experience==before);
        response.Value=1;CHECK(PhongThanEventNumberInput(Player[1],response));
        CHECK(tasks[72]==0 && tasks[4910]==0 && tasks[73]==200000 && experience>before);
        CHECK(!PhongThanEventNumberInput(Player[1],response));
        before=experience;int oldCash=cash,oldCredit=credit;CHECK(astrologers[a].script->CallFunction("Finished",0,""));CHECK(experience==before && cash==oldCash && credit==oldCredit);
    }
    for(int j=0;j<5;++j)delete shops[j].script;for(unsigned int k=0;k<astrologers.size();++k)delete astrologers[k].script;
    printf("PASS CONNECTED_EVENT_SERVICES checks=%d merchants=5 rent_buy_travel_sell=PASS delivery=5 duplicate_rewards=BLOCKED roulette_outcomes=24\n",checks);
    puts("SCOPE production event APIs + actual recovered Lua; currency/inventory/persistence are explicit test hosts, not live-player acceptance");return 0;
}
