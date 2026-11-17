-- Literal records use dedicated headers; legacy journal templates stay intact.
function PTGJournalState()
    local value=""
    for i=1,getn(PTG_JOURNAL) do value=value..":"..GetTask(PTG_JOURNAL[i].task) end
    for i=1,getn(PTG_PROGRESS) do value=value..":"..HaveEventItem(PTG_PROGRESS[i]) end
    value=value..":"..GetTask(4554)..":"..GetTask(4555)..":"..GetTask(4562)
    return value
end
function PTGJournal()
    if GetPlayerType()~=0 then return end
    -- Keep the accepted target/count when moving old distant objectives home.
    if GetTask(4554)>0 and GetTask(4559)~=1007 then SetTask(4559,1007) end
    if GetTask(4554)==13 then SetTask(4554,3) end
    if GetTask(4554)==16 then SetTask(4554,7) end
    -- Retire the former technical "Task 27" placeholder.
    AddNote("27",2," ",0)
    for i=1,getn(PTG_JOURNAL) do
        local q=PTG_JOURNAL[i]
        local phase=GetTask(q.task)
        local row=q.phases[phase]
        if row then
            local text="<color=yellow>"..row.title.."<color><enter><color=cyan>"..row.note.."<color>"
            for n=1,getn(row.needs) do
                local need=row.needs[n]
                local count=HaveNormalItem(need[1],need[2],need[3],need[4])
                if count>need[7] then count=need[7] end
                text=text.."<enter><color=green>"..row.needNames[n]..": "..count.."/"..need[7].."<color>"
            end
            if q.task==3 and phase==87 then
                local mask=GetTask(4562)
                text=text.."<enter><color=green>Thiên Hồn "..mod(mask,2).."/1; Địa Hồn "..mod(floor(mask/2),2).."/1; Mệnh Hồn "..mod(floor(mask/4),2).."/1<color>"
            end
            AddNote(q.header,1,text,phase)
        else AddNote(q.header,2," ",phase) end
    end
    if GetTask(4554)>0 then
        local text="<color=yellow>Giáo Huấn<color><enter><color=cyan>Diệt "..PTG_TARGET_NAMES[GetTask(4554)].." tại "..PTG_TARGET_MAPS[GetTask(4554)]..".<color><enter><color=green>Tiến độ: "..GetTask(4555).."/40<color><enter>Trả tại Tạp Hóa Thương Nhân, Sùng Thành Doanh (207,198)."
        AddNote("3005",1,text,GetTask(4555))
    else AddNote("3005",2," ",0) end
end
