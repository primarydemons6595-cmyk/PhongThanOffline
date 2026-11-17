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

function pt_each(fn)
 local old=PlayerIndex
 local cursor=0
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
 ShowMissionCountdown(PT_ID,0);EventVttRefresh()
 LockCamp(0);SetFightState(0);SetLogoutRV(0)
 NewWorld(PT_EXIT_MAP,PT_EXIT_X,PT_EXIT_Y)
end
function pt_spawn(tid,x,y,death)
 local npc=AddNpc(tid,PT_LEVEL,SubWorld,x,y,1)
 if npc==nil or npc<=0 then return 0 end
 local owned=AddMSNpc(PT_ID,npc,0)
 if owned==nil or owned<=0 then DelNpc(npc);return 0 end
 if death~=nil and SetNpcDeathScript(npc,death)~=1 then DelNpc(npc);return 0 end
 return npc
end
PT_MOB_IDS={}
function pt_spawn_mobs()
 for n=1,getn(PT_MOBS) do
  if PT_MOB_IDS[n]==nil then
   local p=PT_MOBS[n]
   local npc=pt_spawn(p[1],p[2],p[3],"\\script\\phongthan\\van_tien\\mob"..p[1]..".lua")
   if npc>0 then PT_MOB_IDS[n]=npc end
  end
 end
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
 if GetMissionV(11)+GetMissionV(12)+GetMissionV(13)+GetMissionV(14)==4 and GetMissionV(34)==1 and GetMissionV(30)==0 then
  local p=PT_POINTS[5]
  local npc=pt_spawn(PT_BOSS,p[1],p[2],"\\script\\phongthan\\van_tien\\boss"..PT_ID..".lua")
  if npc>0 then SetMissionV(30,npc);Msg2MSAll(PT_ID,"§· h¹ ®ñ bèn Hé Ph¸p. Th«ng Thiªn Gi¸o Chñ xuÊt hiÖn!") end
 end
end
function pt_tick() pt_spawn_mobs();pt_progress() end
function InitMission()
 local generation=floor(SystemTime())
 if PT_LAST_GEN~=nil and generation<=PT_LAST_GEN then generation=PT_LAST_GEN+1 end
 PT_LAST_GEN=generation
 for n=0,94 do SetMissionV(n,0) end
 PT_MOB_IDS={}
 SetMissionV(1,generation);SetGlobalValue(PT_ID,1)
 pt_spawn_mobs()
 local seconds=300
 if EventServiceOwns(PT_ID)==1 then seconds=EventServiceValue(PT_ID,1) end
 StartMissionTimer(PT_ID,3005,18*seconds)
end
function RunMission()
 if GetGlobalValue(PT_ID)~=1 then return end
 SetGlobalValue(PT_ID,2)
 pt_spawn_mobs();pt_progress();pt_each(pt_fight)
 StartMissionTimer(PT_ID,3009,18*30)
 Msg2MSAll(PT_ID,"TrËn chiÕn b¾t ®Çu! NhËn nhiÖm vô t¹i §¹i Phu, tiªu diÖt bèn Hé Ph¸p råi h¹ Th«ng Thiªn Gi¸o Chñ.")
end
function EndMission()
 SetGlobalValue(PT_ID,0)
 StopMissionTimer(PT_ID,3005);StopMissionTimer(PT_ID,3006);StopMissionTimer(PT_ID,3009)
 pt_each(pt_exit)
end
function OnLeave(role)
 PlayerIndex=role;EventVttLeave(PT_ID);for detail=66,69 do for n=1,60 do if HaveNormalItem(3,detail,0,0)<1 then break end DelNormalItem(3,detail,0,0) end end;ShowMissionCountdown(PT_ID,0)
 LockCamp(0);SetFightState(0);SetLogoutRV(0)
end
