--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(GetTask(304)==7)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==7)then
			Talk(1,"no","Kh«ng may råi! Thiªn C­¬ng Tinh ®· h¹ phµm, mau ®i T©y Kú t×m NhÞ Lang thÇn ®Õn cøu. Mau vÒ b¸o tin cho Hoµng Thiªn Hãa.")
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
    if w ~= 1007 then CloseDialog(); return end
    return pt_original_main()
end
