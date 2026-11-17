-- Original VNG source payload; provenance in deployment report.
--description: ÎäÍõ-Ö÷Ïß?Îñ
--author: yichuan
--date:2004/5/8

set_name = 
{
	{"Vò Khóc ","Tinh Cang "," Khai Thiªn ","ChÊn §an "},
	{"XÝch Tïng ","Th¸i Êt ","Th«ng Thiªn ","Hång Qu©n "},
	{"B¸o ThÇn ","Gi¸c thó ","Lam §iªu ","Kh¸ng Long "}
}
part_name =
{
	{" Gi¸p ","ChiÕn Ngoa ","Yªu §¸i ","Kh«i ","Phi Phong "},
	{"§¹o Bµo ","Lý ","C©n ","Qu¸n ","LÖnh "},
	{" Hé Gi¸p ","Ngoa ","Yªu §¸i ","Trô ","KÕt "}
}
task_lvl_2_sel_lvl={[3]=5,[9]=7}
task_lvl_2_sel_idx={[3]=2,[9]=3}


function main()
	tasks = 
	{
		{"Tam s¸ch","renwu1";show=0},
		{"PhÇn th­ëng ","renwu";show=0},
		{"VÞ quèc lËp c«ng","renwu2";show=0},
		{"§¼ng cÊp","renwu3";show=1},
		{"Di b¸o","dongyi";show=0},
		{"§Æng Cöu C«ng hµng Chu","dongyi1";show=0}
	}
	UTask_Wizard = GetTask(1)
	UTask_Knight = GetTask(3)
	UTask_Druid = GetTask(2)
	local UTask_num = GetTask(42);--42ºÅ±äÁ¿ÓÃÀ´¼ÇÂ¼65¼¶Ö÷Ïß?ÎñÏÂ£¬ÊÇ·ñ´òÍê3¸ö?Îñ¹Ö
	if(UTask_Knight ==62)  or  (UTask_Druid ==62) or  (UTask_Wizard ==62 )then
			if(HaveEventItem(4)>=1)then
					tasks[1].show=1;
			end;
	end;
	if(UTask_Knight ==63) or  (UTask_Druid ==63) or  (UTask_Wizard ==63 )then 
				if(HaveEventItem(6)>=1)  and  (HaveEventItem(7)>=1)  and  (HaveEventItem(8)>=1) and (UTask_num==7) then	 
							tasks[1].show=1;
				end;
	end;
	if(GetLevel()>=35) and (GetTask(330)==2)and (SystemTime()<1111917600)then
				tasks[2].show=1;
	end;
	if(1==GetTask(421))or(2==GetTask(421))then
		if (20>GetTask(420))then
			tasks[3].show=1;
		end;
	end;
	if(25==GetTask(597))and(HaveEventItem(110)>=1)and(GetTask(592)~=1)then
		tasks[5].show=1;
	end;
	if(27==GetTask(597))then
		tasks[6].show=1;
	end;
		SayTask(10484,tasks)
end;

function   renwu1()
	UTask_Wizard = GetTask(1);
	UTask_Knight = GetTask(3);
	UTask_Druid = GetTask(2);
	local UTask_num = GetTask(42);--42ºÅ±äÁ¿ÓÃÀ´¼ÇÂ¼65¼¶Ö÷Ïß?ÎñÏÂ£¬ÊÇ·ñ´òÍê3¸ö?Îñ¹Ö
	if(UTask_Knight ==62)  or  (UTask_Druid ==62) or  (UTask_Wizard ==62 )then
					Talk(2,"func_sun",10485,10486)
	end;

	if(UTask_Knight ==63) or  (UTask_Druid ==63) or  (UTask_Wizard ==63 )then 
					if(HaveEventItem(6)>=1)  and  (HaveEventItem(7)>=1)  and  (HaveEventItem(8)>=1) and (UTask_num==7) then	 
								Talk(1,"no",10487)
								DelEventItem(6)
								DelEventItem(7)
								DelEventItem(8)
								local  i=random(14,17)
								AddNormalItem(0,4,i,1,0,0)
								Msg2Player(" NhËn ®­îc ph¸p b¶o cÊp 70.")
								if (GetPlayerType()==1)then
										SetTask(1,70)
										TaskNote(28,30)
								end;
								if(GetPlayerType()==0)then
										SetTask(3,70)
										TaskNote(27,26)
								end;
								if(GetPlayerType()==2)then
										SetTask(2,70)
										TaskNote(29,25)
								end;
							
					end;
	end;
end;

function  func_sun()
		Talk(2,"func_sun1",10488,10623)
end;

function  func_sun1()
		Talk(2,"func_sun2",10489,10490)
end;

function  func_sun2()
		Talk(1,"no",10491)
		Msg2Player("NhËn sù ñy th¸c cña Vâ V­¬ng, ®i t×m 3 bé s¸ch HuyÒn N÷ Binh Ph¸p, Huúnh §Õ Néi Kinh, LuyÖn Kim thuËt ")
		if (GetPlayerType()==1)then
				SetTask(1,63)
				TaskNote(28,29)
		end;
		if(GetPlayerType()==0)then
				SetTask(3,63)
				TaskNote(27,25)
		end;
		if(GetPlayerType()==2)then
				SetTask(2,63)
				TaskNote(29,24)
		end;
		DelEventItem(4)
end;

function  no()
		CloseDialog()
end;

function  renwu()	
		if(GetTask(330)==2)then
			local i=random(1,6)
					if (i==1) then
						AddNormalItem(3,41,0,0,1,0)
						Msg2Player("B¹n nhËn ®­îc 1 viªn Lam B¶o Th¹ch")
						AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: 1 Lam B¶o Th¹ch.",20)
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng lµ  <color=red>1 Lam B¶o Th¹ch<color>")			
					elseif(i==2)then
						if(GetSeries()==0)then
							AddNormalItem2(0,10,0,7,1,0)
							Msg2Player("B¹n nhËn ®­îc U Hån Lang")
							AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, nhËn ®­îc U Hån Lang.",20)
						elseif(GetSeries()==1)then
							AddNormalItem2(0,10,1,7,1,0)
							Msg2Player("B¹n nhËn ®­îc Phiªu TuyÕt H¹c")
							AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: Phiªu TuyÕt H¹c",20)
						elseif(GetSeries()==2)then
							AddNormalItem2(0,10,2,7,1,0)
							Msg2Player("B¹n nhËn ®­îc TrÇm H­¬ng hå ®iÖp.")
							AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: TrÇm H­¬ng hå ®iÖp.",20)
						end;
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ ph¹t Trô Tiªn Phong T­íng Qu©n vµ phÇn th­ëng lµ mét <color=red>con ngùa quý<color>")			
					elseif(i==3)then
						if(GetSeries()==0)then
							local j=random(1,5)
								if (j==1) then
									AddNormalItem2(0,2,3,4,1,0)
									Msg2Player("B¹n nhËn ®­îc Vò Khóc Gi¸p")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: Vò Khóc Gi¸p",20)
								elseif (j==2) then
									AddNormalItem2(0,5,3,4,1,0)
									Msg2Player("B¹n nhËn ®­îc Vò Khóc chiÕn ngoa ")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng:  Vò Khóc chiÕn ngoa.",20)
								elseif (j==3) then
									AddNormalItem2(0,6,3,4,1,0)
									Msg2Player("B¹n nhËn ®­îc Vò Khóc Yªu §¸i ")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: Vò Khóc Yªu §¸i ",20)
								elseif (j==4) then
									AddNormalItem2(0,7,3,4,1,0)
									Msg2Player("B¹n nhËn ®­îc Vò Khóc Kh«i ")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: Vò Khóc Kh«i.",20)
								elseif (j==5) then
									AddNormalItem2(0,9,3,4,1,0)
									Msg2Player("B¹n nhËn ®­îc Vò Khóc Phi Phong")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: Vò Khóc Phi Phong.",20)
								end;
						elseif(GetSeries()==1)then
							local k=random(1,5)
								if (k==1) then
									AddNormalItem2(0,2,4,4,1,0)
									Msg2Player("B¹n nhËn ®­îc XÝch Tïng §¹o Bµo.")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: XÝch Tïng §¹o Bµo",20)
								elseif (k==2) then
									AddNormalItem2(0,5,4,4,1,0)
									Msg2Player("B¹n nhËn ®­îc XÝch Tïng Lý")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: XÝch Tïng Lý",20)
								elseif (k==3) then
									AddNormalItem2(0,6,4,4,1,0)
									Msg2Player("B¹n nhËn ®­îc XÝch Tïng C©n")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: XÝch Tïng C©n",20)
								elseif (k==4) then
									AddNormalItem2(0,7,4,4,1,0)
									Msg2Player("B¹n nhËn ®­îc XÝch Tïng Qu¸n")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: XÝch Tïng Qu¸n",20)
								elseif (k==5) then
									AddNormalItem2(0,9,4,4,1,0)
									Msg2Player("B¹n nhËn ®­îc XÝch Tïng LÖnh")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: XÝch Tïng LÖnh",20)
								end;					
						elseif(GetSeries()==2)then
							local l=random(1,5)
								if (l==1) then
									AddNormalItem2(0,2,5,4,1,0)
									Msg2Player("B¹n nhËn ®­îc B¸o ThÇn Hé Gi¸p")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: B¸o ThÇn Hé Gi¸p",20)
								elseif (l==2) then
									AddNormalItem2(0,5,5,4,1,0)
									Msg2Player("B¹n nhËn ®­îc B¸o ThÇn ngoa")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: B¸o ThÇn ngoa.",20)
								elseif (l==3) then
									AddNormalItem2(0,6,5,4,1,0)
									Msg2Player("B¹n nhËn ®­îc B¸o ThÇn Yªu §¸i")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: B¸o ThÇn Yªu §¸i",20)
								elseif (l==4) then
									AddNormalItem2(0,7,5,4,1,0)
									Msg2Player("B¹n nhËn ®­îc B¸o ThÇn Trô")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: B¸o ThÇn Trô",20)
								elseif (l==5) then
									AddNormalItem2(0,9,5,4,1,0)
									Msg2Player("B¹n nhËn ®­îc B¸o ThÇn KÕt")
									AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: B¸o ThÇn KÕt",20)
								end;					
						end;
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ ph¹t Trô Tiªn Phong T­íng Qu©n vµ phÇn th­ëng lµ mét sè <color=red>trang bÞ quý hiÕm<color>")			
					elseif(i==4)then
						Earn(100000)
						Msg2Player("B¹n nhËn ®­îc 10w")
						AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: 10w",20)
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ ph¹t Trô Tiªn Phong T­íng Qu©n, ®ång thêi th­ëng cho ng­¬i <color=red>10w<color>")			
					elseif(i==5)then
						AddOwnExp(200000)
						Msg2Player("B¹n ®­îc th¨ng lªn mét cÊp.")
						AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: th¨ng lªn mét cÊp.",20)
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng <color=red> th¨ng lªn mét cÊp<color>")			
					elseif(i==6)then
						UseSilver(1,1,1)
						Msg2Player("B¹n nhËn ®­îc mét ngµy ch¬i miÔn phÝ")
						AddGlobalCountNews("Chóc mõng <color=green>"..GetName().."<color> ®­îc Vâ V­¬ng phong lµm Ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng: 1 ngµy ch¬i miÔn phÝ.",20)
						Talk(1,"no","Kh«ng hæ danh lµ dòng sÜ. LÇn nµy ta phong ng­¬i lµ ph¹t Trô Tiªn Phong T­íng Qu©n, phÇn th­ëng <color=red>1 ngµy ch¬i miÔn phÝ<color>")			
					end;
			SetTask(330,3)
		else
				Talk(1,"no","Ng­¬i ®· lµ Tiªn Phong T­íng Qu©n, hy väng sau nµy sÏ lËp nhiÒu chiÕn c«ng hiÓn h¸ch")
		end;
