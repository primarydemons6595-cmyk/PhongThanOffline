Include("\\script\\phongthan\\dialog_format.lua")
--description: Í­½³-ÎäÆ÷ÏúÊÛÉÌ
--author: yichuan
--date: 2004/6/10

function main(sel)
	if(songxin()==0)then
		MsgBox(10475,"yes","no")
	end
end;

function songxin()
	local task_id = 874
	local map_id = 5

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
		Sale(12);
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
    if w ~= 1020 then CloseDialog(); return end
    return pt_original_main()
end
-- VNG Phi Tuyet service; retain existing shop, quests and other services.
pt_before_phi_tuyet_main=main
function main()
    if pt_runtime_world_pos()~=1020 then CloseDialog();return end
    Say("Thî §ång T©y Kú",3,"Thó c­ìi Phi TuyÕt/pt_phi_tuyet","Cöa hµng vµ dÞch vô kh¸c/pt_before_phi_tuyet_main","Rêi ®i/no")
end
function pt_phi_tuyet()
    if pt_runtime_world_pos()~=1020 then CloseDialog();return end
    Say("Chän hÖ thó c­ìi Phi TuyÕt muèn n©ng cÊp:",5,"Mua thó c­ìi Phi TuyÕt/pt_phi_buy","Gi¸p SÜ/pt_phi_js","§¹o SÜ/pt_phi_ds","DÞ Nh©n/pt_phi_yr","Quay l¹i/main")
end
function pt_phi_buy()
    if pt_runtime_world_pos()~=1020 then CloseDialog();return end
    CloseDialog();Sale(66)
end

function pt_phi_info_1()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(1)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Hoµng Phiªu M· (Tr¸c M·)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 1<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,400000)
 description=description..pt_phi_requirement("<color=cyan>LiÔu Méc<color>",wood,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_1","Quay l¹i/pt_phi_js")
end
function pt_phi_do_1()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(1)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c M·.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_2()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(2)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt ThiÕt Gi¸p Lang (Tr¸c Lang)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 2<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,800000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_2","Quay l¹i/pt_phi_js")
end
function pt_phi_do_2()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(2)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c Lang.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_3()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(3)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt §«ng B¾c Hæ (Tr¸c Hæ)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,500)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_3","Quay l¹i/pt_phi_js")
end
function pt_phi_do_3()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(3)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c Hæ.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_4()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(4)
 local description="<color=gold>ChÝ T«n Phi TuyÕt §«ng B¾c Hæ (ChÝ Hæ)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,10)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_4","Quay l¹i/pt_phi_js")
end
function pt_phi_do_4()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(4)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ Hæ.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_5()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(5)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Ngäc Kú L©n (Tr¸c L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,800)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_5","Quay l¹i/pt_phi_js")
end
function pt_phi_do_5()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(5)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_js()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 1/2",7,"Tr¸c ViÖt Hoµng Phiªu M· (Tr¸c M·)/pt_phi_info_1","Tr¸c ViÖt ThiÕt Gi¸p Lang (Tr¸c Lang)/pt_phi_info_2","Tr¸c ViÖt §«ng B¾c Hæ (Tr¸c Hæ)/pt_phi_info_3","ChÝ T«n §«ng B¾c Hæ (ChÝ Hæ)/pt_phi_info_4","Tr¸c ViÖt Ngäc Kú L©n (Tr¸c L©n)/pt_phi_info_5","Trang 2/pt_phi_js_2","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_info_6()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(6)
 local description="<color=gold>Siªu Phµm Phi TuyÕt Ngäc Kú L©n (Siªu phµm L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,3000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_6","Quay l¹i/pt_phi_js_2")
end
function pt_phi_do_6()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(6)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Siªu phµm L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_7()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(7)
 local description="<color=gold>ChÝ T«n Phi TuyÕt Ngäc Kú L©n (ChÝ L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,25)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,2)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_7","Quay l¹i/pt_phi_js_2")
