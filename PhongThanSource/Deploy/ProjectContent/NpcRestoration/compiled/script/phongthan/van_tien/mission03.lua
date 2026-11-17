--description: »ð
--author: yichuan
--date: 2005/3/10

function InitMission()
  StartMissionTimer(3,8,18*15*60);--15·ÖÖÓºó¹Ø±Õ,3misson,4timer±íÊ¾»ð
  SetGlobalValue(3,1);--ÔÊÐí±¨ÃûÁË
  SetMissionV(1, random(100000))
	local d= GetGlobalValue(410)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(410,0)
	end;

	local d= GetGlobalValue(411)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(411,0)
	end;

	local d= GetGlobalValue(412)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(412,0)
	end;

	local d= GetGlobalValue(413)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(413,0)
	end;

	local d= GetGlobalValue(414)--Çå³ý±¦Ïä
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(414,0)
	end;  
  RevivalAllNpc();  
  AddGlobalCountNews("<color=green>Cöa V¹n Tiªn trËn (Háa)<color> ®· më, mäi ng­êi cã thÓ ®Õn <color=green>Ngäc H­ Cung<color> dïng Háa Linh phï ®Ó vµo.", 1);
end;

function RunMission()
    SetGlobalValue(3,2);--ÐÞ¸ÄÕ½¶·×´Ì¬
	idx = 0;
	for i = 1 , 500 do 
	 idx, pidx = GetNextPlayer(3,idx, 0);
	 
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
	local d= GetGlobalValue(429)--Çå³ý´«ËÍÃÅ
	if (d > 0) then
		DelNpc(d)
		SetGlobalValue(429,0)
	end;
  SetGlobalValue(3,0);--²»ÔÊÐí±¨ÃûÁË
  StopMissionTimer(3,4);
  GameOver();
  AddLocalCountNews("<color=green>Cöa V¹n Tiªn trËn (Háa)<color> ®· ®ãng.", 1);
end;

function GameOver()
  idx = 0;
  for i = 1 , 100 do 
	  idx, pidx = GetNextPlayer(3,0,0);
	  if (idx == 0) then 
 	  	break;
 	  end;
	  	PlayerIndex = pidx;
		LockCamp(0)--½â³ýÕóÓªËø¶¨
		SetFightState(0)
		SetLogoutRV(0)
	  	NewWorld(3,1709,3105); -- ÍË³öµØÍ¼
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
PT_ID=3
PT_MAP=69
PT_LEVEL=80
PT_FIRST=139
PT_BOSS=143
PT_LABEL="Hoa"
PT_REWARD_TASK=4932
PT_ENTRY_TASK=4936
PT_CLAIM_TASK=4940
PT_REWARD_DETAIL=65
PT_ENTRY_X=1650
PT_ENTRY_Y=3400
PT_EXIT_MAP=3
PT_EXIT_X=1709
PT_EXIT_Y=3105
PT_POINTS={{50624,103136},{55744,103136},{50624,106976},{55744,106976},{53184,105056}}
PT_MOBS={{57,49984,102496},{58,51200,102560},{57,52544,102496},{58,53824,102496},{57,55104,102496},{58,56288,102400},{57,49984,104096},{58,51296,104032},{57,52512,104128},{58,53824,104096},{57,55104,104096},{58,56288,104000},{57,49984,105696},{58,51264,105696},{57,52544,105696},{58,53824,105696},{57,55104,105696},{58,56288,105600},{57,49984,107296},{58,51264,107296},{57,52544,107296},{58,53824,107296},{57,55104,107296},{58,56288,107200}}


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
function pt_fight() SetFightState(1);ShowMissionCountdown(PT_ID,3006) end
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
    StartMissionTimer(PT_ID,3005,18*seconds)
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
    StopMissionTimer(PT_ID,3005)
    StopMissionTimer(PT_ID,3006)
    StopMissionTimer(PT_ID,3009)
    pt_each(pt_exit)
    AddNews("Van Tien Tran ("..PT_LABEL..") ket thuc.",1)
end
function OnLeave(role)
    PlayerIndex=role;LockCamp(0);SetFightState(0);SetLogoutRV(0)
end
