-- PROJECT_RECONSTRUCTION_FROM_EXPLICIT_F2_DESCRIPTION: IBItem 6789/2.
-- Fixed rewards only; template identities and native pile limits verified.
function main(nItemIndex)
    local genre, detail, part = FindItemEx(nItemIndex)
    if genre ~= 8 or detail ~= 6789 or part ~= 2 then return end
    local phase = GetTask(4897)
    local nextPhase = 1
    if phase == 1 then nextPhase = 0 end
    local needs = {{8,6789,2,0,0,0,1}}
    local rewards = {{6,1,5668,1,0,0,20}}
    if QuestExchange(4897,phase,nextPhase,needs,rewards,1) ~= 1 then
        Msg2Player("Hanh trang khong du cho hoac chua the mo tui. Vat pham duoc giu nguyen.")
        return
    end
    Msg2Player("Da nhan du vat pham trong tui.")
end
