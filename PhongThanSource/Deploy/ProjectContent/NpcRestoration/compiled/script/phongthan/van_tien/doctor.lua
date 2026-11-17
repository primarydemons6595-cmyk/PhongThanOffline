--description: Ò½Éú
--author: yichuan
--date: 2005/2/28

function main(sel)
	if(GetTask(408)==1)then
		SetTask(408,100000)
		MsgBox("Tuy sèng ë ®©y l©u nh­ng ta còng ch¼ng biÕt g× vÒ <color=green>Lôc Hån Phiªn<color>. Ng­¬i lÇn nµy xem nh­ c«ng cèc råi, h·y quay vÒ gÆp <color=green>Thiªn Hïng<color>!","no")
	else
		MsgBox(11209,"yes","no")
	end
end;

function yes()
	CloseDialog()
	Sale(24);
end;

function no()
	CloseDialog()
end;
