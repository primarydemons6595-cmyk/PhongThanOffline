-- Rebuilt from item 224/2 description (30 minutes) and verified NPC 300.
-- The server validates ownership, locks, trade state and exact item mapping.
function main(itemID)
    local genre, detail, part = FindItemEx(itemID)
    if genre ~= 8 or detail ~= 224 or part ~= 2 then return end
    if UseVerifiedMorphItem(itemID) ~= 1 then
        Msg2Player("Chua the bien hinh. Vat pham duoc giu nguyen.")
    end
end
