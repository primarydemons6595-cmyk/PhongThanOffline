---ÍòÏÉÕóÁì??Îñ´¦
--lixuewu
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

task_lvl_2_sel_lvl={[2]=3,[5]=5,[8]=7,[10]=9}     --¸Ë³ÆÃþ§O¡H
task_lvl_2_sel_idx={[2]=1,[5]=2,[8]=3,[10]=4}     --Â¾·~¹ïÀ³ªº¸Ë³ÆÃþ§O¡H
function main()
	local tasks = {
		{"§¹i phu","renwu1";show=0},
		{"Ph¸ V¹n Tiªn trËn","renwu2";show=0},
		{"§¼ng cÊp","chaxun";show=0}
	}
	local level=GetLevel()
	if(level>=30)then
		local wxz_level=GetTask(405)
		if(wxz_level~=0)then
			tasks[2].show=1
			tasks[3].show=1
		else
			tasks[1].show=1
		end
	end
	SayTask("Ma thó trong V¹n Tiªn trËn ®ang léng hµnh. Ta ®ang chiªu mé c¸c dòng sÜ tõ cÊp <color=green>30<color> ®Ó tiªu diÖt bän chóng",tasks)
end

function chaxun()
	local level_req={2,5,8,10}
	local wxz_level=GetTask(405)-1
	if(wxz_level>=0)then
		local str="§¼ng cÊp V¹n Tiªn trËn cña ng­¬i hiÖn lµ <color=green>"..(wxz_level).."<color>"
		if(wxz_level>=10)then
			str=str.."®· ®¹t ®Õn ®¼ng cÊp cao nhÊt!"
		elseif(wxz_level<level_req[get_suit_wxz()])then
			str=str.."ChØ cÇn hoµn thµnh l¹i <color=green>"..GetTask(406).."<color> lÇn nhiÖm vô V¹n Tiªn trËn th× cã thÓ th¨ng cÊp."
		else
			str=str.."B¹n ph¶i vµo <color=green>V¹n Tiªn trËn cao h¬n mét cÊp<color> míi cã thÓ tiÕp tôc th¨ng cÊp."
		end
		MsgBox(str,"no")
	else
		--²»¿ÉÄÜµÄÇéªí
	end
end

function renwu1()
	local task_wxz=GetTask(408)
	if(task_wxz==0)then
		MsgBox("Tr¨m n¨m tr­íc hiÖn th©n cña Phong ThÇn b¶ng ®· dÉn ®Õn mét trËn hµo kiÕp. TriÖt Gi¸o cña Th«ng Thiªn gi¸o chñ lµm tr¸i ý chØ cña Ngäc Hoµng, lËp Chu Tiªn trËn ng¨n c¶n viÖc Phong ThÇn. XiÓn Gi¸o gi¸o chñ liªn thñ víi T©y Ph­¬ng gi¸o chñ míi ph¸ ®­îc Chu Tiªn trËn, ®¸nh b¹i Th«ng Thiªn gi¸o chñ. GÇn ®©y, nghe nãi Ph¸p ®µn cña h¾n cã treo mét ¸c Phiªn gäi lµ <color=green>Lôc Hån Phiªn<color>, kh«ng biÕt nh»m môc ®Ých g×, ng­¬i cã thÓ gióp ta ®iÒu tra lai lÞch cña l¸ cê nµy xem cã nguy hiÓm g× cho §¹i Th­¬ng cña ta kh«ng#¿","yes_1","no")
	elseif(task_wxz==100000)then
		MsgBox("Th× ra <color=green>Lôc Hån Phiªn<color> kh«ng cã trong V¹n Tiªn trËn, xem ra vÉn ph¶i tiÕp tôc t×m kiÕm. Nh­ng phÇn th­ëng nµy ng­¬i h·y nhËn lÊy. V¹n Tiªn trËn lµ n¬i luyÖn tËp n¨ng lùc lý t­ëng tr­íc khi ®i t¸c chiÕn.","no")
		SetTask(405,1)
		SetTask(406,1)
		SetTask(408,0)
		AddOwnExp(5000)
		Msg2Player("B¹n nh©n ®­îc "..(5000).."®iÓm kinh nghiÖm.")
	else
		Talk(1,"no","Th«ng Thiªn gi¸o chñ ®· bµy V¹n Tiªn trËn c¶n trë lÇn Phong ThÇn nµy, cã thÓ Lôc Hån Phiªn n»m trong V¹n Tiªn trËn, tr­íc khi ®i V¹n Tiªn trËn ®iÒu tra tèt nhÊt nªn ®Õn t×m <color=green>§¹i phu<color> trong V¹n Tiªn trËn hái xem.")
	end
