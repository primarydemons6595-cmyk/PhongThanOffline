--description: ÔÓ»õÉÌ
--author: yichuan
--date: 2004/5/12

function main(sel)
	if (songxin()== 0)then
		MsgBox(10217,"yes_1","no")
	end
end;

function songxin()
	local task_id = 870
	local map_id = 3

	local task_val = GetTask(task_id)
	local type1 = GetByte(task_val,1) 
	local type2 = GetByte(task_val,2)
	local finish = GetByte(task_val,3)

	local setbit = 0
	if (type1 == map_id) then
		setbit = 7
	elseif (type2 == map_id)then
		setbit = 8
	end

	if (setbit ~= 0) then
	 	if (GetBit(finish, setbit) == 0)then
	 		Talk(1,"no","Ng­¬i vÊt v¶ qu¸!")
			SetTask(task_id, SetByte(task_val,3,SetBit(finish, setbit, 1)))
	 		return 1
	 	end
	end
	return 0
end


function yes_1()
		CloseDialog()
		Sale(7);
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
    if w ~= 1004 then CloseDialog(); return end
    return pt_original_main()
end

Include("\\script\\phongthan\\restoration\\maolu.lua")
PTMAO_NPC_CLASS=2
PTMAO_NPC_ROLE="giao"
PTMAO_OLD_MAIN=main
PTMAO_OLD_MARK=GetPlayerTaskState
PTMAO_OLD_CATEGORY=GetPlayerTaskCategory
function main()
 if PTMaoMenu()==1 then return end
 if PTMAO_OLD_MAIN then return PTMAO_OLD_MAIN() end
 CloseDialog()
end
function GetPlayerTaskState() return PTMaoMarker() end
function GetPlayerTaskCategory()
 if PTMaoReady() then return 1 end
 if PTMAO_OLD_CATEGORY then return PTMAO_OLD_CATEGORY() end
 if PTGMarker then local stage=PTGMarker();if stage>0 then return 1 end end
 if PT2Marker then local stage=PT2Marker();if stage>0 then return 1 end end
 return 0
end
