PTLG_EXTRAS={{{8,227,2,0,0,0,1,0,"ßÂng Nh©n"},{8,162,3,0,0,0,5,0,"Thanh LÈ"},{8,163,4,0,0,0,5,0,"Ch©n Kh›"},{8,164,0,0,0,0,1,0,"NhÀt Nguy÷t ß¨n"}},{{8,227,2,0,0,0,5,0,"ßÂng Nh©n"},{3,41,0,0,0,0,10,0,"Lam B∂o Thπch"},{8,164,0,0,0,0,1,0,"NhÀt Nguy÷t ß¨n"},{8,141,2,0,0,0,1,0,"HÂng Kh´ng Th≠"},{8,159,2,0,0,0,1,0,"Di Ngoπi PhÔ (Si™u C p)"}},{{8,233,0,0,0,0,1,0,"La H∏n Hi÷u Gi∏c"},{8,193,5,0,0,0,2,0,"M∂nh S∏ch Ch≠ H«u"},{8,164,0,0,0,0,1,0,"NhÀt Nguy÷t ß¨n"},{8,141,2,0,0,0,1,0,"HÂng Kh´ng Th≠"}},{{8,193,5,0,0,0,4,0,"M∂nh S∏ch Ch≠ H«u"},{8,165,2,0,0,0,1,0,"Th∏i C˘c ß¨n"},{8,142,2,0,0,0,1,0,"Hoµng Kh´ng Th≠"},{3,41,0,0,0,0,10,0,"Lam B∂o Thπch"}},{{8,193,5,0,0,0,4,0,"M∂nh S∏ch Ch≠ H«u"},{8,269,2,0,0,0,1,0,"M∂nh V∂i"},{8,142,2,0,0,0,1,0,"Hoµng Kh´ng Th≠"},{3,79,0,0,0,0,10,0,"HÂng B∂o Thπch"}},{{8,193,5,0,0,0,4,0,"M∂nh S∏ch Ch≠ H«u"},{8,165,2,0,0,0,1,0,"Th∏i C˘c ß¨n"},{3,66,0,0,0,0,1,0,"Thanh Long L÷nh"},{8,269,2,0,0,0,2,0,"M∂nh V∂i"}},{}}
-- Tasks: 4704 card issuance, 4705 career+1, 4706 Giap Si weapon choice;
-- 4710..4716 delivered units per level. Card ownership is required every time.
PTLG_ID=9700
PTLG_LEVELS={30,40,50,60,70,80,90}
PTLG_DONE="Bπn ß∑ NhÀn H’t Ph«n Quµ, C„ th” B∏n Shop vÀt ph»m nµy."
function PTLGClose() CloseDialog() end
function PTLGHasCard() return GetItemCount(8,PTLG_ID,0,-1,3)>0 end
function PTLGReady()
 if not PTLGHasCard() then Msg2Player("H∑y Æ∆t ThŒ Quµ: Th®ng C p NhÀn Th≠Îng trong hµnh trang F4.");return nil end
 if GetTrade()~=0 or GetLockState()~=0 then Msg2Player("H∑y k’t thÛc giao dﬁch vµ mÎ kh„a nh©n vÀt tr≠Ìc khi nhÀn quµ.");return nil end
 local career=GetSeries()
 if career<0 or career>2 then Msg2Player("H÷ ph∏i kh´ng hÓp l÷.");return nil end
 local saved=GetTask(4705)
 if saved~=0 and saved~=career+1 then Msg2Player("ThŒ quµ Æ∑ Æ≠Óc chËt theo h÷ ph∏i lÛc nhÀn l«n Æ«u.");return nil end
 return 1