end

function yes_1()
	SetTask(408,1)
	Msg2Player("TiÕp nhËn nhiÖm vô th¸m thÝnh V¹n Tiªn trËn.")
	MsgBox("Th«ng Thiªn gi¸o chñ ®· bµy V¹n Tiªn trËn c¶n trë lÇn Phong ThÇn nµy, cã thÓ Lôc Hån Phiªn n»m trong V¹n Tiªn trËn, tr­íc khi ®i V¹n Tiªn trËn ®iÒu tra tèt nhÊt nªn ®Õn t×m <color=green>§¹i phu<color> trong V¹n Tiªn trËn hái xem.","no")
end

function reward_normal()
	if(GetTask(408)==100000)then
		SetTask(408,0)
		local wxz_reward = {
			{400,5000},{500,5000},{500,5000},{600,5000},{600,10000},{700,10000},
			{700,20000},{800,20000},{800,30000},{900,40000},{1000,50000}
		}
		local wxz_level=GetTask(405)
		local exp=wxz_reward[wxz_level][1]*GetLevel()*2
		local money=wxz_reward[wxz_level][2]
		AddOwnExp(exp)
		Earn(money)
		Msg2Player("B¹n nh©n ®­îc "..money.."l­îng +"..exp.."®iÓm kinh nghiÖm.")
		local level_req={2,5,8,10}
		if((wxz_level<=10)and(wxz_level<=level_req[get_suit_wxz()]))then
			local need_times=GetTask(406)-1
			if(need_times<=0)then
				local levelup={3,5,8,12,18,25,35,50,100,0}
				SetTask(405,wxz_level+1)
				SetTask(406,levelup[wxz_level])
				return 1
			else
				SetTask(406,need_times)
			end
		else
			return 2
		end
	end
	return 0
end

function reward_add()
	local wxz_level=GetTask(405)
	local sel_idx=task_lvl_2_sel_idx[wxz_level]
	if(sel_idx~=nil)then
                               
		local sel_type=GetPlayerType()+1  
		local item_list={}
                if (wxz_level==8)then
                      for i=2,4 do
                       item_list[i-1]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
                       end
                      Say("§¼ng cÊp V¹n Tiªn trËn cña ng­¬i ®· ®¹t <color=green>"..wxz_level.."<color>, ngoµi phÇn th­ëng nhËn ®­îc lÇn nµy, ng­¬i cã thÓ chän 1 bé trang bÞ",3,item_list)
		elseif(wxz_level~=8) then
                        for i=1,5 do
			item_list[i]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
                        end
	             Say("§¼ng cÊp V¹n Tiªn trËn cña ng­¬i ®· ®¹t <color=green>"..wxz_level.."<color>, ngoµi phÇn th­ëng nhËn ®­îc lÇn nµy, ng­¬i cã thÓ chän 1 bé trang bÞ",5,item_list)
	        end
       end
end

