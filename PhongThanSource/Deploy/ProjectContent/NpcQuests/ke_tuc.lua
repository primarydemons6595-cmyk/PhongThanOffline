-- Lua 4. Reconstructed Kế Tục rules; provenance is in the deployment report.
-- Original task slots: 973=local day, 984=acceptances, 993=target/count, 994=kills.
PTK_TARGETS={ [0]={1,4}, [1]={2,5}, [2]={3,7} }
PTK_NAMES={ [1]="Kiếm Nhân",[4]="Xạ Nhân",[2]="Tuyết Quái",[5]="Băng Lang",[3]="Hỏa Diện",[7]="Cuồng Điêu" }
PTK_MATERIAL={ [1]=10,[4]=11,[2]=13,[5]=9,[3]=12,[7]=8 }

function PTK_Day()
    return floor(LocalSystemTime()/86400)
end
function PTK_ResetDay()
    if GetTask(993)==0 and GetTask(973)~=PTK_Day() then
        SetTask(973,PTK_Day())
        SetTask(984,0)
    end
end
function PTK_Count(level)
    if level<=10 then return 10 end
    if level<=20 then return 20 end
    if level<=40 then return 40 end
    if level<=60 then return 80 end
    if level<=80 then return 150 end
    if level<=100 then return 250 end
    return 400
end
function PTK_ValidTarget(target)
    local choices=PTK_TARGETS[GetPlayerType()]
    return choices and (target==choices[1] or target==choices[2])
end
function PTK_Note(step)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if not PTK_ValidTarget(target) or need<=0 then return end
    local text="Kế Tục: tiêu diệt "..PTK_NAMES[target].." ("..GetTask(994).."/"..need..")."
    if GetTask(994)>=need then text=text.." Trở về Thủ Khố nhận thưởng.";step=2 end
    TaskNote(51,step,text)
end
function PTK_Show()
    if GetPlayerType()==PTK_CLASS then return 1 end
    return 0
end
function ptk_close() CloseDialog() end
function ptk_open()
    if PTK_Show()~=1 then CloseDialog() return end
    PTK_ResetDay()
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if not PTK_ValidTarget(target) or need<=0 then
            Msg2Player("Dữ liệu Kế Tục chưa phù hợp. Tiến độ được giữ nguyên; hãy liên hệ GM.")
            CloseDialog() return
        end
        PTK_Note(1)
        if GetTask(994)>=need then
            MsgBox("Đã hoàn thành Kế Tục. Nhận 100 điểm kinh nghiệm?","ptk_finish","ptk_close")
        else
            Talk(1,"ptk_close","Kế Tục: đã tiêu diệt "..GetTask(994).."/"..need.." "..PTK_NAMES[target]..". Hãy tiếp tục rồi trở lại gặp ta.")
        end
        return
    end
    if GetLevel()<1 or GetTask(984)>=5 then
        Talk(1,"ptk_close","Mỗi ngày nhận tối đa 5 lần Kế Tục. Hãy quay lại vào ngày mai.") return
    end
    local choices=PTK_TARGETS[GetPlayerType()]
        Say("Kế Tục: chọn mục tiêu. Cần diệt "..PTK_Count(GetLevel()).." quái; hoàn thành nhận 100 kinh nghiệm. Hôm nay đã nhận "..GetTask(984).."/5 lần.",3,
            PTK_NAMES[choices[1]].."/ptk_take1",PTK_NAMES[choices[2]].."/ptk_take2","Đóng/ptk_close")
end
function ptk_take1() PTK_Take(1) end
function ptk_take2() PTK_Take(2) end
function PTK_Take(choice)
    if PTK_Show()~=1 or GetLevel()<1 then return end
    PTK_ResetDay()
    if GetTask(993)~=0 or GetTask(984)>=5 then ptk_open() return end
    local target=PTK_TARGETS[GetPlayerType()][choice]
    if not target then return end
    local state=SetByte(SetByte(0,1,target),2,PTK_Count(GetLevel())/10)
    local rewards={}
    if GetLevel()<=5 then rewards={{1,0,1,1,0,0,1}} end
    if QuestExchange(993,0,state,{},rewards)~=1 then
        Msg2Player("Chưa nhận được nhiệm vụ. Hãy chừa chỗ trống trong hành trang và kết thúc giao dịch.") return
    end
    SetTask(994,0)
    SetTask(984,GetTask(984)+1)
    PTK_Note(0)
    Msg2Player("Đã nhận Kế Tục. Nhấn F11 để xem tiến độ.")
    CloseDialog()
end
function ptk_finish()
    if PTK_Show()~=1 then return end
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    if state==0 or need<=0 or not PTK_ValidTarget(target) or GetTask(994)<need then return end
    if QuestExchange(993,state,0,{},{})~=1 then return end
    SetTask(994,0)
    AddOwnExp(100)
    TaskNote(51,-1)
    PTK_ResetDay()
    Msg2Player("Hoàn thành Kế Tục: nhận 100 điểm kinh nghiệm.")
    CloseDialog()
end
function PTK_State()
    if PTK_Show()~=1 then return 0,0 end
    local state=GetTask(993)
    if state~=0 then
        local target=GetByte(state,1)
        local need=GetByte(state,2)*10
        if PTK_ValidTarget(target) and need>0 then
            if GetTask(994)>=need then return 3,0 end
            return 2,0
        end
        return 0,0
    end
    if GetLevel()>=1 and (GetTask(973)~=PTK_Day() or GetTask(984)<5) then return 1,0 end
    return 0,0
end
-- Called only after the common death dispatcher validates player and map context.
function PTK_Death(template,teamSize)
    local state=GetTask(993)
    local target=GetByte(state,1)
    local need=GetByte(state,2)*10
    local count=GetTask(994)
    if need<=0 or template+1~=target or not PTK_ValidTarget(target) or count>=need then return end
    SetTask(994,count+1)
    PTK_Note(1)
    Msg2Player("Kế Tục: "..PTK_NAMES[target].." "..(count+1).."/"..need..".")
    -- Archived server variants use 50% solo, 40+10*party size when teamed.
    -- This rate is explicitly reconstructed, not sourced from a client Lua.
    local chance=50
    if teamSize>0 then chance=40+teamSize*10 end
    if chance>100 then chance=100 end
    if random(1,100)<=chance then AddNormalItem(3,PTK_MATERIAL[target],0,0,0,0) end
    if count+1==need then Msg2Player("Đã đủ số quái. Trở về gặp Thủ Khố để trả nhiệm vụ Kế Tục.") end
end
