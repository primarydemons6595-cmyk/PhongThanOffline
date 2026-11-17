--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(GetTask(304)==17)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==17)then
			Talk(1,"no","T©n MiÔn ë T©y Kú ®ang thu thËp nh÷ng thø kú qu¸i. H·y mau vÒ b¸o cho Hoµng Thiªn Hãa.")
			SetTask(314,100)
	else
			MsgBox(10322,"yes","no")
	end;
end;

function yes()
		CloseDialog()
		Sale(15);
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
    if w ~= 1017 then CloseDialog(); return end
    return pt_original_main()
end

-- PT_DOCTOR_SERVICES_20261102
pt_doctor_quest_main=main
function main()
 Say("§¹i Phu",4,"Cöa hµng thuèc/pt_doctor_shop","TrÞ th­¬ng/pt_doctor_heal","NhiÖm vô/pt_doctor_quests","KÕt thóc/pt_doctor_close")
end
function pt_doctor_shop() CloseDialog();Sale(15) end
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
