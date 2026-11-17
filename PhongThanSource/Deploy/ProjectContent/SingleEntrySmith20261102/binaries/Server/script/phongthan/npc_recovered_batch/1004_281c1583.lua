--description: ÍòÏÉÕó[Ë®]
--author: yujin
--date: 2005/4/14

function main(sel)
	tasks = 
	{
		{"V¹n Tiªn trËn","renwu2";show=1}
	}
	if(GetGlobalValue(2)==0)then
		SayTask(11134,tasks)			
	elseif(GetGlobalValue(2)==1)then
		SayTask(11135,tasks)			
	elseif(GetGlobalValue(2)==2)then
		SayTask(11136,tasks)			
	end;
end;

function no()
		CloseDialog()
end;
---------------------------
function   renwu2()
	idx = SubWorldID2Idx(68); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿
	if ( GetMorphType()==364)or(IsPlayerInsideWeapon(PlayerIndex)>0)then
			Talk(1,"no",11137)
	elseif(GetLevel()<=50)or(GetLevel()>=71)then
			Talk(1,"no",11138)
	elseif(7~=GetCamp())or(87<GetPK())then
			Talk(1,"no","ChØ cho phÐp nh©n vËt phe xanh  vµo.")
	elseif(HaveNormalItem(3,63,0,0)>=1)or(GetTask(421)==GetMissionV(1))then
			MsgBox(11139,"yes_wxz","no")
	else
			Talk(1,"no",11140)
	end;
end;

function do_enter()
	DelHandItem(3,66,0,0)
	DelHandItem(3,67,0,0)
	DelHandItem(3,68,0,0)
	DelHandItem(3,69,0,0)
	for i=1,60 do
			if(HaveNormalItem(3,66,0,0)>=1)then
						DelNormalItem(3,66,0,0)
			elseif(HaveNormalItem(3,67,0,0)>=1)then
						DelNormalItem(3,67,0,0)	
			elseif(HaveNormalItem(3,68,0,0)>=1)then
						DelNormalItem(3,68,0,0)
			elseif(HaveNormalItem(3,69,0,0)>=1)then
						DelNormalItem(3,69,0,0)
			else
						break;
			end;
	end;
	local mission_step = GetGlobalValue(2)
	if (mission_step == 1) then
		SetFightState(0)
	else
		SetFightState(1)
	end
	AddMSPlayer(2,1)
	SetLogoutRV(1)
	SetTask(421,GetMissionV(1))
	LockCamp(1)--Ëø¶¨ÕóÓª
	NewWorld(68,1752,3567)
	StopUsePills()
	Msg2Player("Tr¹ng th¸i tu luyÖn tù ®éng t¾t!")
	CloseDialog()
end

function  yes_ib()
	idx = SubWorldID2Idx(68); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿

	local mission_step = GetGlobalValue(2)
	local ibplayer_count =  GetGlobalValue(66)
	local noneib_player_count = GetGlobalValue(62)
	local player_count = ibplayer_count + noneib_player_count
	local player_key = GetTask(421)
	local mission_key = GetMissionV(1)

	if (mission_step == 1) then
		if(player_count <200) then
			if(player_key ~= mission_key) then
				if(FindAValidIBItem(8,145,2,0)>=1)then
					DelNormalItem(3,63,0,0)
					CostIBItem(FindAValidIBItem(8,145,2,0))
					SetGlobalValue(66,GetGlobalValue(66)+1)
					do_enter()
				else
					Talk(1,"no","Xin lçi! Ng­¬i kh«ng cã Thñy Linh th¹ch!")
				end
			else
				do_enter()
			end	
		else
			Talk(1,"no","Sè ng­êi trong trËn ®· ®Çy, Linh Th¹ch còng kh«ng thÓ gióp ng­¬i vµo trong.")
		end
	elseif(mission_step == 2) then
		if(player_count < 200 and player_key==mission_key) then
			do_enter()
		else
			Talk(1,"no","Ng­¬i ®Õn trÔ ma qu¸i trong trËn ®· hãa phÐp ng¨n chÆn cöa vµo!")
		end
	else
		Talk(1,"no",11143)
	end

end