end;

function renwu2()
	if(1==GetTask(421))or(2==GetTask(421))then--421=1Ê±Íê³É?Îñ£¬=2Ê±Íê³É?ÎñÇÒ±¾³¡¾·Àû
		if (3==GetTask(420))or(9==GetTask(420))and(GetTask(373)==1)then
			reward_add()
		else
			Talk(1,"no","LÇn nµy ng­¬i lËp nhiÒu c«ng tr¹ng, h·y nhËn lÊy phÇn th­ëng.")
			reward_normal()
		end;
	else
		CloseDialog()
	end;
end;

function renwu3()
	local sz_level
	if (GetTask(420)>=20)then
		sz_level=GetTask(420)-20
	else
		sz_level=GetTask(420)
	end;
	Talk(1,"no","§¼ng cÊp chiÕn tr­êng hiÖn t¹i cña ng­¬i lµ <color=green>"..sz_level.."<color>.")
end;

function reward_normal()
	local sz_level=GetTask(420)
	if (10==sz_level)then
		AddOwnExp(GetLevel()*(1000+GetTask(421)*1000)*2);--½±Àø£¬°üÀ¨¾·Àû½±Àø
	else
		AddOwnExp(GetLevel()*(sz_level*100+GetTask(421)*1000)*2);--½±Àø£¬°üÀ¨¾·Àû½±Àø
		if (4==sz_level)and(60>GetLevel()) then
			Msg2Player("§¼ng cÊp cña b¹n ch­a ®Õn 60, kh«ng thÓ vµo chiÕn tr­êng Th­¬ng Chu.")
		else
			if (1==GetTask(373)) then--¾­ÑéÖµÒÑÂú£¬µÈ¼¶ÌáÉý
				SetTask(420,sz_level+1)
				SetTask(373,0)
				Msg2Player("§¼ng cÊp chiÕn tr­êng cña ng­¬i t¨ng "..GetTask(420))
			elseif (GetTask(373)<1)and(GetTask(373)>=0) then--¾­ÑéÖµÎ´Âú£¬¾­ÑéÖµ+1
				SetTask(373,GetTask(373)+1)
			else--¾­ÑéÖµÒç³ö»ò?µÃ´íÎóÖµ£¬Ö±½ÓÂ÷Îª1
				SetTask(373,1)
			end;
		end;
	end;
	SetTask(421,0)
end;

function reward_add()
	local sz_level=GetTask(420)
	local sel_idx=task_lvl_2_sel_idx[sz_level]
	if(sel_idx~=nil)then
		local sel_type=GetPlayerType()+1
		local item_list={}
               if(sz_level==9)and(1==GetTask(373))then
                   for i=2,4 do
                     item_list[i-1]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
		    end;
                   Say("Ng­¬i thËt vÊt v¶! Xin  nhËn phÇn th­ëng!",3,item_list)  
                elseif(1==GetTask(373))then
		   for i=1,5 do
			item_list[i]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
		   end;
                   Say("Ng­¬i thËt vÊt v¶! Xin  nhËn phÇn th­ëng!",5,item_list)
               end;
	end;
end;

