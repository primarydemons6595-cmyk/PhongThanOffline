-- Common side quests keep the original saved task phases for all three classes.
function PTSMap() return GetContentWorldPos()+1000 end
function PTSClass() return GetPlayerType()>=0 and GetPlayerType()<=2 end
function PTSClose() CloseDialog() end
function PTSNotice(text) SayButtons(text,0,{}) end
function PTSHave(needs)
    for n=1,getn(needs) do local r=needs[n];if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end end
    return 1
end
function PTSReady(a)
    return PTSClass() and PTSMap()==PTS_MAP and a.role==PTS_ROLE and GetLevel()>=a.level and GetTask(a.task)==a.from and (not a.morph or GetMorphType()==a.morph) and PTSHave(a.needs)==1
end
function PTSAddChoices(options)
    for i=1,getn(PTS_ACTIONS) do local a=PTS_ACTIONS[i];if PTSReady(a) then tinsert(options,(a.label or a.title).."/pts_preview"..i) end end
    if PTS_ROLE=="west" then
        if GetLevel()>=19 then tinsert(options,"Trùng Mộc/pts_mount") end
        if GetLevel()>=39 and (GetTask(51)==3 or GetTask(51)==5) and HaveEventItem(37)>0 then tinsert(options,"Hợp thú cưỡi/pts_fuse") end
    end
    if PTS_ROLE=="duong" and GetLevel()>=59 then
        local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
        if count<36 and (state==0 or state==3 and HaveEventItem(44)>0) then tinsert(options,"Quy Tinh/pts_stars") end
        if state==2 and count<36 then tinsert(options,"Gọi lại tinh/pts_star_retry") end
    end
    if PTS_ROLE=="store_west" or PTS_ROLE=="store_east" then
        local target=GetTask(55)
        if GetLevel()>=31 and (target==0 or target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10) then tinsert(options,"Tứ Tượng/pts_elements") end
    end
    if PTS_ROLE=="caogiac" and GetTask(50)>=4 and GetTask(50)<=6 and HaveEventItem(41)<2 then tinsert(options,"Mua đèn/pts_buy_lamp") end
end
function PTSMenu()
    if not PTSClass() or PTSMap()~=PTS_MAP then CloseDialog();return end
    PTSJournal()
    local choices={};PTSAddChoices(choices)
    if PTS_HAS_SERVICE==1 then tinsert(choices,"Dịch vụ/pts_service") end
    SayButtons("<color=yellow>Nhiệm vụ chung Tam Giới<color><enter>Chỉ hiện nhiệm vụ đủ cấp và đủ điều kiện nhận hoặc trả. Xem bước đang làm ở F11.",getn(choices),choices)
end
function pts_service() if PTS_OLD_MAIN then return PTS_OLD_MAIN() end end
function PTSPreview(i)
    local a=PTS_ACTIONS[i];if not PTSReady(a) then return end
    local text="<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>"
    if a.money then text=text.."<enter><color=green>Thưởng "..a.money.." lượng.<color>" end
    if a.xp then text=text.."<enter><color=green>Thưởng "..a.xp.." kinh nghiệm.<color>" end
    if a.cost then text=text.."<enter><color=yellow>Chi phí: "..a.cost.." lượng.<color>" end
    SayButtons(text,1,"Đồng ý/pts_apply"..i)
end
function PTSExchange(task,from,to,needs,gifts,money,cost)
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()<(cost or 0) then return 0 end
    local old=GetCash()
    if money and money>0 then if old>2000000000-money then return 0 end;Earn(money);if GetCash()~=old+money then return 0 end end
    if cost and cost>0 then if Pay(cost)~=1 then if money and money>0 then Pay(money) end;return 0 end end
    if QuestExchange(task,from,to,needs,gifts,1)~=1 then
        if cost and cost>0 then Earn(cost) end
        if money and money>0 then Pay(money) end
        PTSNotice("Chưa đủ vật phẩm hoặc hành trang chưa đủ chỗ. Nhiệm vụ và phần thưởng được giữ nguyên.");return 0
    end
    return 1
