-- Tasks: 4704 card issuance, 4705 career+1, 4706 Giap Si weapon choice;
-- 4710..4716 delivered units per level. Card ownership is required every time.
PTLG_ID=9700
PTLG_LEVELS={30,40,50,60,70,80,90}
PTLG_DONE="Bạn Đã Nhận Hết Phần Quà, Có thể Bán Shop vật phẩm này."
function PTLGClose() CloseDialog() end
function PTLGHasCard() return GetItemCount(8,PTLG_ID,0,-1,3)>0 end
function PTLGReady()
 if not PTLGHasCard() then Msg2Player("Hãy đặt Thẻ Quà: Thăng Cấp Nhận Thưởng trong hành trang F4.");return nil end
 if GetTrade()~=0 or GetLockState()~=0 then Msg2Player("Hãy kết thúc giao dịch và mở khóa nhân vật trước khi nhận quà.");return nil end
 local career=GetSeries()
 if career<0 or career>2 then Msg2Player("Hệ phái không hợp lệ.");return nil end
 local saved=GetTask(4705)
 if saved~=0 and saved~=career+1 then Msg2Player("Thẻ quà đã được chốt theo hệ phái lúc nhận lần đầu.");return nil end
 return 1
end
function PTLGCard()
 if NearLeQuan()~=1 or GetLevel()<10 or GetTrade()~=0 or GetLockState()~=0 then return nil end
 if GetTask(4704)==1 then return 1 end
 if QuestExchange(4704,0,1,{},{{8,PTLG_ID,2,0,0,0,1}},1)~=1 then
  Msg2Player("Chưa nhận được thẻ quà. Hãy để trống hành trang F4 rồi gặp lại Lễ Quan; không phát lại trang bị đã nhận.");return nil
 end
 Msg2Player("Đã nhận Thẻ Quà: Thăng Cấp Nhận Thưởng. Nhấn chuột phải để xem và nhận từng mốc cấp 30 đến 90.")
 return 1
end
function PTLGRewards(stage,career,weapon)
 local p=3+career
 local low=2+career*5
 local high=4+career*5
 -- row: tuple[1..6], quantity, binding, display name. Table SetID produces green quality.
 local rewards={}
 if stage==1 then rewards={{0,9,p,4,low,0,1,1,"Phi Phong Lục 4x"},{0,7,p,4,low,0,1,1,"Mũ Lục 4x"}}
 elseif stage==2 then rewards={{0,6,p,4,low,0,1,1,"Yêu Đái Lục 4x"},{0,5,p,4,low,0,1,1,"Giày Lục 4x"}}
 elseif stage==3 then rewards={{0,2,p,4,low,0,1,1,"Áo Lục 4x"}}
 elseif stage==4 then rewards={{0,10,24+career,6,5,0,1,2,"Thú cưỡi 6x theo phái"}}
 elseif stage==5 or stage==6 then rewards={{0,7,p,8,high,0,1,1,"Nón Lục 8x"},{0,9,p,8,high,0,1,1,"Phi Phong Lục 8x"}}
 elseif stage==7 then
  local part=28+career
  if career==0 then part=26+weapon end
  rewards={{0,10,24+career,8,5,0,1,2,"Thú cưỡi Chí 9x theo phái"},{0,0,part,8,5,0,1,0,"Vũ khí nhiệm vụ cấp 80"}}
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
 return "<color=yellow>Thẻ Quà: Thăng Cấp Nhận Thưởng<color><enter>Nhấn chuột phải để nhận quà riêng từng mốc cấp 30, 40, 50, 60, 70, 80, 90. Mỗi nhân vật nhận mỗi mốc một lần. Thẻ được giữ lại khi mở và sau khi nhận quà. Không bán thẻ trước khi nhận đủ các mốc."
end
function main(index)
 if not PTLGReady() then return end
 if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE);return end
 local options={}
 for stage=1,7 do
  local rows=PTLGRewards(stage,GetSeries(),GetTask(4706)>0 and GetTask(4706) or 1)
  local status="Chưa đủ cấp"
  if GetTask(4709+stage)>=PTLGTotal(rows) then status="Đã nhận"
  elseif GetLevel()>=PTLG_LEVELS[stage] then status="Có thể nhận" end
  tinsert(options,"Cấp "..PTLG_LEVELS[stage].." - "..status.."/PTLGPreview"..stage)
 end
 tinsert(options,"Đóng/PTLGClose")
 local args={"<color=yellow>Thẻ Quà: Thăng Cấp Nhận Thưởng<color><enter>Chọn mốc để xem quà. Thẻ không mất khi bấm mở hoặc nhận một mốc.",getn(options)}
 for i=1,getn(options) do tinsert(args,options[i]) end
 call(SayButtons,args)
end
function PTLGPreview(stage)
 if not PTLGReady() then return end
 local rows=PTLGRewards(stage,GetSeries(),GetTask(4706)>0 and GetTask(4706) or 1)
 local text="<color=yellow>Quà cấp "..PTLG_LEVELS[stage].."<color>"
 for i=1,getn(rows) do local r=rows[i];text=text.."<enter>"..r[7].." "..r[9];if r[8]>0 then text=text.." (Khóa)" end end
 local progress=GetTask(4709+stage)
 if progress>=PTLGTotal(rows) then Talk(1,"PTLGClose",text.."<enter>Bạn đã nhận đủ quà mốc này.");return end
 if GetLevel()<PTLG_LEVELS[stage] then Talk(1,"PTLGClose",text.."<enter>Cần đạt cấp "..PTLG_LEVELS[stage].." để nhận.");return end
 if progress>0 then text=text.."<enter>Đã nhận "..progress.."/"..PTLGTotal(rows).." vật phẩm; chỉ phát tiếp phần còn thiếu." end
 SayButtons(text,2,"Nhận quà/PTLGClaim"..stage,"Để sau/PTLGClose")
end
function PTLGClaim(stage)
 if not PTLGReady() or GetLevel()<PTLG_LEVELS[stage] then return end
 local career=GetSeries()
 if stage==7 and career==0 and GetTask(4706)==0 then SetTask(4706,random(1,2)) end
 local rows=PTLGRewards(stage,career,GetTask(4706)>0 and GetTask(4706) or 1)
 local progress=GetTask(4709+stage);local total=PTLGTotal(rows)
 if progress<0 or progress>total then Msg2Player("Tiến độ quà không hợp lệ. Hãy liên hệ GM.");return end
 if progress==total then
  if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE) else Msg2Player("Bạn đã nhận đủ quà mốc này.") end
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
    if not item or item<=0 then Msg2Player("Hành trang chưa đủ chỗ hoặc chưa thể nhận vật phẩm. Hãy để trống F4 rồi mở thẻ nhận tiếp phần còn thiếu.");return end
    if r[8]==2 then LockItem(item,1,0,0,0) end
    progress=unit;SetTask(4709+stage,progress)
   end
  end
 end
 if PTLGAllDone() then Talk(1,"PTLGClose",PTLG_DONE)
 else Msg2Player("Đã nhận đủ quà cấp "..PTLG_LEVELS[stage]..". Giữ thẻ để nhận các mốc còn lại.") end
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
