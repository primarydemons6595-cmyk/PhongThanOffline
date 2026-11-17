--description: ³ÂÆæ
--author: yichuan
--date: 2004/5/12

function main(sel)
		Talk(1,"no",10237)
end;

function no()
		CloseDialog()	
end;
pt_sung_original_main=main
GetWorldPos=GetContentWorldPos
SubWorldIdx2ID=SubWorldIdx2ContentID
function main() if GetContentWorldPos()~=2 then CloseDialog();return end;return pt_sung_original_main() end