end
function PTLGCard()
 if NearLeQuan()~=1 or GetLevel()<10 or GetTrade()~=0 or GetLockState()~=0 then return nil end
 if GetTask(4704)==1 then return 1 end
 if QuestExchange(4704,0,1,{},{{8,PTLG_ID,2,0,0,0,1}},1)~=1 then
  Msg2Player("Ch≠a nhÀn Æ≠Óc thŒ quµ. H∑y Æ” trËng hµnh trang F4 rÂi g∆p lπi L‘ Quan; kh´ng ph∏t lπi trang bﬁ Æ∑ nhÀn.");return nil
 end
 Msg2Player("ß∑ nhÀn ThŒ Quµ: Th®ng C p NhÀn Th≠Îng. Nh n chuÈt ph∂i Æ” xem vµ nhÀn tıng mËc c p 30 Æ’n 90.")
 return 1
end
function PTLGRewards(stage,career,weapon)
 local p=3+career
 local low=2+career*5
 local high=4+career*5
 -- row: tuple[1..6], quantity, binding, display name. Table SetID produces green quality.
 local rewards={}
 if stage==1 then rewards={{0,9,p,4,low,0,1,1,"Phi Phong LÙc 4x"},{0,7,p,4,low,0,1,1,"MÚ LÙc 4x"}}
 elseif stage==2 then rewards={{0,6,p,4,low,0,1,1,"Y™u ß∏i LÙc 4x"},{0,5,p,4,low,0,1,1,"Giµy LÙc 4x"}}
 elseif stage==3 then rewards={{0,2,p,4,low,0,1,1,"∏o LÙc 4x"}}
 elseif stage==4 then rewards={{0,10,24+career,6,5,0,1,2,"ThÛ c≠Ïi 6x theo ph∏i"}}
 elseif stage==5 or stage==6 then rewards={{0,7,p,8,high,0,1,1,"N„n LÙc 8x"},{0,9,p,8,high,0,1,1,"Phi Phong LÙc 8x"}}
 elseif stage==7 then
  local part=28+career
  if career==0 then part=26+weapon end
  rewards={{0,10,24+career,8,5,0,1,2,"ThÛ c≠Ïi Ch› 9x theo ph∏i"},{0,0,part,8,5,0,1,0,"VÚ kh› nhi÷m vÙ c p 80"}}
 end
 local extras=PTLG_EXTRAS[stage]
 for i=1,getn(extras) do tinsert(rewards,extras[i]) end
 return rewards
end
function PTLGTotal(rows)
 local total=0;for i=1,getn(rows) do total=total+rows[i][7] end;return total
end
function PTLGAllDone()
 for stage=1,7 do
  local rows=PTLGRewards(stage,GetSeries(),GetTask(4706)>0 and GetTask(4706) or 1)
  if GetTask(4709+stage)<PTLGTotal(rows) then return nil end
 end
 return 1
end
function GetDesc(index)
 return "<color=yellow>ThŒ Quµ: Th®ng C p NhÀn Th≠Îng<color><enter>Nh n chuÈt ph∂i Æ” nhÀn quµ ri™ng tıng mËc c p 30, 40, 50, 60, 70, 80, 90. MÁi nh©n vÀt nhÀn mÁi mËc mÈt l«n. ThŒ Æ≠Óc gi˜ lπi khi mÎ vµ sau khi nhÀn quµ. Kh´ng b∏n thŒ tr≠Ìc khi nhÀn ÆÒ c∏c mËc."
end
function main(index)
 if not PTLGReady() then return end
 if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE);return end
 local options={}
 for stage=1,7 do
  local rows=PTLGRewards(stage,GetSeries(),GetTask(4706)>0 and GetTask(4706) or 1)
  local status="Ch≠a ÆÒ c p"
  if GetTask(4709+stage)>=PTLGTotal(rows) then status="ß∑ nhÀn"
  elseif GetLevel()>=PTLG_LEVELS[stage] then status="C„ th” nhÀn" end
  tinsert(options,"C p "..PTLG_LEVELS[stage].." - "..status.."/PTLGPreview"..stage)
 end
 tinsert(options,"ß„ng/PTLGClose")
 local args={"<color=yellow>ThŒ Quµ: Th®ng C p NhÀn Th≠Îng<color><enter>Ch‰n mËc Æ” xem quµ. ThŒ kh´ng m t khi b m mÎ ho∆c nhÀn mÈt mËc.",getn(options)}
 for i=1,getn(options) do tinsert(args,options[i]) end
 call(SayButtons,args)
