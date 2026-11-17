-- Original VNG source payload; provenance in deployment report.
--description: À’ª§
--author: yichuan
--date: 2004/5/14

function main(sel)
	tasks = 
	{
		{"HÈp g m","renwu1";show=0}
	}
			UTask_10 = GetTask(20);
		if(UTask_10==18) and (HaveEventItem(26)>=1) then
			tasks[1].show=1;
		end;
		if(UTask_10==0) then
			tasks[1].show=1;
		end;
		SayTask(10271,tasks)
end;

function   renwu1()	
    if GetPlayerType() ~= 0 then return end
		UTask_10 = GetTask(20);
		if(UTask_10==18) and (HaveEventItem(26)>=1) then
			Talk(1,"no",10272)
			DelEventItem(26)
			AddNormalItem(1,0,1,1,0,0)
			AddNormalItem(1,0,1,1,0,0)
			AddNormalItem(1,0,1,1,0,0)
			AddNormalItem(1,3,1,1,0,0)
			AddNormalItem(1,3,1,1,0,0)
			AddNormalItem(1,3,1,1,0,0)
			Msg2Player("GiÛp T´ HÈ l y hÈp g m, nhÀn ph«n th≠Îng 3 Ti”u HÂng Æ¨n vµ 3 Ti”u Hoµn Æ¨n.")
			TaskNote(7,10)
			SetTask(20,19)
		end;
		if(UTask_10==0) then
			MsgBox(10273,"yes_1","no")
		end;	
end;

function yes_1()
    if GetPlayerType() ~= 0 then return end
		Talk(1,"no",10274)
		Msg2Player("ß’n ThÒ khË l y hÈp g m v“ cho T´ HÈ.")
		TaskNote(7,0)
		SetTask(20,1)
end;

function no()
		CloseDialog()
end;

-- Reviewed project quest transaction compatibility.
-- Project compatibility appendix; append AFTER the unchanged VNG Su Ho Lua.
-- Source: script.pak /script/Â¥áÂüéÂ§ßËê•/ËãèÊä§.lua
-- Original SHA256: c44b9f6b28692d77a4a4c6f51d35ebb6a509241e0be102d7b7c78eb4b026460c
-- Keeps task 20, EventItem 26, message IDs and all original reward tuples.
-- QuestExchange is a server API: reserve rewards, consume requirements and
-- advance the expected task phase together. A failed exchange changes nothing.
function renwu1()
    if GetPlayerType() ~= 0 then return end
    local phase = GetTask(20)
    if phase == 0 then
        MsgBox(10273, "yes_1", "no")
        return
    end
    if phase ~= 18 then
        CloseDialog()
        return
    end
    if QuestExchange(20, 18, 19,
        {{4, 26, 0, 0, 0, 0, 1}},
        {{1, 0, 1, 1, 0, 0, 3}, {1, 3, 1, 1, 0, 0, 3}}) ~= 1 then
        Msg2Player("Ch≠a th” nhÀn th≠Îng: c«n HÈp G m vµ chÁ trËng hµnh trang.")
        CloseDialog()
        return
    end
    Talk(1, "no", 10272)
    Msg2Player("Giao HÈp G m cho T´ HÈ: nhÀn 3 Ti”u HÂng Æ¨n vµ 3 Ti”u Hoµn Æ¨n.")
    TaskNote(7, 10)
end

function yes_1()
    if QuestExchange(20, 0, 1, {}, {}) ~= 1 then
        CloseDialog()
        return
    end
    Talk(1, "no", 10274)
    Msg2Player("Den Thu Kho lay Hop Gam ve cho To Ho.")
    TaskNote(7, 0)
