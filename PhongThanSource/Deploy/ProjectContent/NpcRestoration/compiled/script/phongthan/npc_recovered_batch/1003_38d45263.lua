--description:npc
--author: zhujialiang
--date:2005/4/13

function  main()
		Talk(1,"no",11381)
end;

function  no()
	CloseDialog()
end;

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1003 then CloseDialog(); return end
    return pt_original_main()
end
