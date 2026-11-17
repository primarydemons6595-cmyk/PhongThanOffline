--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
	{
		{"TrÞ th­¬ng","yes_1";show=1},
		{"Mua b¸n","yes_2";show=1}
	}
	if(GetTask(304)==29)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==29)then
			Talk(1,"no","Nghe nãi ë ®©y cã l¨ng mé cña thÇn linh, mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
			SetTask(314,100)
	else
			SayTask(10186,tasks)
	end;
end;

function yes_1()
	local t=SystemTime()-GetTask(763)
	if (t>=1800)then
		yes_3()
	else
		Talk(1,"no","Mçi lÇn trÞ th­¬ng cho mét ng­êi hao tæn cña ta mét l­îng nguyªn khÝ nhÊt ®Þnh, mÊt n÷a giê míi håi phôc. Ng­¬i h·y quay l¹i sau nhÐ!")
	end;
end;

function yes_3()
	RestoreLife()
	RestoreMana()
	SetTask(763,SystemTime())
	Msg2Player("Sinh lùc vµ néi lùc cña b¹n ®· hoµn toµn håi phôc.")
	CloseDialog()
end;

function yes_2()
	CloseDialog()
	Sale(23);
end;

function no()
		CloseDialog()
end;

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1029 then CloseDialog(); return end
    return pt_original_main()
end

-- PT_DOCTOR_SERVICES_20261102
pt_doctor_quest_main=main
function main()
 Say("§¹i Phu",4,"Cöa hµng thuèc/pt_doctor_shop","TrÞ th­¬ng/pt_doctor_heal","NhiÖm vô/pt_doctor_quests","KÕt thóc/pt_doctor_close")
end
function pt_doctor_shop() CloseDialog();Sale(23) end
function pt_doctor_heal()
 local elapsed=SystemTime()-GetTask(763)
 if elapsed<1800 then
  Talk(1,"pt_doctor_close","Ta ch­a håi phôc nguyªn khÝ, ch­a thÓ trÞ th­¬ng tiÕp. H·y quay l¹i sau.");return
 end
 RestoreLife();RestoreMana();SetTask(763,SystemTime());CloseDialog()
 Msg2Player("Sinh lùc vµ néi lùc cña b¹n ®· hoµn toµn håi phôc.")
end
function pt_doctor_quests() return pt_doctor_quest_main() end
function pt_doctor_close() CloseDialog() end
