--description: ²Ö¿â¹ÜÀí³¥
--author: yichuan
--date: 2004/6/28

function main(sel)
	tasks =
	{
		{"Thu thËp","renwu2";show=0},
		{"Sö dông Thñ khè","renwu1";show=0},
		{"Nguyªn liÖu","ma";show=1},
		{"Long Phông phï","longfeng";show=0},
		{"KÕ Tôc","ptk_open";show=PTK_Show()},
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

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1004 then CloseDialog(); return end
    return pt_original_main()
end

-- Lua 4. Reconstructed KÕ Tôc rules; provenance is in the deployment report.
-- Original task slots: 973=local day, 984=acceptances, 993=target/count, 994=kills.
PTK_TARGETS={ [0]={1,4}, [1]={2,5}, [2]={3,7} }
PTK_NAMES={ [1]="KiÕm Nh©n",[4]="X¹ Nh©n",[2]="TuyÕt Qu¸i",[5]="B¨ng Lang",[3]="Háa DiÖn",[7]="Cuång §iªu" }
PTK_MATERIAL={ [1]=10,[4]=11,[2]=13,[5]=9,[3]=12,[7]=8 }

function PTK_Day()
    return floor(LocalSystemTime()/86400)
end
function PTK_ResetDay()
    if GetTask(993)==0 and GetTask(973)~=PTK_Day() then
        SetTask(973,PTK_Day())
        SetTask(984,0)
    end
end
function PTK_Count(level)
    if level<=10 then return 10 end
    if level<=20 then return 20 end
    if level<=40 then return 40 end
    if level<=60 then return 80 end
    if level<=80 then return 150 end
    if level<=100 then return 250 end
    return 400
end
function PTK_ValidTarget(target)
    local choices=PTK_TARGETS[GetPlayerType()]
    return choices and (target==choices[1] or target==choices[2])
end
function PTK_Note(step)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if not PTK_ValidTarget(target) or need<=0 then return end
    local text="KÕ Tôc: tiªu diÖt "..PTK_NAMES[target].." ("..GetTask(994).."/"..need..")."
    if GetTask(994)>=need then text=text.." Trë vÒ Thñ Khè nhËn th­ëng.";step=2 end
    TaskNote(51,step,text)
end
function PTK_Show()
    if GetPlayerType()==PTK_CLASS then return 1 end
    return 0
end
function ptk_close() CloseDialog() end
function ptk_open()
    if PTK_Show()~=1 then CloseDialog() return end
    PTK_ResetDay()
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if not PTK_ValidTarget(target) or need<=0 then
            Msg2Player("D÷ liÖu KÕ Tôc ch­a phï hîp. TiÕn ®é ®­îc gi÷ nguyªn; h·y liªn hÖ GM.")
            CloseDialog() return
        end
        PTK_Note(1)
        if GetTask(994)>=need then
            MsgBox("§· hoµn thµnh KÕ Tôc. NhËn 100 ®iÓm kinh nghiÖm?","ptk_finish","ptk_close")
        else
            Talk(1,"ptk_close","KÕ Tôc: ®· tiªu diÖt "..GetTask(994).."/"..need.." "..PTK_NAMES[target]..". H·y tiÕp tôc råi trë l¹i gÆp ta.")
        end
        return
    end
    if GetLevel()<1 or GetTask(984)>=5 then
        Talk(1,"ptk_close","Mçi ngµy nhËn tèi ®a 5 lÇn KÕ Tôc. H·y quay l¹i vµo ngµy mai.") return
    end
    local choices=PTK_TARGETS[GetPlayerType()]
        Say("KÕ Tôc: chän môc tiªu. CÇn diÖt "..PTK_Count(GetLevel()).." qu¸i; hoµn thµnh nhËn 100 kinh nghiÖm. H«m nay ®· nhËn "..GetTask(984).."/5 lÇn.",3,
            PTK_NAMES[choices[1]].."/ptk_take1",PTK_NAMES[choices[2]].."/ptk_take2","§ãng/ptk_close")
end
function ptk_take1() PTK_Take(1) end
function ptk_take2() PTK_Take(2) end
function PTK_Take(choice)
    if PTK_Show()~=1 or GetLevel()<1 then return end
    PTK_ResetDay()
    if GetTask(993)~=0 or GetTask(984)>=5 then ptk_open() return end
    local target=PTK_TARGETS[GetPlayerType()][choice]
    if not target then return end
    local state=SetByte(SetByte(0,1,target),2,PTK_Count(GetLevel())/10)
    local rewards={}
    if GetLevel()<=5 then rewards={{1,0,1,1,0,0,1}} end
    if QuestExchange(993,0,state,{},rewards)~=1 then
        Msg2Player("Ch­a nhËn ®­îc nhiÖm vô. H·y chõa chç trèng trong hµnh trang vµ kÕt thóc giao dÞch.") return
    end
    SetTask(994,0)
    SetTask(984,GetTask(984)+1)
    PTK_Note(0)
    Msg2Player("§· nhËn KÕ Tôc. NhÊn F11 ®Ó xem tiÕn ®é.")
    CloseDialog()
end
function ptk_finish()
    if PTK_Show()~=1 then return end
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if state==0 or need<=0 or not PTK_ValidTarget(target) or GetTask(994)<need then return end
    if QuestExchange(993,state,0,{},{})~=1 then return end
    SetTask(994,0)
    AddOwnExp(100)
    TaskNote(51,-1)
    PTK_ResetDay()
    Msg2Player("Hoµn thµnh KÕ Tôc: nhËn 100 ®iÓm kinh nghiÖm.")
    CloseDialog()
end
function PTK_State()
    if PTK_Show()~=1 then return 0,0 end
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if PTK_ValidTarget(target) and need>0 then
            if GetTask(994)>=need then return 3,0 end
            return 2,0
        end
        return 0,0
    end
    if GetLevel()>=1 and (GetTask(973)~=PTK_Day() or GetTask(984)<5) then return 1,0 end
    return 0,0
end
-- Called only after the common death dispatcher validates player and map context.
function PTK_Death(template,teamSize)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    local count=GetTask(994)
    if need<=0 or template+1~=target or not PTK_ValidTarget(target) or count>=need then return end
    SetTask(994,count+1)
    PTK_Note(1)
    Msg2Player("KÕ Tôc: "..PTK_NAMES[target].." "..(count+1).."/"..need..".")
    -- Archived server variants use 50% solo, 40+10*party size when teamed.
    -- This rate is explicitly reconstructed, not sourced from a client Lua.
    local chance=50
    if teamSize>0 then chance=40+teamSize*10 end
    if chance>100 then chance=100 end
    if random(1,100)<=chance then AddNormalItem(3,PTK_MATERIAL[target],0,0,0,0) end
    if count+1==need then Msg2Player("§· ®ñ sè qu¸i. Trë vÒ gÆp Thñ Khè ®Ó tr¶ nhiÖm vô KÕ Tôc.") end
end

PTK_CLASS=2
function GetPlayerTaskState() return PTK_State() end

-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    local ks,kl=PTK_State()
    if ks==3 then return ks,kl end
    local material=GetTask(36)
    if material>0 and HaveNormalItem(3,material,0,0)>=10 then return 3,1 end
    if material==0 and GetLevel()>=6 then return 1,1 end

    if GetTask(33)==1 and HaveNormalItem(3,8,0,0)>=5 then return 3,1 end
    if GetTask(33)==0 then return 1,1 end
    if GetTask(33)==1 then return 2,1 end
    return ks,kl
end
