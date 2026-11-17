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
PT_POINTS={{54064,101264},{48752,107984},{50256,110736},{54128,111152},{51088,104144}}
PT_MOBS={{55,53552,102960},{56,55184,113904},{55,48208,111536},{56,56560,108240},{55,56720,100368},{56,51696,108176},{55,51728,114608},{56,54288,111536},{55,56272,104304},{56,48112,107248},{55,48656,114576},{56,51248,111600},{55,53712,100592},{56,54128,106416},{55,49456,106864},{56,54224,114064},{55,56688,110768},{56,54320,109136},{55,54480,102800},{56,49776,109840},{55,52752,106448},{56,56016,112368},{55,49712,112880},{56,56784,106256},{55,56400,102320},{56,52784,112784},{55,52624,110032},{56,49968,107440},{55,54448,104624},{56,53552,102928},{55,48432,108880},{56,55280,101136},{55,53360,107824},{56,50192,114352},{55,55152,107600},{56,52912,106288},{55,49776,106864},{56,49712,111344},{55,55248,110320},{56,55408,105712},{55,51184,110160},{56,52688,111408},{55,51440,112944},{56,48176,110192},{55,55216,113840},{56,50480,108688},{55,48400,113008},{56,54704,112784},{55,54384,110640},{56,50064,112080},{55,56496,105104},{56,53232,109072},{55,52240,112048},{56,56688,106000},{55,54256,104464},{56,50224,113648},{55,56688,101488},{56,49744,106864},{55,55280,104944},{56,51152,110416},{55,54736,108848},{56,52400,108240},{55,50960,112272},{56,49968,110032},{55,51248,113968},{56,51280,114416},{55,56400,106192},{56,53168,114000},{55,50992,114288},{56,48720,107408},{55,53008,110544},{56,50736,111024},{55,53552,102896},{56,51024,110800},{55,54096,109872},{56,49168,114288},{55,51440,111920},{56,56304,105584},{55,50192,108624},{56,49776,110416},{55,52720,106480},{56,56624,112464},{55,50960,111408},{56,49456,114288},{55,56336,104496},{56,56592,112432},{55,56432,105904},{56,48656,109200},{55,54480,101264},{56,56336,109328},{55,55920,112368},{56,48560,107600},{55,56528,103888},{56,54480,110960},{55,55088,109808},{56,54640,104688},{55,52560,112496},{56,56496,101968},{55,53360,106864},{56,52304,110608},{55,50992,110224},{56,53584,102992},{55,49808,106896},{56,54448,107888},{55,49072,108848},{56,55504,106640},{55,54320,108432},{56,51920,110896},{55,54000,111472},{56,53712,111536},{55,52336,111344},{56,48368,107952},{55,54224,109616},{56,53968,104016},{55,52944,106256},{56,54064,104016},{55,53840,106544},{56,49904,106960},{55,53808,103568},{56,51632,110768},{55,50704,109200},{56,54128,101040},{55,53424,107280},{56,55152,102672},{55,49776,106896},{56,53520,108496},{55,55152,101456},{56,52784,114032},{55,49648,110864},{56,49808,112464},{55,52880,106320},{56,55472,110160},{55,52784,106416},{56,48464,109520},{55,54832,109616},{56,48464,111664},{55,53584,102960},{56,48464,112784},{55,54000,102992},{56,54640,108272},{55,55472,105040},{56,55280,107056},{55,55216,113776},{56,56560,107280},{55,55408,100944},{56,53136,113360},{55,55600,105776},{56,52976,106224},{55,52048,113744},{56,55120,113776},{55,55536,103760},{56,48592,108176},{55,50864,112624},{56,54160,110576},{55,50896,112208},{56,56560,101552},{55,50416,114512},{56,54576,101040},{55,53648,107184},{56,55856,100976},{55,48560,114096},{56,48400,108560},{55,52880,113040},{56,53840,111184},{55,55024,101488},{56,56400,109072},{55,55664,104208},{56,50896,113488},{55,52944,113424},{56,54448,102480},{55,54128,110704},{56,50768,108048},{55,51984,111824},{56,52752,111504},{55,54800,111536},{56,51088,112304},{55,56176,103408},{56,53456,113104},{55,49744,114320},{56,53296,113264},{55,53808,113392},{56,50640,114384},{55,51280,109264},{56,52752,112528},{55,48624,108816},{56,48432,109264},{55,48784,113136},{56,54352,100816},{55,51888,108720},{56,56304,109648},{55,52112,108624},{56,51408,114544},{55,55408,110544},{56,54992,113808},{55,53424,106704},{56,52752,112816},{55,50832,110480},{56,54448,110000},{55,48816,110384},{56,52208,109136},{55,53968,103472},{56,54288,111696},{55,55344,110704},{56,52944,109776},{55,54800,102096},{56,53520,113360},{55,55888,100752},{56,55280,111984},{55,48816,108528},{56,53232,109264},{55,56496,106736},{56,52752,109264},{55,49360,112336},{56,52528,111216},{55,48432,107920},{56,53232,107248},{55,54896,102160},{56,53296,114032},{55,55088,113936},{56,48848,108080},{55,48464,112368},{56,48912,109296},{55,55408,104176},{56,54608,110096},{55,50864,113936},{56,54288,109872},{55,51120,112528},{56,55792,101008},{55,54928,102256},{56,48816,114192},{55,53200,110480},{56,54448,110448},{55,52432,107472},{56,55472,106000},{55,51024,112176},{56,56464,100592},{55,49456,111376},{56,55056,102704},{55,54736,111664},{56,56624,106736},{55,54608,111216},{56,54480,111056},{55,52016,110928},{56,53040,112112},{55,56720,110128},{56,53808,102128},{55,53136,108624},{56,55152,107184},{55,54672,101040},{56,55696,100720}}

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
