--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
	{
		{"TrÞ th­¬ng","yes_1";show=1},
		{"Mua b¸n","yes_2";show=1},
		{"§«ng H¶i ThÝ LuyÖn","shitu";show=0}
	}
	if(GetTask(900)==3)then
		tasks[3].show=1;
	end
	if(GetTask(304)==39)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==39)then
			Talk(1,"no","§Õn ®­îc ®©y qu¶ lµ ®iÒu kh«ng dÔ, mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
			SetTask(314,100)
	elseif(GetTask(339)==1)then
			Talk(1,"no","§a t¹ ng­¬i cho ta biÕt tin quan träng nh­ thÕ! Mãn vò khÝ Hoµng Kim nµy xin tÆng Dòng sÜ. Ngoµi ra cßn cã mét bøc th­ mong ng­¬i chuyÓn gióp ta.")
				if(GetSeries()==0)then
					local i=random(1,2)
						if (i==1) then
							AddNormalItem(0,0,4,2,0,0)
							Msg2Player("B¹n nhËn ®­îc Hãa HuyÕt ®ao")
						elseif(i==2)then
							AddNormalItem(0,0,5,2,0,0)
							Msg2Player("B¹n nhËn ®­îc TÊn ThiÕt m©u")
						end;
				elseif(GetSeries()==1)then
					AddNormalItem(0,0,6,2,0,0)
							Msg2Player("B¹n nhËn ®­îc DiÖt DiÖm kiÕm")
				elseif(GetSeries()==2)then
					AddNormalItem(0,0,7,2,0,0)
							Msg2Player("B¹n nhËn ®­îc Tr­êng Sinh phñ")
				end;		
			SetTask(339,2)
			Msg2Player("Gióp §¹i phu ë §«ng H¶i H¶i C©u chuyÓn th­.")
	else		
			SayTask(10186,tasks)
	end;
end;

function judge_relation()		--º¡¨¬®v®{2¤H¶¤
	local mark=0
	if(GetTeam()~=0)then			-- ¦³¶¤¥î	
		if(GetTeamSize()==2)then	--2¤H¶¤
			if(IsCaptain()==0)then
				n=GetTeamMember(1)
			else
				n=GetTeamMember(2)
			end;
			mark=IsMasterPRRelation(n)
		end
	end
	return mark
end

function shitu()
	local mark=judge_relation()
	if(mark==1)then
		if(HaveIBBuff(216)~=0)then
			RestoreLife()
			RestoreMana()
			SetTask(900,4)
			TaskNote(44,2)
			MsgBox("Chµo mõng ®Õn<color=green>§«ng H¶i H¶i C©u<color>. Ng­¬i ®· hoµn thµnh <color=green>b­íc 3<color> cña nhiÖm vô, ta sÏ gióp ng­¬i håi phôc tinh lùc! Cè g¾ng lªn!","no")
		else
			MsgBox("Xin lçi! NhiÖm vô ®· thÊt b¹i. Xin h·y tiÕn hµnh l¹i tõ ®Çu.","no")
		end
	else
		MsgBox("Xin h·y duy tr× tr¹ng th¸i tæ ®éi, ta sÏ gióp ng­¬i phôc håi thÓ lùc!","no")
	end
end

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
    if w ~= 1039 then CloseDialog(); return end
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
