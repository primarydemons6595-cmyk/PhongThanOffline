-- Four-visit introductory chain. Save 4960 was previously ended at phase 2
-- without rewards; version 4965 upgrades that unfinished implementation safely.
PTMAO_HOME={[0]=1002,[1]=1003,[2]=1004}
PTMAO_ROUTE={
 [0]={"giao","store","lo","giao"},
 [1]={"guide","giao","store","guide"},
 [2]={"guide","giao","store","guide"}}
PTMAO_TARGET={
 [0]={"Tạp Hóa Thương (207,198)","Thủ Khố (202,198)","Lỗ Hùng (194,199)","Tạp Hóa Thương (207,198)"},
 [1]={"Từ Hàng Đạo Nhân (206,196)","Tạp Hóa Thương (216,201)","Thủ Khố (217,200)","Từ Hàng Đạo Nhân (206,196)"},
 [2]={"Thiếu Hạo (198,204)","Tạp Hóa Thương (202,201)","Thủ Khố (197,201)","Thiếu Hạo (198,204)"}}
PTMAO_INTRO={
 [0]="Là tân thủ của Sùng Thành, hãy đến gặp Tạp Hóa Thương báo sắp có mưa lớn, nhắc ông ấy che đậy hàng hóa.",
 [1]="Là tân thủ của Ngọc Hư Cung, hãy đến tìm Từ Hàng Đạo Nhân để nghe chỉ dẫn.",
 [2]="Là tân thủ của Xi Vưu Mộ, hãy đến tìm Thiếu Hạo để nghe chỉ dẫn."}
PTMAO_EXP={100,500,1100,1900}
PTMAO_SPEECH={
 [0]={"Cảm ơn ngươi đã báo tin, ta sẽ thu dọn hàng hóa ngay. Hãy đến gặp Thủ Khố để tìm hiểu cách cất giữ vật phẩm.","Thủ Khố giúp ngươi cất giữ vật phẩm. Hãy đến gặp Lỗ Hùng để nghe chỉ dẫn cho tân thủ.","Muốn trưởng thành phải chăm chỉ rèn luyện. Hãy trở về báo tin cho Tạp Hóa Thương.","Ngươi đã làm quen với mọi người trong Sùng Thành. Mao Lư đã hoàn thành! Có thể nhận Tân Thủ Thí Luyện tại đây."},
 [1]={"Chào mừng ngươi đến Ngọc Hư Cung. Hãy tìm Tạp Hóa Thương để làm quen với các vật phẩm cần thiết.","Ta bán những vật phẩm cần cho hành trình. Tiếp theo hãy tìm Thủ Khố để học cách cất giữ đồ.","Hãy giữ những vật phẩm quý trong kho. Bây giờ quay về gặp Từ Hàng Đạo Nhân phục mệnh.","Ngươi đã biết các đầu mối trong Ngọc Hư Cung. Mao Lư hoàn thành! Hãy đến Thợ Đồng nhận Tân Thủ Thí Luyện."},
 [2]={"Chào mừng ngươi đến Xi Vưu Mộ. Hãy tìm Tạp Hóa Thương để làm quen với các vật phẩm cần thiết.","Ta bán những vật phẩm cần cho hành trình. Tiếp theo hãy tìm Thủ Khố để học cách cất giữ đồ.","Hãy giữ những vật phẩm quý trong kho. Bây giờ quay về gặp Thiếu Hạo phục mệnh.","Ngươi đã biết các đầu mối trong Xi Vưu Mộ. Mao Lư hoàn thành! Hãy đến Thợ Đồng nhận Tân Thủ Thí Luyện."}}
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
  AddNote("3120",1,"<color=yellow>Mao Lư<color><enter>Chặng "..phase.."/4: Đến gặp <color=green>"..PTMAO_TARGET[f][phase].."<color>.<enter>Đối thoại và chọn nhận thưởng để tiếp tục hành trình.",phase)
 elseif phase==5 then AddNote("3120",2," ",5) end
end
function pt_mao_take()
 local f=GetPlayerType();if GetLevel()~=1 or not PTMAO_HOME[f] or GetContentWorldPos()+1000~=PTMAO_HOME[f] or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(4960,0,1,{},{},1)==1 then SetTask(4965,1);PTMaoNote();Msg2Player("Mao Lư: đến gặp "..PTMAO_TARGET[f][1]..".") end
end
function pt_mao_later() CloseDialog() end
function PTMaoOffer()
 PTMaoMigrate();local f=GetPlayerType()
 if GetLevel()==1 and GetTask(4960)==0 and PTMAO_HOME[f] and GetContentWorldPos()+1000==PTMAO_HOME[f] then
  MsgBox("<color=green>Hoan nghênh bạn đến với thế giới Phong Thần!<color><enter><enter><color=yellow>Mao Lư:<color> "..PTMAO_INTRO[f].."<enter>Hoàn thành bốn chặng đối thoại để nhận kinh nghiệm và tìm hiểu tân thủ thôn.","pt_mao_take","pt_mao_later")
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
 if phase<4 then next="<enter>Mục tiêu tiếp theo: <color=green>"..PTMAO_TARGET[f][phase+1].."<color>." end
 SayButtons(PTMAO_SPEECH[f][phase].."<enter>Đã nhận <color=yellow>"..reward.." kinh nghiệm<color>."..next,1,"Kết thúc/pt_mao_later")
end
function PTMaoMenu()
 PTMaoMigrate();if not PTMaoReady() then return 0 end
 PTMaoNote();local f=GetPlayerType();local phase=GetTask(4960)
 SayButtons("<color=yellow>Mao Lư - chặng "..phase.."/4<color><enter>"..PTMAO_SPEECH[f][phase].."<enter>Phần thưởng: <color=yellow>"..PTMaoReward().." kinh nghiệm<color>.",2,"Nhận thưởng và tiếp tục/pt_mao_claim","Để sau/pt_mao_later")
 return 1
end
function PTMaoMeet() return PTMaoMenu() end
function PTMaoMarker()
 if PTMaoReady() then return 3,1 end
 if PTMAO_OLD_MARK then return PTMAO_OLD_MARK() end
 return 0,0
end
