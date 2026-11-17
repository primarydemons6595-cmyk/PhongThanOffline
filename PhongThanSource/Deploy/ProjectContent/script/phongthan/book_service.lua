-- Server-side trusted NPC service. Recipes/base odds are recovered data.
Include("\\script\\phongthan\\book_policy.lua")
if type(PT_BOOK_POLICY)~="table" or type(PT_BOOK_POLICY.FailureBonusBasisPoints)~="number" or type(PT_BOOK_POLICY.MaxBonusBasisPoints)~="number" or PT_BOOK_POLICY.FailureBonusBasisPoints<1 or PT_BOOK_POLICY.FailureBonusBasisPoints>10000 or PT_BOOK_POLICY.FailureBonusBasisPoints~=floor(PT_BOOK_POLICY.FailureBonusBasisPoints) or PT_BOOK_POLICY.MaxBonusBasisPoints<0 or PT_BOOK_POLICY.MaxBonusBasisPoints>10000 or PT_BOOK_POLICY.MaxBonusBasisPoints~=floor(PT_BOOK_POLICY.MaxBonusBasisPoints) or (PT_BOOK_POLICY.ResetOnSuccess~=0 and PT_BOOK_POLICY.ResetOnSuccess~=1) then error("Invalid reconstructed book policy") end
function pt_book_percent(bp) return format("%.1f",bp/100) .. "%" end
function pt_book_luck()
    local luck=GetTask(4888)
    if luck<0 then luck=0 end
    if luck>PT_BOOK_POLICY.MaxBonusBasisPoints then luck=PT_BOOK_POLICY.MaxBonusBasisPoints end
    return luck
end
function pt_book_guard()
    return pt_runtime_world_pos()==PT_BOOK_WORLD
end
function pt_book_menu()
    if not pt_book_guard() then CloseDialog();return end
    local class=GetPlayerType()
    if class<0 or class>2 then return end
    local choices={}
    for i=1,getn(PT_BOOK_RECIPES) do
        local row=PT_BOOK_RECIPES[i]
        if row.class==class or row.class==3 then tinsert(choices,row.name.."/pt_book_offer("..i..")") end
    end
    local args={"§ãng s¸ch. §iÓm may m¾n hiÖn cã: <color=yellow>"..pt_book_percent(pt_book_luck()).."<color> t¨ng thªm.",getn(choices)}
    for i=1,getn(choices) do tinsert(args,choices[i]) end
    call(Say,args)
end
function pt_book_offer(index)
    if not pt_book_guard() or index==nil or index<1 or index>getn(PT_BOOK_RECIPES) then return end
    local row=PT_BOOK_RECIPES[index]
    if row.class~=3 and row.class~=GetPlayerType() then return end
    local chance=row.base+pt_book_luck()
    if chance>10000 then chance=10000 end
    local text="§ãng <color=green>"..row.name.."<color> cÇn:\n"
    for i=1,getn(row.needs) do
        text=text.."<color=cyan>"..row.needs[i][7].." "..row.materials[i].."<color>\n"
    end
    text=text.."Tû lÖ thành c«ng: <color=yellow>"..pt_book_percent(chance).."<color>"
    if row.base<10000 then text=text.."\n§· céng may m¾n: <color=yellow>"..pt_book_percent(pt_book_luck()).."<color>. ThÊt b¹i t¨ng "..pt_book_percent(PT_BOOK_POLICY.FailureBonusBasisPoints).."." end
    Say(text,2,"X¸c nhËn/pt_book_make("..index..")","Quay l¹i/pt_book_menu")
end
function pt_book_make(index)
    CloseDialog()
    if not pt_book_guard() or index==nil or index<1 or index>getn(PT_BOOK_RECIPES) then return end
    local row=PT_BOOK_RECIPES[index]
    if row.class~=3 and row.class~=GetPlayerType() then return end
    local phase=GetTask(4887)
    if phase<0 or phase>=2147483646 then Talk(1,"pt_book_menu","Ch­a thÓ ®ãng s¸ch.");return end
    local luck=pt_book_luck()
    local chance=row.base+luck
    if chance>10000 then chance=10000 end
    local success=row.base==10000 or random(1,10000)<=chance
    local reward=row.reward
    local nextluck=luck
    if success then
        if row.base<10000 and PT_BOOK_POLICY.ResetOnSuccess==1 then nextluck=0 end
    else
        reward=row.failures[random(1,getn(row.failures))]
        nextluck=luck+PT_BOOK_POLICY.FailureBonusBasisPoints
        if nextluck>PT_BOOK_POLICY.MaxBonusBasisPoints then nextluck=PT_BOOK_POLICY.MaxBonusBasisPoints end
    end
    if QuestExchange(4887,phase,phase+1,row.needs,{reward})~=1 then
        Talk(1,"pt_book_menu","Ch­a ®ãng ®­îc s¸ch. KiÓm tra <color=cyan>nguyªn liÖu trong F4<color>, chç trèng và khãa hành trang. §iÓm may m¾n ®­îc gi÷ nguyªn.")
        return
    end
    SetTask(4888,nextluck)
    if success then
        Talk(1,"pt_book_menu","§ãng s¸ch thành c«ng: <color=green>"..row.name.."<color>. §iÓm may m¾n cßn <color=yellow>"..pt_book_percent(nextluck).."<color>.")
    else
        Talk(1,"pt_book_menu","§ãng s¸ch thÊt b¹i, ®· nhËn phÇn th­ëng phô. §iÓm may m¾n t¨ng lªn <color=yellow>"..pt_book_percent(nextluck).."<color> cho lÇn ®ãng tiÕp theo.")
    end
end
