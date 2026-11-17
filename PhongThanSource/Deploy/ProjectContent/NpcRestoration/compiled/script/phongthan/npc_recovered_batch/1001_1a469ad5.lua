--description:Õº Èπ›-√˜÷È∞µÕ∂»ŒŒÒ
--author: chensong
--date: 2004/7/13

function main()
	if(songxin()==0)then
		local tasks = 
		{
			{"Minh Ch©u","renwu1";show=0},
			{"Mua s∏ch k¸ n®ng","renwu2";show=1}
		}
		UTask_cg_1 = GetTask(41);
		if (UTask_cg_1==2) and (HaveEventItem(33)>=1) then
				tasks[1].show=1;
		end;

		SayTask(10294,tasks)
	end
end;

function songxin()
	local task_id = 872
	local map_id = 7

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
	 		Talk(1,"no","V t v∂ cho ng≠¨i qu∏!")
			SetTask(task_id, SetByte(task_val,3,SetBit(finish, setbit, 1)))
	 		return 1
	 	end
	end
	return 0
end

function  renwu1()

		Talk(2,"no",10295,10296)
		SetTask(41,3)
		Msg2Player("Chu»n bﬁ Æ’n Ng‰c H≠ Cung h·i CÔ L≠u T´n v“ chuy÷n ßﬁnh H∂i B∂o ch©u.")
		TaskNote(20,2)

end;

function renwu2()
		CloseDialog()
		Sale(19);
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
    if w ~= 1001 then CloseDialog(); return end
    return pt_original_main()
end