function item_1()
	CloseDialog()
	local sz_level=GetTask(420)
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green>Vâ V­¬ng<color> tÆng b¹n 1 <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][1].."<color>.",20)
		AddNormalItem(0,2,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end;
end;

function item_2()
	CloseDialog()
	local sz_level=GetTask(420)
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green>Vâ V­¬ng<color> tÆng b¹n 1 <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][2].."<color>.",20)
		AddNormalItem(0,5,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end;

end;

function item_3()
	CloseDialog()
	local sz_level=GetTask(420)
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green>Vâ V­¬ng<color> tÆng b¹n 1 <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][3].."<color>.",20)
		AddNormalItem(0,6,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end;
end;
function item_4()
	CloseDialog()
	local sz_level=GetTask(420)
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green>Vâ V­¬ng<color> tÆng b¹n 1 <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][4].."<color>.",20)
		AddNormalItem(0,7,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end;
end;
function item_5()
	CloseDialog()
	local sz_level=GetTask(420)
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green>Vâ V­¬ng<color> tÆng b¹n 1 <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][5].."<color>.",20)
		AddNormalItem(0,9,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end;
end;

function dongyi()
	if(HaveEventItem(110)>=1)then
		if(GetTask(593)==0)then
			Talk(1,"no","Thñ lÜnh §«ng Di träng t×nh träng nghÜa, ng­êi nh­ vËy thËt lµ hiÕm cã! Ta ph¶i cho ng­êi ®Õn hç trî «ng Êy.")
			SetTask(592,1)
			AddCredit(25)--ÉùÍû½±Àø
			AddOwnExp(4000) --¾­Ñé½±Àø
			Msg2Player("NhËn ®­îc 4000 ®iÓm kinh nghiÖm vµ 25 ®iÓm danh väng!")
			TopMessage("PhÇn th­ëng: <color=green>4000 ®iÓm kinh nghiÖm<color> vµ <color=green>25 ®iÓm danh väng<color>")
		elseif(GetTask(593)==1)then
			DelEventItem(110)
			SetTask(592,1)
			Talk(1,"no","Thñ lÜnh §«ng Di träng t×nh träng nghÜa, ng­êi nh­ vËy thËt lµ hiÕm cã! Ta ph¶i cho ng­êi ®Õn hç trî «ng Êy.")
			SetTask(597,26)
			TaskNote(35,33)
			AddCredit(25)--ÉùÍû½±Àø
			AddOwnExp(4000) --¾­Ñé½±Àø
			Msg2Player("NhËn ®­îc 4000 ®iÓm kinh nghiÖm vµ 25 ®iÓm danh väng!")
			TopMessage("PhÇn th­ëng: <color=green>4000 ®iÓm kinh nghiÖm<color> vµ <color=green>25 ®iÓm danh väng<color>")
			Msg2Player("Phôc mÖnh §Æng Cöu C«ng")
		end;
	end;
end;

function dongyi1()
	Talk(1,"no","Cöu C«ng ®øc cao väng träng, qu¶ nh©n sÏ ph¸i ng­êi ®Õn ®ã ®Ó thiÕt lËp bang giao. Ng­¬i h·y mau ®Õn b¸i kiÕn <color=green>Kh­¬ng thõa t­íng<color>!")
	SetTask(597,28)
	TaskNote(35,35)
	AddCredit(30)--ÉùÍû½±Àø
	AddOwnExp(4000) --¾­Ñé½±Àø
	Msg2Player("NhËn ®­îc 4000 ®iÓm kinh nghiÖm vµ 30 ®iÓm danh väng")
	TopMessage("PhÇn th­ëng: <color=green>4000 ®iÓm kinh nghiÖm<color> vµ <color=green>30 ®iÓm danh väng!<color>")
	Msg2Player("§èi tho¹i víi Kh­¬ng Tö Nha")
end;






pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1020_02; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1020 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("Chñ so¸i T©y Kú, nh©n vËt cña chÝnh tuyÕn. B¶n nµy lµ NPC ®èi tho¹i, kh«ng ph¶i Vâ V­¬ng chiÕn ®Êu trong phô b¶n.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Kh­¬ng Tö Nha (158/189); L«i ChÊn Tö (163/187); D­¬ng TiÔn (165/185); NhËm §¹i Ca (170/195); NhËm §¹i TÈu (170/195)", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("Cã nh¸nh Lua VNG gèc trong PAK. C¸c ®iÒu kiÖn vµ giao dÞch do nh¸nh gèc kiÓm tra; ch­a nghiÖm thu toµn bé nhiÖm vô.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_close()
    CloseDialog()
end
function pt_original()
    if pt_guard() == 0 then return end
    pt_original_main()
end

-- PT_GIAPSI_20261024_BEGIN
PTG_STEPS={{role="lo",title="T©n Thøc",level=3,task=21,from=0,to=1,note="GÆp ¢u Thiªn Hãa t¹i Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=1,to=2,note="GÆp Sïng øng B­u.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="buu",title="T©n Thøc",level=3,task=21,from=2,to=3,note="Thu thËp 10 §o¶n KiÕm råi gÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=3,to=4,note="GÆp Sïng HÇu Hæ.",needs={{3,10,0,0,0,0,10}},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hau",title="T©n Thøc",level=3,task=21,from=4,to=5,note="Trë l¹i gÆp Lç Hïng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=5,to=6,note="GÆp Sïng H¾c Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="hac",title="T©n Thøc",level=3,task=21,from=6,to=7,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="au",title="T©n Thøc",level=3,task=21,from=7,to=8,note="Trë l¹i gÆp Lç Hïng nhËn th­ëng.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{role="lo",title="T©n Thøc",level=3,task=21,from=8,to=9,note="Hoµn thµnh T©n Thøc.",needs={},gifts={},xp=500,money=600,credit=0,label="T©n Thøc"},{role="loan",title="Dòng §ao",level=7,task=25,from=0,to=1,note="DiÖt Hoµn CÈu V­¬ng t¹i YÕn S¬n (235,208), lÊy ®ñ ba bé phËn ®ao.",needs={},gifts={},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=1,to=2,note="Nhê ¢u Thiªn Hãa söa Dòng §ao.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,keepNeeds=1,label="Dòng §ao"},{role="au",title="Dòng §ao",level=7,task=25,from=2,to=3,note="Mang Dòng §ao vÒ TriÒu §iÒn.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},gifts={{4,24,0,0,0,0,1}},xp=0,money=0,credit=0,label="Dòng §ao"},{role="dien",title="Dòng §ao",level=7,task=25,from=3,to=4,note="Hoµn thµnh Dòng §ao.",needs={{4,24,0,0,0,0,1}},gifts={{0,4,0,1,0,0,1}},xp=0,money=0,credit=10,label="Dòng §ao",rewardText="Ph¸p b¶o cÊp 10; 10 ®iÓm danh väng."},{role="loan",title="KiÕm ƒi",level=14,task=24,from=0,to=1,note="GÆp ¢u Thiªn Hãa.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="au",title="KiÕm ƒi",level=14,task=24,from=1,to=2,note="DiÖt Hoµn CÈu Tinh t¹i YÕn S¬n (213,187), (202,193), (213,194); vÒ Sïng øng Loan.",needs={},gifts={},xp=0,money=0,credit=0,label="KiÕm ƒi"},{role="loan",title="KiÕm ƒi",level=14,task=24,from=3,to=4,note="Hoµn thµnh KiÕm ƒi.",needs={{4,25,0,0,0,0,1}},gifts={{5,0,0,0,0,0,5},{7,58,62,1,0,0,1}},xp=3000,money=0,credit=0,camp=7,label="KiÕm ƒi",rewardText="5 Håi Thµnh Phï; s¸ch Bµn Cæ Khai Thiªn."},{role="hau",title="Trung Thµnh",level=25,task=3,from=0,to=1,note="GÆp TrÞnh Lu©n ë Sïng Thµnh Doanh.",needs={},gifts={},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="trinh",title="Trung Thµnh",level=25,task=3,from=1,to=2,note="Mang HuyÕt Th­ vÒ Sïng HÇu Hæ.",needs={},gifts={{4,11,0,0,0,0,1}},xp=0,money=0,credit=0,label="Trung Thµnh"},{role="hau",title="Trung Thµnh",level=25,task=3,from=2,to=10,note="Hoµn thµnh Trung Thµnh.",needs={{4,11,0,0,0,0,1}},gifts={},xp=20000,money=30000,credit=0,label="Trung Thµnh"},{role="hac",title="TrÇm H­¬ng",level=35,task=3,from=10,to=11,note="Mang TrÇm H­¬ng Méc tíi Hoµng Phi Hæ ë TriÒu Ca.",needs={},gifts={{4,45,0,0,0,0,1}},xp=0,money=0,credit=0,label="TrÇm H­¬ng"},{role="hoang",title="TrÇm H­¬ng",level=35,task=3,from=11,to=20,note="Hoµn thµnh TrÇm H­¬ng.",needs={{4,45,0,0,0,0,1}},gifts={{7,59,128,1,0,0,1}},xp=80000,money=0,credit=0,label="TrÇm H­¬ng",rewardText="MËt tÞch Ban M«n Léng Phñ."},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=20,to=21,note="Hái chuyÖn Lý TÞnh vµ §Æng Cöu C«ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=21,to=22,note="GÆp §Æng Cöu C«ng t¹i Tam S¬n.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=21,to=23,note="GÆp Lý TÞnh t¹i TrÇn §­êng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="dang",title="Khuyªn Hµng",level=45,task=3,from=22,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="ly",title="Khuyªn Hµng",level=45,task=3,from=23,to=24,note="Trë vÒ Hoµng Phi Hæ.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=24,to=25,note="DiÖt TrÊn T­íng Qu©n t¹i Môc D· (187,189), lÊy Ên tÝn Vâ Thµnh V­¬ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="hoang",title="Khuyªn Hµng",level=45,task=3,from=26,to=27,note="B¸o tin cho Kh­¬ng Tö Nha t¹i T©y Kú.",needs={{4,12,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Khuyªn Hµng"},{role="khuong",title="Khuyªn Hµng",level=45,task=3,from=27,to=30,note="Hoµn thµnh Khuyªn Hµng.",needs={},gifts={{0,0,31,4,0,0,1}},xp=240000,money=0,credit=0,label="Khuyªn Hµng",rewardText="Vò khÝ hoµng kim cÊp 40."},{role="duong",title="ThÇn Long",level=55,task=3,from=30,to=31,note="DiÖt ThÇn Long t¹i Long Cung tÇng 3 (206,175), lÊy R©u ThÇn Long.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Long"},{role="dac",title="ThÇn Long",level=55,task=3,from=32,to=33,note="Mang R©u ThÇn Long cho Hå Hû MÞ.",needs={{4,13,0,0,0,0,1}},gifts={{4,9,0,0,0,0,1}},xp=0,money=0,credit=0,keepNeeds=1,label="ThÇn Long"},{role="ho",title="ThÇn Long",level=55,task=3,from=33,to=40,note="Hoµn thµnh ThÇn Long.",needs={{4,13,0,0,0,0,1}},gifts={{3,46,0,0,0,0,1}},xp=600000,money=0,credit=0,label="ThÇn Long",rewardText="B¸ L¹c Nh·n cÊp 10."},{role="dac",title="B×nh An",level=65,task=3,from=40,to=41,note="GÆp T©y V­¬ng MÉu t¹i Diªu Tr×.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=41,to=42,note="DiÖt Cù D· t¹i BÝch Du Cung tÇng 4: (181,199), (196,204), (185,217); lÊy An C­ §å.",needs={},gifts={},xp=0,money=0,credit=0,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=3,from=43,to=50,note="Hoµn thµnh B×nh An.",needs={{4,3,0,0,0,0,1}},gifts={},xp=1400000,money=0,credit=0,randomBook=1,label="B×nh An",rewardText="S¸ch kü n¨ng ngÉu nhiªn cña ba hÖ ph¸i."},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=50,to=51,note="DiÖt Tr¹nh Nanh t¹i Khæn Tiªn Cung tÇng 1 (197,212), lÊy ThÇn Dô KÝnh.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Dô KÝnh"},{role="nong",title="ThÇn Dô KÝnh",level=70,task=3,from=52,to=60,note="Hoµn thµnh ThÇn Dô KÝnh. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",needs={{4,4,0,0,0,0,1}},gifts={{0,0,31,7,0,0,1}},xp=2000000,money=0,credit=0,keepNeeds=1,label="ThÇn Dô KÝnh",rewardText="Vò khÝ hoµng kim cÊp 70."},{role="dac",title="Tam S¸ch",level=75,task=3,from=60,to=61,note="Mang hai b¶ng Phong ThÇn tíi Kh­¬ng Tö Nha. Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={},gifts={},xp=0,money=0,credit=0,ensureBoard=1,label="Tam S¸ch"},{role="khuong",title="Tam S¸ch",level=75,task=3,from=61,to=62,note="Mang ThÇn Dô KÝnh tíi Vâ V­¬ng.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=62,to=63,note="LÊy Huúnh §Õ Néi Kinh ë tÇng 2 (196,189); LuyÖn Kim ThuËt ë tÇng 3 (206,204); HuyÒn N÷ Binh Ph¸p ë tÇng 4 (191,210), Khæn Tiªn Cung.",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=3,from=63,to=70,note="Hoµn thµnh Tam S¸ch.",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},gifts={{0,4,14,1,0,0,1},{0,4,15,1,0,0,1},{0,4,16,1,0,0,1},{0,4,17,1,0,0,1}},xp=3000000,money=0,credit=0,label="Tam S¸ch",rewardText="Mét bé bèn ph¸p b¶o cÊp 70."},{role="tay",title="Cæ Kim",level=80,task=3,from=70,to=71,note="GÆp §¾c Kû t¹i TriÒu Ca m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1063,198,200},label="Cæ Kim"},{role="dac_future",title="Cæ Kim",level=80,task=3,from=71,to=72,note="GÆp Nguyªn Thñy Thiªn T«n t¹i Ngäc H­ Cung m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1062,203,198},label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=72,to=73,note="DiÖt Bé §Çu qu©n Chu t¹i Ngäc H­ Cung m­êi n¨m sau (206,204), lÊy Tróc Gi¶n.",needs={},gifts={},xp=0,money=0,credit=0,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=3,from=74,to=80,note="Hoµn thµnh Cæ Kim. Dïng Khø Lai Phï ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",needs={{4,10,0,0,0,0,1}},gifts={{3,41,0,0,0,0,1},{6,1,34,1,0,0,1}},xp=5000000,money=0,credit=0,label="Cæ Kim",rewardText="Lam B¶o Th¹ch; Khø Lai Phï."},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=80,to=81,note="Nãi chuyÖn víi Kh­¬ng Tö Nha thêi niªn thiÕu.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="khuong_young",title="Thiªn Duyªn",level=85,task=3,from=81,to=82,note="Trë l¹i gÆp §¾c Kû thêi niªn thiÕu.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=82,to=85,note="DiÖt Bµn Cæ t¹i Khæn Tiªn Cung tÇng 5 (199,212), lÊy Bµn Cæ ThÇn KhÝ.",needs={},gifts={},xp=0,money=0,credit=0,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=3,from=85,to=83,note="Hoµn thµnh Thiªn Duyªn.",needs={{4,175,0,0,0,0,1}},gifts={{0,5,6,8,4,0,1}},xp=6500000,money=0,credit=0,label="Thiªn Duyªn",rewardText="Mét trang bÞ lôc cÊp 80."},{role="khuong_young",title="Th«ng Thiªn",level=92,task=3,from=83,to=84,note="Tham Lam: Th­¬ng Nh©n, §¹i Phong (200,200). V« Tri: Cöu Di, Tam S¬n (200,200). Tµn NhÉn: dïng MËt §å t¹i B¾c H¶i (190,206).",needs={},gifts={{6,1,142,1,0,0,1},{6,1,181,1,0,0,1}},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="merchant",title="Th«ng Thiªn",level=92,task=4560,from=0,to=1,note="NhËn Tham Lam.",needs={},gifts={{4,176,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="cuudi",title="Th«ng Thiªn",level=92,task=4561,from=0,to=1,note="NhËn V« Tri.",needs={},gifts={{4,177,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,label="Th«ng Thiªn"},{role="thong",title="Th«ng Thiªn",level=92,task=3,from=84,to=86,note="Dïng Khø Lai Phï gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Th«ng Thiªn"},{role="khuong_young",title="Th«ng Thiªn: nhËn giµy",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,5,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=1,label="NhËn giµy",rewardText="Giµy lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn yªu ®¸i",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,6,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=2,label="NhËn yªu ®¸i",rewardText="Yªu ®¸i lôc cÊp 90."},{role="khuong_young",title="Th«ng Thiªn: nhËn m·o",level=92,task=3,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,7,6,9,5,0,1}},xp=0,money=0,credit=0,rewardChoice=3,label="NhËn m·o",rewardText="M·o lôc cÊp 90."},{role="doctor_manh",title="Trõ Häa: M¹nh T©n",level=18,task=4550,from=0,to=1,note="DiÖt Hoµn CÈu t¹i M¹nh T©n (188,210) ®Õn (192,210), råi trong 10 gi©y diÖt Gi¸p Cèt ngay phÝa nam ®Ó lÊy ®ñ 5 H¹p §»ng.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: tr¶ H¹p §»ng",level=18,task=4550,from=1,to=2,note="§Õn §¹i Phu t¹i Tam S¬n.",needs={{4,191,0,0,0,0,5}},gifts={},xp=6000,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: Tam S¬n",level=18,task=4550,from=2,to=3,note="H¸i 10 Nguyªn S©m t¹i Tam S¬n, tõ (196,203) ®Õn (205,203). CÈn thËn Nguyªn Tham Tinh ë gÇn ®ã.",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: tr¶ Nguyªn S©m",level=18,task=4550,from=3,to=4,note="§Õn §¹i Phu t¹i §ång Quan.",needs={{4,192,0,0,0,0,10}},gifts={},xp=8000,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: §ång Quan",level=18,task=4550,from=4,to=5,note="DiÖt Ng­u S¸t t¹i §ång Quan (195,208) ®Õn (199,208), dïng TÞnh Hãa Phï, råi tÞnh hãa 5 S¬n Xuyªn LiÔu ë hµng (195,211) ®Õn (199,211).",needs={},gifts={},xp=0,money=0,credit=0,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: tr¶ S¬n Xuyªn LiÔu",level=18,task=4550,from=5,to=6,note="Hoµn thµnh Trõ Häa.",needs={{4,201,0,0,0,0,5}},gifts={{0,2,6,2,1,0,1}},xp=10000,money=0,credit=0,label="Trõ Häa",rewardText="Mét trang bÞ lôc."}}
PTG_KILLS={{576,1007,24,2,3,25},{122,1018,3,25,26,12},{113,1039,3,31,32,13},{121,1045,3,42,43,3},{120,1047,3,51,52,4},{117,1048,3,63,63,7},{118,1049,3,63,63,8},{119,1050,3,63,63,6},{116,1062,3,73,74,10},{82,1051,3,85,85,175}}
PTG_BOOKS={{7,16,19,1,0,0,1},{7,17,20,1,0,0,1},{7,18,21,1,0,0,1},{7,19,22,1,0,0,1},{7,33,36,1,0,0,1},{7,34,37,1,0,0,1},{7,35,38,1,0,0,1},{7,36,39,1,0,0,1},{7,45,48,1,0,0,1},{7,46,49,1,0,0,1},{7,55,456,1,0,0,1},{7,60,1481,1,0,0,1},{7,66,1487,1,0,0,1},{7,81,1548,1,0,0,1},{7,83,1551,1,0,0,1}}
PTG_TARGET_NAMES={[3]="KiÕm Nh©n hoÆc X¹ Nh©n",[7]="Hoµn CÈu"}
PTG_TARGET_MAPS={[3]="YÕn S¬n (204,224), (227,224), qu¸i s½n cã ngoµi Sïng Thµnh",[7]="YÕn S¬n (235,226), qu¸i s½n cã ngoµi Sïng Thµnh"}
PTG_JOURNAL={{task=3,header="3004",phases={[1]={title="Trung Thµnh",note="GÆp TrÞnh Lu©n, Sïng Thµnh Doanh (200,203) ë Sïng Thµnh Doanh.",needs={},needNames={}},[2]={title="Trung Thµnh",note="Mang HuyÕt Th­ vÒ Sïng HÇu Hæ, Sïng Thµnh Doanh (212,193).",needs={{4,11,0,0,0,0,1}},needNames={"HuyÕt th­"}},[10]={title="Trung Thµnh",note="Hoµn thµnh Trung Thµnh. §¹t cÊp 35, gÆp Sïng H¾c Hæ, Sïng Thµnh Doanh (213,200) nhËn TrÇm H­¬ng.",needs={},needNames={}},[11]={title="TrÇm H­¬ng",note="Mang TrÇm H­¬ng Méc tíi Hoµng Phi Hæ, TriÒu Ca (231,185) ë TriÒu Ca.",needs={{4,45,0,0,0,0,1}},needNames={"Gç trÇm h­¬ng"}},[20]={title="TrÇm H­¬ng",note="Hoµn thµnh TrÇm H­¬ng. §¹t cÊp 45, gÆp Hoµng Phi Hæ, TriÒu Ca (231,185) nhËn Khuyªn Hµng.",needs={},needNames={}},[21]={title="Khuyªn Hµng",note="Hái chuyÖn Lý TÞnh, TrÇn §­êng (193,207) vµ §Æng Cöu C«ng.",needs={},needNames={}},[22]={title="Khuyªn Hµng",note="GÆp §Æng Cöu C«ng t¹i Tam S¬n. Sau ®ã gÆp §Æng Cöu C«ng, Tam S¬n (204,197).",needs={},needNames={}},[23]={title="Khuyªn Hµng",note="GÆp Lý TÞnh, TrÇn §­êng (193,207) t¹i TrÇn §­êng.",needs={},needNames={}},[24]={title="Khuyªn Hµng",note="Trë vÒ Hoµng Phi Hæ, TriÒu Ca (231,185).",needs={},needNames={}},[25]={title="Khuyªn Hµng",note="DiÖt TrÊn T­íng Qu©n t¹i Môc D· (187,189), lÊy Ên tÝn Vâ Thµnh V­¬ng.",needs={},needNames={}},[26]={title="Khuyªn Hµng",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Hoµng Phi Hæ, TriÒu Ca (231,185).",needs={{4,12,0,0,0,0,1}},needNames={"Ên tÝn cña Vâ Thµnh v­¬ng"}},[27]={title="Khuyªn Hµng",note="B¸o tin cho Kh­¬ng Tö Nha, T©y Kú (158,189) t¹i T©y Kú.",needs={},needNames={}},[30]={title="Khuyªn Hµng",note="Hoµn thµnh Khuyªn Hµng. §¹t cÊp 55, gÆp D­¬ng TiÔn, T©y Kú (165,185) nhËn ThÇn Long.",needs={},needNames={}},[31]={title="ThÇn Long",note="DiÖt ThÇn Long t¹i Long Cung tÇng 3 (206,175), lÊy R©u ThÇn Long.",needs={},needNames={}},[32]={title="ThÇn Long",note="§· lÊy ®­îc vËt phÈm. Mang vÒ §¾c Kû, TriÒu Ca (220,179).",needs={{4,13,0,0,0,0,1}},needNames={"R©u ThÇn long"}},[33]={title="ThÇn Long",note="Mang R©u ThÇn Long cho Hå Hû MÞ, TriÒu Ca (205,184).",needs={{4,13,0,0,0,0,1}},needNames={"R©u ThÇn long"}},[40]={title="ThÇn Long",note="Hoµn thµnh ThÇn Long. §¹t cÊp 65, gÆp §¾c Kû, TriÒu Ca (220,179) nhËn B×nh An.",needs={},needNames={}},[41]={title="B×nh An",note="GÆp T©y V­¬ng MÉu t¹i Diªu Tr×. Sau ®ã gÆp T©y V­¬ng MÉu, Diªu Tr× (221,188).",needs={},needNames={}},[42]={title="B×nh An",note="DiÖt Cù D· t¹i BÝch Du Cung tÇng 4: (181,199), (196,204), (185,217); lÊy An C­ §å.",needs={},needNames={}},[43]={title="B×nh An",note="§· lÊy ®­îc vËt phÈm. Mang vÒ T©y V­¬ng MÉu, Diªu Tr× (221,188).",needs={{4,3,0,0,0,0,1}},needNames={"An C­ ®å"}},[50]={title="B×nh An",note="Hoµn thµnh B×nh An. §¹t cÊp 70, gÆp ThÇn N«ng, Háa V©n §éng (194,191) nhËn ThÇn Dô KÝnh.",needs={},needNames={}},[51]={title="ThÇn Dô KÝnh",note="DiÖt Tr¹nh Nanh t¹i Khæn Tiªn Cung tÇng 1 (197,212), lÊy ThÇn Dô KÝnh.",needs={},needNames={}},[52]={title="ThÇn Dô KÝnh",note="§· lÊy ®­îc vËt phÈm. Mang vÒ ThÇn N«ng, Háa V©n §éng (194,191).",needs={{4,4,0,0,0,0,1}},needNames={"ThÇn Dô KÝnh"}},[60]={title="ThÇn Dô KÝnh",note="Hoµn thµnh ThÇn Dô KÝnh. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch. §¹t cÊp 75, gÆp §¾c Kû, TriÒu Ca (220,179) nhËn Tam S¸ch.",needs={},needNames={}},[61]={title="Tam S¸ch",note="Mang hai b¶ng Phong ThÇn tíi Kh­¬ng Tö Nha, T©y Kú (158,189). Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={{4,9,0,0,0,0,1}},needNames={"Phong ThÇn b¶ng"}},[62]={title="Tam S¸ch",note="Mang ThÇn Dô KÝnh tíi Vâ V­¬ng, T©y Kú (159,189).",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},needNames={"ThÇn Dô KÝnh","Phong ThÇn b¶ng"}},[63]={title="Tam S¸ch",note="LÊy Huúnh §Õ Néi Kinh ë tÇng 2 (196,189); LuyÖn Kim ThuËt ë tÇng 3 (206,204); HuyÒn N÷ Binh Ph¸p ë tÇng 4 (191,210), Khæn Tiªn Cung. Sau ®ã gÆp Vâ V­¬ng, T©y Kú (159,189).",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},needNames={"HuyÒn N÷ binh ph¸p","Huúnh §Õ néi kinh","LuyÖn Kim thuËt"}},[70]={title="Tam S¸ch",note="Hoµn thµnh Tam S¸ch. §¹t cÊp 80, gÆp T©y V­¬ng MÉu, Diªu Tr× (221,188) nhËn Cæ Kim.",needs={},needNames={}},[71]={title="Cæ Kim",note="GÆp §¾c Kû t¹i TriÒu Ca m­êi n¨m sau. Sau ®ã gÆp §¾c Kû, TriÒu Ca m­êi n¨m sau (198,200).",needs={},needNames={}},[72]={title="Cæ Kim",note="GÆp Nguyªn Thñy Thiªn T«n t¹i Ngäc H­ Cung m­êi n¨m sau. Sau ®ã gÆp Nguyªn Thñy Thiªn T«n, Ngäc H­ Cung m­êi n¨m sau (203,198).",needs={},needNames={}},[73]={title="Cæ Kim",note="DiÖt Bé §Çu qu©n Chu t¹i Ngäc H­ Cung m­êi n¨m sau (206,204), lÊy Tróc Gi¶n.",needs={},needNames={}},[74]={title="Cæ Kim",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Nguyªn Thñy Thiªn T«n, Ngäc H­ Cung m­êi n¨m sau (203,198).",needs={{4,10,0,0,0,0,1}},needNames={"Tróc gi¶n"}},[80]={title="Cæ Kim",note="Hoµn thµnh Cæ Kim. Dïng Khø Lai Phï ®Õn Ngäc H­ Cung m­êi n¨m tr­íc. §¹t cÊp 85, gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203) nhËn Thiªn Duyªn.",needs={{4,9,0,0,0,0,1}},needNames={"Phong ThÇn b¶ng"}},[81]={title="Thiªn Duyªn",note="Nãi chuyÖn víi Kh­¬ng Tö Nha thêi niªn thiÕu. Sau ®ã gÆp Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201).",needs={},needNames={}},[82]={title="Thiªn Duyªn",note="Trë l¹i gÆp §¾c Kû thêi niªn thiÕu. Sau ®ã gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203).",needs={},needNames={}},[83]={title="Thiªn Duyªn",note="Hoµn thµnh Thiªn Duyªn. §¹t cÊp 92, gÆp Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201) nhËn Th«ng Thiªn.",needs={},needNames={}},[84]={title="Th«ng Thiªn",note="Tham Lam: Th­¬ng Nh©n, §¹i Phong (200,200). V« Tri: Cöu Di, Tam S¬n (200,200). Tµn NhÉn: dïng MËt §å t¹i B¾c H¶i (190,206). Sau ®ã gÆp Th«ng Thiªn Gi¸o Chñ, ViÔn Cæ Háa DiÖm (204,210).",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},needNames={"Tham lam","V« tri","Tµn nhÉn"}},[85]={title="Thiªn Duyªn",note="DiÖt Bµn Cæ t¹i Khæn Tiªn Cung tÇng 5 (199,212), lÊy Bµn Cæ ThÇn KhÝ. Sau ®ã gÆp §¾c Kû, Ngäc H­ Cung m­êi n¨m tr­íc (207,203).",needs={{4,175,0,0,0,0,1}},needNames={"Bµn Cæ thÇn khÝ"}},[86]={title="Th«ng Thiªn",note="Dïng Khø Lai Phï gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={},needNames={}},[87]={title="Th«ng Thiªn",note="DiÖt Thiªn Hån, §Þa Hån, MÖnh Hån t¹i ViÔn Cæ Háa DiÖm (202,209), (203,209), (204,209). §ñ ba hån sÏ b¾t ®Çu giíi h¹n 3 phót.",needs={},needNames={}},[88]={title="Th«ng Thiªn",note="Trong 3 phót kÓ tõ lóc h¹ ®ñ ba hån, bÊm Khiªu chiÕn ë Th«ng Thiªn Gi¸o Chñ, ViÔn Cæ Háa DiÖm (204,210), råi h¹ b¶n thÓ chiÕn ®Êu ®Ó lÊy Ngäc Thiªn C¬.",needs={},needNames={}},[89]={title="Th«ng Thiªn",note="Mang Ngäc Thiªn C¬ vÒ Kh­¬ng Tö Nha, Ngäc H­ Cung m­êi n¨m tr­íc (198,201); chän mét phÇn th­ëng.",needs={{4,174,0,0,0,0,1}},needNames={"Hµn B¨ng nguyªn khÝ"}}}},{task=21,header="3000",phases={[1]={title="T©n Thøc",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193) t¹i Sïng Thµnh Doanh.",needs={},needNames={}},[2]={title="T©n Thøc",note="GÆp Sïng øng B­u, Sïng Thµnh Doanh (211,198).",needs={},needNames={}},[3]={title="T©n Thøc",note="Mua ®ñ 10 §o¶n KiÕm ë ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193). Sau ®ã giao cho ¢u Thiªn Hãa.",needs={{3,10,0,0,0,0,10}},needNames={"§o¶n KiÕm"}},[4]={title="T©n Thøc",note="GÆp Sïng HÇu Hæ, Sïng Thµnh Doanh (212,193).",needs={},needNames={}},[5]={title="T©n Thøc",note="Trë l¹i gÆp Lç Hïng, Sïng Thµnh Doanh (194,199).",needs={},needNames={}},[6]={title="T©n Thøc",note="GÆp Sïng H¾c Hæ, Sïng Thµnh Doanh (213,200).",needs={},needNames={}},[7]={title="T©n Thøc",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193).",needs={},needNames={}},[8]={title="T©n Thøc",note="Trë l¹i gÆp Lç Hïng, Sïng Thµnh Doanh (194,199) nhËn th­ëng.",needs={},needNames={}}}},{task=24,header="3002",phases={[1]={title="KiÕm ƒi",note="GÆp ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193).",needs={},needNames={}},[2]={title="KiÕm ƒi",note="DiÖt Hoµn CÈu Tinh t¹i YÕn S¬n (213,187), (202,193), (213,194); vÒ Sïng øng Loan.",needs={},needNames={}},[3]={title="KiÕm ƒi",note="§· lÊy ®­îc vËt phÈm. Mang vÒ Sïng øng Loan, Sïng Thµnh Doanh (212,198).",needs={{4,25,0,0,0,0,1}},needNames={"Cuèc"}}}},{task=25,header="3001",phases={[1]={title="Dòng §ao",note="DiÖt Hoµn CÈu V­¬ng t¹i YÕn S¬n (235,208), lÊy ®ñ ba bé phËn ®ao. Tr¶ t¹i TriÒu §iÒn, Sïng Thµnh Doanh (218,198).",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},needNames={"L­ìi ®ao","Th©n ®ao","C¸n ®ao"}},[2]={title="Dòng §ao",note="Nhê ¢u Thiªn Hãa, Sïng Thµnh Doanh (206,193) söa Dòng §ao.",needs={{4,21,0,0,0,0,1},{4,22,0,0,0,0,1},{4,23,0,0,0,0,1}},needNames={"L­ìi ®ao","Th©n ®ao","C¸n ®ao"}},[3]={title="Dòng §ao",note="Mang Dòng §ao vÒ TriÒu §iÒn, Sïng Thµnh Doanh (218,198).",needs={{4,24,0,0,0,0,1}},needNames={"Dòng §ao"}}}},{task=4550,header="3003",phases={[1]={title="Trõ Häa",note="DiÖt Hoµn CÈu t¹i M¹nh T©n (188,210) ®Õn (192,210), råi trong 10 gi©y diÖt Gi¸p Cèt ngay phÝa nam ®Ó lÊy ®ñ 5 H¹p §»ng. Sau ®ã gÆp §¹i Phu, M¹nh T©n (192,212).",needs={{4,191,0,0,0,0,5}},needNames={"H¹p §»ng"}},[2]={title="Trõ Häa",note="§Õn §¹i Phu t¹i Tam S¬n. Sau ®ã gÆp §¹i Phu, Tam S¬n (204,200).",needs={},needNames={}},[3]={title="Trõ Häa",note="H¸i 10 Nguyªn S©m t¹i Tam S¬n, tõ (196,203) ®Õn (205,203). CÈn thËn Nguyªn Tham Tinh ë gÇn ®ã. Sau ®ã gÆp §¹i Phu, Tam S¬n (204,200).",needs={{4,192,0,0,0,0,10}},needNames={"Nguyªn S©m"}},[4]={title="Trõ Häa",note="§Õn §¹i Phu t¹i §ång Quan. Sau ®ã gÆp §¹i Phu, §ång Quan (197,210).",needs={},needNames={}},[5]={title="Trõ Häa",note="DiÖt Ng­u S¸t t¹i §ång Quan (195,208) ®Õn (199,208), dïng TÞnh Hãa Phï, råi tÞnh hãa 5 S¬n Xuyªn LiÔu ë hµng (195,211) ®Õn (199,211). Sau ®ã gÆp §¹i Phu, §ång Quan (197,210).",needs={{4,201,0,0,0,0,5}},needNames={"S¬n Xuyªn LiÔu"}}}}}
PTG_PROGRESS={3,4,6,7,8,10,11,12,13,21,22,23,25,174,175,176,177,178,191,192,201}
-- Literal records use dedicated headers; legacy journal templates stay intact.
function PTGJournalState()
    local value=""
    for i=1,getn(PTG_JOURNAL) do value=value..":"..GetTask(PTG_JOURNAL[i].task) end
    for i=1,getn(PTG_PROGRESS) do value=value..":"..HaveEventItem(PTG_PROGRESS[i]) end
    value=value..":"..GetTask(4554)..":"..GetTask(4555)..":"..GetTask(4562)
    return value
