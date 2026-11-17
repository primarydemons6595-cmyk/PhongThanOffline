--description: Ë®
--author: yichuan
--date: 2005/3/10

function InitMission()
  StartMissionTimer(2,7,18*15*60);--15·ÖÖÓºó¹Ø±Õ
  SetGlobalValue(2,1);--ÔÊÐí±¨ÃûÁË
  SetMissionV(1, random(100000))
	local d= GetGlobalValue(405)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(405,0)
	end;

	local d= GetGlobalValue(406)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(406,0)
	end;

	local d= GetGlobalValue(407)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(407,0)
	end;

	local d= GetGlobalValue(408)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(408,0)
	end;

	local d= GetGlobalValue(409)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(409,0)
	end;  
  RevivalAllNpc();  
  AddGlobalCountNews("<color=green>Cöa V¹n Tiªn trËn ( thñy)<color> ®· më, mäi ng­êi cã thÓ ®Õn <color=green>Xi V­u Mé<color> dïng Thñy Linh phï ®Ó vµo.", 1);
end;

function RunMission()
    SetGlobalValue(2,2);--ÐÞ¸ÄÕ½¶·×´Ì¬
	idx = 0;
	for i = 1 , 500 do 
	 idx, pidx = GetNextPlayer(2,idx, 0);
	 
	 if (idx == 0) then 
	 break;
	 end;
	 
	 if (pidx > 0) then
	   PlayerIndex = pidx;
	   SetFightState(1);
	 end;
    end;
	--´ËÊ±ÎªGM·¢²¼ÃüÁî£¬ÕýÊ½´òÏìÕ½¶·
	PutMessage("TrËn chiÕn b¾t ®Çu, mäi ng­êi h·y anh dòng chiÕn ®Êu!");
	Msg2MSAll(2, "TrËn chiÕn b¾t ®Çu, mäi ng­êi h·y anh dòng chiÕn ®Êu!");
	 
end;

function EndMission()
	local d= GetGlobalValue(428)--Çå³ý´«ËÍÃÅ
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(428,0)
	end;
  SetGlobalValue(2,0);--²»ÔÊÐí±¨ÃûÁË
  StopMissionTimer(2,3);
  GameOver();
  AddLocalCountNews("<color=green>Cöa V¹n Tiªn trËn ( thñy)<color> ®· ®ãng.", 1);
end;

function GameOver()
  idx = 0;
  for i = 1 , 100 do 
	  idx, pidx = GetNextPlayer(2,0,0);
	  if (idx == 0) then 
 	  	break;
 	  end;
	  	PlayerIndex = pidx;
		LockCamp(0)--½â³ýÕóÓªËø¶¨
		SetFightState(0)
		SetLogoutRV(0)
	  	NewWorld(4,1548,3232); -- ÍË³öµØÍ¼
  end;
end;

function OnLeave(RoleIndex)
PlayerIndex = RoleIndex;
str2 = GetName().."Rêi khái chiÕn tr­êng";
Msg2MSAll(1, str2);
LockCamp(0)--½â³ýÕóÓªËø¶¨
SetFightState(0)
		for x=66,69 do
			if(HaveHandItem(3,x,0,0)>=1)then
				DelHandItem(3,x,0,0)
			end;
		end;

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
end;

-- Project lifecycle reconstruction; original payload retained above.
PT_ID=2
PT_MAP=68
PT_LEVEL=60
PT_FIRST=134
PT_BOSS=138
PT_LABEL="Thuy"
PT_REWARD_TASK=4931
PT_ENTRY_TASK=4935
PT_CLAIM_TASK=4939
PT_REWARD_DETAIL=64
PT_ENTRY_X=1752
PT_ENTRY_Y=3567
PT_EXIT_MAP=4
PT_EXIT_X=1548
PT_EXIT_Y=3232
PT_POINTS={{51040,105568},{56128,105568},{51008,109376},{56192,109376},{53600,107488}}
PT_MOBS={{55,50400,104928},{56,51680,104928},{55,52960,104960},{56,54272,104928},{55,55520,104928},{56,56832,104896},{55,50400,106528},{56,51680,106528},{55,52960,106528},{56,54240,106528},{55,55520,106528},{56,56800,106528},{55,50400,108128},{56,51680,108128},{55,52960,108128},{56,54240,108128},{55,55520,108128},{56,56768,108096},{55,50400,109728},{56,51712,109760},{55,52960,109728},{56,54240,109728},{55,55520,109728},{56,56800,109728}}


