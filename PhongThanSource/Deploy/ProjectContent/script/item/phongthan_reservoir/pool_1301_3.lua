-- Project reconstruction from IBItem detail=1301, part=3; original Lua unavailable.
-- Read-only snapshot supplied by CoreClient. No item use or mutation on hover.
function GetDesc(nRemaining)
    if nRemaining < 0 then nRemaining = 0 end
    if nRemaining > 7000 then nRemaining = 7000 end
    return "<color=0,255,0>Sinh lùc cßn l¹i: <color=255,255,0>" .. format("%d", nRemaining) .. " / 7000<color><enter>Håi phôc 500 ®iÓm / 0.5 gi©y.<enter>NhÊp chuét ph¶i ®Ó bËt/t¾t b×nh.<color>"
end
