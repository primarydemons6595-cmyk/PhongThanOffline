--description: ²Ö¿â¹ÜÀí³¥
--author: yichuan
--date: 2004/6/28

function main(sel)
			tasks =
			{
				 {"Tø T­îng","renwu2";show=0},
				 {"Nguyªn liÖu","ma";show=1}
			}
		UTask_bianliang=GetTask(55); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
		if (HaveNormalItem(3,UTask_bianliang,0,0)>=10)and(UTask_bianliang~=0) then
			tasks[1].show=1;
		end;
		if(GetLevel()>=31)and(UTask_bianliang==0) then
			tasks[1].show=1;
		end;



			SayTask(10393,tasks)
end;

function  renwu2()
		UTask_bianliang=GetTask(55); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
		if (UTask_bianliang==0)and(GetLevel()>=31) then  --½Ó?ÎñÊ±µÄ¶Ô»°
			MsgBox(10394,"yes_2","no")
		else
			local  w=""
			if (UTask_bianliang==22) then
					w="§Þa t©m"
			elseif (UTask_bianliang==23) then
					w="Phong lÖ"
			elseif (UTask_bianliang==24) then
					w="Thñy hån"
			elseif (UTask_bianliang==25) then
					w="Háa linh"
			end;
			if (HaveNormalItem(3,UTask_bianliang,0,0)>=10) then
					MsgBox("Ng­¬i ®· t×m ®­îc "..w..", ®­a cho ta chø?","huan","no")
			end;				
		end;
end;

function  yes_2()
		UTask_bianliang=GetTask(55); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
	    local i=random(1,4);
		local  w=""
					if (i==1) then
							w="§Þa t©m"
					elseif(i==2)then
							w="Phong lÖ"
					elseif(i==3)then
							w="Thñy hån"
					elseif(i==4)then
							w="Háa linh"
					end;
		Talk(2,"no","§©y chÝnh lµ thø ta cÇn: <color=Red>"..w.."<color>.","§Ó ta ®i t×m nguyªn liÖu vÒ.")
		SetTask(55,i+21)
		SetTask(54,1)
		Msg2Player("gióp thñ khè T©y Kú t×m "..w.." ")
		TaskNote(24,0,w)
end;

function huan()     --Íê³É?Îñ£¬¸øÓè½±Àø¡£
	local UTask_bianliang=GetTask(55); --ÒÔ´ËÇø±ðÃ¿´Î?ÎñËùÐèµÄÔ­ÁÏÖÖ¯C
	if (HaveNormalItem(3,UTask_bianliang,0,0)>=10) then
		local k=random(1,100000)
		local green40=GetTask(811)--°O¿ý40¯Åºñ¸Ë¥ô°È¤¤ºñ¸ËªºÀò±o¦¸¼Æ
		local PlayerLevel=GetLevel()
		if (PlayerLevel>70) then
			PlayerLevel=70
		end
		if (k<=10000) then
			Talk(1,"no",10008)
			local p=random(6,13)
			AddNormalItem(0,4,p,1,0,0)
		elseif (k>10000) and (k<=15000) and  (PlayerLevel<=50) and  (green40==0)  then
			GreenEquipment()
		elseif (k>15000) and (k<=15600) and  (PlayerLevel<=50) and  (green40>=1) and  (green40<=2)  then
			GreenEquipment()
		elseif (k>15600) and (k<=15900) and  (PlayerLevel<=50) and  (green40>=3) then
			GreenEquipment()
		elseif (k>15900)  and (k<16100-PlayerLevel*PlayerLevel*17/490) and (PlayerLevel>50) then
			GreenEquipment()
		else
			Talk(1,"no",10086)
		end
		for  i=1,10 do
			DelNormalItem(3,UTask_bianliang,0,0)
		end;
		Earn(1200)
		Msg2Player("Hoµn thµnh nhiÖm vô, nhËn ®iÓm danh väng vµ 1200 l­îng.")
		AddCredit(5)
		SetTask(55,0)
		SetTask(54,0)
		TaskNote(24,-1)
	end
end;

function GreenEquipment()
	local k=random(1,3)
	if(k==1)then
		Talk(1,"no",10007)
		local n=random(3,5)--?¶¨»ñµÃ×°±¸µÄ¾ß·¥¯C±ð
		AddNormalItem2(0,5,n,3,1,0)--µÀ¾ßÖÖ¯C0£¬¾ß·¥¯C±ðn£¬ÏêÏ¸¯C±ðp£¬µÈ¼¶3
		SetTask(811,GetTask(811)+1)
		l=GetName()
		AddGlobalCountNews("<color=green>"..l.."<color> t×m ®ñ vËt liÖu, nhËn ®­îc phÇn th­ëng <color=green>"..l.."<color>.",20)
	elseif(k==2)then
		Talk(1,"no",10007)
		local n=random(3,5)--?¶¨»ñµÃ×°±¸µÄ¾ß·¥¯C±ð
		AddNormalItem2(0,6,n,3,1,0)--µÀ¾ßÖÖ¯C0£¬¾ß·¥¯C±ðn£¬ÏêÏ¸¯C±ðp£¬µÈ¼¶3
		SetTask(811,GetTask(811)+1)
		l=GetName()
		AddGlobalCountNews("<color=green>"..l.."<color> t×m ®ñ vËt liÖu, nhËn ®­îc phÇn th­ëng <color=green>"..l.."<color>.",20)
	elseif(k==3)then
		Talk(1,"no",10007)
		local n=random(3,5)--?¶¨»ñµÃ×°±¸µÄ¾ß·¥¯C±ð
		AddNormalItem2(0,7,n,3,1,0)--µÀ¾ßÖÖ¯C0£¬¾ß·¥¯C±ðn£¬ÏêÏ¸¯C±ðp£¬µÈ¼¶3
		SetTask(811,GetTask(811)+1)
		l=GetName()
		AddGlobalCountNews("<color=green>"..l.."<color> t×m ®ñ vËt liÖu, nhËn ®­îc phÇn th­ëng <color=green>"..l.."<color>.",20)
	end
end

function no()
		CloseDialog()
end;


function   ma()
			tasks1 =
			{
				{"§Þa t©m","dj";show=1},
				{"Thñy hån","bj";show=1},
				{"Háa linh","yg";show=1},
				{"Phong lÖ","sj";show=1}
			}
		SayTask(10085,tasks1)
end;

function   dj()
		MsgBox(10009,"ma")
end;

function  sj()
		MsgBox(10010,"ma")
end;

function   bj()
		MsgBox(10011,"ma")
end;

function   yg()
		MsgBox(10012,"ma")
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
