--description:npc
--author: zhujialiang
--date:2005/4/13

function  main()
	tasks=
	{
		{"§¹o cô","renwu1";show=0}
	}
		if(GetLevel()>=50)then
			tasks[1].show=1
		end;

	SayTask(11212,tasks)
end;


function  renwu1()
		local  renwu=GetTask(310)
		local  thistime=GetTask(311)
		local  cishu=GetTask(313)
		local thisday=floor(SystemTime()/86400)
		if(renwu>0)then
					tasks1 = 
					{
						{"Cung cÊp","wancheng";show=1},
						{"Hñy bá","esc";show=1}
					}
				SayTask(11271,tasks1)
		elseif(thisday<thistime)then					
				Talk(1,"no",11272)
		else
				MsgBox("Ta ®ang rÊt cÇn nhiÒu d­îc liÖu ®Ó luyÖn Tiªn ®¬n, ng­¬i cã thÓ gióp ta 1 ngµy <color=green>10<color>. Ngoµi ra cø mçi lÇn giao thuèc h·y gióp ta <color=green>"..(floor(GetLevel()/20-2.5)*5000+5000).."<color> phİ dông. Ng­¬i còng cã thÓ tïy ı hñy bá nhiÖm vô. Sao h¶?","check_1","no")
		end;
end;

function   wancheng()
		local  renwu=GetTask(310)
		local  thistime=GetTask(311)
		local  cishu=GetTask(313)
		if((renwu==1)and(HaveNormalItem(3,6,0,0)>=5))then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,5 do
								DelNormalItem(3,6,0,0)
						end;
				end;
				TaskNote(32,42)
		elseif(renwu==2)and(HaveNormalItem(3,8,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,8,0,0)
						end;
				end;
				TaskNote(32,42)
		elseif(renwu==3)and(HaveNormalItem(3,9,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,9,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==4)and(HaveNormalItem(3,10,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,10,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==5)and(HaveNormalItem(3,11,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,11,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==6)and(HaveNormalItem(3,12,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,12,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==7)and(HaveNormalItem(3,13,0,0)>=10)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,10 do
								DelNormalItem(3,13,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==8)and(GetItemLevel2(0,4,0)>=1)then
				local  nLevel=GetItemLevel2(0,4,0)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,0,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==9)and(GetItemLevel2(0,4,1)>=1)then
				local  nLevel=GetItemLevel2(0,4,1)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,1,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==10)and(GetItemLevel2(0,4,2)>=1)then
				local  nLevel=GetItemLevel2(0,4,2)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,2,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==11)and(GetItemLevel2(0,4,3)>=1)then
				local   nLevel=GetItemLevel2(0,4,3)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,3,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==12)and(GetItemLevel2(0,4,4)>=1)then
				local   nLevel=GetItemLevel2(0,4,4)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,4,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==13)and(GetItemLevel2(0,4,5)>=1)then
				local   nLevel=GetItemLevel2(0,4,5)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,5,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==14)and(HaveNormalItem(3,22,0,0)>=5)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,5 do
								DelNormalItem(3,22,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==15)and(HaveNormalItem(3,23,0,0)>=5)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,5 do
								DelNormalItem(3,23,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==16)and(HaveNormalItem(3,24,0,0)>=5)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,5 do
								DelNormalItem(3,24,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==17)and(HaveNormalItem(3,25,0,0)>=5)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						for i=1,5 do
								DelNormalItem(3,25,0,0)
						end;	
				end;
				TaskNote(32,42)
		elseif(renwu==18)and(GetItemLevel2(0,4,6)>=1)then
				local   nLevel=GetItemLevel2(0,4,6)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,6,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==19)and(GetItemLevel2(0,4,7)>=1)then
				local   nLevel=GetItemLevel2(0,4,7)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,7,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==20)and(GetItemLevel2(0,4,8)>=1)then
				local   nLevel=GetItemLevel2(0,4,8)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,8,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==21)and(GetItemLevel2(0,4,9)>=1)then
				local   nLevel=GetItemLevel2(0,4,9)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,9,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==22)and(GetItemLevel2(0,4,18)>=1)then
				local   nLevel=GetItemLevel2(0,4,18)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,18,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==23)and(GetItemLevel2(0,4,19)>=1)then
				local   nLevel=GetItemLevel2(0,4,19)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,19,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==24)and(GetItemLevel2(0,4,20)>=1)then
				local   nLevel=GetItemLevel2(0,4,20)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,20,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==25)and(GetItemLevel2(0,4,21)>=1)then
				local   nLevel=GetItemLevel2(0,4,21)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,21,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==26)and(GetItemLevel2(0,4,10)>=1)then
				local   nLevel=GetItemLevel2(0,4,10)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,10,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==27)and(GetItemLevel2(0,4,11)>=1)then
				local   nLevel=GetItemLevel2(0,4,11)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,11,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==28)and(GetItemLevel2(0,4,12)>=1)then
				local   nLevel=GetItemLevel2(0,4,12)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,12,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==29)and(GetItemLevel2(0,4,13)>=1)then
				local   nLevel=GetItemLevel2(0,4,13)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,13,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==30)and(GetItemLevel2(0,4,22)>=1)then
				local   nLevel=GetItemLevel2(0,4,22)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,22,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==31)and(GetItemLevel2(0,4,23)>=1)then
				local   nLevel=GetItemLevel2(0,4,23)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,23,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==32)and(GetItemLevel2(0,4,24)>=1)then
				local   nLevel=GetItemLevel2(0,4,24)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,24,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==33)and(GetItemLevel2(0,4,25)>=1)then
				local   nLevel=GetItemLevel2(0,4,25)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,25,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==34)and(GetItemLevel2(0,4,26)>=1)then
				local   nLevel=GetItemLevel2(0,4,26)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,26,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==35)and(GetItemLevel2(0,4,27)>=1)then
				local   nLevel=GetItemLevel2(0,4,27)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,27,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==36)and(GetItemLevel2(0,4,28)>=1)then
				local   nLevel=GetItemLevel2(0,4,28)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,28,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==37)and(GetItemLevel2(0,4,29)>=1)then
				local   nLevel=GetItemLevel2(0,4,29)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,29,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==38)and(HaveEventItem(39)==1)then
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelEventItem(39)
				end;
				TaskNote(32,42)
		elseif(renwu==39)and(GetItemLevel2(0,4,14)>=1)then
				local   nLevel=GetItemLevel2(0,4,14)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,14,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==40)and(GetItemLevel2(0,4,15)>=1)then
				local   nLevel=GetItemLevel2(0,4,15)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,15,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==41)and(GetItemLevel2(0,4,16)>=1)then
				local   nLevel=GetItemLevel2(0,4,16)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,16,nLevel)
				end;
				TaskNote(32,42)
		elseif(renwu==42)and(GetItemLevel2(0,4,17)>=1)then
				local   nLevel=GetItemLevel2(0,4,17)
				if(jiangli(renwu,thistime,cishu)~=0)then
						DelItem2(0,4,17,nLevel)
				end;
				TaskNote(32,42)
		else
				Talk(1,"no",11321)
		end;	