function cost_ib()
		local today=floor(SystemTime()/86400)
		if(GetTask(767)~=today)then
			SetTask(767,today)
			SetTask(768,0)
		end
		if(FindAValidIBItem(8,138,2,0)>=1)then
			if(GetTask(768) < 2) then
				CostIBItem(FindAValidIBItem(8,138,2,0))
				SetTask(409,0)
				SetTask(768,GetTask(768)+1)
				renwu2()
			else
				Talk(1,"no","H«m nay ®· sö dông 2 lÇn Th­ëng Kim Bµi, ngµy mai h·y quay l¹i.")
			end
		else
			MsgBox("Xin lçi! Ng­¬i kh«ng cã Th­ëng Kim Bµi.","no")
		end
end

function renwu2()
	local names = {
		"",
		{{"ThiÕt trïng","Sinh lùc rÊt m¹nh, hé gi¸p L«i yÕu"},{"Ngäc N÷","Sinh lùc rÊt m¹nh, hé gi¸p Thæ yÕu"},{"Lam Cèt","Sinh lùc vµ tÊn c«ng c¬ b¶n cao"},{"§íi Tr¹i","Sinh lùc vµ phßng thñ Ma ph¸p cao"}},
		{{"Phi Gi¸p","Sinh lùc vµ tÊn c«ng c¬ b¶n cao"},{"D· Mao","Sinh lùc rÊt cao, hé gi¸p Háa yÕu"},{"Ma N÷","Sinh lùc rÊt m¹nh, kh«ng dÔ ph¸t hiÖn."},{"Lôc Ng« ThÇn","Sinh lùc vµ phßng thñ Ma ph¸p cao"}},
		{"¤ V©n Quy","Mét trong tø tiªn cña V¹n Tiªn trËn, rÊt giái Ma ph¸p."},{"§ao Ng­u","Mét trong tø tiªn cña V¹n Tiªn trËn, rÊt giái Ma ph¸p."},{"§¹i Th¹ch ThÇn","Mét trong tø tiªn cña V¹n Tiªn trËn, rÊt giái Ma ph¸p."},{"ThiÕt Kinh thÇn","Mét trong tø tiªn cña V¹n Tiªn trËn, rÊt giái Ma ph¸p."},
		{"4 §Çu lÜnh thñ trËn","Muèn ®¸nh b¹i bän chóng kh«ng ph¶i chuyÖn dÔ, ®ång ®éi t¸c chiÕn lµ quan träng nhÊt."},{"Th«ng Thiªn gi¸o chñ","§øng ®Çu ma chóng trong V¹n Tiªn trËn. Ph¸p lùc v« biªn. "}
	}
	local today=floor(SystemTime()/86400)
	local task_wxz=GetTask(408)
	if(task_wxz==0)then
		--½Ó?Îñ
		if(today~=GetTask(409))then
			SetTask(409,today)
			local wxz_level=GetTask(405)
			local need_times=GetTask(406)
			if((wxz_level==8)and(need_times==1))then
				SetTask(408,10)
				MsgBox("V¹n Tiªn trËn cã nh÷ng n¬i ng­êi th­êng kh«ng thÓ ®Õn ®­îc, muèn ®Õn ph¶i cã <color=green>LÖnh bµi ®Æc biÖt<color>. Nghe nãi ®· cã ng­êi thÊy cã kÎ bÞ biÕn mÊt khi ®øng gÇn vËt tæ trong ®ã. Hy väng ng­¬i b×nh an trë vÒ.","no")
			else
				local wxz_tasks = {
					{{2,50}},{{2,100}},{{2,100},{3,100}},{{4,1},{5,1},{6,1},{7,1}},
					{{2,150},{3,150}},{{2,200},{3,200},{4,1},{5,1},{6,1},{7,1}},{{2,300},{3,300}},
					{{2,400},{3,400},{8,1},{9,1}},{{2,500},{3,500},{8,1},{9,1}},
					{{2,500},{3,500},{8,1},{9,1}},{{2,500},{3,500},{8,1},{9,1}}
				}
				local wxz_sel=random(1,getn(wxz_tasks[wxz_level]))
				local killtype=wxz_tasks[wxz_level][wxz_sel][1]
				local killcount=wxz_tasks[wxz_level][wxz_sel][2]
				SetTask(407,killcount)
				SetTask(408,killtype)
				local str="NhiÖm vô lÇn nµy: vµo V¹n Tiªn trËn giÕt <color=red>"
				if(killcount~=1)then
					str=str..killcount.."<color>  <color=red>"
				end
				if(killtype<=3)then
					str=str..names[killtype][get_suit_wxz()][1].."<color>."..names[killtype][get_suit_wxz()][2]
				else
					str=str..names[killtype][1].."<color>,"..names[killtype][2]
				end
			
				MsgBox(str..". Ng­¬i ph¶i hoµn thµnh trong <color=red>h«m nay<color> míi cã thÓ nhËn phÇn th­ëng.","no")
			end
		else
				MsgBox("NhiÖm vô h«m nay ®· hoµn thµnh, nÕu ng­¬i cã <color=green>Th­ëng Kim bµi<color> cã thÓ nhËn nhiÖm vô míi. Ng­¬i muèn dïng <color=green>Th­ëng Kim bµi<color> ®Ó ®æi nhiÖm vô kh«ng?","cost_ib","no")
		end
	elseif(task_wxz==100000)then
		--½»?Îñ
		if(today==GetTask(409))then
			local wxz_level=GetTask(405)
			local need_times=GetTask(406)
			if(need_times==1)then
				if((wxz_level==2)or(wxz_level==5)or(wxz_level==10))then
					reward_add()
				elseif(wxz_level==8)then
					SetTask(404,SetBit(GetTask(404),1,1))
					MsgBox("Th× ra lµ vËy! Nh­ng ng­¬i hiÖn nay kh«ng cÇn lÖnh bµi còng cã thÓ v­ît qua. Tiªu diÖt Th«ng Thiªn gi¸o chñ lµ t©m nguyÖn cña tÊt c¶ c¸c chiÕn binh. §i hay kh«ng tïy ng­¬i!","reward_add")
				else
					local str="Ng­¬i ®· hoµn thµnh nhiÖm vô. Xin nhËn phÇn th­ëng.\n"
					local ret=reward_normal()
					if(ret==1)then
						str=str.."§¼ng cÊp V¹n Tiªn trËn cña b¹n t¨ng lªn <color=green>"..wxz_level.."<color>, nhiÖm vô sau nµy sÏ cµng khã h¬n nh­ng phÇn th­ëng cµng nhiÒu h¬n."
					elseif(ret==2)then
						if(wxz_level<=10)then
							str=str.."Ph¶i t¨ng ®¼ng cÊp cña m×nh míi cã thÓ vµo V¹n Tiªn trËn cao h¬n 1 cÊp."
						end
					end
					MsgBox(str,"no")
				end
			else
				local str="Ng­¬i ®· hoµn thµnh nhiÖm vô. Xin nhËn phÇn th­ëng.\n"
				local ret=reward_normal()
				if(ret==1)then
					str=str.."§¼ng cÊp V¹n Tiªn trËn cña b¹n t¨ng lªn <color=green>"..wxz_level.."<color>, nhiÖm vô sau nµy sÏ cµng khã h¬n nh­ng phÇn th­ëng cµng nhiÒu h¬n."
				elseif(ret==2)then
					if(wxz_level<=10)then
						str=str.."Ph¶i t¨ng ®¼ng cÊp cña m×nh míi cã thÓ vµo V¹n Tiªn trËn cao h¬n 1 cÊp."
					end
				end
				MsgBox(str,"no")
			end
		else
			SetTask(407,0)
			SetTask(408,0)
			Talk(1,"no","Thùc hiÖn nhiÖm vô vuît qu¸ thêi gian quy ®Þnh, kh«ng thÓ nhËn phÇn th­ëng.")
		end
	else
		--?ÎñÖÐ
		if(today==GetTask(409))then
			if(task_wxz~=10)then
				local killcount = GetTask(407)
				local killtype = GetByte(GetTask(408),1)
				local str="Ng­¬i h·y ®i khuÊt phôc <color=red>"

				if(killcount~=1)then
					str=str..killcount.."<color>  <color=red>"
				end
		
				if(killtype<=3)then
					str=str..names[killtype][get_suit_wxz()][1].."<color>."..names[killtype][get_suit_wxz()][2]
				else
					str=str..names[killtype][1].."<color>,"..names[killtype][2]
				end
				Talk(1,"no",str.."Mau ®i mau vÒ!")
			else
				MsgBox("V¹n Tiªn trËn cã nh÷ng n¬i ng­êi th­êng kh«ng thÓ ®Õn ®­îc, muèn ®Õn ph¶i cã <color=green>LÖnh bµi ®Æc biÖt<color>. Nghe nãi ®· cã ng­êi thÊy cã kÎ bÞ biÕn mÊt khi ®øng gÇn vËt tæ trong ®ã. Hy väng ng­¬i b×nh an trë vÒ.","no")
			end
		else
			SetTask(407,0)
			SetTask(408,0)
			Talk(1,"no","Thùc hiÖn nhiÖm vô vuît qu¸ thêi gian quy ®Þnh, kh«ng thÓ nhËn phÇn th­ëng.")
		end
	end
