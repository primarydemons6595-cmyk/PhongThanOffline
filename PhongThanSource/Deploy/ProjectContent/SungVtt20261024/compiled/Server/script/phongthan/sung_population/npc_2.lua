--description:npc
--author: zhujialiang
--date:2005/4/13

function main()
	local tasks = {
		{"Rêi m¹ng luyÖn ®¬n","liandan";show=0},
		{"NhËn ®¬n d­îc","lingqu";show=0},
		{"B¾t ®Çu tu luyÖn","doublexp";show=0},
		{"Ng­ng tu luyÖn","tingzhi";show=0}
	}
	if(GetPlayerType()==0)then
		tasks[1].show=1;
	end;

	if((GetNewPills()>0)and(GetPlayerType()==0))then
		tasks[2].show=1;
	end;

	local state = GetPillsState();
	if(GetPlayerType()==0)then
		if(state==2)then
			tasks[4].show=1;
			tasks[3].show=0;
		else
			tasks[4].show=0;
			tasks[3].show=1;
		end;
	end;

	SayTask(10686,tasks)
end;

function no()
	CloseDialog()
end;

--ÀëÏßÁ¶µ¤
function liandan()
	if(GetNewPills()==12)then
		if(GetPillsCount()<84)then
			MsgBox(11427,"no")
		else
			MsgBox(11428,"no")
		end;
	else
		StartMakePills()
		if(GetCanMakePills()>0)then
			MsgBox(10687,"no")
			--ÉèÖÃÖ±½Óµ÷ÓÃalt+q
		else
			Talk(2,"no",10688,10689)			
		end;
	end;
end;

--ÁìÈ¡µ¤Ò©
function lingqu()
	local cantake = (84 - GetPillsCount())-GetNewPills();
	local take = 0
	local leave = 0
	local ndmoney=0
	if(GetPillsCount()==84)then
		MsgBox(10690,"no")
		return
	end;
	if(cantake>=0) then
		take=GetNewPills()
		leave=0
	else
		take=(84 - GetPillsCount())
		leave=GetNewPills()-(84 - GetPillsCount());
	end;
	ndmoney=10*GetLevel()*take
	if(GetCash()>=ndmoney)then
			Pay(ndmoney)
			GetAllPills();
			MsgBox("§©y lµ <color=red>"..take.."<color> viªn ®¬n d­îc, cßn d­ <color=red>"..leave.."<color> viªn, tæng céng ng­¬i ®· tèn <color=red>"..ndmoney.."<color>.","no")
	else
			MsgBox("Muèn lÊy linh d­îc cÇn ph¶i tèn <color=red>"..ndmoney.."<color> l­îng, ng­¬i ch­a ®ñ tiÒn!","no")
	end;
end;

--¿ªÊ¼ÐÞÁ¶
function kaishi()
	if(GetPillsCount()>0) then
		if(HaveNormalItem(3,81,0,0)>=1)then
			MsgBox(10691,"shiyong","no")
		else
			StartUsePills(50)
			MsgBox(10692,"no")
		end;
	else
		if(GetNewPills()>0)then
			MsgBox(10693,"lingqu","no")
		else
			MsgBox(10694,"no")	
		end;
	end;
end;

--Í£Ö¹ÐÞÁ¶
function tingzhi()
	StopUsePills()
	MsgBox(10695,"no")
end;

function shiyong()
	if(HaveNormalItem(3,81,0,0)>=1)then
		DelNormalItem(3,81,0,0)
		StartUsePills(75)
		MsgBox(10696,"no")
	else
		MsgBox(10697,"no")	
	end;
end;


--ÅÐ¶ÏÊÇ·ñ´¦ÓÚË«±¶¾­ÑéÊ±¼äÄÚ
function doublexp()
	local time=SystemTime()-GetTask(379)      --È¡Ë«±¶¾­ÑéÊ±¼äÊ£ÓàÖµ
	if(time<7200) then
		MsgBox(11159,"no")
	else
		kaishi()
	end;
end;

pt_sung_original_main=main
GetWorldPos=GetContentWorldPos
SubWorldIdx2ID=SubWorldIdx2ContentID
function main() if GetContentWorldPos()~=2 then CloseDialog();return end;return pt_sung_original_main() end