end
function pt_phi_do_7()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(7)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_8()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(8)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt XÝch DiÖm Kú L©n (ViÖt L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1500)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,10)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_8","Quay l¹i/pt_phi_js_2")
end
function pt_phi_do_8()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(8)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ViÖt L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_9()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(9)
 local description="<color=gold>ChÝ T«n Phi TuyÕt XÝch DiÖm Kú L©n (ChÝ L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,8000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,30)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_9","Quay l¹i/pt_phi_js_2")
end
function pt_phi_do_9()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(9)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_10()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(10)
 local description="<color=gold>NghÞch Thiªn Phi TuyÕt XÝch DiÖm Kú L©n (NghÞch L©n)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi M· cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,10000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,120)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_10","Quay l¹i/pt_phi_js_2")
end
function pt_phi_do_10()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(10)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: NghÞch L©n.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_js_2()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 2/2",7,"Siªu Phµm Ngäc Kú L©n (Siªu phµm L©n)/pt_phi_info_6","ChÝ T«n Ngäc Kú L©n (ChÝ L©n)/pt_phi_info_7","Tr¸c ViÖt XÝch DiÖm Kú L©n (ViÖt L©n)/pt_phi_info_8","ChÝ T«n XÝch DiÖm Kú L©n (ChÝ L©n)/pt_phi_info_9","NghÞch Thiªn XÝch DiÖm Kú L©n (NghÞch L©n)/pt_phi_info_10","Trang 1/pt_phi_js","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_info_11()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(11)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt L©m Gian T­íc (Tr¸c T­íc)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 1<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,400000)
 description=description..pt_phi_requirement("<color=cyan>LiÔu Méc<color>",wood,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_11","Quay l¹i/pt_phi_ds")
end
function pt_phi_do_11()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(11)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c T­íc.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_12()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(12)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Th­¬ng ¦ng (Tr¸c ¦ng)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 2<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,800000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_12","Quay l¹i/pt_phi_ds")
end
function pt_phi_do_12()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(12)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c ¦ng.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_13()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(13)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt §¹i §iªu (Tr¸c §iªu)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,500)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_13","Quay l¹i/pt_phi_ds")
end
function pt_phi_do_13()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(13)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c §iªu.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_14()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(14)
 local description="<color=gold>ChÝ T«n Phi TuyÕt §¹i §iªu (ChÝ §iªu)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,10)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_14","Quay l¹i/pt_phi_ds")
end
function pt_phi_do_14()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(14)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ §iªu.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_15()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(15)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Kim Phông Hoµng (Tr¸c Ph­îng)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,800)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_15","Quay l¹i/pt_phi_ds")
end
function pt_phi_do_15()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(15)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c Ph­îng.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_ds()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 1/2",7,"Tr¸c ViÖt L©m Gian T­íc (Tr¸c T­íc)/pt_phi_info_11","Tr¸c ViÖt Th­¬ng ¦ng (Tr¸c ¦ng)/pt_phi_info_12","Tr¸c ViÖt §¹i §iªu (Tr¸c §iªu)/pt_phi_info_13","ChÝ T«n §¹i §iªu (ChÝ §iªu)/pt_phi_info_14","Tr¸c ViÖt Kim Phông Hoµng (Tr¸c Ph­îng)/pt_phi_info_15","Trang 2/pt_phi_ds_2","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_info_16()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(16)
 local description="<color=gold>Siªu Phµm Phi TuyÕt Kim Phông Hoµng (Siªu Ph­îng)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,3000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_16","Quay l¹i/pt_phi_ds_2")
end
function pt_phi_do_16()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(16)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Siªu Ph­îng.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_17()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(17)
 local description="<color=gold>ChÝ T«n Phi TuyÕt Kim Phông Hoµng (ChÝ Ph­îng)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,25)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,2)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_17","Quay l¹i/pt_phi_ds_2")
