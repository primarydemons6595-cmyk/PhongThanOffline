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
PT_POINTS={{49648,97072},{57168,102672},{52976,106896},{49360,102352},{52976,102416}}
PT_MOBS={{59,53808,97296},{60,50384,108720},{59,57680,104688},{60,48656,102000},{59,54032,100880},{60,49040,96720},{59,57712,99984},{60,54736,107984},{59,51184,105072},{60,57328,96336},{59,51152,99568},{60,54704,104912},{59,54960,99792},{60,57392,107408},{59,48720,106192},{60,51504,96368},{59,52368,107248},{60,50992,102640},{59,57264,102320},{60,48848,99056},{59,49264,104080},{60,56592,98256},{59,55344,102992},{60,52720,104368},{59,53040,100016},{60,55792,106448},{59,50480,97872},{60,56304,100912},{59,51760,101136},{60,50160,106960},{59,52272,97776},{60,54192,106448},{59,49232,100560},{60,54992,101328},{59,56624,103760},{60,56112,107920},{59,50320,101456},{60,51664,103216},{59,54256,103728},{60,51632,108336},{59,50544,103856},{60,53424,105232},{59,49904,105200},{60,53456,107920},{59,49520,102864},{60,49264,97904},{59,56144,99568},{60,56400,97072},{59,53520,104528},{60,55472,104912},{59,48688,104112},{60,49328,106096},{59,51632,103248},{60,57136,106928},{59,48976,103664},{60,51216,96848},{59,53808,100080},{60,53808,107728},{59,52592,104304},{60,49072,103984},{59,52688,104816},{60,56336,107824},{59,53424,107216},{60,52080,107248},{59,52752,104400},{60,49104,105552},{59,55312,103504},{60,51632,103792},{59,53648,100272},{60,50832,99504},{59,51184,107280},{60,51632,103184},{59,49648,98960},{60,54736,107056},{59,51824,99440},{60,52208,100368},{59,54192,100240},{60,56656,100496},{59,55696,99920},{60,56976,98256},{59,50896,108400},{60,57232,101392},{59,52112,101168},{60,50736,99824},{59,57392,106576},{60,55984,106000},{59,55664,104656},{60,54224,103760},{59,51152,103504},{60,50608,100304},{59,57072,98480},{60,54000,100848},{59,56496,96176},{60,48720,104688},{59,49520,106864},{60,54864,103824},{59,52144,107760},{60,53264,99728},{59,56272,98480},{60,49072,105008},{59,51248,108368},{60,57136,101584},{59,53968,100816},{60,51888,101392},{59,50288,99824},{60,53136,107344},{59,54896,103184},{60,52048,101232},{59,52752,105136},{60,57456,104304},{59,50960,97264},{60,56816,98576},{59,56496,97264},{60,57584,104080},{59,57264,104208},{60,55440,101328},{59,55696,105456},{60,51920,108016},{59,56368,101264},{60,56464,96880},{59,52464,97040},{60,54448,103536},{59,51728,97328},{60,54320,103664},{59,52496,107536},{60,51728,97552},{59,56912,104368},{60,49104,105296},{59,54960,101904},{60,51888,99920},{59,55056,104656},{60,53232,105072},{59,51376,96720},{60,56656,101360},{59,49424,96464},{60,50512,97200},{59,54384,100336},{60,50448,98192},{59,55088,102928},{60,51312,108144},{59,50224,99504},{60,52272,101008},{59,55792,102512},{60,54448,99952},{59,51056,97520},{60,57136,107216},{59,52304,104816},{60,56016,99024},{59,53424,104816},{60,56784,108048},{59,50832,96976},{60,57168,102736},{59,54640,100272},{60,49104,98736},{59,49616,97264},{60,49872,97584},{59,54096,100496},{60,57616,104176},{59,49072,104336},{60,50544,101488},{59,56368,101360},{60,48784,105104},{59,50704,99536},{60,55056,107504},{59,53424,104688},{60,51472,99760},{59,49584,98128},{60,55184,103440},{59,48944,102768},{60,52400,106768},{59,53776,100368},{60,51248,100112},{59,51312,107536},{60,52848,104752},{59,57040,106832},{60,54832,107728},{59,49616,98192},{60,48688,104816},{59,56528,97232},{60,56912,100624},{59,50576,100144},{60,55472,100752},{59,50736,104080},{60,51280,103184},{59,50672,97936},{60,55056,106896},{59,52208,107984},{60,56112,106544},{59,48720,103248},{60,52432,104848},{59,52176,96912},{60,49104,104880},{59,54384,106864},{60,55568,104368},{59,49392,106544},{60,49872,98928},{59,50352,99696},{60,52688,107760},{59,57264,101040},{60,53008,107472},{59,52816,107152},{60,55344,101552},{59,52176,106864},{60,54160,104336},{59,51312,100400},{60,50096,105072},{59,50192,99856},{60,52624,107216},{59,55120,104240},{60,56944,101232},{59,52400,97680},{60,53968,107632},{59,52304,106896},{60,51472,108048},{59,52048,108080},{60,56848,104464},{59,51856,97136},{60,55312,103952},{59,57552,100240},{60,55344,102256},{59,57328,98256},{60,51088,97744},{59,51056,100176},{60,56720,103920},{59,54800,104112},{60,51472,96560},{59,53424,100240},{60,49744,98320},{59,56400,106096},{60,54736,103824},{59,50768,101424},{60,53456,100208},{59,49296,104912},{60,51920,104624},{59,49328,96560},{60,57488,101744},{59,56048,107536},{60,56976,97968},{59,55408,101072},{60,52112,107472},{59,52176,107856},{60,52528,97008},{59,50736,100656},{60,57360,104496},{59,51600,97744},{60,50672,105648},{59,48656,104016},{60,56944,104784},{59,51568,100688},{60,56208,98672}}

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
