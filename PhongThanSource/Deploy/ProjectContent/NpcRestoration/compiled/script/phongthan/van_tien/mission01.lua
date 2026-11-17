--description: ÍÁ
--author: yichuan
--date: 2005/3/10

function InitMission()
StartMissionTimer(1,6,18*15*60);--15·ÖÖÓºó¹Ø±Õ
SetGlobalValue(1,1);--ÔÊÐí±¨ÃûÁË
SetMissionV(1, random(100000))
	local d= GetGlobalValue(400)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(400,0)
	end;

	local d= GetGlobalValue(401)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(401,0)
	end;

	local d= GetGlobalValue(402)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(402,0)
	end;

	local d= GetGlobalValue(403)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(403,0)
	end;

	local d= GetGlobalValue(404)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(404,0)
	end;  
RevivalAllNpc();  
AddGlobalCountNews("<color=green>V¹n Tiªn trËn (thæ)<color> ®· më, mäi ng­êi cã thÓ ®Õn <color=green>Sïng Thµnh §¹i Doanh<color> dïng thæ Linh phï ®Ó vµo.", 1);
end;

function RunMission()
    SetGlobalValue(1,2);--ÐÞ¸ÄÕ½¶·×´Ì¬
	idx = 0;
	for i = 1 , 500 do 
	 idx, pidx = GetNextPlayer(1,idx, 0);
	 
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
	Msg2MSAll(1, "TrËn chiÕn b¾t ®Çu, mäi ng­êi h·y anh dòng chiÕn ®Êu!");
	 
end;

function EndMission()
	local d= GetGlobalValue(427)--Çå³ý´«ËÍÃÅ
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(427,0)
	end;
  SetGlobalValue(1,0);--²»ÔÊÐí±¨ÃûÁË
  StopMissionTimer(1,2);
  GameOver();
  AddLocalCountNews("<color=green>Cöa V¹n Tiªn trËn (thæ)<color> ®· ®ãng.",1);
end;

function GameOver()
  idx = 0;
  for i = 1 , 100 do 
	  idx, pidx = GetNextPlayer(1,0,0);
	  if (idx == 0) then 
 	  	break;
 	  end;
	  	PlayerIndex = pidx;
		LockCamp(0)--½â³ýÕóÓªËø¶¨
		SetFightState(0)
		SetLogoutRV(0)
	  	NewWorld(2,1650,3159); -- ÍË³öµØÍ¼
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
PT_ID=1
PT_MAP=67
PT_LEVEL=40
PT_FIRST=129
PT_BOSS=133
PT_LABEL="Tho"
PT_REWARD_TASK=4930
PT_ENTRY_TASK=4934
PT_CLAIM_TASK=4938
PT_REWARD_DETAIL=63
PT_ENTRY_X=1325
PT_ENTRY_Y=3280
PT_EXIT_MAP=2
PT_EXIT_X=1650
PT_EXIT_Y=3159
PT_POINTS={{44640,101344},{49760,101344},{44608,105152},{49760,105184},{47200,103264}}
PT_MOBS={{53,44000,100704},{54,45248,100640},{53,46560,100704},{54,47840,100704},{53,49120,100704},{54,50400,100704},{53,44000,102304},{54,45280,102304},{53,46560,102304},{54,47840,102304},{53,49120,102304},{54,50336,102240},{53,44032,103872},{54,45280,103904},{53,46528,103904},{54,47776,103840},{53,49120,103904},{54,50368,103872},{53,44000,105504},{54,45280,105504},{53,46560,105472},{54,47776,105472},{53,49120,105504},{54,50336,105536}}


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
function pt_fight() SetFightState(1);ShowMissionCountdown(PT_ID,3002) end
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
    StartMissionTimer(PT_ID,3001,18*seconds)
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
    StopMissionTimer(PT_ID,3001)
    StopMissionTimer(PT_ID,3002)
    StopMissionTimer(PT_ID,3009)
    pt_each(pt_exit)
    AddNews("Van Tien Tran ("..PT_LABEL..") ket thuc.",1)
end
function OnLeave(role)
    PlayerIndex=role;LockCamp(0);SetFightState(0);SetLogoutRV(0)
end
