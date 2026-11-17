--description: ²Ö¿â¹ÜÀí³¥
--author: yichuan
--date: 2004/6/28

function main(sel)
	tasks =
	{
		{"Thu thËp","renwu2";show=0},
		{"Sö dông Thñ khè","renwu1";show=0},
		{"Nguyªn liÖu","ma";show=1},
		{"Long Phông phï","longfeng";show=0}
	}
	UTask_xiangzi=GetTask(33);
	UTask_bianliang=GetTask(36); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
	if(UTask_xiangzi==1)and(HaveNormalItem(3,8,0,0)>=5)then
			tasks[2].show=1;
	end;
	if(UTask_xiangzi==0)then
			tasks[2].show=1;
	end;
	if (HaveNormalItem(3,UTask_bianliang,0,0)>=10) then
			tasks[1].show=1;
	end;
	if (UTask_bianliang==0)and(GetLevel()>=6) then
			tasks[1].show=1;
	end;
	---»î¶¯¹Ø±Õ
	---if(GetExtPoint(6)==1)then
	---			tasks[4].show=1
	---end;
		SayTask(10073,tasks)
end;

function  renwu1()
		UTask_xiangzi=GetTask(33);

		if(UTask_xiangzi==1)and(HaveNormalItem(3,8,0,0)>=5)then
				Talk(1,"no",10074)
				for  i=1,5 do
						DelNormalItem(3,8,0,0)
				end;
				Msg2Player("Cã thÓ sö dông r­¬ng chøa ®å ®Ó cÊt gi÷ vËt phÈm!")
				TaskNote(3,1)
				TaskNote(9,1)
				TaskNote(15,1)
				SetTask(33,2)
				SetTask(13,2)
				SetTask(23,2)
		end;


		if(UTask_xiangzi==0)then
				MsgBox("Hµnh trang cña ng­¬i hÕt chç trèng råi! T×m gióp ta <color=red> 5 Háa vò<color>, ta sÏ lµm cho ng­¬i 1 chiÕc r­¬ng!","yes_1","no")
		end;
end;

function   longfeng()
	if(GetExtPoint(6)>=1)then
			MsgBox("N¨m x­a vÞ Èn sÜ cã tÆng ta mét tÊm Long Phông phï, nhê ta tÆng l¹i cho ng­êi cã duyªn. Xem ra ta ®· t×m ®óng ng­êi, ng­¬i muèn nhËn kh«ng?","lf","no")
	end;
end;

function   lf()
		PayExtPoint(6,1)
		AddNormalItem(6,1,32,1,0,0)
		CloseDialog()
		Msg2Player("B¹n nhËn ®­îc Long Phông phï.")
end;

function  renwu2()
		UTask_bianliang=GetTask(36); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
			local  w=""
			if (UTask_bianliang==8) then
					w="Háa vò"
			elseif (UTask_bianliang==12) then
					w="MÆt Quû"
			end;
			if (HaveNormalItem(3,UTask_bianliang,0,0)>=10) then
					MsgBox("Ng­¬i ®· vÒ råi µ? Cã ph¶i muèn ®­a "..w.." cho ta kh«ng?","huan","no")
			end;

			if (UTask_bianliang==0)and(GetLevel()>=6) then  --½Ó?ÎñÊ±µÄ¶Ô»°
					Talk(1,"yes_2",10075)
			end;
end;

function  yes_2()
	    local i=random(1,2);
		local  w="";
					if (i==1) then
							w="Háa vò"
							MsgBox("Nguyªn liÖu lÇn nµy cÇn 10 <color=Red>"..w.."<color>, kh«ng vÊn ®Ò chø?","qd1","no")
					else
							w="MÆt Quû"
							MsgBox("Nguyªn liÖu lÇn nµy cÇn 10 <color=Red>"..w.."<color>, kh«ng vÊn ®Ò chø?","qd2","no")
					end;

end;

function  qd1()
		SetTask(36,8)
		Msg2Player("§ång ý t×m 10 Háa vò cho Thñ khè Xi V­u mé.")
		TaskNote(18,0,"Háa vò")
		CloseDialog()
end;

