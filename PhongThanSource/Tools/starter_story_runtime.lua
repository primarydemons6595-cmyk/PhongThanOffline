-- Original dialogue text and source IDs are emitted by build_starter_story.py.
-- Rewards below are the user's Loose policy, not original VNG reward values.
function PTStarterStep(a)
    return a and PT_STARTER_STORY[a.task] and PT_STARTER_STORY[a.task][a.from]
end
function PTStarterReward(a)
    return a.to==PT_STARTER_FINAL[a.task] and 5000 or 500
end
function PTStarterText(a,completed)
    local d=PTStarterStep(a)
    local text="<color=yellow>"..a.title.."<color><enter>"..d.text
    text=text.."<enter><enter><color=cyan>Mục tiêu hiện tại: "..a.note.."<color>"
    local verb=completed and "Đã nhận" or "Thưởng khi hoàn tất bước này"
    text=text.."<enter><color=green>"..verb..": "..PTStarterReward(a).." EXP"
    if a.money>0 then text=text.." + "..a.money.." lượng" end
    return text.."<color>"
end
-- Patch only the three reviewed level-three chains. Preserve all other steps.
function PTStarterInstall(prefix)
    local steps=prefix=="ptg" and PTG_STEPS or PT2_STEPS
    if not steps then return end
    for i=1,getn(steps) do
        if PTStarterStep(steps[i]) then steps[i].xp=PTStarterReward(steps[i]) end
    end
end
PTStarterInstall("ptg")
PTStarterInstall("pt2")
if PTGPreview and PTGApply and not PT_STARTER_G_INSTALLED then
    PT_STARTER_G_INSTALLED=1
    PT_STARTER_OLD_G_PREVIEW=PTGPreview
    PT_STARTER_OLD_G_APPLY=PTGApply
    function PTGPreview(i)
        local a=PTG_STEPS[i]
        if not PTStarterStep(a) then return PT_STARTER_OLD_G_PREVIEW(i) end
        if not PTGReady(a) then return end
        SayButtons(PTStarterText(a,nil),2,"Tiếp tục/ptg_yes"..i,"Quay lại/main")
    end
    function PTGApply(i)
        local a=PTG_STEPS[i]
        local before=a and GetTask(a.task)
        PT_STARTER_OLD_G_APPLY(i)
        if PTStarterStep(a) and before==a.from and GetTask(a.task)==a.to then
            Msg2Player("Nhận được "..a.xp.." điểm kinh nghiệm"..(a.money>0 and " và "..a.money.." lượng." or "."))
            PTGNotice(PTStarterText(a,1))
        end
    end
end
if PT2Preview and PT2Apply and not PT_STARTER_2_INSTALLED then
    PT_STARTER_2_INSTALLED=1
    PT_STARTER_OLD_2_PREVIEW=PT2Preview
    PT_STARTER_OLD_2_APPLY=PT2Apply
    function PT2Preview(i)
        local a=PT2_STEPS[i]
        if not PTStarterStep(a) then return PT_STARTER_OLD_2_PREVIEW(i) end
        if not PT2Ready(a) then return end
        SayButtons(PTStarterText(a,nil),2,"Tiếp tục/pt2_yes"..i,"Quay lại/main")
    end
    function PT2Apply(i)
        local a=PT2_STEPS[i]
        local before=a and GetTask(a.task)
        PT_STARTER_OLD_2_APPLY(i)
        if PTStarterStep(a) and before==a.from and GetTask(a.task)==a.to then
            Msg2Player("Nhận được "..a.xp.." điểm kinh nghiệm"..(a.money>0 and " và "..a.money.." lượng." or "."))
            PT2Notice(PTStarterText(a,1))
        end
    end
end
