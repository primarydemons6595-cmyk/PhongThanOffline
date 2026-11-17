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
PT_MOBS={{55,53552,102960},{56,55184,113904},{55,48208,111536},{56,56560,108240},{55,56720,100368},{56,51696,108176},{55,51728,114608},{56,54288,111536},{55,56272,104304},{56,48112,107248},{55,48656,114576},{56,51248,111600},{55,53712,100592},{56,54128,106416},{55,49456,106864},{56,54224,114064},{55,56688,110768},{56,54320,109136},{55,54480,102800},{56,49776,109840},{55,52752,106448},{56,56016,112368},{55,49712,112880},{56,56784,106256},{55,56400,102320},{56,52784,112784},{55,52624,110032},{56,49968,107440},{55,54448,104624},{56,53552,102928},{55,48432,108880},{56,55280,101136},{55,53360,107824},{56,50192,114352},{55,55152,107600},{56,52912,106288},{55,49776,106864},{56,49712,111344},{55,55248,110320},{56,55408,105712},{55,51184,110160},{56,52688,111408},{55,51440,112944},{56,48176,110192},{55,55216,113840},{56,50480,108688},{55,48400,113008},{56,54704,112784},{55,54384,110640},{56,50064,112080},{55,56496,105104},{56,53232,109072},{55,52240,112048},{56,56688,106000},{55,54256,104464},{56,50224,113648},{55,56688,101488},{56,49744,106864},{55,55280,104944},{56,51152,110416},{55,54736,108848},{56,52400,108240},{55,50960,112272},{56,49968,110032},{55,51248,113968},{56,51280,114416},{55,56400,106192},{56,53168,114000},{55,50992,114288},{56,48720,107408},{55,53008,110544},{56,50736,111024},{55,53552,102896},{56,51024,110800},{55,54096,109872},{56,49168,114288},{55,51440,111920},{56,56304,105584},{55,50192,108624},{56,49776,110416},{55,52720,106480},{56,56624,112464},{55,50960,111408},{56,49456,114288},{55,56336,104496},{56,56592,112432},{55,56432,105904},{56,48656,109200},{55,54480,101264},{56,56336,109328},{55,55920,112368},{56,48560,107600},{55,56528,103888},{56,54480,110960},{55,55088,109808},{56,54640,104688},{55,52560,112496},{56,56496,101968},{55,53360,106864},{56,52304,110608},{55,50992,110224},{56,53584,102992},{55,49808,106896},{56,54448,107888},{55,49072,108848},{56,55504,106640},{55,54320,108432},{56,51920,110896},{55,54000,111472},{56,53712,111536},{55,52336,111344},{56,48368,107952},{55,54224,109616},{56,53968,104016},{55,52944,106256},{56,54064,104016},{55,53840,106544},{56,49904,106960},{55,53808,103568},{56,51632,110768},{55,50704,109200},{56,54128,101040},{55,53424,107280},{56,55152,102672},{55,49776,106896},{56,53520,108496},{55,55152,101456},{56,52784,114032},{55,49648,110864},{56,49808,112464},{55,52880,106320},{56,55472,110160},{55,52784,106416},{56,48464,109520},{55,54832,109616},{56,48464,111664},{55,53584,102960},{56,48464,112784},{55,54000,102992},{56,54640,108272},{55,55472,105040},{56,55280,107056},{55,55216,113776},{56,56560,107280},{55,55408,100944},{56,53136,113360},{55,55600,105776},{56,52976,106224},{55,52048,113744},{56,55120,113776}}

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
