-- Original VNG source payload; provenance in deployment report.
--description: ËãÃüÏÈÉú
--author: yichuan
--date: 2004/7/13

function main()
	tasks = 
	{
		{"Phi Tiªn","renwu1";show=0}
	}
		UTask_xq_1=GetTask(51);
		if(UTask_xq_1==1)then
					tasks[1].show=1;
		end;
		SayTask(10467,tasks)
end;

function   renwu1()
		MsgBox(10468,"yes_1","no")
end;




function yes_1()
	if(GetCash()>=1000)then
		Talk(5,"no",10469,10470,10471,10472,10473)
		Msg2Player("T×m Cao Minh hái th¨m vÞ trÝ cô thÓ cña m¶nh L­u Tinh, nhËn ®­îc B¸ L¹c Nh·n.")
		AddNormalItem(3,29,0,0,0,0)
		Pay(1000)
		TaskNote(22,1)
		SetTask(51,2)
	else
		Talk(1,"no",10474)
	end;
end;

function no()
		CloseDialog()
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1020_04; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1020 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("§Çu mèi t­íng sè vµ c¸c nhiÖm vô thó c­ìi. Chøc n¨ng phô thuéc map; kh«ng lÊy Lua T©y Kú ®Ó thay cho TriÒu Ca.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("A Tµi (178/189); NhËm §¹i Ca (170/195); NhËm §¹i TÈu (170/195); ChuyÓn Sinh L·o L·o (182/192); D­¬ng TiÔn (165/185)", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("Cã nh¸nh Lua VNG gèc trong PAK. C¸c ®iÒu kiÖn vµ giao dÞch do nh¸nh gèc kiÓm tra; ch­a nghiÖm thu toµn bé nhiÖm vô.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_close()
    CloseDialog()
end
function pt_original()
    if pt_guard() == 0 then return end
    pt_original_main()
end

-- Restored Linh thu service; keep original quest main.
pt_pet_original_main = main
Include("\\script\\phongthan\\pet_service.lua")
function main() return pt_pet_menu() end
