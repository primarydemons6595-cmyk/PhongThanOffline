--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(GetTask(304)==5)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==5)then
			Talk(1,"no","Chóng ta ®· bÞ qu©n ®Þch bao v©y s¾p tö chiÕn sa tr­êng, mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
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
    if w ~= 1005 then CloseDialog(); return end
    return pt_original_main()
end
