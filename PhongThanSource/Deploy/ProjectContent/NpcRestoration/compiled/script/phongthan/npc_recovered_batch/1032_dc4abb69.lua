--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
			{
			{"B¨ng Xuyªn ThÝ LuyÖn","shitu";show=0}
		}
	if(GetTask(902)==1)then
		tasks[1].show=1;
		SayTask("Th¸m hiÓm mª cung sÏ gióp n©ng cao n¨ng lùc, ta sÏ gióp ng­¬i trong suèt hµnh tr×nh. ",tasks)
	else
		if(GetTask(304)==32)then
				Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
				SetTask(304,100)
				TaskNote(31,0)
		elseif(GetTask(314)==32)then
				Talk(1,"no","§õng nãi n÷a...mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
				SetTask(314,100)
		else
				MsgBox(10322,"yes","no")
		end;
	end
end;

function judge_relation()		--Âú×ãÊ¦Í½2ÈË¶Ó
	local mark=0
	if(GetTeam()~=0)then			-- ÓÐ¶ÓÎé	
		if(GetTeamSize()==2)then	--2ÈË¶Ó
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
			SetTask(902,2)
			TaskNote(46,0)
			MsgBox("Chµo mõng ®Õn<color=green>Ngäc TuyÒn B¨ng Xuyªn<color>. Ng­¬i ®· hoµn thµnh <color=green>b­íc 1<color> cña nhiÖm vô, ta sÏ gióp ng­¬i håi phôc tinh lùc! Cè g¾ng lªn!","no")
		else
			MsgBox("Xin lçi! NhiÖm vô ®· thÊt b¹i. Xin h·y tiÕn hµnh l¹i tõ ®Çu.","no")
		end
	else
		MsgBox("Xin h·y duy tr× tr¹ng th¸i tæ ®éi, ta sÏ gióp ng­¬i phôc håi thÓ lùc!","no")
	end
end


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
    if w ~= 1032 then CloseDialog(); return end
    return pt_original_main()
end