end
function PTSApply(i)
    local a=PTS_ACTIONS[i];if not PTSReady(a) then return end
    if a.loan then
        if GetTrade()~=0 or GetLockState()~=0 or GetCash()<a.cost then return end
        if Pay(a.cost)==1 then PTSNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>") end
        return
    end
    local needs=a.needs;if a.keep then needs={} end
    local gifts={};for n=1,getn(a.gifts) do tinsert(gifts,a.gifts[n]) end
    if a.crystals then local roll=RANDOM(1,10);tinsert(gifts,{3,28,0,0,0,0,roll<=5 and 3 or (roll<=9 and 4 or 5)}) end
    if a.ruby and RANDOM(1,4)==1 then tinsert(gifts,{3,79,0,0,0,0,1}) end
    if PTSExchange(a.task,a.from,a.to,needs,gifts,a.money,a.cost)~=1 then return end
    if a.weight then AddWeightMax(a.weight) end
    if a.xp then AddOwnExp(a.xp) end
    if a.credit and a.credit<0 then DecCredit(-a.credit) end
    if a.task==50 and a.to==5 then SetTask(52,1) end
    if a.task==50 and a.to==8 then SetTask(52,0) end
    PTSJournal();PTSNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTSMountRecipe()
    for n=1,getn(PTS_MOUNTS) do local r=PTS_MOUNTS[n]
        if HaveNormalItem(0,10,r[1],r[2])>0 and HaveNormalItem(3,r[3],0,0)>0 then return r end
    end
end
function pts_mount()
    if not PTSClass() or PTSMap()~=1020 or GetLevel()<19 or PTS_ROLE~="west" then return end
    local r=PTSMountRecipe()
    if not r or HaveEventItem(39)<3 then PTSNotice("<color=yellow>Trùng Mộc<color><enter>Cần 3 Liễu Mộc, thú cưỡi trắng và Bá Lạc Nhãn cùng cấp trong hành trang.");return end
    SayButtons("<color=yellow>Trùng Mộc<color><enter>Đổi 3 Liễu Mộc, thú cưỡi trắng và Bá Lạc Nhãn cùng cấp thành thú cưỡi xanh.",1,"Hợp thành/pts_mount_apply")
end
function pts_mount_apply()
    if not PTSClass() or PTSMap()~=1020 or GetLevel()<19 or PTS_ROLE~="west" then return end
    local r=PTSMountRecipe();if not r then return end
    local phase=GetTask(4570)
    if PTSExchange(4570,phase,1-phase,{{4,39,0,0,0,0,3},{0,10,r[1],r[2],0,0,1},{3,r[3],0,0,0,0,1}},{{0,10,r[4],r[2],0,0,1}},nil,nil)==1 then PTSNotice("<color=green>Đã hợp thành thú cưỡi xanh cùng cấp.<color>") end
end
function pts_fuse()
    if not PTSClass() or PTSMap()~=1020 or PTS_ROLE~="west" or GetLevel()<39 or (GetTask(51)~=3 and GetTask(51)~=5) then return end
    local r=PTSMountRecipe();if not r then return end
    local phase=GetTask(51)
    if PTSExchange(51,phase,6,{{4,37,0,0,0,0,1},{0,10,r[1],r[2],0,0,1},{3,r[3],0,0,0,0,1}},{{0,10,r[4],r[2],0,0,1}},nil,nil)==1 then SetTask(51,3);PTSJournal() end
end
function pts_buy_lamp()
    if PTSMap()~=1004 or PTS_ROLE~="caogiac" or GetTask(50)<4 or GetTask(50)>6 or HaveEventItem(41)>=2 then return end
    local phase=GetTask(4571)
    if PTSExchange(4571,phase,1-phase,{},{{4,41,0,0,0,0,1}},nil,5000)==1 then PTSJournal() end
end
function pts_elements()
    if not PTSClass() or GetLevel()<31 or PTSMap()~=PTS_MAP or (PTS_ROLE~="store_west" and PTS_ROLE~="store_east") then return end
    local target=GetTask(55)
    if target==0 then
        local next=21+RANDOM(1,4)
        if PTSExchange(55,0,next,{},{},nil,nil)==1 then SetTask(54,1);PTSJournal() end
    elseif target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10 then
        local gifts={};local roll=RANDOM(1,100000);local green=0
        if roll<=10000 then gifts={{0,4,RANDOM(6,13),3,0,0,1}}
        elseif roll<=15000 then local part=RANDOM(3,5);gifts={{0,4+RANDOM(1,3),part,4,2+5*(part-3),0,1}};green=1 end
        if PTSExchange(55,target,0,{{3,target,0,0,0,0,10}},gifts,1200,nil)==1 then AddCredit(5);SetTask(54,0);if green==1 then SetTask(811,GetTask(811)+1) end;PTSJournal();PTSNotice("<color=green>Nhận 1200 lượng, 5 danh vọng và phần thưởng ngẫu nhiên nếu trúng.<color>") end
    end