end
function PTGJournal()
    if GetPlayerType()~=0 then return end
    -- Keep the accepted target/count when moving old distant objectives home.
    if GetTask(4554)>0 and GetTask(4559)~=1007 then SetTask(4559,1007) end
    if GetTask(4554)==13 then SetTask(4554,3) end
    if GetTask(4554)==16 then SetTask(4554,7) end
    -- Retire the former technical "Task 27" placeholder.
    AddNote("27",2," ",0)
    for i=1,getn(PTG_JOURNAL) do
        local q=PTG_JOURNAL[i]
        local phase=GetTask(q.task)
        local row=q.phases[phase]
        if row then
            local text="<color=yellow>"..row.title.."<color><enter><color=cyan>"..row.note.."<color>"
            for n=1,getn(row.needs) do
                local need=row.needs[n]
                local count=HaveNormalItem(need[1],need[2],need[3],need[4])
                if count>need[7] then count=need[7] end
                text=text.."<enter><color=green>"..row.needNames[n]..": "..count.."/"..need[7].."<color>"
            end
            if q.task==3 and phase==87 then
                local mask=GetTask(4562)
                text=text.."<enter><color=green>Thiªn Hån "..mod(mask,2).."/1; §Þa Hån "..mod(floor(mask/2),2).."/1; MÖnh Hån "..mod(floor(mask/4),2).."/1<color>"
            end
            AddNote(q.header,1,text,phase)
        else AddNote(q.header,2," ",phase) end
    end
    if GetTask(4554)>0 then
        local text="<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>DiÖt "..PTG_TARGET_NAMES[GetTask(4554)].." t¹i "..PTG_TARGET_MAPS[GetTask(4554)]..".<color><enter><color=green>TiÕn ®é: "..GetTask(4555).."/40<color><enter>Tr¶ t¹i T¹p Hãa Th­¬ng Nh©n, Sïng Thµnh Doanh (207,198)."
        AddNote("3005",1,text,GetTask(4555))
    else AddNote("3005",2," ",0) end