function  yes_wxz()
	idx = SubWorldID2Idx(68); -- ?±£µØÍ¼ÔÚÕâÌ¨·þÎñÆ÷
	if (idx == -1) then 
		return
	end;
	SubWorld = idx; -- ?Îñ¿ªÆô±ØÐèµÄ±äÁ¿

	local mission_step = GetGlobalValue(2)
	local ibplayer_count =  GetGlobalValue(66)
	local noneib_player_count = GetGlobalValue(62)
	local player_count = ibplayer_count + noneib_player_count
	local player_key = GetTask(421)
	local mission_key = GetMissionV(1)
	
	if (player_key == mission_key) then
		if (mission_step == 1 or mission_step == 2) then
			do_enter()
		else
			Talk(1,"no",11143)
		end
	else
		if (mission_step == 1) then
			if(player_count < 200 )then
				if(noneib_player_count < 50) then
					DelNormalItem(3,63,0,0)
					SetGlobalValue(62,noneib_player_count+1)
					do_enter()
				else
					MsgBox("Sè ng­êi trong V¹n Tiªn trËn ®· ®Çy, nÕu ng­¬i cã <color=green>Thñy Linh th¹ch<color> ta sÏ gióp ng­êi vµo bªn trong.","yes_ib","no")
				end
			else
				Talk(1,"no","Ng­¬i ®Õn trÔ ma qu¸i trong trËn ®· hãa phÐp ng¨n chÆn cöa vµo!")
			end
		else
			Talk(1,"no",11143)
		end
	end
end;

PT_ID=2
PT_MAP=68
PT_ENTRY_X=1752
PT_ENTRY_Y=3567

function renwu3()
 SubWorld=SubWorldID2Idx(PT_MAP)
 if SubWorld<0 then CloseDialog();return end
 if GetTask(4789+PT_ID)==GetMissionV(1) then pt_enter();return end
 SayButtons("Vµo V¹n Tiªn TrËn: tù dïng <color=green>1 Linh Phï ®óng hÖ<color>; nÕu kh«ng cã hoÆc suÊt th­êng ®· ®ñ, kiÓm tra <color=green>Linh Th¹ch ®óng hÖ cßn h¹n<color>. Linh Th¹ch kh«ng tiªu hao. Vµo l¹i <color=red>cïng l­ît<color> kh«ng tèn thªm vÐ.",1,"Vµo trËn/pt_enter")
end
function pt_enter() pt_enter_route(0) end
function pt_enter_ib() pt_enter_route(1) end
function pt_enter_route(premium)
 SubWorld=SubWorldID2Idx(PT_MAP)
 local result=EventEnterVanTien(PT_ID,PT_ENTRY_X,PT_ENTRY_Y,premium)
 if result~=1 then
  local message="Ch­a ®Õn giê ®¨ng ký, trËn ®· ®ñ ng­êi hoÆc cÊp ®é ch­a phï hîp."
  if result==-1 then
   if premium==1 then message="Ng­¬i cÇn mang <color=green>Thñy Linh Th¹ch<color> ®Ó vµo trËn. Kh«ng cÇn kÌm Linh Phï. Linh Th¹ch ®­îc gi÷ l¹i sau khi vµo trËn."
   else message="Ng­¬i cÇn <color=green>Thñy Linh Phï<color> hoÆc <color=green>Thñy Linh Th¹ch<color> ®Ó vµo trËn. Linh Th¹ch kh«ng tiªu hao; Linh Phï dïng mét chiÕc mçi l­ît míi." end
  elseif result==-2 then message="H·y kÕt thóc giao dÞch vµ thö l¹i."
  elseif result==-4 then message="Sè ng­êi ®¨ng ký trËn nµy ®· ®ñ." end
  Talk(1,"no",message);return
 end
 SetLogoutRV(1);LockCamp(1)
 if GetGlobalValue(PT_ID)==1 then SetFightState(0);ShowMissionCountdown(PT_ID,2999+PT_ID*2)
 else SetFightState(1);ShowMissionCountdown(PT_ID,3000+PT_ID*2) end
 StopUsePills();CloseDialog()
end
do_enter=pt_enter
yes_wxz=pt_enter
yes_ib=pt_enter_ib

renwu1=renwu3
renwu2=renwu3

function main()
 Say("V¹n Tiªn TrËn Thñy: 10h05, 13h05, 18h05, 21h05, 23h05. Mçi l­ît 35 phót. Dïng Thñy Linh Phï hoÆc Linh Th¹ch t­¬ng øng.",2,"Vµo trËn/pt_enter","KÕt thóc/no")
end
