PlayerIndex=1
lastCount=0
lastOptions={}
closed=0
function Say(text,n,options) lastCount=n;lastOptions=options end
function Msg2Player(text) end
function CloseDialog() closed=closed+1 end
function MsgBox(text,yes,no) confirmation=yes end
function TravelItem(op,arg) if op==0 then return profile end;chosen=arg;return 1 end
function CompassItem(op,slot)
 if op==0 then if slot==1 then return 1,1015,195,195 end;return 0,0,0,0 end
 operation=op;position=slot;return 1,1015,195,195
end
profile=12
Include("script/phongthan/items/compass.lua")
main(7);assert(lastCount==4)
pt_compass_save();assert(lastCount==6)
assert(strfind(lastOptions[1],"195"))
pt_compass_save1();assert(confirmation=="pt_compass_confirm1")
pt_compass_confirm1();assert(operation==1 and position==1)
pt_compass_save5();assert(operation==1 and position==5)
pt_compass_go3();assert(operation==2 and position==3)
pt_compass_clear4();assert(operation==3 and position==4)
assert(PT_COMPASS_MAPS[1006]~=nil)
function GetMorphType() return 364 end
function IsPlayerInsideWeapon(p) return 0 end
function GetTask(k) if k==333 then return 1 end;return 0 end
Include("script/phongthan/travel_items.lua")
profile=11;main(7);assert(lastCount==1)
pt_travel_5();assert(chosen==5)
used=0
function UseF2Gift(item) used=item;return 1 end
Include("script/phongthan/items/f2_gifts.lua")
main(42);assert(used==42)
function UseOriginalBuffItem(item) used=item;return 1 end
Include("script/phongthan/items/experience_lock.lua")
main(43);assert(used==43)
print("PASS actual Lua4: compass five slots/overwrite/save/travel/clear menus, camel return, exact gift and original experience lock dispatch")
