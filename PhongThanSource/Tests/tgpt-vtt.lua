PlayerIndex=1
tasks={{},{}}
menus=0
lastCount=0
xp=0
cash=0
nativeExtra={}
function GetTask(k) return tasks[PlayerIndex][k] or 0 end
function SetTask(k,v) tasks[PlayerIndex][k]=v end
function GetLevel() return 90 end
function GetPlayerType() return 0 end
function GetWorldPos() return 1021,100,100 end
function GetContentWorldPos() return 67,100,100 end
function SubWorldIdx2ContentID() return 67 end
function SystemTime() return 1700000000 end
function Say(text,n,...) menus=menus+1;lastCount=n end
function SayButtons(text,n,...) menus=menus+1;lastCount=n end
function SayTask(text,options) menus=menus+1;lastCount=getn(options) end
function Talk(...) end
function MsgBox(...) end
function Msg2Player(...) end
function Msg2MSAll(...) end
function Msg2SubWorld(...) end
function AddGlobalCountNews(...) end
function GetName() return "Test" end
function TopMessage(...) end
function CloseDialog() end
function Earn(n) cash=cash+n end
function AddOwnExp(n) xp=xp+n end
function GetBit() return 0 end
function SetBit() return 1 end
function EventVttExpMultiplier() return 2 end
function EventVttExtra(op) tinsert(nativeExtra,op);return 1 end
function EventVttGreen(slot) lastGreen=slot;return 1 end
function EventVttExchange(id,profession) lastExchange={id,profession};return 1 end
function EventVttMerge(tier) lastMerge=tier;return 1 end
function EventVttBuyLottery(count) lastBuy=count;return 1 end
Include("script/phongthan/van_tien/thien_hung.lua")
main()
assert(lastCount==5)
tasks[1][405]=7;tasks[1][406]=1;tasks[1][408]=100000
assert(reward_normal()==1)
assert(xp==252000 and cash==20000 and tasks[1][405]==8 and tasks[1][406]==35)
assert(nativeExtra[1]==0 and nativeExtra[2]==1)
tasks[1][408]=0
for i=1,5 do pt_green(i);assert(lastGreen==i) end
pt_chau();assert(lastCount==5)
for id=1,4 do
 pt_chau_class(id);assert(lastCount==4)
 for profession=0,2 do pt_chau_confirm(id,profession);assert(lastCount==2);pt_chau_do(id,profession);assert(lastExchange[1]==id and lastExchange[2]==profession) end
end
Include("script/phongthan/npc_services/xich_tung_tu.lua")
main();assert(lastCount==7)
pt_vtt_merge_menu();assert(lastCount==4)
for tier=1,3 do pt_vtt_merge(tier);assert(lastMerge==tier) end
Include("script/phongthan/van_tien/ung_tiem_thuong.lua")
main();assert(lastCount==3);yes_1();assert(lastBuy==1);yes_5();assert(lastBuy==5)
function GetTeam() return 0 end
function GetNpcTemplateID() return 129 end
function GetGlobalValue() return 0 end
function SetGlobalValue(...) end
function EventVttKill() return 1 end
Include("script/phongthan/van_tien/guard1.lua")
tasks[1][408]=4;tasks[1][407]=1;tasks[1][409]=floor((SystemTime()+7*3600)/86400)
OnDeath(999);assert(tasks[1][408]==100000 and tasks[1][407]==0 and PlayerIndex==1)
OnDeath(999);assert(tasks[1][407]==0)
legacyChests=0
pearlSpawns=0
function AddNpc(...) legacyChests=legacyChests+1;return legacyChests end
function AddMSNpc(...) return 1 end
function SetNpcKind(...) end
function SetNpcName(...) end
function SetNpcScript(...) end
function SetNpcDeathScript(...) end
function EventVttBossLoot(...) pearlSpawns=pearlSpawns+1 end
function GetNpcPos() return 1067,1594,3330 end
Include("script/phongthan/van_tien/boss1.lua")
tasks[1][408]=9;tasks[1][407]=1
OnDeath(999);assert(legacyChests==4 and pearlSpawns==1 and tasks[1][408]==100000)
lootOp=-1
function EventVttLoot(op) lootOp=op;return 1 end
Include("script/phongthan/van_tien/pearl_chest.lua")
main();assert(lootOp==0);pt_vtt_loot_finish();assert(lootOp==1)
shop=0
function Sale(id) shop=id end
Include("script/phongthan/van_tien/doctor.lua")
tasks[1][408]=1;main();assert(tasks[1][408]==100000);yes();assert(shop==24)
print("PASS Lua 4 TGPT: Thien Hung tasks/Tuesday XP/green choices/12 weapon options, Xich menu/3 merges, lottery buy, original guard/boss task credit, legacy caches, pearl callbacks and original doctor")