end;


function   check_1()	
		local  renwu=GetTask(310)
		local  thistime=GetTask(311)
		local  cishu=GetTask(313)
		local thisday=floor(SystemTime()/86400)
		if(cishu>=10)then
				if(thisday>thistime)then
						SetTask(310,0)
						SetTask(311,thisday)
						SetTask(312,0)
						SetTask(313,0)
						Talk(1,"no",11274)
				else
						SetTask(310,0)
						SetTask(311,thisday+1)
						SetTask(312,0)
						SetTask(313,0)
						Talk(1,"no",11275)
				end;
		else

			if(GetLevel()>=50)then
								local  i=random(1,1100)
								if(i<=80)then
										Talk(1,"no",11276)
										SetTask(310,1)
										TaskNote(32,0)
								elseif(i<=160)then
										Talk(1,"no",11277)
										SetTask(310,2)
										TaskNote(32,1)
								elseif(i<=240)then
										Talk(1,"no",11278)
										SetTask(310,3)
										TaskNote(32,2)
								elseif(i<=320)then
										Talk(1,"no",11279)
										SetTask(310,4)
										TaskNote(32,3)
								elseif(i<=400)then
										Talk(1,"no",11280)
										SetTask(310,5)
										TaskNote(32,4)
								elseif(i<=480)then
										Talk(1,"no",11281)
										SetTask(310,6)
										TaskNote(32,5)
								elseif(i<=560)then
										Talk(1,"no",11282)
										SetTask(310,7)
										TaskNote(32,6)
								elseif(i<=570)then
										Talk(1,"no",11283)
										SetTask(310,8)
										TaskNote(32,7)
								elseif(i<=580)then
										Talk(1,"no",11284)
										SetTask(310,9)
										TaskNote(32,8)
								elseif(i<=590)then
										Talk(1,"no",11285)
										SetTask(310,10)
										TaskNote(32,9)
								elseif(i<=600)then
										Talk(1,"no",11286)
										SetTask(310,11)
										TaskNote(32,10)
								elseif(i<=610)then
										Talk(1,"no",11287)
										SetTask(310,12)
										TaskNote(32,11)
								elseif(i<=620)then
										Talk(1,"no",11288)
										SetTask(310,13)
										TaskNote(32,12)
								elseif(i<=720)then
										Talk(1,"no",11289)
										SetTask(310,14)
										TaskNote(32,13)
								elseif(i<=820)then
										Talk(1,"no",11290)
										SetTask(310,15)
										TaskNote(32,14)
								elseif(i<=920)then
										Talk(1,"no",11291)
										SetTask(310,16)
										TaskNote(32,15)
								elseif(i<=1020)then
										Talk(1,"no",11292)
										SetTask(310,17)
										TaskNote(32,16)
								elseif(i<=1030)then
										Talk(1,"no",11293)
										SetTask(310,18)
										TaskNote(32,17)
								elseif(i<=1040)then
										Talk(1,"no",11294)
										SetTask(310,19)
										TaskNote(32,18)
								elseif(i<=1050)then
										Talk(1,"no",11295)
										SetTask(310,20)
										TaskNote(32,19)
								elseif(i<=1060)then
										Talk(1,"no",11296)
										SetTask(310,21)
										TaskNote(32,20)
								elseif(i<=1100)then
										Talk(1,"no",11313)
										SetTask(310,38)
										TaskNote(32,37)
								end;


			end;
			if(thisday-1>=thistime)then
					SetTask(313,1)
					SetTask(312,0)
					SetTask(311,thisday)
					Msg2Player("NhiÖm vô thu thËp ®¹o cô h«m qua ®· hÕt hiÖu lùc, h«m nay ph¶i nhËn l¹i nhiÖm vô míi.")
			else
					SetTask(313,cishu+1)
					Msg2Player("B¹n nhËn ®­îc nhiÖm vô thu thËp ®¹o cô míi.")
			end;

		end;