end
function PTLGPreview(stage)
 if not PTLGReady() then return end
 local rows=PTLGRewards(stage,GetSeries(),GetTask(4706)>0 and GetTask(4706) or 1)
 local text="<color=yellow>Quµ c p "..PTLG_LEVELS[stage].."<color>"
 for i=1,getn(rows) do local r=rows[i];text=text.."<enter>"..r[7].." "..r[9];if r[8]>0 then text=text.." (Kh„a)" end end
 local progress=GetTask(4709+stage)
 if progress>=PTLGTotal(rows) then Talk(1,"PTLGClose",text.."<enter>Bπn Æ∑ nhÀn ÆÒ quµ mËc nµy.");return end
 if GetLevel()<PTLG_LEVELS[stage] then Talk(1,"PTLGClose",text.."<enter>C«n Æπt c p "..PTLG_LEVELS[stage].." Æ” nhÀn.");return end
 if progress>0 then text=text.."<enter>ß∑ nhÀn "..progress.."/"..PTLGTotal(rows).." vÀt ph»m; chÿ ph∏t ti’p ph«n cﬂn thi’u." end
 SayButtons(text,2,"NhÀn quµ/PTLGClaim"..stage,"ß” sau/PTLGClose")
end
function PTLGClaim(stage)
 if not PTLGReady() or GetLevel()<PTLG_LEVELS[stage] then return end
 local career=GetSeries()
 if stage==7 and career==0 and GetTask(4706)==0 then SetTask(4706,random(1,2)) end
 local rows=PTLGRewards(stage,career,GetTask(4706)>0 and GetTask(4706) or 1)
 local progress=GetTask(4709+stage);local total=PTLGTotal(rows)
 if progress<0 or progress>total then Msg2Player("Ti’n ÆÈ quµ kh´ng hÓp l÷. H∑y li™n h÷ GM.");return end
 if progress==total then
  if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE) else Msg2Player("Bπn Æ∑ nhÀn ÆÒ quµ mËc nµy.") end
  return
 end
 if GetTask(4705)==0 then SetTask(4705,career+1) end
 local unit=0
 for i=1,getn(rows) do local r=rows[i]
  for n=1,r[7] do
   unit=unit+1
   if unit>progress then
    local bind=0;if r[8]==1 then bind=1 end
    local item=AddNormalItem(r[1],r[2],r[3],r[4],r[5],r[6],bind)
    if not item or item<=0 then Msg2Player("Hµnh trang ch≠a ÆÒ chÁ ho∆c ch≠a th” nhÀn vÀt ph»m. H∑y Æ” trËng F4 rÂi mÎ thŒ nhÀn ti’p ph«n cﬂn thi’u.");return end
    if r[8]==2 then LockItem(item,1,0,0,0) end
    progress=unit;SetTask(4709+stage,progress)
   end
  end
 end
 if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE)
 else Msg2Player("ß∑ nhÀn ÆÒ quµ c p "..PTLG_LEVELS[stage]..". Gi˜ thŒ Æ” nhÀn c∏c mËc cﬂn lπi.") end
end
function PTLGPreview1() PTLGPreview(1) end
function PTLGPreview2() PTLGPreview(2) end
function PTLGPreview3() PTLGPreview(3) end
function PTLGPreview4() PTLGPreview(4) end
function PTLGPreview5() PTLGPreview(5) end
function PTLGPreview6() PTLGPreview(6) end
function PTLGPreview7() PTLGPreview(7) end
function PTLGClaim1() PTLGClaim(1) end
function PTLGClaim2() PTLGClaim(2) end
function PTLGClaim3() PTLGClaim(3) end
function PTLGClaim4() PTLGClaim(4) end
function PTLGClaim5() PTLGClaim(5) end
function PTLGClaim6() PTLGClaim(6) end
function PTLGClaim7() PTLGClaim(7) end