end

-- UTF-8 authoring source; build_giapsi_quests.cjs emits engine TCVN3 bytes.
-- Saved phases retain the original VNG task IDs. New slots: 4550..4569.
function PTGMap() return GetContentWorldPos()+1000 end
function PTGAllowed()
    return GetPlayerType()==0 and PTGMap()==PTG_MAP
end
function PTGReady(a)
    return PTGAllowed() and a.role==PTG_ROLE and GetLevel()>=a.level and
        GetTask(a.task)==a.from and (not a.mainPhase or GetTask(3)==a.mainPhase)
end
function PTGHave(rows)
    for i=1,getn(rows) do
        local r=rows[i]
        if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end
    end
    return 1
end
function PTGNotice(text)
    SayButtons(text,1,"§ãng/ptg_close")
end
function ptg_close() CloseDialog() end
function PTGPreview(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    local text="<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>"
    if a.xp>0 then text=text.."<enter><color=green>Kinh nghiÖm: "..a.xp.."<color>" end
    if a.money>0 then text=text.."<enter><color=green>L­îng: "..a.money.."<color>" end
    if a.rewardText then text=text.."<enter><color=green>"..a.rewardText.."<color>" end
    SayButtons(text,2,"TiÕp tôc/ptg_yes"..i,"Quay l¹i/main")
end
function PTGApply(i)
    local a=PTG_STEPS[i]
    if not PTGReady(a) then return end
    if GetTrade()~=0 or GetLockState()~=0 then return end
    if PTGHave(a.needs)~=1 then
        PTGNotice("<color=yellow>"..a.title.."<color><enter>Ch­a ®ñ vËt phÈm.<enter><color=cyan>"..a.note.."<color>")
        return
    end
    local needs=a.needs
    if a.keepNeeds then needs={} end
    local gifts=a.gifts
    if a.ensureBoard then
        gifts={}
        local count=HaveEventItem(9)
        if count<3 then gifts={{4,9,0,0,0,0,3-count}} end
    end
    if a.randomBook then gifts={PTG_BOOKS[RANDOM(1,getn(PTG_BOOKS))]} end
    -- Earn has no result in the legacy API: verify the actual wallet delta,
    -- then roll it back if inventory/phase CAS fails. No yielded Lua calls.
    local old=GetCash()
    if a.money>0 then
        if old>2000000000-a.money then return end
        Earn(a.money)
        if GetCash()~=old+a.money then return end
    end
    if QuestExchange(a.task,a.from,a.to,needs,gifts,1)~=1 then
        if a.money>0 then Pay(a.money) end
        PTGNotice("Ch­a thÓ giao nhiÖm vô. H·y më khãa vËt phÈm vµ dµnh ®ñ « trèng trong hµnh trang.")
        return
    end
    if a.xp>0 then AddOwnExp(a.xp) end
    if a.credit>0 then AddCredit(a.credit) end
    if a.camp then SetCamp(a.camp) end
    if a.task==3 and a.to==84 then SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0) end
    PTGJournal()
    Msg2Player(a.note)
    if a.travel then NewWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8) end
    PTGNotice("<color=yellow>"..a.title.."<color><enter><color=cyan>"..a.note.."<color>")
