-- Rebuilt to match the original item tuple and description.
function main(itemID)
    local genre, detail, part = FindItemEx(itemID)
    if genre ~= 8 or detail ~= 415 or part ~= 2 then return end
    if UsePetItem(itemID) ~= 1 then
        Msg2Player("Linh thu chua du dieu kien. Vat pham chua bi tru.")
    end
end
