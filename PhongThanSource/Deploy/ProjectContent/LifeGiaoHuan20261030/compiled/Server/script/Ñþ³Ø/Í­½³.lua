--description: Í­½³-ÎäÆ÷ÏúÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(songxin()==0)then
		MsgBox(10508,"yes","no")
	end
end;

function songxin()
	local task_id = 874
	local map_id = 6

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
-- PT_BRONZE_GATE_20261030
-- General weapons shop and original delivery callback remain available.
PT_BRONZE_OLD_MAIN=main
function main()
 if GetContentWorldPos()~=52 then return PT_BRONZE_OLD_MAIN() end
 if songxin and songxin()==1 then return end
 SayButtons("Thî §ång Diªu Tr×",3,"Mua b¸n vò khÝ/yes","Khai Kho¸ng/pt_bronze_info","§ãng/no")
end
function pt_bronze_info()
 if GetContentWorldPos()~=52 then return end
 SayButtons("§Õn Thanh §ång S¬n ®Ó ®µo ®ång thau. CÇn häc Bµn Cæ Khai Thiªn vµ cÇm Cuèc chim. Chän hè Thanh §ång Kho¸ng ®Ó khai th¸c; cã c¬ héi nhËn thªm Hoµng ®ång, §ång ®á hoÆc XÝch ®ång.",3,
 "Vµo Thanh §ång S¬n/pt_bronze_enter","Mua Cuèc chim/yes","Quay l¹i/main")
end
function pt_bronze_enter()
 if GetContentWorldPos()~=52 or GetTrade()~=0 or GetLockState()~=0 then return end
 NewWorld(57,1608,3094);CloseDialog()
end