end
function PTGMenu()
    PTGJournal()
    if not PTGAllowed() then
        if PTG_OLD_MAIN then return PTG_OLD_MAIN() end
        CloseDialog();return
    end
    local options={}
    if PTSAddChoices then PTSAddChoices(options) end
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) and (getn(a.needs)==0 or PTGHave(a.needs)==1) then tinsert(options,a.label.."/ptg_a"..i) end
    end
    if PTG_ROLE=="giao" and GetLevel()>=20 then
        if GetTask(4554)==0 and ((GetLevel()<29 and (GetTask(4556)~=PTGDay() or GetTask(4557)<3)) or (GetLevel()>=29 and GetTask(4558)==0)) then tinsert(options,"Gi¸o HuÊn/ptg_giao") end
        if GetTask(4554)>0 and GetTask(4555)>=40 then tinsert(options,"NhËn th­ëng/ptg_giao_claim") end
    end
    if PTG_ROLE=="thong" and GetTask(3)==88 then tinsert(options,"Khiªu chiÕn/ptg_challenge") end
    if PTG_OLD_MAIN then tinsert(options,"DÞch vô/ptg_services") end
    local text="<color=yellow>NhiÖm vô Gi¸p SÜ<color><enter>Chän nót nhiÖm vô ®Ó xem môc tiªu vµ phÇn th­ëng."
    if getn(options)==0 then text="<color=yellow>NhiÖm vô Gi¸p SÜ<color><enter>Ch­a cã nhiÖm vô ®ñ ®iÒu kiÖn nhËn hoÆc tr¶. Xem chØ dÉn ®ang lµm b»ng F11." end
    SayButtons(text,getn(options),options)
