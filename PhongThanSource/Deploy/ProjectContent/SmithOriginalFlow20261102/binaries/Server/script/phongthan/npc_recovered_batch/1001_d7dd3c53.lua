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

-- Original-style smith: short menu, choose item, confirm exact ingredients.
PT_SMITH_PREVIOUS_MAIN=main
function main()
 SayButtons("Kh¸ch quan muèn mua <color=green>trang bÞ vò khÝ g×<color>?",3,"Thuéc tÝnh trang bÞ/pt_smith_property_info","Giao dÞch/pt_smith_trade","T×m hiÓu/pt_smith_previous")
end
function pt_smith_previous() return PT_SMITH_PREVIOUS_MAIN() end
function pt_smith_trade() return yes() end
function pt_smith_property_info()
 SayButtons("Thî ®ång: H·y chän trang bÞ hoÆc vò khÝ muèn t¹o thuéc tÝnh.",2,"X¸c nhËn/pt_smith_property_open","Hñy/no")
end
function pt_smith_property_open()
 CloseDialog()
 if SmithPropertyOpen()~=1 then Talk(1,"no","H·y ®Õn gÇn Thî ®ång råi thö l¹i.") end
end
PT_SMITH_STONES={
 [236]="Hoµng Ngäc",[237]="Hoµng Ngäc cao cÊp",[238]="Hoµng Ngäc ®Æc biÖt",
 [239]="Lôc Tïng Th¹ch",[240]="Lôc Tïng Th¹ch cao cÊp",[241]="Lôc Tïng Th¹ch ®Æc biÖt"}
function pt_smith_confirm()
 local ok,name,stone=SmithPropertyAction(0)
 if ok~=1 then Talk(1,"no","H·y chän l¹i trang bÞ muèn t¹o thuéc tÝnh.");return end
 name=gsub(name,"^[#$]+","")
 local text="Thî ®ång: Ng­¬i muèn tiÕp tôc t¹o thuéc tÝnh? Muèn t¹o thuéc tÝnh <color=green>"..name.."<color>, cÇn ph¶i cã 1 <color=cyan>"..name.."<color> vµ 1 <color=cyan>"..PT_SMITH_STONES[stone].."<color>."
 SayButtons(text,2,"X¸c nhËn/pt_smith_property_commit","Hñy/pt_smith_property_cancel")
end
function pt_smith_property_commit()
 local result,stone=SmithPropertyAction(1)
 if result==101 then Talk(1,"no","Thî ®ång: Chóc mõng ng­¬i t¹o thuéc tÝnh thµnh c«ng. <color=green>Thuéc tÝnh trang bÞ ®· th¨ng cÊp.<color>")
 elseif result==102 then Talk(1,"no","Thî ®ång: T¹o thuéc tÝnh thÊt b¹i.")
 elseif result==103 then Talk(1,"no","B¹n kh«ng ®ñ <color=cyan>"..(PT_SMITH_STONES[stone] or "ngäc").."<color>.")
 elseif result==105 then Talk(1,"no","Thuéc tÝnh trang bÞ ®· ®¹t møc cao nhÊt.")
 else Talk(1,"no","H·y chän l¹i trang bÞ muèn t¹o thuéc tÝnh.") end
end
function pt_smith_property_cancel() SmithPropertyAction(2);CloseDialog() end
