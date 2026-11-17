--description: Í­½³-ÎäÆ÷ÏúÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(songxin()==0)then
		MsgBox(10319,"yes","no")
	end
end;

function songxin()
	local task_id = 874
	local map_id = 7

	local task_val = GetTask(task_id)
	local type1 = GetByte(task_val,1) 
	local type2 = GetByte(task_val,2)
	local finish = GetByte(task_val,3)

	local setbit = 0
	if (type1 == map_id) then
		setbit = 7
	elseif (type2 == map_id)then
		setbit = 8
	end

	if (setbit ~= 0) then
	 	if (GetBit(finish, setbit) == 0)then
	 		Talk(1,"no","Ng­¬i vÊt v¶ qu¸!")
			SetTask(task_id, SetByte(task_val,3,SetBit(finish, setbit, 1)))
	 		return 1
	 	end
	end
	return 0
end

function yes()
		CloseDialog()
		Sale(18);
end;

function no()
		CloseDialog()
end;
-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1001 then CloseDialog(); return end
    return pt_original_main()
end

-- Property enhancement: retain every existing quest/shop callback.
PT_SMITH_PREVIOUS_MAIN=main
function main()
 SayButtons("Thî §ång",3,"Cöa hµng vµ nhiÖm vô/pt_smith_previous","N©ng thuéc tÝnh/pt_smith_property_info","KÕt thóc/no")
end
function pt_smith_previous() return PT_SMITH_PREVIOUS_MAIN() end
function pt_smith_property_info()
 SayButtons("§Æt trang bÞ vµ ngäc trong tói F4. §å xanh dïng Lôc Tïng Th¹ch; vò khÝ vµng dïng Hoµng Ngäc. Quy t¾c Loose: bËc 1-6 lo¹i th­êng, 7-8 cao cÊp, 9-10 ®Æc biÖt. Mçi lÇn tèn 1 ngäc, thµnh c«ng 50% t¨ng mét dßng ngÉu nhiªn tíi giíi h¹n gèc; thÊt b¹i gi÷ nguyªn ®å. Dßng tèi ®a cã sao. Chän tiÕp tôc råi bÊm vµo ®å trong F4.",2,"TiÕp tôc/pt_smith_property_open","Quay l¹i/main")
end
function pt_smith_property_open()
 if SmithPropertyOpen()==1 then CloseDialog() else Talk(1,"no","H·y ®Õn gÇn Thî §ång, kÕt thóc giao dÞch vµ më khãa tói.") end
end
