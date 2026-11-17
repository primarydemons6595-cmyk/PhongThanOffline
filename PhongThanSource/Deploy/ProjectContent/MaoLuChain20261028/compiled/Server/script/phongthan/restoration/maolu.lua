-- Four-visit introductory chain. Save 4960 was previously ended at phase 2
-- without rewards; version 4965 upgrades that unfinished implementation safely.
PTMAO_HOME={[0]=1002,[1]=1003,[2]=1004}
PTMAO_ROUTE={
 [0]={"giao","store","lo","giao"},
 [1]={"guide","giao","store","guide"},
 [2]={"guide","giao","store","guide"}}
PTMAO_TARGET={
 [0]={"T¹p Hãa Th­¬ng (207,198)","Thñ Khè (202,198)","Lç Hïng (194,199)","T¹p Hãa Th­¬ng (207,198)"},
 [1]={"Tõ Hµng §¹o Nh©n (206,196)","T¹p Hãa Th­¬ng (216,201)","Thñ Khè (217,200)","Tõ Hµng §¹o Nh©n (206,196)"},
 [2]={"ThiÕu H¹o (198,204)","T¹p Hãa Th­¬ng (202,201)","Thñ Khè (197,201)","ThiÕu H¹o (198,204)"}}
PTMAO_INTRO={
 [0]="Lµ t©n thñ cña Sïng Thµnh, h·y ®Õn gÆp T¹p Hãa Th­¬ng b¸o s¾p cã m­a lín, nh¾c «ng Êy che ®Ëy hµng hãa.",
 [1]="Lµ t©n thñ cña Ngäc H­ Cung, h·y ®Õn t×m Tõ Hµng §¹o Nh©n ®Ó nghe chØ dÉn.",
 [2]="Lµ t©n thñ cña Xi V­u Mé, h·y ®Õn t×m ThiÕu H¹o ®Ó nghe chØ dÉn."}
PTMAO_EXP={100,500,1100,1900}
PTMAO_SPEECH={
 [0]={"C¶m ¬n ng­¬i ®· b¸o tin, ta sÏ thu dän hµng hãa ngay. H·y ®Õn gÆp Thñ Khè ®Ó t×m hiÓu c¸ch cÊt gi÷ vËt phÈm.","Thñ Khè gióp ng­¬i cÊt gi÷ vËt phÈm. H·y ®Õn gÆp Lç Hïng ®Ó nghe chØ dÉn cho t©n thñ.","Muèn tr­ëng thµnh ph¶i ch¨m chØ rÌn luyÖn. H·y trë vÒ b¸o tin cho T¹p Hãa Th­¬ng.","Ng­¬i ®· lµm quen víi mäi ng­êi trong Sïng Thµnh. Mao L­ ®· hoµn thµnh! Cã thÓ nhËn T©n Thñ ThÝ LuyÖn t¹i ®©y."},
 [1]={"Chµo mõng ng­¬i ®Õn Ngäc H­ Cung. H·y t×m T¹p Hãa Th­¬ng ®Ó lµm quen víi c¸c vËt phÈm cÇn thiÕt.","Ta b¸n nh÷ng vËt phÈm cÇn cho hµnh tr×nh. TiÕp theo h·y t×m Thñ Khè ®Ó häc c¸ch cÊt gi÷ ®å.","H·y gi÷ nh÷ng vËt phÈm quý trong kho. B©y giê quay vÒ gÆp Tõ Hµng §¹o Nh©n phôc mÖnh.","Ng­¬i ®· biÕt c¸c ®Çu mèi trong Ngäc H­ Cung. Mao L­ hoµn thµnh! H·y ®Õn Thî §ång nhËn T©n Thñ ThÝ LuyÖn."},
 [2]={"Chµo mõng ng­¬i ®Õn Xi V­u Mé. H·y t×m T¹p Hãa Th­¬ng ®Ó lµm quen víi c¸c vËt phÈm cÇn thiÕt.","Ta b¸n nh÷ng vËt phÈm cÇn cho hµnh tr×nh. TiÕp theo h·y t×m Thñ Khè ®Ó häc c¸ch cÊt gi÷ ®å.","H·y gi÷ nh÷ng vËt phÈm quý trong kho. B©y giê quay vÒ gÆp ThiÕu H¹o phôc mÖnh.","Ng­¬i ®· biÕt c¸c ®Çu mèi trong Xi V­u Mé. Mao L­ hoµn thµnh! H·y ®Õn Thî §ång nhËn T©n Thñ ThÝ LuyÖn."}}
