-- Presentation only: payment amounts remain integer luong on the server.
function PTDialogMoney(value)
    value = floor(value)
    if value < 10000 then return value .. " l­îng" end
    local whole = floor(value / 10000)
    local remainder = value - whole * 10000
    if remainder == 0 then return whole .. " v¹n l­îng" end
    local fraction = format("%04d",remainder)
    fraction = gsub(fraction,"0+$","")
    return whole .. "." .. fraction .. " v¹n l­îng"
end
function PTDialogMaterial(count,name)
    return "<color=cyan>" .. count .. " " .. name .. "<color>"
end
function PTDialogMoneyRequirement(have,need)
    local color = "red"
    if have >= need then color = "green" end
    return "<color=" .. color .. ">" .. PTDialogMoney(have) .. "<color> / <color=yellow>" .. PTDialogMoney(need) .. "<color><enter>"
end