end

function item_1()
	local wxz_level=GetTask(405)
	local sel_lvl=task_lvl_2_sel_lvl[wxz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[wxz_level]
		AddGlobalCountNews("<color=green>"..GetName().."<color> t¨ng ®¼ng cÊp V¹n Tiªn trËn lªn "..wxz_level..", <color=green>®­îc Thiªn Hïng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][1].."<color>.",20)
		AddNormalItem(0,2,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
	CloseDialog()
end

function item_2()
	local wxz_level=GetTask(405)
	local sel_lvl=task_lvl_2_sel_lvl[wxz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[wxz_level]
		AddGlobalCountNews("<color=green>"..GetName().."<color> t¨ng ®¼ng cÊp V¹n Tiªn trËn lªn "..wxz_level..", <color=green>®­îc Thiªn Hïng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][2].."<color>.",20)
		AddNormalItem(0,5,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
	CloseDialog()
end

function item_3()
	local wxz_level=GetTask(405)
	local sel_lvl=task_lvl_2_sel_lvl[wxz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[wxz_level]
		AddGlobalCountNews("<color=green>"..GetName().."<color> t¨ng ®¼ng cÊp V¹n Tiªn trËn lªn "..wxz_level..", <color=green>®­îc Thiªn Hïng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][3].."<color>.",20)
		AddNormalItem(0,6,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
	CloseDialog()
end
function item_4()
	local wxz_level=GetTask(405)
	local sel_lvl=task_lvl_2_sel_lvl[wxz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[wxz_level]
		AddGlobalCountNews("<color=green>"..GetName().."<color> t¨ng ®¼ng cÊp V¹n Tiªn trËn lªn "..wxz_level..", <color=green>®­îc Thiªn Hïng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][4].."<color>.",20)
		AddNormalItem(0,7,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
	CloseDialog()
end
function item_5()
	local wxz_level=GetTask(405)
	local sel_lvl=task_lvl_2_sel_lvl[wxz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[wxz_level]
		AddGlobalCountNews("<color=green>"..GetName().."<color> t¨ng ®¼ng cÊp V¹n Tiªn trËn lªn "..wxz_level..", <color=green>®­îc Thiªn Hïng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][5].."<color>.",20)
		AddNormalItem(0,9,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
	CloseDialog()
end

function get_suit_wxz()
	local level=GetLevel()
	if(level>90)then
		return 4
	elseif(level>70)then
		return 3
	elseif(level>50)then
		return 2
	else
		return 1
	end
end

function no()
	CloseDialog()
end

-- Exact original payload above. Explicit map-ID compatibility below.
pt_original_main = main
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID
function main()
    local w = pt_runtime_world_pos()
    if w ~= 1021 then CloseDialog(); return end
    return pt_original_main()
end
