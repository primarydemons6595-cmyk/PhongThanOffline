--Trap Óü·¨É½toÒÄ·½²¿Âä
--TrapID£º215
--Update: yujin(2005-10-23)½¨Á¢

function main(sel)
	if (GetTask(597)>=15) and (GetTask(597)<=299) then
		Msg2Player("§· ®Õn Bé L¹c Di Ph­¬ng")
		NewWorld(75,1605,3831);
	else
		Talk(1,"no","Phİa tr­íc khİ ®éc mŞt mï, kh«ng thÓ tiÕn tíi. B¹n cã thÓ vµo <color=red>Ngôc Ph¸p s¬n<color> t×m c¸ch ®i tiÕp")
		Msg2Player("Phİa tr­íc khİ ®éc mŞt mï, kh«ng thÓ tiÕn tíi")
	end;
end;

function no()
	CloseDialog()
end;
