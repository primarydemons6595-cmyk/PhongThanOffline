--description:≤…“©¿œ»À
--author: yichuan
--date: 2004/7/8

function main(sel)
	tasks = 
	{
		{"Vi Lao","renwu1";show=0}
	}

	UTask_xq_0=GetTask(50);
	if(UTask_xq_0==1)then
			tasks[1].show=1;
	end;
	SayTask(10391,tasks)
end;

function  renwu1()
				Talk(1,"no",10392)
				Msg2Player("ß’n ThÒ D≠¨ng s¨n gi’t D≠Óc L©u Tˆ, l y Æ≠Óc Huy™n Th∂o.")
				TaskNote(21,1)
				SetTask(50,2)
end;

function  no()
		CloseDialog()
end;

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1020 then CloseDialog(); return end
    return pt_original_main()
end
