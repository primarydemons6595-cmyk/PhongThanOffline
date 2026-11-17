--description: Ò©µê-Ò©¢»ºØÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
	{
		{"TrÞ th­¬ng","yes_1";show=1},
		{"Mua b¸n","yes_2";show=1}
	}
	if(GetTask(304)==51)then
			Talk(1,"no","T×nh h×nh ë ®©y ®¹i kh¸i nh­ thÕ, mau quay vÒ thao tr­êng b¸o cho vâ s­ ®i.")
			SetTask(304,100)
			TaskNote(31,0)
	elseif(GetTask(314)==51)then
			Talk(1,"no","§¹i v­¬ng ë ®©y lµ m¹nh nhÊt, ng­¬i kh«ng muèn chÕt th× h·y mau vÒ b¸o l¹i víi Hoµng Thiªn Hãa.")
			SetTask(314,100)
	else
			SayTask(10186,tasks)
	end;
end;

function yes_1()
	local t=SystemTime()-GetTask(763)
	if (t>=1800)then
		yes_3()
	else
		Talk(1,"no","Mçi lÇn trÞ th­¬ng cho mét ng­êi hao tæn cña ta mét l­îng nguyªn khÝ nhÊt ®Þnh, mÊt n÷a giê míi håi phôc. Ng­¬i h·y quay l¹i sau nhÐ!")
	end;
end;

function yes_3()
	RestoreLife()
	RestoreMana()
	SetTask(763,SystemTime())
	Msg2Player("Sinh lùc vµ néi lùc cña b¹n ®· hoµn toµn håi phôc.")
	CloseDialog()
end;

function yes_2()
	CloseDialog()
	Sale(23);
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
    if w ~= 1051 then CloseDialog(); return end
    return pt_original_main()
end