-- Original InitMission revived the arena population during preparation.
-- Track each owned normal monster to avoid duplicate spawns on RunMission.
function pt_spawn_mobs()
    for n=1,getn(PT_MOBS) do
        if GetMissionV(39+n)==0 then
            local p=PT_MOBS[n]
            local npc=pt_spawn(p[1],p[2],p[3],"\\script\\phongthan\\van_tien\\mob"..p[1]..".lua")
            if npc>0 then SetMissionV(39+n,npc) end
        end
    end
end

function pt_each(fn)
    local cursor=0
    local old=PlayerIndex
    for n=1,500 do
        local player
        cursor,player=GetNextPlayer(PT_ID,cursor,0)
        if cursor==0 then break end
        if player>0 then PlayerIndex=player;fn() end
    end
    PlayerIndex=old
end
function pt_fight() SetFightState(1);ShowMissionCountdown(PT_ID,3004) end
function pt_exit()
    LockCamp(0);SetFightState(0);SetLogoutRV(0)
    NewWorld(PT_EXIT_MAP,PT_EXIT_X,PT_EXIT_Y)
end
function pt_spawn(tid,x,y,death)
    local npc=AddNpc(tid,PT_LEVEL,SubWorld,x,y,1)
    if npc==0 then return 0 end
    local owned=AddMSNpc(PT_ID,npc,0)
    if owned==nil or owned<=0 then DelNpc(npc);return 0 end
    if death~=nil then
        if SetNpcDeathScript(npc,death)~=1 then DelNpc(npc);return 0 end
    end
    return npc
end
function pt_progress()
    if GetGlobalValue(PT_ID)~=2 then return end
    for n=1,4 do
        if GetMissionV(10+n)==0 and GetMissionV(20+n)==0 then
            local p=PT_POINTS[n]
            local npc=pt_spawn(PT_FIRST+n-1,p[1],p[2],"\\script\\phongthan\\van_tien\\guard"..PT_ID..".lua")
            if npc>0 then SetMissionV(20+n,npc) end
        end
    end
    if GetMissionV(11)+GetMissionV(12)+GetMissionV(13)+GetMissionV(14)==4 and GetMissionV(30)==0 then
        local p=PT_POINTS[5]
        local npc=pt_spawn(PT_BOSS,p[1],p[2],"\\script\\phongthan\\van_tien\\boss"..PT_ID..".lua")
        if npc>0 then SetMissionV(30,npc);Msg2MSAll(PT_ID,"Da ha bon ho phap. Thong Thien Giao Chu xuat hien!") end
    end
end

function InitMission()
    SetMissionV(1,floor(SystemTime()/60)*4+PT_ID)
    for n=10,64 do SetMissionV(n,0) end
    SetGlobalValue(PT_ID,1)
    pt_spawn_mobs()
    local seconds=300
    if EventServiceOwns(PT_ID)==1 then seconds=EventServiceValue(PT_ID,1) end
    StartMissionTimer(PT_ID,3003,18*seconds)
    AddNews("Van Tien Tran ("..PT_LABEL..") mo dang ky. Thoi luong theo cau hinh Event Phong Than.",1)
end
function RunMission()
    if GetGlobalValue(PT_ID)~=1 then return end
    SetGlobalValue(PT_ID,2)
    pt_each(pt_fight)
    pt_spawn_mobs()
    pt_progress()
    StartMissionTimer(PT_ID,3009,18*30)
    Msg2MSAll(PT_ID,"Tran dau bat dau! Ha bon ho phap de goi Thong Thien Giao Chu.")
end
function EndMission()
    SetGlobalValue(PT_ID,0)
    StopMissionTimer(PT_ID,3003)
    StopMissionTimer(PT_ID,3004)
    StopMissionTimer(PT_ID,3009)
    pt_each(pt_exit)
    AddNews("Van Tien Tran ("..PT_LABEL..") ket thuc.",1)
end
function OnLeave(role)
    PlayerIndex=role;LockCamp(0);SetFightState(0);SetLogoutRV(0)
end