function PTMaoMigrate()
 local f=GetPlayerType();if not PTMAO_HOME[f] or GetTask(4965)~=0 then return end
 local phase=GetTask(4960)
 if phase==2 then
  -- The old phase 2 paid no XP and omitted the remaining route. Restart it at
  -- the first NPC, rather than mistaking it for the new second visit.
  if QuestExchange(4960,2,1,{},{},1)~=1 then return end
 elseif phase~=0 and phase~=1 then return end
 SetTask(4965,1)
end
function PTMaoNote()
 PTMaoMigrate();local f=GetPlayerType();if not PTMAO_HOME[f] then return end
 local phase=GetTask(4960)
 if phase>=1 and phase<=4 then
  AddNote("3120",1,"<color=yellow>Mao L­<color><enter>ChÆng "..phase.."/4: §Õn gÆp <color=green>"..PTMAO_TARGET[f][phase].."<color>.<enter>§èi tho¹i vµ chän nhËn th­ëng ®Ó tiÕp tôc hµnh tr×nh.",phase)
 elseif phase==5 then AddNote("3120",2," ",5) end
end
function pt_mao_take()
 local f=GetPlayerType();if GetLevel()~=1 or not PTMAO_HOME[f] or GetContentWorldPos()+1000~=PTMAO_HOME[f] or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(4960,0,1,{},{},1)==1 then SetTask(4965,1);PTMaoNote();Msg2Player("Mao L­: ®Õn gÆp "..PTMAO_TARGET[f][1]..".") end
end
function pt_mao_later() CloseDialog() end
function PTMaoOffer()
 PTMaoMigrate();local f=GetPlayerType()
 if GetLevel()==1 and GetTask(4960)==0 and PTMAO_HOME[f] and GetContentWorldPos()+1000==PTMAO_HOME[f] then
  MsgBox("<color=green>Hoan nghªnh b¹n ®Õn víi thÕ giíi Phong ThÇn!<color><enter><enter><color=yellow>Mao L­:<color> "..PTMAO_INTRO[f].."<enter>Hoµn thµnh bèn chÆng ®èi tho¹i ®Ó nhËn kinh nghiÖm vµ t×m hiÓu t©n thñ th«n.","pt_mao_take","pt_mao_later")
 else PTMaoNote() end
end
function PTMaoReady()
 local f=GetPlayerType();local phase=GetTask(4960)
 return PTMAO_HOME[f] and PTMAO_NPC_CLASS==f and GetContentWorldPos()+1000==PTMAO_HOME[f] and phase>=1 and phase<=4 and PTMAO_NPC_ROLE==PTMAO_ROUTE[f][phase]
end
function PTMaoReward()
 local phase=GetTask(4960);if phase<1 or phase>4 then return 0 end
 local reward=PTMAO_EXP[phase]
 -- Give one level at every visit for the new character's level 1..4 journey.
 -- Keep a bounded introductory award for older characters resuming the chain.
 if GetLevel()>=1 and GetLevel()<=4 then
  local left=GetNextExp()-GetExp();if left>reward then reward=left end
 end
 return reward
end
function pt_mao_claim()
 PTMaoMigrate();if not PTMaoReady() or GetTrade()~=0 or GetLockState()~=0 then return end
 local phase=GetTask(4960);local reward=PTMaoReward()
 if reward<=0 or reward>=2147483647 then return end
 if QuestExchange(4960,phase,phase+1,{},{},1)~=1 then return end
 AddOwnExp(reward);PTMaoNote()
 local f=GetPlayerType();local next=""
 if phase<4 then next="<enter>Môc tiªu tiÕp theo: <color=green>"..PTMAO_TARGET[f][phase+1].."<color>." end
 SayButtons(PTMAO_SPEECH[f][phase].."<enter>§· nhËn <color=yellow>"..reward.." kinh nghiÖm<color>."..next,1,"KÕt thóc/pt_mao_later")
end
function PTMaoMenu()
 PTMaoMigrate();if not PTMaoReady() then return 0 end
 PTMaoNote();local f=GetPlayerType();local phase=GetTask(4960)
 SayButtons("<color=yellow>Mao L­ - chÆng "..phase.."/4<color><enter>"..PTMAO_SPEECH[f][phase].."<enter>PhÇn th­ëng: <color=yellow>"..PTMaoReward().." kinh nghiÖm<color>.",2,"NhËn th­ëng vµ tiÕp tôc/pt_mao_claim","§Ó sau/pt_mao_later")
 return 1
end
function PTMaoMeet() return PTMaoMenu() end
function PTMaoMarker()
 if PTMaoReady() then return 3,1 end
 if PTMAO_OLD_MARK then return PTMAO_OLD_MARK() end
 return 0,0
end