end
function ptg_services()
    if PTG_OLD_MAIN then PTG_OLD_MAIN() else CloseDialog() end
end
function PTGMarker()
    if not PTGAllowed() then
        if PTG_OLD_MARK then return PTG_OLD_MARK() end
        return 0,0
    end
    for i=1,getn(PTG_STEPS) do
        local a=PTG_STEPS[i]
        if PTGReady(a) then
            if a.xp>0 or a.money>0 or getn(a.gifts)>0 and a.from>0 then return 3,a.level end
            return 1,a.level
        end
    end
    if PTG_ROLE=="giao" and GetLevel()>=20 then return 1,20 end
    return 0,0
end
function PTGEvent(id,limit)
    if HaveEventItem(id)>=limit then return 0 end
    return AddEventItem(id)
end
function PTGDeath(template)
    if GetPlayerType()~=0 then return end
    local before=PTGJournalState()
    local map=PTGMap()
    for i=1,getn(PTG_KILLS) do
        local k=PTG_KILLS[i]
        if template==k[1] and map==k[2] and GetTask(k[3])==k[4] then
            if PTGEvent(k[6],1)>0 and k[4]~=k[5] then SetTask(k[3],k[5]) end
        end
    end
    -- Original one-of-each Dung Dao admission remains enforced natively.
    if template==575 and map==1007 and GetTask(25)==1 then PTGEvent(20+RANDOM(1,3),1) end
    if GetTask(4550)==1 and map==1015 then
        if template==7 then if AddIBBuff(335,10)>0 then SetTask(4551,SystemTime()+10);TopMessage(11631) else SetTask(4551,0) end end
        if template==13 and GetTask(4551)>SystemTime() and HaveIBBuff(335)==1 then PTGEvent(191,5) end
    end
    if GetTask(4550)==5 and map==1014 and template==18 then
        if HaveNormalItem(6,1,277,1)<1 and HaveNormalItem(6,1,9201,1)<1 then AddNormalItem(6,1,277,1,0,0) end
    end
    local target=GetTask(4554)
    if target>0 and map==GetTask(4559) then
        local eligible=0
        if template==target then eligible=1 end
        if target==3 and template==0 then eligible=1 end
        if eligible==1 and GetTask(4555)<40 then SetTask(4555,GetTask(4555)+1) end
    end
    -- Soul masks are persistent, idempotent, map-checked and killer-scoped.
    if GetTask(3)==87 and map==1070 and template>=541 and template<=543 then
        local bit=2^(template-541)
        local mask=GetTask(4562)
        if mod(floor(mask/bit),2)==0 then SetTask(4562,mask+bit) end
        if GetTask(4562)==7 then
            SetTask(3,88);SetTask(4563,SystemTime()+180)
            Msg2Player("ViÔn Cæ Háa DiÖm: cßn 3 phót h¹ Th«ng Thiªn Gi¸o Chñ!")
        end
    end
    if template==2703 and map==1070 and GetTask(3)==88 then
        if SystemTime()<=GetTask(4563) and PTGEvent(174,1)>0 then SetTask(3,89) end
    end
    if before~=PTGJournalState() then PTGJournal() end
end
function PTGDay() return floor((SystemTime()+25200)/86400) end
function ptg_giao()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetLevel()<20 then return end
    if GetTask(4554)>0 then
        PTGNotice("<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>"..GetTask(4555).." / 40 "..PTG_TARGET_NAMES[GetTask(4554)].."<color><enter>§ñ môc tiªu h·y chän nót nhËn th­ëng.")
        if GetTask(4555)>=40 then SayButtons("<color=green>PhÇn th­ëng: 4600 kinh nghiÖm vµ 3000 l­îng.<color>",2,"NhËn th­ëng/ptg_giao_claim","§ãng/ptg_close") end
        return
    end
    if GetLevel()>=29 then
        if GetTask(4558)~=0 then PTGNotice("§· hoµn thµnh l­ît Gi¸o HuÊn cuèi cïng.");return end
    elseif GetTask(4556)==PTGDay() and GetTask(4557)>=3 then
        PTGNotice("§· nhËn ®ñ 3 l­ît Gi¸o HuÊn h«m nay.");return
    end
    SayButtons("<color=yellow>Gi¸o HuÊn<color><enter><color=cyan>Tiªu diÖt 40 qu¸i ®­îc giao.<color><enter><color=green>4600 kinh nghiÖm vµ 3000 l­îng.<color>",2,"NhËn nhiÖm vô/ptg_giao_take","Quay l¹i/main")
end
function ptg_giao_take()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetLevel()<20 or GetTask(4554)~=0 then return end
    if GetTrade()~=0 or GetLockState()~=0 then return end
    if GetLevel()>=29 then
        if GetTask(4558)~=0 then return end
        SetTask(4558,1);SetTask(4554,7);SetTask(4559,1007)
    else
        if GetTask(4556)~=PTGDay() then SetTask(4556,PTGDay());SetTask(4557,0) end
        if GetTask(4557)>=3 then return end
        SetTask(4557,GetTask(4557)+1);SetTask(4554,3);SetTask(4559,1007)
    end
    SetTask(4555,0)
    PTGJournal()
    Msg2Player("Gi¸o HuÊn: diÖt 40 "..PTG_TARGET_NAMES[GetTask(4554)]..". Môc tiªu n»m t¹i "..PTG_TARGET_MAPS[GetTask(4554)]..".")
    PTGMenu()
