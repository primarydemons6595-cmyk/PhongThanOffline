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
PT_POINTS={{48240,106192},{48304,109808},{52880,102736},{54768,104304},{49776,102992}}
PT_MOBS={{57,53872,109808},{58,52016,101648},{57,55120,103440},{58,47760,107792},{57,51088,105040},{58,50224,110576},{57,52144,101488},{58,55088,106736},{57,51792,107824},{58,47856,104944},{57,53424,105072},{58,52144,102960},{57,49776,106832},{58,52272,111344},{57,48144,109936},{58,52272,103472},{57,53648,107952},{58,49424,108624},{57,51760,109616},{58,48464,105392},{57,55248,108400},{58,53744,101840},{57,52816,106576},{58,54960,105200},{57,47888,104912},{58,48304,106384},{57,51536,106320},{58,53552,103120},{57,50512,107920},{58,50512,109328},{57,51088,111440},{58,49328,109808},{57,54224,104208},{58,52880,109232},{57,48912,107600},{58,51216,104624},{57,52880,110384},{58,52016,101680},{57,52688,102448},{58,52400,104688},{57,54224,106096},{58,48400,108912},{57,51312,108720},{58,54416,108976},{57,52048,101744},{58,54672,107600},{57,52912,108144},{58,53104,104048},{57,48464,105424},{58,52560,103376},{57,49168,108624},{58,54672,104624},{57,51888,110928},{58,49488,109072},{57,48272,106608},{58,47920,104944},{57,48304,109616},{58,53264,106832},{57,47920,106608},{58,52176,102896},{57,52048,101712},{58,50416,109776},{57,49232,108816},{58,47856,104912},{57,51152,104688},{58,52240,108208},{57,51248,104592},{58,47888,104944},{57,51792,107504},{58,52464,105264},{57,49648,107184},{58,53456,108240},{57,52688,102192},{58,49584,109520},{57,53072,103408},{58,50064,106352},{57,48560,105968},{58,50064,107760},{57,50192,106704},{58,52816,110736},{57,49040,109744},{58,53104,107088},{57,53584,110000},{58,53936,109552},{57,52048,101680},{58,51280,104560},{57,51024,111184},{58,48976,110064},{57,54864,104880},{58,51248,110768},{57,52176,102928},{58,50672,110544},{57,48720,107568},{58,50000,107152},{57,48208,107088},{58,51984,106000},{57,51344,104496},{58,52720,104336},{57,54768,105584},{58,54576,103600},{57,52880,105136},{58,48912,106960},{57,48272,107344},{58,49520,110032},{57,53104,109712},{58,49584,106800},{57,47696,107600},{58,54960,108560},{57,54288,109328},{58,54864,104048},{57,47920,104976},{58,53104,102640},{57,47984,105040},{58,54448,104368},{57,49168,106928},{58,47824,104912},{57,52048,109168},{58,52048,101648},{57,52944,102896},{58,52080,101776},{57,51184,104656},{58,53232,107952},{57,52048,101616},{58,54768,106160},{57,54448,104784},{58,50704,108240},{57,48720,110000},{58,48784,109648},{57,52944,107216},{58,47888,109808},{57,52176,102960},{58,52176,102992},{57,50096,109904},{58,51088,104752},{57,52528,110864},{58,52208,102864},{57,55312,105744},{58,49808,109872},{57,48144,105520},{58,50384,110224},{57,53552,101872},{58,52816,103152},{57,52912,102160},{58,52176,103024},{57,52432,102800},{58,50928,110416},{57,52496,101360},{58,52624,102000},{57,55472,103792},{58,54736,103728}}

pt_original_boss_death=OnDeath
GetWorldPos=GetContentWorldPos
if pt_boss_native_add==nil then pt_boss_native_add=AddNpc end
function AddNpc(tid,level,world,x,y,barrier)
 local n=pt_boss_native_add(tid,level,world,x,y,barrier)
 if n>0 and (tid==410 or tid==413 or tid==414 or tid==415) then
  if AddMSNpc(PT_ID,n,0)==0 then DelNpc(n);return 0 end
  SetNpcDeathScript(n,"\\script\\phongthan\\van_tien\\chest.lua")
 end
 return n
end
function OnDeath(npc)
 if EventVttKill(npc)==1 then
  pt_original_boss_death()
  Msg2MSAll(PT_ID,"ß∑ ti™u di÷t Th´ng Thi™n Gi∏o ChÒ. Ng≠Íi c„ nhi÷m vÙ hÓp l÷ Æ∑ nhÀn th≠Îng; n’u h‡nh trang Æ«y h∑y g∆p ßπi Phu.")
 end
end
