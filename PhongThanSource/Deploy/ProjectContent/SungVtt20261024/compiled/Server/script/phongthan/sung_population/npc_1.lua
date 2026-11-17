--description:npc
--author: zhujialiang
--date:2005/4/13

function  main()
		Talk(1,"no",11162)
end;

function  no()
	CloseDialog()
end;

pt_sung_original_main=main
GetWorldPos=GetContentWorldPos
SubWorldIdx2ID=SubWorldIdx2ContentID
function main() if GetContentWorldPos()~=2 then CloseDialog();return end;return pt_sung_original_main() end
