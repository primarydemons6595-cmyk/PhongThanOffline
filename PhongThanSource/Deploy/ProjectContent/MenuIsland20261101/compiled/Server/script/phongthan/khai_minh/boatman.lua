--description:¿ªÃ÷µº
--author:xiakun
--date:2005/06/21

function main()
	tasks =
		{
			{"Rêi khái","likai";show=1},
		}
	SayTask("TruyÒn thuyÕt nãi ®¶o nµy do mét vŞ anh hïng ®Æt tªn lµ Khai Minh, vŞ Êy ®· c¶m hãa mét ¸c thÇn ë ®©y. ChuyÖn nµy do «ng néi ta cho ta biÕt. Ph¶i råi! Ng­¬i muèn rêi khái ®©y µ?",tasks)
end;

function likai()
	if(GetPK()<=0)then
		CloseDialog()
		Msg2Player("Rêi Khai Minh ®¶o, ®iÓm Trïng sinh cña b¹n lµ ë Phong ThÇn ®µi.")
		SetFightState(0)
		NewWorld(1,1523,3241)
		SetRevPos(1,1)
	else
		MsgBox("Xin lçi! Ng­êi ng­¬i ®Çy s¸t khİ, ta kh«ng hoan nghªnh, ng­¬i h·y kiÓm ®iÓm l¹i m×nh!","no")
	end;
end;
function no()
		CloseDialog()
end;
