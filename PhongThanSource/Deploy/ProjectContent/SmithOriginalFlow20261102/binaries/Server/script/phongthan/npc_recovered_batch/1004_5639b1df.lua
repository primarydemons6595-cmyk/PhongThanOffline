--description: Í­½³-ò¿ÓÈÄ¹
--author: yujin
--date: 2005/3/21

function main(sel)
	if(songxin()==0)then
		local tasks = 
		{
			{"§Ëp trøng","zadan";show=0}
		}
		if (HaveNormalItem(3,70,0,0)>=10)and(GetCash()>=1000)then
			tasks[1].show=1
			Talk(2,"egg",11151,11152)
		elseif (HaveNormalItem(3,71,0,0)>=10)and(GetCash()>=2000)then
			tasks[1].show=1
			Talk(2,"egg",11151,11153)
		elseif (HaveNormalItem(3,72,0,0)>=10)and(GetCash()>=10000)then
			tasks[1].show=1
			Talk(2,"egg",11151,11154)
		else
			MsgBox(10275,"yes_1","no")
		end;
	end
end;

function songxin()
	local task_id = 874
	local map_id = 3

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

function yes_1()
		CloseDialog()
		Sale(4);
end;

function no()
		CloseDialog()
end;

function egg()
	SayTask(11424,tasks)
end;

function  zadan()
	if (HaveNormalItem(3,70,0,0)>=10)and(GetCash()>=1000)then
			for a=1,10 do
				DelNormalItem(3,70,0,0)
			end;
		Pay(1000)
		local   k=random(1,10)
			if(k==1)then
				AddNormalItem(6,1,89,1,0,0)
			elseif(k==2)then
				AddNormalItem(6,1,95,1,0,0)
			elseif(k==3)then
				AddNormalItem(6,1,51,1,0,0)
			elseif(k==4)then
				AddNormalItem(6,1,93,1,0,0)
			elseif(k==5)then
				AddNormalItem(6,1,80,1,0,0)
			elseif(k==6)then
				AddNormalItem(6,1,82,1,0,0)
			elseif(k==7)then
				AddNormalItem(6,1,79,1,0,0)
			elseif(k==8)then
				AddNormalItem(6,1,65,1,0,0)
			elseif(k==9)then
				AddNormalItem(6,1,52,1,0,0)
			elseif(k==10)then
				AddNormalItem(6,1,70,1,0,0)
			end;
		Msg2Player("NhËn ®­îc ViÔn Cæ phï s¬ cÊp")
	elseif (HaveNormalItem(3,71,0,0)>=10)and(GetCash()>=2000)then
			for a=1,10 do
				DelNormalItem(3,71,0,0)
			end;
		Pay(2000)
		local   j=random(1,5)
			if(j==1)then
				AddNormalItem(6,1,73,1,0,0)
			elseif(j==2)then
				AddNormalItem(6,1,53,1,0,0)
			elseif(j==3)then
				AddNormalItem(6,1,75,1,0,0)
			elseif(j==4)then
				AddNormalItem(6,1,63,1,0,0)
			elseif(j==5)then
				AddNormalItem(6,1,66,1,0,0)
			end;
		Msg2Player("NhËn ®­îc ViÔn Cæ phï trung cÊp")
	elseif (HaveNormalItem(3,72,0,0)>=10)and(GetCash()>=10000)then
			for a=1,10 do
				DelNormalItem(3,72,0,0)
			end;
		Pay(10000)
		local   i=random(1,5)
			if(i==1)then
				AddNormalItem(6,1,46,1,0,0)
			elseif(i==2)then
				AddNormalItem(6,1,62,1,0,0)
			elseif(i==3)then
				AddNormalItem(6,1,64,1,0,0)
			elseif(i==4)then
				AddNormalItem(6,1,50,1,0,0)
			elseif(i==5)then
				AddNormalItem(6,1,98,1,0,0)
			end;
		Msg2Player("NhËn ®­îc ViÔn Cæ phï cao cÊp")
	end;
	CloseDialog()
end;

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1004 then CloseDialog(); return end
    return pt_original_main()
end

Include("\\script\\phongthan\\restoration\\trial.lua")
PTTR_ROLE=2
PTTR_OLD_MAIN=main
PTTR_OLD_MARK=GetPlayerTaskState
function main() return PTTrialMenu() end
function GetPlayerTaskState() return PTTrialMarker() end

function GetPlayerTaskCategory() return 0 end

-- Original-style smith: short menu, choose item, confirm exact ingredients.
PT_SMITH_PREVIOUS_MAIN=main
function main()
 SayButtons("Kh¸ch quan muèn mua <color=green>trang bÞ vò khÝ g×<color>?",3,"Thuéc tÝnh trang bÞ/pt_smith_property_info","Giao dÞch/pt_smith_trade","T×m hiÓu/pt_smith_previous")
end
function pt_smith_previous() return PT_SMITH_PREVIOUS_MAIN() end
function pt_smith_trade() return yes_1() end
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
