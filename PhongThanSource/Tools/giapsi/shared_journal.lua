function PTSJournal()
    if not PTSClass() then return end
    for id=20,26 do AddNote(tostring(id),2," ",0) end
    for i=1,getn(PTS_JOURNALS) do
        local q=PTS_JOURNALS[i];local phase=GetTask(q.task);local row=q.phases[phase]
        if row then
            local text="<color=yellow>"..q.title.."<color><enter><color=cyan>"..row.note.."<color>"
            if getn(row.needs)>0 then
                text=text.."<enter><color=green>"
                for n=1,getn(row.needs) do local r=row.needs[n];local count=HaveNormalItem(r[1],r[2],r[3],r[4]);if count>r[7] then count=r[7] end
                    text=text..row.names[n].." "..count.."/"..r[7].."; "
                end
                text=text.."<color>"
            end
            AddNote(q.header,1,text,phase)
        else AddNote(q.header,2," ",phase) end
    end
    local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
    if count>0 and count<36 or state>0 and count<36 then
        local tower=PTS_TOWERS[floor(count/6)+1]
        local text="<color=yellow>Thiên Cương Tinh Quy Vị<color><enter><color=green>Đã thu phục: "..count.."/36<color><enter>"
        if state==1 then text=text.."<color=cyan>Mở Phong Ấn Tháp tại "..tower[5].." ("..tower[2]..","..tower[3]..").<color>"
        elseif state==2 then text=text.."Hạ Thiên Cương Tinh vừa thả. Nếu thất lạc, gặp Dương Tiễn (165,185), Tây Kỳ, gọi lại tinh."
        else text=text.."Gặp Dương Tiễn, Tây Kỳ (165,185), để trả chân khí hoặc nhận Bách Linh Phướn cho lượt kế tiếp." end
        AddNote("3025",1,text,value)
    else AddNote("3025",2," ",value) end
    local target=GetTask(55)
    if target>=22 and target<=25 then
        local count=HaveNormalItem(3,target,0,0);if count>10 then count=10 end
        AddNote("3026",1,"<color=yellow>Tứ Tượng Đồng Quy<color><enter><color=cyan>Thu thập "..PTS_MATERIALS[target].." tại Mục Dã (185,190)-(188,190), rồi gặp Thủ Khố ở Tây Kỳ (195,188) hoặc Triều Ca (221,189).<color><enter><color=green>Tiến độ: "..count.."/10<color>",count)
    else AddNote("3026",2," ",0) end
end
function PTSDeath(template)
    if not PTSClass() then return end
    local map=PTSMap()
    if template==108 and map==1022 and GetTask(51)==3 then
        if HaveEventItem(37)<1 then AddEventItem(37) end
        if HaveEventItem(37)>0 then SetTask(51,4) end
    elseif template==104 and map==1010 and GetTask(50)==2 then
        if HaveEventItem(38)<1 then AddEventItem(38) end
        if HaveEventItem(38)>0 then SetTask(50,3) end
    elseif template==114 and map==1043 and GetTask(50)==5 then
        if HaveEventItem(40)<1 then AddEventItem(40) end
    elseif map==1018 and template>=7 and template<=10 and GetTask(55)==template+15 then
        local target=GetTask(55)
        if HaveNormalItem(3,target,0,0)<10 then AddNormalItem(3,target,0,0,0,0) end
    end
    if map==1037 and template>=110 and template<=112 and GetTask(41)>=20 and GetTask(41)<=27 then
        local item=PTS_FISH[template]
        if HaveEventItem(item)<1 and AddEventItem(item)>0 then
            local bit=PTS_FISH_BITS[template];local mask=GetTask(41)-20
            if mod(floor(mask/bit),2)==0 then SetTask(41,20+mask+bit) end
        end
    end
    PTSJournal()
end
