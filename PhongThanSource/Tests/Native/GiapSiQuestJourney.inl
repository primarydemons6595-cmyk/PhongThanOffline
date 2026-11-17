// Independent player journeys against the emitted Lua4 payload and mocked
// player APIs. Not a claim of live UI, persistence, or combat verification.
static int world=1002,now=1789516800,spawned=100,failSpawn=0;
static int ContentWorld(Lua_State*L){Lua_PushNumber(L,world-1000);Lua_PushNumber(L,190*8+4);Lua_PushNumber(L,206*16+8);return 3;}
static int World(Lua_State*L){return Result(L,world);}
static int TimeNow(Lua_State*L){return Result(L,now);}
static int Pay(Lua_State*L){int n=N(L,1);if(player.money<n)return Result(L,0);player.money-=n;return Result(L,1);}
static int Spawn(Lua_State*L){return Result(L,failSpawn?0:++spawned);}
static int One(Lua_State*L){return Result(L,1);}
static int ByteGet(Lua_State*L){return Result(L,(N(L,1)>>((N(L,2)-1)*8))&255);}
static int ByteSet(Lua_State*L){int shift=(N(L,2)-1)*8;return Result(L,(N(L,1)&~(255<<shift))|((N(L,3)&255)<<shift));}
static char journalText[100][512];static int journalAct[100];
static int morphType=0,weightBonus=0;
static int Morph(Lua_State*L){return Result(L,morphType);}
static int WeightReward(Lua_State*L){if(player.task[92]==6&&player.task[4572]==0&&N(L,1)==20){weightBonus+=20;player.task[4572]=1;return Result(L,1);}return Result(L,0);}
static int DebitCredit(Lua_State*L){player.credit-=N(L,1);return 0;}
static int JournalNote(Lua_State*L){
 const char* key=Lua_ValueToString(L,1);int id=key?atoi(key):-1;
 if(id>=3000&&id<=3099){const char* text=Lua_ValueToString(L,3);if(!text||strlen(text)>510)lua_error(L,"journal text overflow");strcpy(journalText[id-3000],text);journalAct[id-3000]=N(L,2);}
 return 0;
}
static int LegacySay(Lua_State*L){for(int i=3;i<=lua_gettop(L);++i){const char*p=Lua_ValueToString(L,i);if(p){const char*s=strchr(p,'/');if(s)Offer(L,s+1);}}return 0;}
static int Buttons(Lua_State*L){
 int total=N(L,2),seen=0;
 if(lua_istable(L,3)){
  for(int i=1;i<=lua_getn(L,3);++i){lua_rawgeti(L,3,i);const char*p=Lua_ValueToString(L,-1);if(p){const char*s=strchr(p,'/');if(!s||s-p>13)lua_error(L,"button label would be clipped");Offer(L,s+1);++seen;}lua_pop(L,1);}
 }else for(int i=3;i<=lua_gettop(L);++i){const char*p=Lua_ValueToString(L,i);if(p){const char*s=strchr(p,'/');if(!s||s-p>13)lua_error(L,"button label would be clipped");Offer(L,s+1);++seen;}}
 if(seen!=total)lua_error(L,"button count mismatch");
 return 0;
}
static bool Eval(const char*code){CHECK(Lua_ExecuteBuffer(scripts[0].m_LuaState,code,strlen(code),"journey")==0);return true;}
static void Role(const char*role,int map){world=map;lua_pushstring(scripts[0].m_LuaState,role);lua_setglobal(scripts[0].m_LuaState,"PTG_ROLE");lua_pushnumber(scripts[0].m_LuaState,map);lua_setglobal(scripts[0].m_LuaState,"PTG_MAP");}
static bool Step(int id,const char*role,int map){Role(role,map);char fn[40];CHECK(Call(0,"main"));sprintf(fn,"ptg_a%d",id);bool available=Offered(fn);if(available)CHECK(Call(0,fn,true));sprintf(fn,"ptg_yes%d",id);CHECK(Call(0,fn,available));return true;}
static bool Kill(int id,int map){world=map;char code[64];sprintf(code,"PTGDeath(%d)",id);return Eval(code);}
static bool Journeys(){
 Reset("TAN THUC actual NPC order and objective gate");player.level=3;CHECK(Step(1,"lo",1002));CHECK(Step(2,"au",1002));CHECK(Step(3,"buu",1002));Role("au",1002);CHECK(Call(0,"ptg_yes4"));CHECK(player.task[21]==3);
 Seed(3,10,10);CHECK(Step(4,"au",1002));CHECK(CountItem(3,10)==0);CHECK(Step(5,"hau",1002));CHECK(Step(6,"lo",1002));CHECK(Step(7,"hac",1002));CHECK(Step(8,"au",1002));CHECK(Step(9,"lo",1002));CHECK(player.task[21]==9&&player.money==600&&player.exp==500);CHECK(Call(0,"ptg_yes9"));CHECK(player.money==600);
 Reset("DUNG DAO every part, repair and reward");player.level=14;CHECK(Step(10,"loan",1002));Seed(4,21,1);Seed(4,22,1);CHECK(Step(11,"dien",1002));CHECK(player.task[25]==1);Seed(4,23,1);CHECK(Step(11,"dien",1002));CHECK(Step(12,"au",1002));CHECK(CountItem(4,21)==0&&CountItem(4,24)==1);CHECK(Step(13,"dien",1002));CHECK(player.task[25]==4&&player.credit==10);
 Reset("KIEM AI level14 and real death objective");player.level=13;Role("loan",1002);CHECK(Call(0,"ptg_yes14"));CHECK(player.task[24]==0);player.level=14;CHECK(Step(14,"loan",1002));CHECK(Step(15,"au",1002));CHECK(Kill(576,1006));CHECK(player.task[24]==2);CHECK(Kill(576,1007));CHECK(Step(16,"loan",1002));CHECK(player.exp==3000&&CountItem(5,0)==5&&player.task[24]==4);
 Reset("TEN MAINLINE QUESTS full state journey");player.level=92;
 CHECK(Step(17,"hau",1002));CHECK(Step(18,"trinh",1002));CHECK(Step(19,"hau",1002));CHECK(player.task[3]==10&&player.exp==20000&&player.money==30000);
 CHECK(Step(20,"hac",1002));CHECK(Step(21,"hoang",1021));CHECK(player.exp==100000);
 CHECK(Step(22,"hoang",1021));CHECK(Step(23,"ly",1065));CHECK(Step(25,"dang",1016));CHECK(Step(27,"hoang",1021));CHECK(Kill(122,1018));CHECK(Step(28,"hoang",1021));CHECK(Step(29,"khuong",1020));CHECK(player.exp==340000&&player.task[3]==30);
 CHECK(Step(30,"duong",1020));CHECK(Kill(113,1039));CHECK(Step(31,"dac",1021));CHECK(Step(32,"ho",1021));CHECK(player.exp==940000&&CountItem(4,9)==1);
 CHECK(Step(33,"dac",1021));CHECK(Step(34,"tay",1052));CHECK(Kill(121,1045));CHECK(Step(35,"tay",1052));CHECK(player.exp==2340000&&player.task[3]==50);
 CHECK(Step(36,"nong",1064));CHECK(Kill(120,1047));CHECK(Step(37,"nong",1064));CHECK(player.exp==4340000&&CountItem(4,4)==1);
 CHECK(Step(38,"dac",1021));CHECK(CountItem(4,9)==3);CHECK(Step(39,"khuong",1020));CHECK(Step(40,"vo",1020));CHECK(CountItem(4,9)==1);
 CHECK(Kill(117,1048));CHECK(Kill(118,1049));CHECK(Step(41,"vo",1020));CHECK(player.task[3]==63);CHECK(Kill(119,1050));CHECK(Step(41,"vo",1020));CHECK(player.task[3]==70&&player.exp==7340000);
 CHECK(Step(42,"tay",1052));CHECK(Step(43,"dac_future",1063));CHECK(Step(44,"nguyen",1062));CHECK(Kill(116,1062));CHECK(Step(45,"nguyen",1062));CHECK(player.task[3]==80&&player.exp==12340000);
 CHECK(Step(46,"dac_young",1061));CHECK(Step(47,"khuong_young",1061));CHECK(Step(48,"dac_young",1061));CHECK(Kill(82,1051));CHECK(Step(49,"dac_young",1061));CHECK(player.exp==18840000&&player.task[3]==83);
 CHECK(Step(50,"khuong_young",1061));CHECK(Step(51,"merchant",1034));CHECK(Step(52,"cuudi",1016));Role("item",1006);CHECK(Eval("PTG_ITEM=142;ptg_find_cruel()"));CHECK(CountItem(4,178)==1);CHECK(Step(53,"thong",1070));CHECK(player.task[3]==86);
 CHECK(Eval("ptg_souls()"));CHECK(player.task[3]==87);CHECK(Kill(541,1070));CHECK(Kill(541,1070));CHECK(player.task[4562]==1);CHECK(Kill(542,1070));CHECK(Kill(543,1070));CHECK(player.task[3]==88);CHECK(Kill(2703,1070));CHECK(player.task[3]==89&&CountItem(4,174)==1);CHECK(Step(55,"khuong_young",1061));CHECK(player.task[3]==90);CHECK(Call(0,"ptg_yes54"));CHECK(player.task[3]==90&&CountItem(0,6,6,9)==1&&CountItem(0,5,6,9)==0);
 Reset("TRU HOA cannot skip regions");player.level=20;Role("doctor_tam",1016);CHECK(Call(0,"ptg_yes59"));CHECK(player.task[4550]==0);return true;
}
static bool HopGamCurrent(){
 Reset("HOP GAM current runtime NPCs through new button service bridge");player.level=10;world=1002;
 CHECK(Visit(6,"renwu1"));CHECK(Call(6,"yes_1",true));CHECK(Visit(7,"renwu3"));CHECK(player.task[20]==10);
 CHECK(Call(8,"main"));CHECK(Call(8,"ptg_services",true));CHECK(Call(8,"renwu1",true));CHECK(player.task[20]==11);
 CHECK(Visit(9,"renwu1"));CHECK(player.task[20]==13);
 CHECK(Call(10,"main"));CHECK(Call(10,"ptg_services",true));CHECK(Call(10,"renwu1",true));CHECK(player.task[20]==17);
 CHECK(Visit(7,"renwu3"));CHECK(CountItem(4,26)==1);CHECK(Visit(6,"renwu1"));CHECK(player.task[20]==19&&CountItem(4,26)==0&&player.grants==6);
 return true;
}
static bool ExtraCases(){
 Reset("TRU HOA three regions");player.level=20;CHECK(Step(57,"doctor_manh",1015));CHECK(Kill(13,1015));CHECK(CountItem(4,191)==0);CHECK(Kill(7,1015));now+=11;CHECK(Kill(13,1015));CHECK(CountItem(4,191)==0);CHECK(Kill(7,1015));for(int i=0;i<5;i++)CHECK(Kill(13,1015));CHECK(Step(58,"doctor_manh",1015));CHECK(player.exp==6000);CHECK(Step(59,"doctor_tam",1016));world=1016;CHECK(Eval("for i=0,9 do PTGGather(1,2^i) end; PTGGather(1,1)"));CHECK(CountItem(4,192)==10);CHECK(Step(60,"doctor_tam",1016));CHECK(player.exp==14000);CHECK(Step(61,"doctor_dong",1014));world=1014;CHECK(Eval("PTGGather(2,1)"));CHECK(CountItem(4,201)==0);Seed(6,1,1,9201,1);CHECK(Eval("PTG_ITEM=9201;ptg_item_main();for i=0,4 do PTGGather(2,2^i) end"));CHECK(CountItem(4,201)==5);CHECK(Step(62,"doctor_dong",1014));CHECK(player.exp==24000&&player.task[4550]==6);
 Reset("GIAO HUAN three accepts per Saigon day and one final lifetime");player.level=20;Role("giao",1002);
 for(i=0;i<3;i++){CHECK(Call(0,"ptg_giao_take"));CHECK(player.task[4554]==3);for(int k=0;k<40;k++)CHECK(Kill(k%2?3:0,1007));CHECK(player.task[4555]==40);Role("giao",1002);CHECK(Call(0,"ptg_giao_claim"));CHECK(player.task[4554]==0);}CHECK(player.money==9000&&player.exp==13800);CHECK(Call(0,"ptg_giao_take"));CHECK(player.task[4554]==0);now+=86400;CHECK(Call(0,"ptg_giao_take"));CHECK(player.task[4554]==3);
 player.task[4554]=0;player.level=29;CHECK(Call(0,"ptg_giao_take"));CHECK(player.task[4554]==7&&player.task[4558]==1);for(i=0;i<40;i++)CHECK(Kill(7,1007));Role("giao",1002);CHECK(Call(0,"ptg_giao_claim"));CHECK(Call(0,"ptg_giao_take"));CHECK(player.task[4554]==0);
 Reset("Full bag retains requirements and wallet, no repeated completion");player.level=92;player.task[3]=11;Seed(4,45,1);player.freeCells=0;Role("hoang",1021);CHECK(Call(0,"ptg_yes21"));CHECK(player.task[3]==11&&CountItem(4,45)==1&&player.exp==0);player.freeCells=10;CHECK(Call(0,"ptg_yes21"));CHECK(player.task[3]==20&&player.exp==80000);CHECK(Call(0,"ptg_yes21"));CHECK(player.exp==80000);
 Reset("Wrong class and wrong world cannot advance");player.level=92;player.playerType=1;Role("hau",1002);CHECK(Call(0,"ptg_yes17"));CHECK(player.task[3]==0);player.playerType=0;world=1021;CHECK(Call(0,"ptg_yes17"));CHECK(player.task[3]==0);
 Reset("Thong Thien failure keeps prior mainline progress");player.level=92;player.task[3]=88;player.task[4563]=now-1;Role("thong",1070);CHECK(Kill(2703,1070));CHECK(player.task[3]==88&&CountItem(4,174)==0);CHECK(Call(0,"ptg_challenge"));CHECK(player.task[3]==83&&player.task[4562]==0);
 Reset("Soul spawn failure cannot lose stage");player.level=92;player.task[3]=86;Seed(6,1,1,34,1);world=1070;failSpawn=1;CHECK(Eval("ptg_souls()"));CHECK(player.task[3]==86);failSpawn=0;
 return true;
}
static bool JournalCases(){
 Reset("Accepted quest creates persistent literal instructions and gated buttons");player.level=3;
 CHECK(Step(1,"lo",1002));CHECK(journalAct[0]==1&&strstr(journalText[0],"(206,193)"));
 CHECK(Step(2,"au",1002));CHECK(Step(3,"buu",1002));Role("au",1002);CHECK(Call(0,"main"));
 CHECK(!Offered("ptg_a4"));Seed(3,10,10);CHECK(Call(0,"main"));CHECK(Offered("ptg_a4"));
 CHECK(Eval("PTGJournal()"));CHECK(strstr(journalText[0],"10/10"));
 Reset("Old remote Giao Huan migrates without losing kills; remote kills do not count");player.level=20;player.task[4554]=13;player.task[4555]=17;player.task[4559]=1015;
 Role("giao",1002);CHECK(Call(0,"main"));CHECK(player.task[4559]==1007&&player.task[4555]==17);CHECK(strstr(journalText[5],"Y")&&strstr(journalText[5],"17/40"));
 CHECK(Kill(13,1015));CHECK(player.task[4555]==17);CHECK(Kill(3,1007));CHECK(player.task[4555]==18&&strstr(journalText[5],"18/40"));
 Role("giao",1002);CHECK(Call(0,"main"));CHECK(!Offered("ptg_giao_claim"));player.task[4555]=40;CHECK(Call(0,"main"));CHECK(Offered("ptg_giao_claim"));
 Reset("Instructor opens on content map and calls a supported master API");world=1002;player.level=20;
 CHECK(Call(12,"main"));CHECK(Offered("jineng")&&Offered("mission_PR_intro"));
 return true;
}
static bool SharedEval(const char*code){CHECK(Lua_ExecuteBuffer(scripts[11].m_LuaState,code,strlen(code),"shared journey")==0);return true;}
static void SharedRole(const char*role,int map){world=map;lua_pushstring(scripts[11].m_LuaState,role);lua_setglobal(scripts[11].m_LuaState,"PTS_ROLE");lua_pushnumber(scripts[11].m_LuaState,map);lua_setglobal(scripts[11].m_LuaState,"PTS_MAP");}
static bool SharedStep(int n,const char*role,int map){SharedRole(role,map);char code[64];sprintf(code,"PTSApply(%d)",n);return SharedEval(code);}
static bool SharedKill(int n,int map){world=map;char code[64];sprintf(code,"PTSDeath(%d)",n);return SharedEval(code);}
static bool SharedCases(){
 for(int type=0;type<3;type++){
 Reset("Shared Phi Tien available to all three classes, costs and unlock");player.playerType=type;player.level=39;player.money=2000;
 CHECK(SharedStep(1,"west",1020));CHECK(player.task[51]==1);player.freeCells=0;CHECK(SharedStep(2,"west_boi",1020));CHECK(player.money==2000&&player.task[51]==1);player.freeCells=20;
 CHECK(SharedStep(2,"west_boi",1020));CHECK(player.money==1000&&CountItem(3,29)==1&&player.task[51]==2);CHECK(SharedStep(3,"cao",1004));CHECK(SharedKill(108,1022));CHECK(player.task[51]==4&&CountItem(4,37)==1);CHECK(SharedStep(4,"west",1020));CHECK(player.task[51]==5&&CountItem(4,37)==0);
 Reset("Shared Yen Phuc truthful and purchase branches");player.playerType=type;player.level=23;
 CHECK(SharedStep(20,"tong",1021));CHECK(SharedStep(21,"east_boi",1021));CHECK(SharedStep(22,"tong",1021));CHECK(player.task[91]==5&&player.money==5000);CHECK(SharedStep(22,"tong",1021));CHECK(player.money==5000);
 player.task[91]=2;player.money=300;CHECK(SharedStep(23,"tong",1021));CHECK(player.money==0&&player.task[91]==3);CHECK(SharedStep(24,"east_boi",1021));CHECK(player.task[91]==4&&player.credit==-1&&CountItem(6,1,12,1)==0);
 Reset("Shared Vi Lao exact rewards and repeat");player.playerType=type;player.level=57;
 CHECK(SharedStep(5,"tieu",1020));CHECK(SharedStep(6,"herbalist",1020));CHECK(SharedKill(104,1010));CHECK(player.task[50]==3);CHECK(SharedStep(7,"vocat",1020));CHECK(SharedStep(8,"xich",1003));CHECK(SharedKill(114,1043));Seed(4,39,1);Seed(4,41,2);
 CHECK(SharedStep(9,"xich",1003));CHECK(player.task[50]==6&&CountItem(4,41)==2);CHECK(SharedStep(10,"vocat",1020));CHECK(CountItem(4,41)==0&&player.task[50]==7);CHECK(SharedStep(11,"xich",1003));CHECK(player.task[50]==8&&player.exp==500&&CountItem(4,41)==1&&CountItem(3,28)>=1&&CountItem(3,28)<=3);CHECK(SharedStep(11,"xich",1003));CHECK(player.exp==500);CHECK(SharedStep(12,"vocat",1020));CHECK(player.task[50]==4);
 Reset("Shared Phu The forms, permanent endurance and once-only reward");player.playerType=type;player.level=27;weightBonus=0;morphType=0;
 CHECK(SharedStep(13,"nham_tau",1020));CHECK(SharedStep(14,"duong",1020));CHECK(player.task[92]==2&&CountItem(6,1,11,1)==3);CHECK(SharedStep(15,"nham_ca",1020));CHECK(player.task[92]==2);morphType=50;CHECK(SharedStep(15,"nham_ca",1020));morphType=34;CHECK(SharedStep(17,"nham_ca",1020));CHECK(player.task[92]==5);morphType=249;CHECK(SharedStep(19,"nham_tau",1020));CHECK(player.task[92]==6&&weightBonus==20&&player.exp==5000);CHECK(SharedStep(19,"nham_tau",1020));CHECK(weightBonus==20&&player.exp==5000);
 Reset("Shared Minh Chau three fish, required minerals, money and repeat");player.playerType=type;player.level=43;
 CHECK(SharedStep(32,"gambler",1021));CHECK(player.task[41]==0&&player.money==0);
 player.money=100;CHECK(SharedStep(32,"gambler",1021));CHECK(player.task[41]==0&&player.money==0);
 CHECK(SharedStep(25,"gambler",1021));CHECK(SharedStep(26,"pawn",1021));CHECK(SharedStep(27,"library",1001));CHECK(SharedStep(28,"culuu",1021));CHECK(SharedKill(110,1037));CHECK(SharedKill(111,1037));CHECK(SharedKill(112,1037));CHECK(player.task[41]==27);CHECK(SharedStep(31,"culuu",1021));CHECK(player.task[41]==27&&player.money==0);Seed(3,82,3);Seed(3,3,10);CHECK(SharedStep(31,"culuu",1021));CHECK(player.task[41]==10&&player.money==5000);CHECK(SharedStep(31,"culuu",1021));CHECK(player.money==5000);CHECK(SharedStep(29,"pawn",1021));CHECK(SharedStep(30,"culuu",1021));CHECK(player.task[41]==20);
 Reset("Shared Tu Tuong assigned materials and guaranteed reward");player.playerType=type;player.level=31;SharedRole("store_west",1020);CHECK(Call(11,"pts_elements"));CHECK(player.task[55]>=22&&player.task[55]<=25);int target=player.task[55];for(int k=0;k<10;k++)CHECK(SharedKill(target-15,1018));CHECK(CountItem(3,target)==10);SharedRole("store_east",1021);CHECK(Call(11,"pts_elements"));CHECK(player.task[55]==0&&player.money==1200&&player.credit==5&&CountItem(3,target)==0);
 Reset("Shared Trung Moc exchanges all required ingredients");player.playerType=type;player.level=19;SharedRole("west",1020);Seed(0,10,1,0,1);Seed(3,29,1);Seed(4,39,3);CHECK(Call(11,"pts_mount_apply"));CHECK(CountItem(4,39)==0&&CountItem(3,29)==0&&CountItem(0,10,5000,1)==1);
 }
 Reset("Shared 36 actual tower and death payloads, final guide reward");player.level=59;player.playerType=2;player.freeCells=500;
 const int maps[]={1006,1007,1012,1013,1010,1009};
 for(int n=0;n<36;++n){SharedRole("duong",1020);CHECK(Call(11,"pts_stars"));CHECK((player.task[53]&255)==1);world=maps[n/6];CHECK(Visit(13+n/6,"pts_tower"));CHECK((player.task[53]&255)==2);CHECK(Call(20+n,"OnDeath"));CHECK((player.task[53]&255)==3&&CountItem(4,44)==1);SharedRole("duong",1020);CHECK(Call(11,"pts_stars"));CHECK(((player.task[53]>>8)&255)==n+1&&CountItem(4,44)==0);}
 CHECK(player.exp==40000&&CountItem(3,45)==1&&player.credit==143);CHECK(Call(11,"pts_stars"));CHECK(player.exp==40000&&CountItem(3,45)==1);
 return true;
}

