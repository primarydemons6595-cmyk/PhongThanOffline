-- Four-visit introductory chain. Save 4960 was previously ended at phase 2
-- without rewards; version 4965 upgrades that unfinished implementation safely.
PTMAO_HOME={[0]=1002,[1]=1003,[2]=1004}
PTMAO_ROUTE={
 [0]={"giao","store","lo","giao"},
 [1]={"guide","giao","store","guide"},
 [2]={"guide","giao","store","guide"}}
PTMAO_TARGET={
 [0]={"Tπp H„a Th≠¨ng (207,198)","ThÒ KhË (202,198)","LÁ HÔng (194,199)","Tπp H„a Th≠¨ng (207,198)"},
 [1]={"Tı Hµng ßπo Nh©n (206,196)","Tπp H„a Th≠¨ng (216,201)","ThÒ KhË (217,200)","Tı Hµng ßπo Nh©n (206,196)"},
 [2]={"Thi’u Hπo (198,204)","Tπp H„a Th≠¨ng (202,201)","ThÒ KhË (197,201)","Thi’u Hπo (198,204)"}}
PTMAO_INTRO={
 [0]="Lµ t©n thÒ cÒa SÔng Thµnh, h∑y Æ’n g∆p Tπp H„a Th≠¨ng b∏o sæp c„ m≠a lÌn, nhæc ´ng  y che ÆÀy hµng h„a.",
 [1]="Lµ t©n thÒ cÒa Ng‰c H≠ Cung, h∑y Æ’n t◊m Tı Hµng ßπo Nh©n Æ” nghe chÿ d…n.",
 [2]="Lµ t©n thÒ cÒa Xi V≠u MÈ, h∑y Æ’n t◊m Thi’u Hπo Æ” nghe chÿ d…n."}
PTMAO_EXP={100,500,1100,1900}
PTMAO_SPEECH={
 [0]={"C∂m ¨n ng≠¨i Æ∑ b∏o tin, ta sœ thu d‰n hµng h„a ngay. H∑y Æ’n g∆p ThÒ KhË Æ” t◊m hi”u c∏ch c t gi˜ vÀt ph»m.","ThÒ KhË giÛp ng≠¨i c t gi˜ vÀt ph»m. H∑y Æ’n g∆p LÁ HÔng Æ” nghe chÿ d…n cho t©n thÒ.","MuËn tr≠Îng thµnh ph∂i ch®m chÿ rÃn luy÷n. H∑y trÎ v“ b∏o tin cho Tπp H„a Th≠¨ng.","Ng≠¨i Æ∑ lµm quen vÌi m‰i ng≠Íi trong SÔng Thµnh. Mao L≠ Æ∑ hoµn thµnh! C„ th” nhÀn T©n ThÒ Th› Luy÷n tπi Æ©y."},
 [1]={"Chµo mıng ng≠¨i Æ’n Ng‰c H≠ Cung. H∑y t◊m Tπp H„a Th≠¨ng Æ” lµm quen vÌi c∏c vÀt ph»m c«n thi’t.","Ta b∏n nh˜ng vÀt ph»m c«n cho hµnh tr◊nh. Ti’p theo h∑y t◊m ThÒ KhË Æ” h‰c c∏ch c t gi˜ ÆÂ.","H∑y gi˜ nh˜ng vÀt ph»m qu˝ trong kho. B©y giÍ quay v“ g∆p Tı Hµng ßπo Nh©n phÙc m÷nh.","Ng≠¨i Æ∑ bi’t c∏c Æ«u mËi trong Ng‰c H≠ Cung. Mao L≠ hoµn thµnh! H∑y Æ’n ThÓ ßÂng nhÀn T©n ThÒ Th› Luy÷n."},
 [2]={"Chµo mıng ng≠¨i Æ’n Xi V≠u MÈ. H∑y t◊m Tπp H„a Th≠¨ng Æ” lµm quen vÌi c∏c vÀt ph»m c«n thi’t.","Ta b∏n nh˜ng vÀt ph»m c«n cho hµnh tr◊nh. Ti’p theo h∑y t◊m ThÒ KhË Æ” h‰c c∏ch c t gi˜ ÆÂ.","H∑y gi˜ nh˜ng vÀt ph»m qu˝ trong kho. B©y giÍ quay v“ g∆p Thi’u Hπo phÙc m÷nh.","Ng≠¨i Æ∑ bi’t c∏c Æ«u mËi trong Xi V≠u MÈ. Mao L≠ hoµn thµnh! H∑y Æ’n ThÓ ßÂng nhÀn T©n ThÒ Th› Luy÷n."}}