end

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1002_00; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1002 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("ß«u mËi nhi÷m vÙ t©n thÒ Gi∏p S‹. HÈp G m bæt Æ«u tπi T´ HÈ, sau Æ„ Æ’n ThÒ KhË trong SÔng Thµnh Doanh.", 2, "Quay lπi/main", "ß„ng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Trﬁnh Lu©n (200/203); Sinh Hoπt S≠ (199/200); T©n ThÒ Th› Luy÷n (200/199); Th≠¨ng Nh©n T©y V˘c (199/199); Chuy”n Sinh L∑o L∑o (202/199)", 2, "Quay lπi/main", "ß„ng/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("C„ nh∏nh Lua VNG gËc trong PAK. C∏c Æi“u ki÷n vµ giao dﬁch do nh∏nh gËc ki”m tra; ch≠a nghi÷m thu toµn bÈ nhi÷m vÙ.", 2, "Quay lπi/main", "ß„ng/pt_close")
end
function pt_close()
    CloseDialog()
end
function pt_original()
    if pt_guard() == 0 then return end
    pt_original_main()
end

-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetPlayerType() ~= 0 then return 0,0 end
    if GetTask(20)==18 and HaveEventItem(26)>=1 then return 3,1 end
    if GetTask(20)==0 then return 1,1 end
    if GetTask(20)>0 and GetTask(20)<19 then return 2,1 end
    return 0,0
end

-- Enforce class ownership when projecting the original NPC's quest menu.
pt_class_original_say_task=SayTask
pt_class_quest_callbacks={renwu1=0, yes_1=0}
function SayTask(text,choices)
    for i=1,getn(choices) do
        local expected=pt_class_quest_callbacks[choices[i][2]]
        if expected~=nil and GetPlayerType()~=expected then choices[i].show=0 end
    end
    return pt_class_original_say_task(text,choices)
end

-- PT_THREE_FACTION_BOOK_BEGIN
PT3_BOOK={f=0,map=1002,level=6,task=1035,id=3160,detail=24,skill=27,gifts={},note="<color=yellow>C«n M…n<color><enter>ß’n V‚ S≠ mua s∏ch <color=yellow>T’ Huy’t Tr∂m<color>, rÂi mang v“ cho <color=green>T´ HÈ<color> xem. C„ th” h‰c s∏ch tr≠Ìc khi phÙc m÷nh. Gi˜ s∏ch sau khi hoµn thµnh.<enter><color=green>Ph«n th≠Îng: 400 kinh nghi÷m.<color>"}
-- C«n M…n: bounded reconstruction from the hash-pinned reference scripts.
-- Their original saved slots are retained. Showing the book does not consume it.
function PT3BookAllowed()
 return GetPlayerType()==PT3_BOOK.f and GetContentWorldPos()+1000==PT3_BOOK.map and GetLevel()>=PT3_BOOK.level
end
function PT3BookHave()
 return HaveNormalItem(7,PT3_BOOK.detail,PT3_BOOK.skill,-1)>0 or IsSkillActived(PT3_BOOK.skill)>0
end
function PT3BookNote()
 if not PT3BookAllowed() then return end
 local phase=GetTask(PT3_BOOK.task)
 if phase==1 then AddNote(PT3_BOOK.id,1,PT3_BOOK.note,1)
 elseif phase==2 then AddNote(PT3_BOOK.id,2," ",2) end
end
function pt3_book_close() CloseDialog() end
function pt3_book_other() if PT3_BOOK_OLD_MAIN then return PT3_BOOK_OLD_MAIN() end CloseDialog() end
function pt3_book_view()
 if not PT3BookAllowed() then return end
 local phase=GetTask(PT3_BOOK.task)
 if phase==0 then SayButtons(PT3_BOOK.note,2,"NhÀn nhi÷m vÙ/pt3_book_take","Quay lπi/main")
 elseif phase==1 then
  if PT3BookHave() then SayButtons(PT3_BOOK.note,2,"Hoµn thµnh/pt3_book_claim","Quay lπi/main")
  else SayButtons(PT3_BOOK.note,1,"Quay lπi/main") end
 end
end
function pt3_book_take()
 if not PT3BookAllowed() or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(PT3_BOOK.task,0,1,{},{},1)~=1 then return end
 PT3BookNote();pt3_book_view()
end
function pt3_book_claim()
 if not PT3BookAllowed() or GetTrade()~=0 or GetLockState()~=0 or not PT3BookHave() then return end
 -- No yielding calls between checking possession and committing the phase.
 if QuestExchange(PT3_BOOK.task,1,2,{},PT3_BOOK.gifts,1)~=1 then return end
 AddOwnExp(400);PT3BookNote();Msg2Player("C«n M…n: nhÀn 400 kinh nghi÷m.")
end
PT3_BOOK_OLD_MAIN=main
PT3_BOOK_OLD_MARK=GetPlayerTaskState
function main(sel)
 PT3BookNote()
 if PT3BookAllowed() and GetTask(PT3_BOOK.task)<2 then
  return SayButtons(PT3_BOOK.note,2,"C«n M…n/pt3_book_view","Nhi÷m vÙ kh∏c/pt3_book_other")
 end
 if PT3_BOOK_OLD_MAIN then return PT3_BOOK_OLD_MAIN(sel) end CloseDialog()
end
function GetPlayerTaskState()
 local state,level=0,0
 if PT3_BOOK_OLD_MARK then state,level=PT3_BOOK_OLD_MARK() end
 state=state or 0
 if PT3BookAllowed() then
  local phase=GetTask(PT3_BOOK.task);local own=0
  if phase==0 then own=1 elseif phase==1 then own=PT3BookHave() and 3 or 2 end
  if own>state then return own,PT3_BOOK.level end
 end
 return state,level
end