end;


function   esc()
		MsgBox(11373,"quxiao","no")
end;

function   quxiao()
		local  cishu=GetTask(313)
		local  thistime=GetTask(311)
		local  shengyu=10-cishu
		Talk(1,"no","Ng­¬i ®· hñy bá nhiÖm vô lÇn nµy, h«m nay chØ cßn l¹i <color=red>"..shengyu.."<color> lÇn")
		SetTask(312,1)
		SetTask(310,0)
end;

function   jiangli(renwu,thistime,cishu)
		local  cishu=GetTask(313)
		local  shengyu=10-cishu
		local thisday=floor(SystemTime()/86400)
		local  money=floor(GetLevel()/20-2.5)*5000+5000
					if(GetTask(313)>=10)and(GetTask(312)<=0)then
										Talk(1,"no",11319)					
										AddOwnExp(GetLevel()*1000)									
										SetTask(311,thistime+1)
										SetTask(313,0)
										SetTask(310,0)
										TopMessage("B¹n nhËn ®­îc <color=green>"..(GetLevel()*1000).."<color> kinh nghiÖm")
										if(thisday-1>=thistime)then
													SetTask(313,0)
													SetTask(311,thisday)
													SetTask(312,0)
													Msg2Player("NhiÖm vô thu thËp ®¹o cô h«m qua ®· hÕt hiÖu lùc, h«m nay ph¶i nhËn l¹i nhiÖm vô míi.")
										else
													Msg2Player("Ng­¬i ®· hoµn thµnh nhiÖm vô lÇn nµy, cã thÓ nhËn nhiÖm vô tiÕp theo.")
										end;
					else
							if(GetCash()>=money)then
									Talk(1,"no","Lµm tèt l¾m! H«m nay ng­¬i cßn cã thÓ nhËn <color=red>"..shengyu.."<color> lÇn")
									AddOwnExp(GetLevel()*200)
									Pay(money)
									SetTask(310,0)
									TopMessage("B¹n nhËn ®­îc <color=green>"..(GetLevel()*200).."<color> kinh nghiÖm")
							else
									Talk(1,"no",11320)
									return  0
							end;
					end;
end;

function  no()
	CloseDialog()
end;

Include("\\script\\phongthan\\daily\\reward.lua")

pt_daily_original_tools=jiangli
function jiangli(r,t,c)
 if GetTask(310)<=0 then return 0 end
 local before=GetTask(310)
 local result=pt_daily_original_tools(r,t,c)
 if before>0 and GetTask(310)==0 then PTDailyHope(4,PTDailyNextReceipt(4)) end
 return result
end
