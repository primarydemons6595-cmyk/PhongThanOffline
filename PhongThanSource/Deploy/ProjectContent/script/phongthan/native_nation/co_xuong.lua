--description: ¼§²ý
--author: yichuan
--date: 2004/6/10

function main(sel)
	tasks = 
	{
		{"KiÕn lËp ","renwu1";show=1},
		{"gia nhËp","renwu2";show=1},
		{"Tho¸t ly","renwu3";show=0},
		{"Gi¶i t¸n","renwu4";show=0},
		{"H­ng thÞnh","renwu5";show=1},
		{"Hñy xin phÐp","renwu6";show=0}
	}
	if(IsRegMember()==1)then
			tasks[6].show=1
	elseif(IsTongMember()==1)then
			tasks[3].show=1
	end;
	if(IsTongMaster()==1)then
			tasks[4].show=1
	end;
	SayTask(10287,tasks)
end;

function renwu1()
		if(IsTongMember()==1)then
				Talk(1,"no",10288)
		elseif(GetLevel()<60)then
				Talk(1,"no","B¹n ph¶i ®¹t cÊp 60 míi cã thÓ kiÕn lËp l·nh ®Þa.")
		elseif(GetLevel()>=60)and(GetCredit()>=500)and(GetCash()>=500000)then
				CloseDialog()
				CreateTongDialog()
		else
				Talk(1,"no",10290)
		end;
end;

function renwu2()
		if(IsTongMember()==1)then
				Talk(1,"no",10291)
		elseif(IsRegMember()==1)then
				Talk(1,"no",10292)
		elseif(GetLevel()<12)then
				Talk(1,"no",10293)
		else
				CloseDialog()
				AddTongDialog()
		end;
end;

function   renwu6()
		MsgBox(10631,"yes","no")
end;

function   renwu3()
		if(IsTongMaster()==1)then
				Talk(1,"no",10630)
		elseif(IsTongMember()==1)then
				MsgBox(10632,"yes","no")
		else
				Talk(1,"no",10633)
		end;
end;

function  yes()
		LeaveTong()
		CloseDialog()
end;

function no()
		CloseDialog()
end;

function   renwu4()
		MsgBox(11191,"quanzu","no")
end;

function   quanzu()
		MsgBox(11192,"jiesan","no")
end;

function   jiesan()
		tong=GetTongName()
		local destroyed = DestroyTong()
		CloseDialog()
		l=GetName()
		if destroyed == 1 then
		AddGlobalCountNews("<color=green>"..l.."<color> ®· gi¶i t¸n L·nh ®Þa <color=green>"..tong.."<color> cña m×nh.",20)
		end
end;

function   renwu5()
		Talk(1,"no",11193)
end;
