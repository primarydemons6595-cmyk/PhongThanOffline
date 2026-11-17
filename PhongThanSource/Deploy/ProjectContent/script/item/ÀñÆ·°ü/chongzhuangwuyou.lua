-- PROJECT_RECONSTRUCTION_FROM_EXPLICIT_F2_DESCRIPTION: IBItem 232/2.
-- Fixed rewards only; template identities and native pile limits verified.
function main(nItemIndex)
    local genre, detail, part = FindItemEx(nItemIndex)
    if genre ~= 8 or detail ~= 232 or part ~= 2 then return end
    local phase = GetTask(4897)
    local nextPhase = 1
    if phase == 1 then nextPhase = 0 end
    local needs = {{8,232,2,0,0,0,1}}
    local rewards = {{3,41,0,0,0,0,3},{8,191,2,0,0,0,3}}
    if QuestExchange(4897,phase,nextPhase,needs,rewards,1) ~= 1 then
        Msg2Player("Hanh trang khong du cho hoac chua the mo tui. Vat pham duoc giu nguyen.")
        return
    end
    Msg2Player("Da nhan du vat pham trong tui.")
end