end
function pt_phi_do_17()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(17)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ Ph­îng.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_18()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(18)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt øng Long (Tr¸c Long)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1500)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,10)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_18","Quay l¹i/pt_phi_ds_2")
end
function pt_phi_do_18()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(18)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c Long.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_19()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(19)
 local description="<color=gold>ChÝ T«n Phi TuyÕt øng Long (ChÝ Long)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,8000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,30)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_19","Quay l¹i/pt_phi_ds_2")
end
function pt_phi_do_19()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(19)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ Long.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_20()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(20)
 local description="<color=gold>NghÞch Thiªn Phi TuyÕt øng Long (NghÞch Long)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi T­íc cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,10000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,120)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_20","Quay l¹i/pt_phi_ds_2")
end
function pt_phi_do_20()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(20)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: NghÞch Long.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_ds_2()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 2/2",7,"Siªu Phµm Kim Phông Hoµng (Siªu Ph­îng)/pt_phi_info_16","ChÝ T«n Kim Phông Hoµng (ChÝ Ph­îng)/pt_phi_info_17","Tr¸c ViÖt øng Long (Tr¸c Long)/pt_phi_info_18","ChÝ T«n øng Long (ChÝ Long)/pt_phi_info_19","NghÞch Thiªn øng Long (NghÞch Long)/pt_phi_info_20","Trang 1/pt_phi_ds","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_info_21()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(21)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Hoµng V©n Hå §iÖp (Tr¸c ®iÖp)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 1<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,400000)
 description=description..pt_phi_requirement("<color=cyan>LiÔu Méc<color>",wood,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_21","Quay l¹i/pt_phi_yr")
end
function pt_phi_do_21()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(21)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c ®iÖp.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_22()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(22)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Phong V©n Hå §iÖp (ViÖt ®iÖp)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 2<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,800000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_22","Quay l¹i/pt_phi_yr")
end
function pt_phi_do_22()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(22)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ViÖt ®iÖp.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_23()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(23)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Xuyªn V©n Hå §iÖp (Phi ®iÖp)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,500)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_23","Quay l¹i/pt_phi_yr")
end
function pt_phi_do_23()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(23)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Phi ®iÖp.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_24()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(24)
 local description="<color=gold>ChÝ T«n Phi TuyÕt Xuyªn V©n Hå §iÖp (ChÝ ®iÖp)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 3<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,1500000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,10)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 7<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_24","Quay l¹i/pt_phi_yr")
end
function pt_phi_do_24()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(24)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ ®iÖp.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_25()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(25)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Hoµng Linh Hå §iÖp (Tr¸c Linh)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,800)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_25","Quay l¹i/pt_phi_yr")
end
function pt_phi_do_25()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(25)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c Linh.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_yr()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 1/2",7,"Tr¸c ViÖt Hoµng V©n Hå §iÖp (Tr¸c ®iÖp)/pt_phi_info_21","Tr¸c ViÖt Phong V©n Hå §iÖp (ViÖt ®iÖp)/pt_phi_info_22","Tr¸c ViÖt Xuyªn V©n Hå §iÖp (Phi ®iÖp)/pt_phi_info_23","ChÝ T«n Xuyªn V©n Hå §iÖp (ChÝ ®iÖp)/pt_phi_info_24","Tr¸c ViÖt Hoµng Linh Hå §iÖp (Tr¸c Linh)/pt_phi_info_25","Trang 2/pt_phi_yr_2","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_info_26()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(26)
 local description="<color=gold>Siªu Phµm Phi TuyÕt Hoµng Linh Hå §iÖp (Siªu Linh)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,3000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1200)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,3)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_26","Quay l¹i/pt_phi_yr_2")
end
function pt_phi_do_26()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(26)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Siªu Linh.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_27()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(27)
 local description="<color=gold>ChÝ T«n Phi TuyÕt Hoµng Linh Hå §iÖp (ChÝ Linh)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 4<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,5000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,25)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,2)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 11<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_27","Quay l¹i/pt_phi_yr_2")
