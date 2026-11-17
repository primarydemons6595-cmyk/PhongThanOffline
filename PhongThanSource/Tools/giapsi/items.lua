function ptg_item_main()
    if GetPlayerType()~=0 then return PTG_ITEM_OLD and PTG_ITEM_OLD() end
    if PTG_ITEM==34 then
        if PTGMap()==1070 and (GetTask(3)==86 or GetTask(3)==87) then
            SayButtons("<color=yellow>Khứ Lai Phù<color><enter>Gọi ba hồn của Thông Thiên Giáo Chủ.",2,"Gọi ba hồn/ptg_souls","Đóng/ptg_close")
        else
            SayButtons("<color=yellow>Khứ Lai Phù<color><enter>Trở về Ngọc Hư Cung mười năm trước để tiếp tục nhiệm vụ.",2,"Ngọc Hư Cung/ptg_time_travel","Đóng/ptg_close")
        end
    elseif PTG_ITEM==142 then
        if GetTask(3)~=84 or PTGMap()~=1006 or HaveEventItem(178)>0 then return end
        local map,x,y=GetContentWorldPos()
        local dx=x-190*8-4
        local dy=y-206*16-8
        if dx*dx+dy*dy<=64 then
            SayButtons("<color=yellow>Mật Đồ Da Dê<color><enter><color=cyan>Tàn Nhẫn đang ở ngay đây.<color>",2,"Tìm Tàn Nhẫn/ptg_find_cruel","Đóng/ptg_close")
        else
            PTGNotice("<color=yellow>Mật Đồ Da Dê<color><enter><color=cyan>Tìm Tàn Nhẫn tại Bắc Hải (190,206).<color>")
        end
    elseif PTG_ITEM==181 then
        PTGNotice("<color=yellow>Thông Thiên<color><enter><color=cyan>Tham Lam: Thương Nhân Đại Phong (200,200). Vô Tri: Cửu Di Tam Sơn (200,200). Tàn Nhẫn: dùng Mật Đồ tại Bắc Hải (190,206).<color>")
    elseif PTG_ITEM==9201 then
        if GetTask(4550)~=5 or PTGMap()~=1014 then return end
        local seq=GetTask(4490)
        if seq>=0 and seq<1000000000 and QuestExchange(4490,seq,seq+1,{{6,1,9201,1,0,0,1}},{{6,1,277,1,0,0,1}},0)==1 then
            PTGNotice("Đã đổi sang Tĩnh Hóa Phù gốc. Nhấn chuột phải vào phù vừa nhận để sử dụng.")
        end
    end
end
function ptg_time_travel()
    if GetPlayerType()~=0 or HaveNormalItem(6,1,34,1)<1 then return end
    local map=GetContentWorldPos()
    if map==64 or map==71 or map==66 or map==72 or GetMorphType()==364 or IsPlayerInsideWeapon(PlayerIndex)>0 then return end
    NewRuntimeWorld(1061,198*8+4,201*16+8)
end
function ptg_find_cruel()
    if GetPlayerType()~=0 or GetTask(3)~=84 or PTGMap()~=1006 or HaveNormalItem(6,1,142,1)<1 then return end
    local map,x,y=GetContentWorldPos()
    if (x-190*8-4)^2+(y-206*16-8)^2>64 then return end
    PTGEvent(178,1)
    PTGJournal()
end
function ptg_souls()
    if GetPlayerType()~=0 or (GetTask(3)~=86 and GetTask(3)~=87) or PTGMap()~=1070 or HaveNormalItem(6,1,34,1)<1 then return end
    -- A partial spawn failure cleans up every created entity and keeps phase86.
    local spawned={}
    for i=1,3 do
      local prior=GetTask(4563+i)
      if mod(floor(GetTask(4562)/(2^(i-1))),2)==0 and
         (prior<=0 or GetNpcTemplateID(prior)~=540+i or GetNpcWorldPos(prior)~=1070 or GetNpcLife(prior)<=0) then
        local n=AddNpc(540+i,92,SubWorld,(201+i)*256+128,209*512+256,1)
        if not n or n<=0 or SetNpcDeathScript(n,"\\script\\phongthan\\giapsi\\death_"..(540+i)..".lua")~=1 then
            if n and n>0 then DelNpc(n) end
            for j=1,getn(spawned) do DelNpc(spawned[j]) end
            return
        end
        tinsert(spawned,n)
        SetTask(4563+i,n)
      end
    end
    if GetTask(3)==86 then SetTask(4562,0) end
    SetTask(3,87)
    PTGJournal()
    CloseDialog()
end
