--description: Õ®ÃÏΩÃ÷˜
--author: yichuan
--date: 2005/2/28

function OnDeath()

	--≤ªª·”–¿∂π÷
	w,x,y=GetWorldPos()
	if(GetTeam()~=0)then
		-- ”–∂”ŒÈ(∞¸¿®÷ª”–◊‘º∫“ª∏ˆ»Àµƒ)
		local oldPlayer=PlayerIndex
		local membercount=GetTeamSize()
		-- ±È¿˙∂”÷–∂”‘±
		for i=1,membercount do
			PlayerIndex=GetTeamMember(i)
			calc_task(w)
		end
		PlayerIndex=oldPlayer
	else
		-- Œﬁ∂”ŒÈ
		calc_task(w)
	end

	local  mapid,x,y=GetWorldPos()
	if(mapid==67)then
		local pos = {
			{mx=1531,my=3267},
			{mx=1547,my=3164},
			{mx=1443,my=3164},
			{mx=1423,my=3325},
			{mx=1479,my=3279},
			{mx=1335,my=3277},
			{mx=1464,my=3368},
			{mx=1475,my=3227},
			{mx=1318,my=3302},
			{mx=1397,my=3251}
		}
		name_0="B∂o r≠¨ng"

		local i1=random(1,10)
		local k1=AddNpc(410,40,SubWorld,pos[i1].mx*32,pos[i1].my*32)
		SetGlobalValue(400,k1)
		SetNpcName(k1,name_0)
		SetNpcScript(k1,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k2=AddNpc(410,40,SubWorld,1595*32,3335*32)
		SetGlobalValue(401,k2)
		SetNpcName(k2,name_0)
		SetNpcScript(k2,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k3=AddNpc(410,40,SubWorld,1336*32,3168*32)
		SetGlobalValue(402,k3)
		SetNpcName(k3,name_0)
		SetNpcScript(k3,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k4=AddNpc(410,40,SubWorld,1368*32,3376*32)
		SetGlobalValue(403,k4)
		SetNpcName(k4,name_0)
		SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

	--						k5=AddNpc(410,40,SubWorld,200*8*32,208*16*32)
	--						SetGlobalValue(404,k5)
	--						SetNpcName(k4,name_0)
	--						SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		nx=floor(pos[i1].mx/8)
		ny=floor(pos[i1].my/16)
		AddGlobalCountNews("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi <color=green>Vπn Ti™n trÀn (thÊ)<color> Æ∑ bﬁ ph©n t∏n vµo b∂o r≠¨ng Î <color=green> ("..nx..", "..ny.."<color>.",20)
		Msg2SubWorld("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi Vπn Ti™n trÀn Æ∑ bﬁ ph©n t∏n, vµo b∂o r≠¨ng ("..nx..", "..ny.."). ")

	elseif(mapid==68)then
		local pos = {
			{mx=1520,my=3383},
			{mx=1555,my=3509},
			{mx=1691,my=3177},
			{mx=1692,my=3490},
			{mx=1734,my=3552},
			{mx=1763,my=3575},
			{mx=1661,my=3552},
			{mx=1521,my=3567},
			{mx=1729,my=3242},
			{mx=1675,my=3359}
		}
		name_0="B∂o r≠¨ng"

		local i1=random(1,10)
		local k1=AddNpc(413,60,SubWorld,pos[i1].mx*32,pos[i1].my*32)
		SetGlobalValue(405,k1)
		SetNpcName(k1,name_0)
		SetNpcScript(k1,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k2=AddNpc(413,60,SubWorld,1589*32,3263*32)
		SetGlobalValue(406,k2)
		SetNpcName(k2,name_0)
		SetNpcScript(k2,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k3=AddNpc(413,60,SubWorld,1529*32,3156*32)
		SetGlobalValue(407,k3)
		SetNpcName(k3,name_0)
		SetNpcScript(k3,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k4=AddNpc(413,60,SubWorld,1513*32,3228*32)
		SetGlobalValue(408,k4)
		SetNpcName(k4,name_0)
		SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

	--						k5=AddNpc(413,60,SubWorld,200*8*32,208*16*32)
	--						SetGlobalValue(409,k4)
	--						SetNpcName(k4,name_0)
	--						SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		nx=floor(pos[i1].mx/8)
		ny=floor(pos[i1].my/16)
		AddGlobalCountNews("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi <color=green>Vπn Ti™n trÀn (thÒy)<color> Æ∑ bﬁ ph©n t∏n vµo b∂o r≠¨ng Î <color=green> ("..nx..", "..ny.."<color>.",20)
		Msg2SubWorld("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi Vπn Ti™n trÀn Æ∑ bﬁ ph©n t∏n, vµo b∂o r≠¨ng ("..nx..", "..ny.."). ")

	elseif(mapid==69)then
		local pos = {
			{mx=1509,my=3225},
			{mx=1510,my=3430},
			{mx=1653,my=3219},
			{mx=1710,my=3262},
			{mx=1642,my=3392},
			{mx=1647,my=3400},
			{mx=1724,my=3377},
			{mx=1583,my=3442},
			{mx=1564,my=3367},
			{mx=1662,my=3283}
		}
		name_0="B∂o r≠¨ng"

		local i1=random(1,10)
		local k1=AddNpc(414,80,SubWorld,pos[i1].mx*32,pos[i1].my*32)
		SetGlobalValue(410,k1)
		SetNpcName(k1,name_0)
		SetNpcScript(k1,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k2=AddNpc(414,80,SubWorld,1558*32,3225*32)
		SetGlobalValue(411,k2)
		SetNpcName(k2,name_0)
		SetNpcScript(k2,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k3=AddNpc(414,80,SubWorld,1730*32,3437*32)
		SetGlobalValue(412,k3)
		SetNpcName(k3,name_0)
		SetNpcScript(k3,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k4=AddNpc(414,80,SubWorld,1722*32,3178*32)
		SetGlobalValue(413,k4)
		SetNpcName(k4,name_0)
		SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

	--						k5=AddNpc(414,80,SubWorld,200*8*32,208*16*32)
	--						SetGlobalValue(414,k4)
	--						SetNpcName(k4,name_0)
	--						SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		nx=floor(pos[i1].mx/8)
		ny=floor(pos[i1].my/16)
		AddGlobalCountNews("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi <color=green>Vπn Ti™n trÀn (h·a)<color> Æ∑ bﬁ ph©n t∏n vµo b∂o r≠¨ng Î <color=green> ("..nx..", "..ny.."<color>.",20)
		Msg2SubWorld("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi Vπn Ti™n trÀn Æ∑ bﬁ ph©n t∏n, vµo b∂o r≠¨ng ("..nx..", "..ny.."). ")

	elseif(mapid==70)then
		local pos = {
			{mx=1549,my=4093},
			{mx=1766,my=3351},
			{mx=1776,my=3034},
			{mx=1548,my=3056},
			{mx=1527,my=3144},
			{mx=1593,my=3386},
			{mx=1781,my=3265},
			{mx=1670,my=3036},
			{mx=1605,my=3272},
			{mx=1731,my=3154}
		}
		name_0="B∂o r≠¨ng"

		local i1=random(1,10)
		local k1=AddNpc(413,100,SubWorld,pos[i1].mx*32,pos[i1].my*32)
		SetGlobalValue(415,k1)
		SetNpcName(k1,name_0)
		SetNpcScript(k1,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k2=AddNpc(414,100,SubWorld,1657*32,3205*32)
		SetGlobalValue(416,k2)
		SetNpcName(k2,name_0)
		SetNpcScript(k2,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k3=AddNpc(414,100,SubWorld,1725*32,3049*32)
		SetGlobalValue(417,k3)
		SetNpcName(k3,name_0)
		SetNpcScript(k3,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		k4=AddNpc(414,100,SubWorld,1512*32,3362*32)
		SetGlobalValue(418,k4)
		SetNpcName(k4,name_0)
		SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

	--						k5=AddNpc(414,100,SubWorld,200*8*32,208*16*32)
	--						SetGlobalValue(419,k4)
	--						SetNpcName(k4,name_0)
	--						SetNpcScript(k4,"\\script\\ÕÚœ…’Û\\±¶œ‰.lua")

		nx=floor(pos[i1].mx/8)
		ny=floor(pos[i1].my/16)
		AddGlobalCountNews("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi <color=green>Vπn Ti™n trÀn (phong)<color> Æ∑ bﬁ ph©n t∏n vµo b∂o r≠¨ng Î <color=green> ("..nx..", "..ny.."<color>.",20)
		Msg2SubWorld("Ph©n th©n cÒa Th´ng Thi™n Gi∏o ChÒ tπi Vπn Ti™n trÀn Æ∑ bﬁ ph©n t∏n, vµo b∂o r≠¨ng ("..nx..", "..ny.."). ")
	end;
end;

function calc_task(w1)
	w,x,y=GetWorldPos()
	if(w==w1)then
		local today=floor(SystemTime()/86400)
		if(today==GetTask(409))then
			if(GetTask(408)==9)then
				SetTask(407,0)
				SetTask(408,100000)
				Msg2Player("Bπn Æ∑ chi’n thæng Th´ng Thi™n Gi∏o ChÒ")
				TopMessage("Bπn Æ∑ chi’n thæng Th´ng Thi™n Gi∏o ChÒ")
			end
		else
			SetTask(407,0)
			SetTask(408,0)
			TopMessage("Nhi÷m vÙ Vπn Ti™n trÀn Æ∑ qu∏ hπn")
			Msg2Player("Nhi÷m vÙ Vπn Ti™n trÀn Æ∑ qu∏ hπn")
		end
	end
end

pt_original_boss_death=OnDeath

-- Deterministic deferred reward replaces the empty original chest reward.
PT_ID=4
PT_MAP=70
PT_LEVEL=100
PT_FIRST=144
PT_BOSS=148
PT_LABEL="Phong"
PT_REWARD_TASK=4933
PT_ENTRY_TASK=4937
PT_CLAIM_TASK=4941
PT_REWARD_DETAIL=65
PT_ENTRY_X=1671
PT_ENTRY_Y=3035
PT_EXIT_MAP=1
PT_EXIT_X=1510
PT_EXIT_Y=3290
PT_POINTS={{52832,99008},{57952,99008},{52832,102848},{57952,102848},{55392,100928}}
PT_MOBS={{144,52192,98368},{145,53472,98368},{146,54752,98368},{147,56000,98336},{144,57312,98368},{145,58336,98112},{146,52192,99968},{147,53472,99968},{144,54752,99968},{145,56032,99968},{146,57312,99968},{147,58336,99712},{144,52192,101568},{145,53472,101568},{146,54752,101568},{147,56032,101568},{144,57312,101568},{145,58336,101312},{146,52192,103168},{147,53472,103168},{144,54784,103200},{145,56032,103168},{146,57280,103136},{147,58336,102912}}

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
function pt_fight() SetFightState(1) end
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

GetWorldPos=GetContentWorldPos
function pt_award()
    calc_task(PT_MAP)
    if GetTask(PT_REWARD_TASK)==0 then SetTask(PT_REWARD_TASK,GetMissionV(1)) end
    Msg2Player("Da thang Van Tien Tran. Nhan thuong tai cong vao sau khi ra ngoai.")
end
function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 or GetMissionV(30)~=npc or GetMissionV(31)==1 then return end
    SetMissionV(31,1)
    pt_each(pt_award)
    Msg2MSAll(PT_ID,"Thong Thien Giao Chu da bi danh bai. Thuong duoc luu de nhan tai cong vao.")
end

-- Restore original boss quest credit and four original native-drop chests.
pt_boss_native_add=AddNpc
function AddNpc(tid,level,world,x,y,barrier)
    local n=0
    if barrier==nil then n=pt_boss_native_add(tid,level,world,x,y)
    else n=pt_boss_native_add(tid,level,world,x,y,barrier) end
    if n>0 and (tid==410 or tid==413 or tid==414 or tid==415) then
        if AddMSNpc(PT_ID,n,0)==0 then DelNpc(n);return 0 end
        SetNpcDeathScript(n,"\\script\\phongthan\\van_tien\\chest.lua")
    end
    return n
end
function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 or GetMissionV(30)~=npc or GetMissionV(31)==1 then return end
    SetMissionV(31,1)
    pt_original_boss_death()
    Msg2MSAll(PT_ID,"Thong Thien Giao Chu da bi danh bai. Bao ruong goc da xuat hien; thuong nhiem vu tra tai Thien Hung.")
end