end
function pts_stars()
    if not PTSClass() or PTSMap()~=1020 or PTS_ROLE~="duong" or GetLevel()<59 then return end
    local value=GetTask(53);local count=GetByte(value,2);local state=GetByte(value,1)
    if count>=36 then return end
    if state==0 then
        PTSExchange(53,value,SetByte(value,1,1),{},{{4,42,0,0,0,0,1}},nil,nil)
    elseif state==3 and HaveEventItem(44)>0 then
        local gifts={};local xp=0;local credit=1
        if count==35 then local p=GetPlayerType();local part=p==0 and (RANDOM(1,200)<100 and 31 or 32) or (p==1 and 33 or 34);gifts={{0,0,part,6,0,0,1}};xp=500000;credit=108 end
        if PTSExchange(53,value,SetByte(SetByte(value,1,0),2,count+1),{{4,44,0,0,0,0,1}},gifts,nil,nil)==1 then AddCredit(credit);if xp>0 then AddOwnExp(xp) end end
    end
    PTSJournal()
end
function pts_star_retry()
    if PTSMap()~=1020 or PTS_ROLE~="duong" then return end
    local value=GetTask(53)
    if GetByte(value,1)==2 then PTSExchange(53,value,SetByte(value,1,1),{{4,43,0,0,0,0,1}},{{4,42,0,0,0,0,1}},nil,nil);PTSJournal() end
end
function pts_tower()
    if not PTSClass() or GetLevel()<59 or PTSMap()~=PTS_TOWER_MAP then return end
    local value=GetTask(53);local count=GetByte(value,2)
    if count>=36 or floor(count/6)~=PTS_TOWER_GROUP or GetByte(value,1)~=1 or HaveEventItem(42)<1 then return end
    local tid=123;local level=60;if mod(count,6)==5 then tid=124;level=80 end
    local npc=AddNpc(tid,level,SubWorld,PTS_TOWER_X*256+160,PTS_TOWER_Y*512+288,1)
    if not npc or npc<=0 then return end
    local names={{"Thiên Khôi Tinh","Thiên Sát Tinh","Thiên Cơ Tinh","Thiên Nhàn Tinh","Thiên Dũng Tinh","Thiên Hùng Tinh"},{"Thiên Mãnh Tinh","Thiên Uy Tinh","Thiên Anh Tinh","Thiên Quý Tinh","Thiên Phúc Tinh","Thiên Mãn Tinh"},{"Thiên Hồ Tinh","Thiên Thương Tinh","Thiên Lập Tinh","Thiên Tiệp Tinh","Thiên ám Tinh","Thiên Hữu Tinh"},{"Thiên Không Tinh","Thiên Tốc Tinh","Thiên Dị Tinh","Thiên Sát Tinh","Thiên Vi Tinh","Thiên Cứu Tinh"},{"Thiên Thoái Tinh","Thiên Thọ Tinh","Thiên Kiếm Tinh","Thiên Bình Tinh","Thiên Tội Tinh","Thiên Tổn Tinh"},{"Thiên Bại Tinh","Thiên Lao Tinh","Thiên Huệ Tinh","Thiên Bạo Tinh","Thiên Khốc Tinh","Thiên Xảo Tinh"}}
    SetNpcName(npc,names[floor(count/6)+1][mod(count,6)+1])
    if SetNpcDeathScript(npc,"\\script\\phongthan\\shared\\star_"..count..".lua")~=1 then DelNpc(npc);return end
    if PTSExchange(53,value,SetByte(value,1,2),{{4,42,0,0,0,0,1}},{{4,43,0,0,0,0,1}},nil,nil)~=1 then DelNpc(npc) end
    PTSJournal();CloseDialog()
end

function PTSMarker()
    if not PTSClass() or PTSMap()~=PTS_MAP then return 0,0 end
    local best,level=0,0
    for i=1,getn(PTS_ACTIONS) do
        local a=PTS_ACTIONS[i]
        if a.role==PTS_ROLE and GetLevel()>=a.level and GetTask(a.task)==a.from then
            local state=PTSHave(a.needs)==1 and (not a.morph or GetMorphType()==a.morph) and (a.from==0 and 1 or 3) or 2
            if state>best then best=state;level=a.level end
        end
    end
    if PTS_ROLE=="duong" and GetLevel()>=59 then
        local v=GetTask(53);local state=GetByte(v,1);local count=GetByte(v,2)
        if count<36 then local mark=state==0 and 1 or (state==3 and HaveEventItem(44)>0 and 3 or 2);if mark>best then best=mark;level=59 end end
    end
    if (PTS_ROLE=="store_west" or PTS_ROLE=="store_east") and GetLevel()>=31 then
        local target=GetTask(55);local mark=target==0 and 1 or (target>=22 and target<=25 and HaveNormalItem(3,target,0,0)>=10 and 3 or 2)
        if mark>best then best=mark;level=31 end
    end
    return best,level
end