end
function pt_phi_do_27()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(27)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ Linh.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_28()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(28)
 local description="<color=gold>Tr¸c ViÖt Phi TuyÕt Phong L«i Hå §iÖp (Tr¸c L«i)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,8000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,30)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_28","Quay l¹i/pt_phi_yr_2")
end
function pt_phi_do_28()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(28)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: Tr¸c L«i.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_29()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(29)
 local description="<color=gold>ChÝ T«n Phi TuyÕt Phong L«i Hå §iÖp (ChÝ L«i)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,8000000)
 description=description..pt_phi_requirement("<color=cyan>§ång thau<color>",bronze,1000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,30)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_29","Quay l¹i/pt_phi_yr_2")
end
function pt_phi_do_29()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(29)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: ChÝ L«i.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_info_30()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local horse,money,wood,bronze,orders,jade,eye=PhiTuyetInventory(30)
 local description="<color=gold>NghÞch Thiªn Phi TuyÕt Phong L«i Hå §iÖp (NghÞch L«i)<color><enter>"
 description=description..pt_phi_requirement("<color=gold>Phi §iÖp cÊp 5<color>",horse,1)
 description=description..pt_phi_requirement("<color=yellow>B¹c (l­îng)<color>",money,10000000)
 description=description..pt_phi_requirement("<color=cyan>T­íng Qu©n LÖnh<color>",orders,120)
 description=description..pt_phi_requirement("<color=cyan>Lam b¶o th¹ch<color>",jade,5)
 description=description..pt_phi_requirement("<color=cyan>B¸ L¹c Nh·n cÊp 13<color>",eye,1)
 description=description.."§Æt thó vµ nguyªn liÖu trong tói chÝnh F4; th¸o thó tr­íc khi ®æi."
 Say(description,2,"X¸c nhËn n©ng cÊp/pt_phi_do_30","Quay l¹i/pt_phi_yr_2")
end
function pt_phi_do_30()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 local result=PhiTuyetUpgrade(30)
 if result==1 then Say("N©ng cÊp thµnh c«ng. Thó míi ®· vµo F4: NghÞch L«i.",1,"TiÕp tôc/pt_phi_tuyet")
 elseif result==-4 then Say("Kh«ng ®ñ b¹c mang theo.",1,"Quay l¹i/pt_phi_tuyet")
 else Say("Ch­a ®æi ®­îc: h·y ®øng gÇn Thî §ång T©y Kú, më khãa tói, kÕt thóc giao dÞch, kiÓm tra ®óng thó vµ nguyªn liÖu trong tói chÝnh F4 cïng chç trèng. LÇn ®æi thÊt b¹i kh«ng trõ b¹c hay vËt phÈm.",1,"Quay l¹i/pt_phi_tuyet") end
end

function pt_phi_yr_2()
 if pt_runtime_world_pos()~=1020 then CloseDialog();return end
 Say("Thó c­ìi Phi TuyÕt - trang 2/2",7,"Siªu Phµm Hoµng Linh Hå §iÖp (Siªu Linh)/pt_phi_info_26","ChÝ T«n Hoµng Linh Hå §iÖp (ChÝ Linh)/pt_phi_info_27","Tr¸c ViÖt Phong L«i Hå §iÖp (Tr¸c L«i)/pt_phi_info_28","ChÝ T«n Phong L«i Hå §iÖp (ChÝ L«i)/pt_phi_info_29","NghÞch Thiªn Phong L«i Hå §iÖp (NghÞch L«i)/pt_phi_info_30","Trang 1/pt_phi_yr","Chän hÖ kh¸c/pt_phi_tuyet")
end

function pt_phi_requirement(label,have,need)
 if strfind(label,"<color=yellow>",1,1) then return PTDialogMoneyRequirement(have,need) end
 local color="red"
 if have>=need then color="green" end
 return label..": <color="..color..">"..have.."/"..need.."<color><enter>"
end