function PTMaoMigrate()
 local f=GetPlayerType();if f~=1 or not PTMAO_HOME[f] or GetTask(4965)~=0 then return end
 local phase=GetTask(4960)
 if phase==2 then
  -- The old phase 2 paid no XP and omitted the remaining route. Restart it at
  -- the first NPC, rather than mistaking it for the new second visit.
  if QuestExchange(4960,2,1,{},{},1)~=1 then return end
 elseif phase~=0 and phase~=1 then return end
 SetTask(4965,1)
end
function PTMaoNote()
 PTMaoMigrate();local f=GetPlayerType();if f~=1 or not PTMAO_HOME[f] then return end
 local phase=GetTask(4960)
 if phase>=1 and phase<=4 then
  AddNote("3120",1,"<color=yellow>Mao L≠<color><enter>Ch∆ng "..phase.."/4: ß’n g∆p <color=green>"..PTMAO_TARGET[f][phase].."<color>.<enter>ßËi thoπi vµ ch‰n nhÀn th≠Îng Æ” ti’p tÙc hµnh tr◊nh.",phase)
 elseif phase==5 then AddNote("3120",2," ",5) end
end
function PTMaoCanAccept() return GetLevel()==1 or (IsServerBot and IsServerBot()==1 and GetLevel()>1) end
function pt_mao_take()
 local f=GetPlayerType();if not PTMaoCanAccept() or not PTMAO_HOME[f] or GetContentWorldPos()+1000~=PTMAO_HOME[f] or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(4960,0,1,{},{},1)==1 then SetTask(4965,1);PTMaoNote();Msg2Player("Mao L≠: Æ’n g∆p "..PTMAO_TARGET[f][1]..".") end
end
function pt_mao_later() CloseDialog() end
function PTMaoOffer()
 PTMaoMigrate();local f=GetPlayerType()
 if PTMaoCanAccept() and GetTask(4960)==0 and PTMAO_HOME[f] and GetContentWorldPos()+1000==PTMAO_HOME[f] then
  MsgBox("<color=green>Hoan ngh™nh bπn Æ’n vÌi th’ giÌi Phong Th«n!<color><enter><enter><color=yellow>Mao L≠:<color> "..PTMAO_INTRO[f].."<enter>Hoµn thµnh bËn ch∆ng ÆËi thoπi Æ” nhÀn kinh nghi÷m vµ t◊m hi”u t©n thÒ th´n.","pt_mao_take","pt_mao_later")
 else PTMaoNote() end
end
function PTMaoReady()
 local f=GetPlayerType();local phase=GetTask(4960)
 return f==1 and PTMAO_HOME[f] and PTMAO_NPC_CLASS==f and GetContentWorldPos()+1000==PTMAO_HOME[f] and phase>=1 and phase<=4 and PTMAO_NPC_ROLE==PTMAO_ROUTE[f][phase]
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
 if phase<4 then next="<enter>MÙc ti™u ti’p theo: <color=green>"..PTMAO_TARGET[f][phase+1].."<color>." end
 SayButtons(PTMAO_SPEECH[f][phase].."<enter>ß∑ nhÀn <color=yellow>"..reward.." kinh nghi÷m<color>."..next,1,"K’t thÛc/pt_mao_later")
end
function PTMaoMenu()
 PTMaoMigrate();if not PTMaoReady() then return 0 end
 PTMaoNote();local f=GetPlayerType();local phase=GetTask(4960)
 SayButtons("<color=yellow>Mao L≠ - ch∆ng "..phase.."/4<color><enter>"..PTMAO_SPEECH[f][phase].."<enter>Ph«n th≠Îng: <color=yellow>"..PTMaoReward().." kinh nghi÷m<color>.",2,"NhÀn th≠Îng vµ ti’p tÙc/pt_mao_claim","ß” sau/pt_mao_later")
 return 1
end
function PTMaoMeet() return PTMaoMenu() end
function PTMaoMarker()
 if PTMaoReady() then return 3,1 end
 if PTMAO_OLD_MARK then return PTMAO_OLD_MARK() end
 return 0,0
end
