--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
	{
		{"TrÞ th­¬ng","yes_1";show=1},
		{"Mua b¸n","yes_2";show=1},
		{"B¨ng Xuyªn ThÝ LuyÖn","shitu";show=0}
	}
	if(GetTask(902)==5)then
		tasks[3].show=1;
	end
	if(GetTask(304)==36)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==36)then
			Talk(1,"no","Ma V­¬ng ë ®©y rÊt ®¸ng sî, cã thÓ khiÕn ng­¬i ®ãng b¨ng, mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
			SetTask(314,100)
	else
			SayTask(10186,tasks)
	end;
end;

function judge_relation()		--³ûåÍ…ÑÆb2?½Ë
	local mark=0
	if(GetTeam()~=0)then			-- ô¬½Ë”×	
		if(GetTeamSize()==2)then	--2?½Ë
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
			SetTask(902,6)
			TaskNote(46,4)
			MsgBox("Chµo mõng ®Õn<color=green>B¨ng Xuyªn Chi Cùc<color>. Ng­¬i ®· hoµn thµnh <color=green>ThÝ LuyÖn B¨ng Xuyªn S­ M«n<color>. Mau vÒ phôc mÖnh D­¬ng TiÔn!","no")
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
    if w ~= 1036 then CloseDialog(); return end
    return pt_original_main()
end
