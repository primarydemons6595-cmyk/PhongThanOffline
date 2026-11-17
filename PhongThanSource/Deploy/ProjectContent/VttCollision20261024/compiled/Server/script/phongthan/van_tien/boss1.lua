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
PT_POINTS={{46288,101104},{49520,101328},{48912,104400},{45584,106416},{51024,106576}}
PT_MOBS={{53,44816,106832},{54,52272,100816},{53,49584,107792},{54,45712,100304},{53,48720,103920},{54,48048,108560},{53,52944,104336},{54,44624,103472},{53,48976,100656},{54,46512,105040},{53,46800,102448},{54,48752,106288},{53,50736,104976},{54,43888,101456},{53,51472,102800},{54,46800,107024},{53,43728,105200},{54,49680,102352},{53,52560,105392},{54,50672,100176},{53,49552,107824},{54,47280,100848},{53,53072,102416},{54,49648,105584},{53,48240,102000},{54,46576,108464},{53,46032,103696},{54,45136,105424},{53,49392,105104},{54,45520,101616},{53,47376,103632},{54,50032,103856},{53,51664,104080},{54,48112,105168},{53,51312,101584},{54,50000,101200},{53,46064,106128},{54,44592,100560},{53,44624,102320},{54,45744,107472},{53,49616,108208},{54,52432,103312},{53,48560,107344},{54,45712,102672},{53,44560,104528},{54,49072,108752},{53,52528,105360},{54,47600,107632},{53,49968,105264},{54,45296,101200},{53,44912,103824},{54,48752,104400},{53,45488,106864},{54,51056,100368},{53,47216,106960},{54,51984,102896},{53,47248,107696},{54,46416,101808},{53,50768,100784},{54,49456,101584},{53,52752,105488},{54,46896,107408},{53,45232,105808},{54,49968,103504},{53,49296,108144},{54,49520,107632},{53,49072,107856},{54,46416,104208},{53,52752,104400},{54,48656,107856},{53,48944,101584},{54,48592,100688},{53,52848,101872},{54,49328,101200},{53,44816,104176},{54,48560,104944},{53,45488,106512},{54,45264,107280},{53,51440,101328},{54,47024,101424},{53,48080,102320},{54,47984,108304},{53,51024,104912},{54,49744,105488},{53,45776,107248},{54,44432,102672},{53,52624,103344},{54,48560,108176},{53,49776,105456},{54,50896,105040},{53,51696,103280},{54,49872,102064},{53,50576,100720},{54,48432,105520},{53,49040,107216},{54,49424,107536},{53,46128,100784},{54,52432,105264},{53,49840,105392},{54,48528,101904},{53,49776,100976},{54,44368,101456},{53,52208,104496},{54,52496,105328},{53,49648,100368},{54,50928,105008},{53,46576,107376},{54,45840,106960},{53,50960,100752},{54,51376,100816},{53,47184,103088},{54,45296,106704},{53,50416,104752},{54,52080,101136},{53,49488,107600},{54,44336,104528},{53,51888,104304},{54,47248,105200},{53,49904,105296},{54,49552,107696},{53,47120,102672},{54,49552,107728},{53,44208,103952},{54,44400,103024},{53,46192,107056},{54,45776,103536},{53,52688,102864},{54,47984,101968},{53,52432,103120},{54,50576,105136},{53,49776,105040},{54,44048,104304},{53,48816,108464},{54,46000,102288},{53,46352,105968},{54,49616,108240},{53,48784,105968},{54,48016,107920},{53,47344,103344},{54,47696,102640},{53,44656,100816},{54,47440,107760},{53,48368,105328},{54,44048,101200},{53,50608,105104},{54,46512,100592},{53,44144,104880},{54,46896,100912},{53,51472,103024},{54,46000,101936},{53,46320,103920},{54,45584,106736},{53,44272,104272},{54,46800,106800},{53,44048,105424},{54,47472,108528},{53,45776,103792},{54,49616,101872},{53,44688,100944},{54,52208,104368},{53,49264,107472},{54,44592,104208},{53,47280,103280},{54,51024,104656},{53,47472,104944},{54,52752,104560},{53,51888,103184},{54,49520,101744},{53,44624,103408},{54,49200,101136},{53,50320,101680},{54,46864,101008},{53,48272,104240},{54,48688,101136},{53,46864,103056},{54,51280,103600},{53,46448,102192},{54,47440,108144},{53,47472,108432},{54,51216,104368},{53,49872,102384},{54,47408,107312},{53,46384,102320},{54,44112,101616},{53,51408,104272},{54,44752,101488},{53,50320,100560},{54,49072,104048},{53,50992,104272},{54,46896,107120},{53,45136,105584},{54,46288,102192},{53,45648,102000},{54,47088,102608},{53,49328,104752},{54,45200,106704},{53,48528,101232},{54,51312,104560},{53,46640,107568},{54,49264,104496},{53,44720,101264},{54,49904,102352},{53,47376,105008},{54,45168,101264},{53,50640,104464},{54,48784,107248},{53,47696,102256},{54,46896,107632},{53,47024,101488},{54,45776,105520},{53,52112,103856},{54,44912,103888},{53,48784,100784},{54,45456,105648},{53,46448,107920},{54,45936,102896},{53,51280,100656},{54,44336,101872},{53,46704,106704},{54,49328,100432},{53,44144,101104},{54,48848,107024},{53,49840,100080},{54,44176,105040},{53,46416,101968},{54,48432,108336},{53,45712,100464},{54,48560,106096},{53,52752,104496},{54,49392,105200},{53,46512,104528},{54,45648,103824},{53,52400,101456},{54,49008,101520},{53,46320,106608},{54,49584,104176},{53,52656,101648},{54,49712,101936},{53,51120,104208},{54,49264,103952},{53,51568,101552},{54,52688,105392},{53,49808,104048},{54,49072,101424},{53,49520,102288},{54,45584,100752},{53,47184,103216},{54,49104,104144},{53,52016,101680},{54,50032,105136}}

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
