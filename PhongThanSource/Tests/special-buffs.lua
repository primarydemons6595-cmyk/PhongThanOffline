Include("script/phongthan/test/event_handbook.lua")
lastOptions=nil
function EBShow(text,options) lastOptions=options end
EBItem(16226,836)
assert(getn(lastOptions)==4)
assert(strfind(lastOptions[1],"EBGive%(16226,1,836%)"))
assert(strfind(lastOptions[2],"EBGive%(16226,10,836%)"))
assert(strfind(lastOptions[3],"EBGive%(16226,30,836%)"))
assert(strfind(lastOptions[4],"EBMenu%(836,1%)"))
local priorItem=EBOriginalItemMenu
forwarded=0
function EBOriginalItemMenu(id,parent) forwarded=id+parent end
EBItem(2,7)
assert(forwarded==9)
EBOriginalItemMenu=priorItem
assert(strlen(EBResultText(-8))>30)
assert(EBResultText(1)==EBOriginalResultText(1))
Include("script/vnevent/items/thanh_long_lenh.lua")
used=0
function UseOriginalBuffItem(item) used=item return 1 end
assert(main(123)==1 and used==123)
print("PASS Lua 4: full GM file parses, hope options, menu/result delegation, original buff item callback")