function  qd2()
		SetTask(36,12)
		Msg2Player("§ång ý t×m 10 MÆt Quû cho Thñ khè Xi V­u mé.")
		TaskNote(18,0,"MÆt Quû")
		CloseDialog()
end;

function huan()     --Íê³É?Îñ£¬¸øÓè½±Àø¡£
	local UTask_bianliang=GetTask(36); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
	if (HaveNormalItem(3,UTask_bianliang,0,0)>=10) then
		local k=random(1,100000)
		local green20=GetTask(810)--°O¿ý20¯Åºñ¸Ë¥ô°È¤¤ºñ¸ËªºÀò±o¦¸¼Æ
		local PlayerLevel=GetLevel()
		if (PlayerLevel>50) then
			PlayerLevel=50
		end
		if (k<=10000) then
			Talk(1,"no",10077)
			AddEventItem(39)
		elseif (k>10000) and (k<=20000) and  (PlayerLevel<=25) and  (green20==0)  then
			GreenEquipment()
		elseif (k>20000) and (k<=21000) and  (PlayerLevel<=25) and  (green20>=1) and  (green20<=2)  then
			GreenEquipment()
		elseif (k>21000) and (k<=21500) and  (PlayerLevel<=25) and  (green20>=3) then
			GreenEquipment()
		elseif (k>21500)  and (k<21900-PlayerLevel*PlayerLevel*7/50) and (PlayerLevel>25) then
			GreenEquipment()
		else
			Talk(1,"no",10086)
		end
		for  i=1,10 do
			DelNormalItem(3,UTask_bianliang,0,0)
		end;
		Earn(600)
		Msg2Player("®· hoµn thµnh nhiÖm vô Thñ khè, danh väng t¨ng lªn, nhËn ®­îc 600 l­îng.")
		TaskNote(18,-1)
		AddCredit(1)
		SetTask(36,0)
	end
end;

function GreenEquipment()
	Talk(1,"no",10007)
	local n=random(0,2)--?¶¨»ñµÃ×°±¸µÄ¾ß·¥¯C±ð
	local p=random(3,5)
	if (n==0 )then --?¶¨p£¬¼´×°±¸µÄÏêÏ¸¯C±ð
			n=5
	elseif (n==1) then
			n=6
	elseif (n==2) then
			n=7
	end;
	AddNormalItem2(0,n,p,1,1,0)--µÀ¾ßÖÖ¯C0£¬¾ß·¥¯C±ðn£¬ÏêÏ¸¯C±ðp£¬µÈ¼¶1
	SetTask(810,GetTask(810)+1)
	l=GetName()
	if(GetLevel()<30)then
		AddGlobalCountNews("<color=green>"..l.."<color> ®· thu thËp ®ñ nguyªn liÖu cho Thñ khè Xi V­u mé, ®­îc tÆng mét trang bÞ <color=green>"..l.."<color>.",20)
	end;
end




function no()
		CloseDialog()
end;


function   ma()
			tasks1 =
			{
				{"§o¶n KiÕm","dj";show=1},
				{"M¶nh Gi¸p","sj";show=1},
				{"B¨ng c¬","bj";show=1},
				{"Ngäc cèt","yg";show=1},
				{"MÆt Quû","gm";show=1},
				{"Háa vò","hy";show=1}
			}
		SayTask(10085,tasks1)
end;

function   dj()
		MsgBox(10078,"ma")
end;

function  sj()
		MsgBox(10079,"ma")
end;

function   bj()
		MsgBox(10080,"ma")
end;

function   yg()
		MsgBox(10081,"ma")
end;

function   gm()
		MsgBox(10082,"ma")
end;

function   hy()
		MsgBox(10083,"ma")
end;


function yes_1()
		SetTask(33,1)
		Talk(1,"no",10084)
		Msg2Player("TiÕp nhËn nhiÖm vô, gióp Thñ khè t×m 5 Háa vò.")
		TaskNote(15,0)
end;

PTBOT_BANK={task=33,detail=8}

-- Bot capability metadata; no quest or reward logic is replaced.
Include("\\script\\phongthan\\bots\\npc_goals.lua")
