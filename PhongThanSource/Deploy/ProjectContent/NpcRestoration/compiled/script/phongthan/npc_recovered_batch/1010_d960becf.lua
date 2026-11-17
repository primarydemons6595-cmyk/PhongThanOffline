--description: “©µÍ-“©¢ª∫ÿ €…Ã
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(GetTask(304)==10)then
			Talk(1,"no","T◊nh h◊nh Î Æ©y Æπi kh∏i nh≠ th’, mau quay v“ thao tr≠Íng b∏o cho v‚ s≠ Æi.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==10)then
			Talk(1,"no","Qu∏i vÀt Î Æ©y sao lπi x u th’, ng≠¨i c„ bi’t kh´ng? H∑y mau v“ phÙc m÷nh Hoµng Thi™n H„a.")
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
    if w ~= 1010 then CloseDialog(); return end
    return pt_original_main()
end