end
function ptg_giao_claim()
    if not PTGAllowed() or PTG_ROLE~="giao" or GetTask(4554)==0 or GetTask(4555)<40 then return end
    if GetTrade()~=0 or GetLockState()~=0 or GetCash()>1999997000 then return end
    local old=GetCash();Earn(3000)
    if GetCash()~=old+3000 then return end
    if QuestExchange(4554,GetTask(4554),0,{},{})~=1 then Pay(3000);return end
    SetTask(4555,0);AddOwnExp(4600);PTGJournal()
    PTGNotice("<color=green>§· nhËn 4600 kinh nghiÖm vµ 3000 l­îng.<color>")
end
function PTGGather(kind,bit)
    local before=PTGJournalState()
    if GetPlayerType()~=0 then return end
    local phase=GetTask(4550)
    local mask=GetTask(4553)
    if kind==1 and phase==3 and PTGMap()==1016 then
        return GiapGather(0)
    elseif kind==2 and phase==5 and PTGMap()==1014 then
        return GiapGather(0)
    end
    if before~=PTGJournalState() then PTGJournal() end
end
function ptg_challenge()
    if not PTGAllowed() or PTG_ROLE~="thong" or GetTask(3)~=88 then return end
    if SystemTime()>GetTask(4563) then
        -- Retry the whole Thong Thien stage, retaining the preceding quests.
        for id=176,178 do for n=1,2 do if HaveEventItem(id)>0 then DelEventItem(id) end end end
        SetTask(3,83);SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0)
        PTGJournal()
        PTGNotice("§· qu¸ 3 phót. Trë l¹i Kh­¬ng Tö Nha thêi niªn thiÕu nhËn l¹i Th«ng Thiªn.")
        return
    end
    local previous=GetTask(4567)
    if previous>0 and GetNpcTemplateID(previous)==2703 and GetNpcWorldPos(previous)==1070 and GetNpcLife(previous)>0 then return end
    local npc=AddNpc(2703,95,SubWorld,204*256+128,210*512+256,1)
    if npc and npc>0 then
        if SetNpcDeathScript(npc,"\\script\\phongthan\\giapsi\\death_2703.lua")==1 then SetTask(4567,npc) else DelNpc(npc) end
    end
    CloseDialog()
end

PTG_MAP=1020
PTG_ROLE="vo"
PTG_OLD_MAIN=main
PTG_OLD_MARK=GetPlayerTaskState
function main() return PTGMenu() end
function GetPlayerTaskState() return PTGMarker() end
PTG_OLD_SAYTASK=SayTask
function SayTask(text,choices)
 if GetPlayerType()==0 then for n=1,getn(choices) do local t=choices[n][1];if PTG_OLD_TITLES[t] then choices[n].show=0 end end end
 return PTG_OLD_SAYTASK(text,choices)
end
PTG_OLD_TITLES={["T©n Thøc"]=1,["Dòng §ao"]=1,["KiÕm ƒi"]=1,["Trung Thµnh"]=1,["TrÇm H­¬ng"]=1,["Khuyªn Hµng"]=1,["ThÇn Long"]=1,["B×nh An"]=1,["ThÇn Dô KÝnh"]=1,["Tam S¸ch"]=1,["Cæ Kim"]=1,["Thiªn Duyªn"]=1,["Th«ng Thiªn"]=1,["Trõ Häa"]=1,["Khuyªn hµng"]=1,["Tam s¸ch"]=1}
function ptg_a1() PTGPreview(1) end
function ptg_yes1() PTGApply(1) end
function ptg_a2() PTGPreview(2) end
function ptg_yes2() PTGApply(2) end
function ptg_a3() PTGPreview(3) end
function ptg_yes3() PTGApply(3) end
function ptg_a4() PTGPreview(4) end
function ptg_yes4() PTGApply(4) end
function ptg_a5() PTGPreview(5) end
function ptg_yes5() PTGApply(5) end
function ptg_a6() PTGPreview(6) end
function ptg_yes6() PTGApply(6) end
function ptg_a7() PTGPreview(7) end
function ptg_yes7() PTGApply(7) end
function ptg_a8() PTGPreview(8) end
function ptg_yes8() PTGApply(8) end
function ptg_a9() PTGPreview(9) end
function ptg_yes9() PTGApply(9) end
function ptg_a10() PTGPreview(10) end
function ptg_yes10() PTGApply(10) end
function ptg_a11() PTGPreview(11) end
function ptg_yes11() PTGApply(11) end
function ptg_a12() PTGPreview(12) end
function ptg_yes12() PTGApply(12) end
function ptg_a13() PTGPreview(13) end
function ptg_yes13() PTGApply(13) end
function ptg_a14() PTGPreview(14) end
function ptg_yes14() PTGApply(14) end
function ptg_a15() PTGPreview(15) end
function ptg_yes15() PTGApply(15) end
function ptg_a16() PTGPreview(16) end
function ptg_yes16() PTGApply(16) end
function ptg_a17() PTGPreview(17) end
function ptg_yes17() PTGApply(17) end
function ptg_a18() PTGPreview(18) end
function ptg_yes18() PTGApply(18) end
function ptg_a19() PTGPreview(19) end
function ptg_yes19() PTGApply(19) end
function ptg_a20() PTGPreview(20) end
function ptg_yes20() PTGApply(20) end
function ptg_a21() PTGPreview(21) end
function ptg_yes21() PTGApply(21) end
function ptg_a22() PTGPreview(22) end
function ptg_yes22() PTGApply(22) end
function ptg_a23() PTGPreview(23) end
function ptg_yes23() PTGApply(23) end
function ptg_a24() PTGPreview(24) end
function ptg_yes24() PTGApply(24) end
function ptg_a25() PTGPreview(25) end
function ptg_yes25() PTGApply(25) end
function ptg_a26() PTGPreview(26) end
function ptg_yes26() PTGApply(26) end
function ptg_a27() PTGPreview(27) end
function ptg_yes27() PTGApply(27) end
function ptg_a28() PTGPreview(28) end
function ptg_yes28() PTGApply(28) end
function ptg_a29() PTGPreview(29) end
function ptg_yes29() PTGApply(29) end
function ptg_a30() PTGPreview(30) end
function ptg_yes30() PTGApply(30) end
function ptg_a31() PTGPreview(31) end
function ptg_yes31() PTGApply(31) end
function ptg_a32() PTGPreview(32) end
function ptg_yes32() PTGApply(32) end
function ptg_a33() PTGPreview(33) end
function ptg_yes33() PTGApply(33) end
function ptg_a34() PTGPreview(34) end
function ptg_yes34() PTGApply(34) end
function ptg_a35() PTGPreview(35) end
function ptg_yes35() PTGApply(35) end
function ptg_a36() PTGPreview(36) end
function ptg_yes36() PTGApply(36) end
function ptg_a37() PTGPreview(37) end
function ptg_yes37() PTGApply(37) end
function ptg_a38() PTGPreview(38) end
function ptg_yes38() PTGApply(38) end
function ptg_a39() PTGPreview(39) end
function ptg_yes39() PTGApply(39) end
function ptg_a40() PTGPreview(40) end
function ptg_yes40() PTGApply(40) end
function ptg_a41() PTGPreview(41) end
function ptg_yes41() PTGApply(41) end
function ptg_a42() PTGPreview(42) end
function ptg_yes42() PTGApply(42) end
function ptg_a43() PTGPreview(43) end
function ptg_yes43() PTGApply(43) end
function ptg_a44() PTGPreview(44) end
function ptg_yes44() PTGApply(44) end
function ptg_a45() PTGPreview(45) end
function ptg_yes45() PTGApply(45) end
function ptg_a46() PTGPreview(46) end
function ptg_yes46() PTGApply(46) end
function ptg_a47() PTGPreview(47) end
function ptg_yes47() PTGApply(47) end
function ptg_a48() PTGPreview(48) end
function ptg_yes48() PTGApply(48) end
function ptg_a49() PTGPreview(49) end
function ptg_yes49() PTGApply(49) end
function ptg_a50() PTGPreview(50) end
function ptg_yes50() PTGApply(50) end
function ptg_a51() PTGPreview(51) end
function ptg_yes51() PTGApply(51) end
function ptg_a52() PTGPreview(52) end
function ptg_yes52() PTGApply(52) end
function ptg_a53() PTGPreview(53) end
function ptg_yes53() PTGApply(53) end
function ptg_a54() PTGPreview(54) end
function ptg_yes54() PTGApply(54) end
function ptg_a55() PTGPreview(55) end
function ptg_yes55() PTGApply(55) end
function ptg_a56() PTGPreview(56) end
function ptg_yes56() PTGApply(56) end
function ptg_a57() PTGPreview(57) end
function ptg_yes57() PTGApply(57) end
function ptg_a58() PTGPreview(58) end
function ptg_yes58() PTGApply(58) end
function ptg_a59() PTGPreview(59) end
function ptg_yes59() PTGApply(59) end
function ptg_a60() PTGPreview(60) end
function ptg_yes60() PTGApply(60) end
function ptg_a61() PTGPreview(61) end
function ptg_yes61() PTGApply(61) end
function ptg_a62() PTGPreview(62) end
function ptg_yes62() PTGApply(62) end
