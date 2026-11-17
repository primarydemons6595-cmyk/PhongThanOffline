-- Original VNG source payload; provenance in deployment report.
--description: æûÍõ
--author: yichuan
--date: 2004/6/10



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

function main(sel)
	tasks = 
	{
		{"Hµnh thiÖn","pk";show=1},
		{"VÞ quèc lËp c«ng","renwu2";show=0},
		{"§¼ng cÊp","renwu3";show=1},
		{"Di b¸o","dongyi";show=0},
		{"Th­ Viªn Hång","kouxin";show=0}
	}
	if((1==GetTask(421))or(2==GetTask(421)))and(20<=GetTask(420)) then
		tasks[2].show=1;
	end;
	if(25==GetTask(597))and(HaveEventItem(110)>=1)and(GetTask(593)~=1)then
		tasks[4].show=1;
	end;
	SayTask(10121,tasks)
end;

function  pk()
	MsgBox("Ng­¬i nghiÖp ch­íng qu¸ nÆng, e r»ng ta kh«ng thÓ gióp ng­¬i! Khai Minh ®¶o ë §«ng H¶i cã thÓ gióp röa s¹ch ¸c t©m, thuyÒn phu cña TrÇn §­êng cã thÓ ®­a ng­¬i ®Õn ®ã.","yes_1","no")
end;

function yes_1()
	MsgBox("ChØ cÇn ë trªn ®¶o mét thêi gian sÏ tiªu trõ ®­îc s¸t khÝ.","no")
end

function renwu2()
	if(1==GetTask(421))or(2==GetTask(421))then--421=1Ê±Íê³ÉÈÎÎñ£¬=2Ê±Íê³ÉÈÎÎñÇÒ±¾³¡Ê¤Àû
		if ((23==GetTask(420))or(29==GetTask(420)))and(GetTask(373)==1)then
			reward_add()
		else
			Talk(1,"no","LÇn nµy ng­¬i ®· lËp ®¹i c«ng! Xin nhËn phÇn th­ëng!")
			reward_normal()
		end
	else
		CloseDialog()
	end
end

function   no()
		CloseDialog()
end;

function renwu3()
	local sz_level
	if (GetTask(420)>=20)then
		sz_level=GetTask(420)-20
	else
		sz_level=GetTask(420)
	end
	Talk(1,"no","§¼ng cÊp chiÕn tr­êng hiÖn t¹i cña ng­¬i lµ <color=green>"..sz_level.."<color>.")
end

function reward_normal()
	local sz_level=GetTask(420)-20
	if (10==sz_level)then
		AddOwnExp(GetLevel()*(1000+GetTask(421)*1000)*2);--½±Àø£¬°üÀ¨Ê¤Àû½±Àø
	else
		AddOwnExp(GetLevel()*(sz_level*100+GetTask(421)*1000)*2);--½±Àø£¬°üÀ¨Ê¤Àû½±Àø
		if (4==sz_level)and(60>GetLevel()) then
			Msg2Player("§¼ng cÊp cña b¹n ch­a ®Õn 60, kh«ng thÓ vµo chiÕn tr­êng Th­¬ng Chu.")
		else
			if (1==GetTask(373)) then--¾­ÑéÖµÒÑÂú£¬µÈ¼¶ÌáÉý
				SetTask(420,sz_level+1)
				SetTask(373,0)
				Msg2Player("§¼ng cÊp chiÕn tr­êng cña ng­¬i t¨ng "..GetTask(420))
			elseif (GetTask(373)<1)and(GetTask(373)>=0) then--¾­ÑéÖµÎ´Âú£¬¾­ÑéÖµ+1
				SetTask(373,GetTask(373)+1)
			else--¾­ÑéÖµÒç³ö»òÈ¡µÃ´íÎóÖµ£¬Ö±½ÓÖÃÎª1
				SetTask(373,1)
			end
		end
	end
	SetTask(421,0)
end

function reward_add()
	local sz_level=GetTask(420)-20
	local sel_idx=task_lvl_2_sel_idx[sz_level]
	if(sel_idx~=nil)then
		local sel_type=GetPlayerType()+1
		local item_list={}
		if(sz_level==9)then
			for i=2,4 do
				item_list[i-1]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
			end;
			Say("LÇn nµy ng­¬i ®· lËp ®¹i c«ng! Xin nhËn phÇn th­ëng!",3,item_list) 
                else
			for i=1,5 do
				item_list[i]=set_name[sel_type][sel_idx]..part_name[sel_type][i].."/item_"..i
			end
			Say("LÇn nµy ng­¬i ®· lËp ®¹i c«ng! Xin nhËn phÇn th­ëng!",5,item_list)
		end
	end
end

function item_1()
	CloseDialog()
	local sz_level=GetTask(420)-20
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green> ®­îc Trô V­¬ng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][1].."<color>.",20)
		AddNormalItem(0,2,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
end

function item_2()
	CloseDialog()
	local sz_level=GetTask(420)-20
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green> ®­îc Trô V­¬ng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][2].."<color>.",20)
		AddNormalItem(0,5,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end

end

function item_3()
	CloseDialog()
	local sz_level=GetTask(420)-20
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green> ®­îc Trô V­¬ng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][3].."<color>.",20)
		AddNormalItem(0,6,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
end
function item_4()
	CloseDialog()
	local sz_level=GetTask(420)-20
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green> ®­îc Trô V­¬ng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][4].."<color>.",20)
		AddNormalItem(0,7,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
end
function item_5()
	CloseDialog()
	local sz_level=GetTask(420)-20
	local sel_lvl=task_lvl_2_sel_lvl[sz_level]
	if(sel_lvl~=nil)then
		local player_type=GetPlayerType()+1
		local sel_idx=task_lvl_2_sel_idx[sz_level]
		sz_level=sz_level+1
		AddGlobalCountNews("<color=green>"..GetName().."<color> ®¼ng cÊp chiÕn tr­êng t¨ng "..sz_level..", <color=green> ®­îc Trô V­¬ng<color> tÆng <color=green>"..set_name[player_type][sel_idx]..part_name[player_type][5].."<color>.",20)
		AddNormalItem(0,9,player_type+5,sel_lvl,0,0,0)
		reward_normal()
	end
end

function dongyi()
	if(HaveEventItem(110)>=1)then
		if(GetTask(592)==0)then
			Talk(1,"no","Bän §«ng Di l¹i d¸m xua binh x©m ph¹m x· t¾c cña Thµnh Thang ta? Nghe lÖnh truyÒn, <color=yellow>Tiªu diÖt §«ng Di Man Nh©n<color>!")
			SetTask(593,1)
			AddOwnExp(4000) --¾­Ñé½±Àø
			Msg2Player("NhËn ®­îc 4000 ®iÓm kinh nghiÖm!")
			TopMessage("PhÇn th­ëng: <color=green>4000<color> ®iÓm kinh nghiÖm!")
		elseif(GetTask(592)==1)then
			DelEventItem(110)
			SetTask(593,1)
			Talk(1,"no","Bän §«ng Di l¹i d¸m xua binh x©m ph¹m x· t¾c cña Thµnh Thang ta? Nghe lÖnh truyÒn, <color=yellow>Tiªu diÖt §«ng Di Man Nh©n<color>!")
			SetTask(597,26)
			TaskNote(35,33)
			AddOwnExp(4000) --¾­Ñé½±Àø
			Msg2Player("NhËn ®­îc 4000 ®iÓm kinh nghiÖm!")
			TopMessage("PhÇn th­ëng: <color=green>4000<color> ®iÓm kinh nghiÖm!")
			Msg2Player("Håi b¸o §Æng Cöu C«ng!")
		end;
	end;
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1021_14; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1021 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("Nh©n vËt Trô V­¬ng ®èi tho¹i t¹i TriÒu Ca. Kh«ng tù t¹o boss sù kiÖn hoÆc b¶ng rít vËt phÈm.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Hoang Phi Ho (231/185); Phu An Su (225/191); Dac Ky (220/179); Chu Tuu Diem (227/195); Thong That Tau (222/192)", 2, "Quay l¹i/main", "§ãng/pt_close")
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

-- PT_THUONG_CHU_TGPT_20261103
pt_tc_previous_main=main
function main() Say("ChiÕn c«ng Th­¬ng Chu",6,"NhËn th­ëng trËn tr­íc/pt_tc_reward","50 danh väng ®æi T­íng Ên/pt_tc_exchange","200 T­íng Ên ®æi trang bÞ/pt_tc_gamble","Danh väng vµ c«ng tr¹ng/pt_tc_saved","NhiÖm vô kh¸c/pt_tc_old","KÕt thóc/pt_tc_close") end
function pt_tc_exchange() if ThuongChu(13,1)==1 then Talk(1,"main","§· ®æi 50 danh väng phe thµnh 1 T­íng Ên cã thÓ giao dÞch, bµy b¸n vµ vøt bá.") else Talk(1,"main","CÇn 50 danh väng ®óng phe vµ chç trèng trong tói.") end end
function pt_tc_gamble() local result=ThuongChu(24);if result==1 then Talk(1,"main","Thµnh c«ng: ®· nhËn trang bÞ lôc nhiÖm vô 8x-10x ®óng hÖ ph¸i.") elseif result==2 then Talk(1,"main","Ch­a thµnh c«ng, ®· nhËn l¹i 150 T­íng Ên.") else Talk(1,"main","CÇn ®ñ 200 T­íng Ên vµ chç trèng nhËn phÇn th­ëng.") end end
function pt_tc_old() return pt_tc_previous_main() end

function pt_tc_close() CloseDialog() end
function pt_tc_status()
 local names={"TruyÒn thèng","ViÔn Cæ","ViÔn Cæ Thèng So¸i"}
 local mode=ThuongChu(19);local name="Ch­a më";if mode>0 then name=names[mode] end
 Talk(1,"main","Phiªn nµy: "..name..". §iÓm Th­¬ng/Chu: "..ThuongChu(7,1).."/"..ThuongChu(7,2)..". Cê So¸i Th­¬ng/Chu: "..ThuongChu(26,1).."/"..ThuongChu(26,2)..".")
end
function pt_tc_saved() Talk(1,"main","TÝch lòy: "..ThuongChu(12).."; c«ng tr¹ng: "..ThuongChu(15).."; danh väng Th­¬ng: "..ThuongChu(14,1).."; danh väng Chu: "..ThuongChu(14,2)..".") end
function pt_tc_reward() local xp=ThuongChu(8);if xp>0 then Talk(1,"main","§· nhËn "..xp.." kinh nghiÖm.") else Talk(1,"main","Kh«ng cã th­ëng chê nhËn t¹i ®©y.") end end
function pt_tc_buff() if ThuongChu(25)==1 then Talk(1,"main","§· dïng 1200 ®iÓm tÝch lòy: t¨ng 10% sinh lùc, néi lùc vµ xuÊt chiªu phï hîp hÖ ph¸i trong 2 ngµy.") else Talk(1,"main","CÇn 1200 ®iÓm tÝch lòy vµ ch­a cã buff Th­¬ng Chu cßn h¹n.") end end
function pt_tc_rent() Say("Thuª 3 ngµy, trang bÞ nhiÖm vô khãa. Gi¸p +6: 20 T­íng Ên vµ 15000 c«ng tr¹ng. Vò khÝ: 30 T­íng Ên vµ 20000 c«ng tr¹ng.",5,"Trang bÞ lôc 8x/pt_tc_armor80","Trang bÞ lôc 10x/pt_tc_armor100","Vò khÝ 9x/pt_tc_weapon90","Vò khÝ 10x/pt_tc_weapon100","Quay l¹i/main") end
function pt_tc_rented(ok) if ok==1 then Talk(1,"main","§· nhËn trang bÞ thuª, h¹n dïng 3 ngµy kÓ tõ lóc nhËn.") else Talk(1,"main","Ch­a thuª ®­îc. KiÓm tra T­íng Ên, c«ng tr¹ng, chç trèng vµ trang bÞ ®óng hÖ ph¸i.") end end
function pt_tc_armor80() pt_tc_rented(ThuongChu(22,80)) end
function pt_tc_armor100() pt_tc_rented(ThuongChu(22,100)) end
function pt_tc_weapon90() pt_tc_rented(ThuongChu(23,90)) end
function pt_tc_weapon100() pt_tc_rented(ThuongChu(23,100)) end
