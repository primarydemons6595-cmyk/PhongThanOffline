Include("\\script\\phongthan\\dialog_format.lua")
Include("\\script\\phongthan\\pet_attributes_data.lua")
PT_PET_FOOD_NAMES={[414]="#Phôc Ma Linh",[415]="#Hãa Linh T¸n",[416]="#Ph¸c Ngäc",[417]="#Tô Linh Ch©u"}
PT_PET_FOOD_DESCS={[414]="Gióp gäi Linh thó thøc giÊc.",[415]="<color=Cyan>ThÝch hîp<color> tÈy Linh lùc cho Linh Thó",[416]="Thøc ¨n cña Linh Long. Mçi lÇn sÏ t¨ng <color=Cyan>50 <color> ®iÓm c­êng tr¸ng",[417]="Thøc ¨n cña Linh Long. Mçi lÇn sÏ t¨ng <color=Cyan>5<color> ®iÓm tËp trung Linh lùc"}
PT_PET_SPECIES={
{name="Hå Hû MÞ",steps=PTAP_TaskTable,values={2071,2072,2073}},
{name="Na Tra",steps=PTAP_TaskTable_NeZha,values={2078,2079,2080}},
{name="L«i ChÊn Tö",steps=PTAP_TaskTable_LeiZhenZi,values={2091,2092,2093}},
{name="Th¹ch C¬ N­¬ng N­¬ng",steps=PTAP_TaskTable_ShiJi,values={2106,2107,2108}},
{name="Th¸i Êt Ch©n Nh©n",steps=PTAP_TaskTable_TaiYi,values={2114,2115,2116}},
{name="§¸t Kû",steps=PTAP_TaskTable_DaJi,values={2117,2118,2119}},
{name="Th©n C«ng B¸o",steps=PTAP_TaskTable_ShenGongBao,values={2120,2121,2122}},
{name="Hoàng Phi Hæ",steps=PTAP_TaskTable_HuangFeiHu,values={2123,2124,2125}},
}
for i=1,getn(PTAP_TaskTable_NewAllPet) do
 local row=PTAP_TaskTable_NewAllPet[i]
 tinsert(PT_PET_SPECIES,{name=row.petname,steps=row.task,values=row.taskvalue,description=row.wenzi})
end
PT_PET_MATERIAL_NAMES={
[41]="Lam b¶o th¹ch",
[487]="Néi ®¬n-Ch­ëng T©m L«i",
[488]="Néi ®¬n-L­u Tinh Th¹ch",
[489]="Néi ®¬n-B¨ng TuyÕt §¹n",
[490]="Néi ®¬n-TÝch LÞch Háa",
[491]="Néi ®¬n-Tinh Th«ng L«i HÖ",
[492]="Néi ®¬n-Thiªn Phong §Þa NhÉn",
[493]="Néi ®¬n-B¨ng C¬ TuyÕt Cèt",
[494]="Néi ®¬n-Phong L©m Háa S¬n",
[495]="Néi ®¬n-H¹n §Þa L«i",
[496]="Néi ®¬n-Tinh Th«ng Thæ HÖ",
[497]="Néi ®¬n-ThiÕt M· B¨ng Qua",
[498]="Néi ®¬n-Tinh Th«ng Háa HÖ",
[499]="Néi ®¬n-Phong V©n L«i §éng",
[500]="Néi ®¬n-Ngò Nh¹c TriÒu T«ng",
[501]="Néi ®¬n-Tinh Th«ng B¨ng HÖ",
[502]="Néi ®¬n-ThËp Ph­¬ng LiÖt Háa",
[503]="Néi ®¬n-L«i Phong Gi¸p",
[504]="Néi ®¬n-Thiªn B¨ng §Þa LiÖt",
[505]="Néi ®¬n-B¨ng Phong B¹o",
[506]="Néi ®¬n-Chóc Dung Ch©n KhÝ",
[507]="Néi ®¬n-L«i §éng Cöu Thiªn",
[508]="Néi ®¬n-HuyÒn N÷ Bæ Thiªn",
[509]="Néi ®¬n-B¨ng Phong V¹n Lý",
[510]="Néi ®¬n-Tam Muéi Ch©n Háa",
[511]="Néi ®¬n-TÕ HuyÕt Tr¶m",
[512]="Néi ®¬n-L¨ng Ba Vi Bé",
[513]="Néi ®¬n-Khai S¬n Tr¶m",
[514]="Néi ®¬n-Håi Phong Tr¶m",
[515]="Néi ®¬n-§iÖn Quang Tr¶m",
[516]="Néi ®¬n-Hoµnh Kh«ng Tr¶m",
[517]="Néi ®¬n-Tinh th«ng ®o¶n ®ao",
[518]="Néi ®¬n-Tinh th«ng tr­êng ®ao",
[519]="Néi ®¬n-Tam §Çu Lôc Thñ",
[520]="Néi ®¬n-HuyÒn B¨ng Tr¶m",
[521]="Néi ®¬n-Háa Quang Tr¶m",
[522]="Néi ®¬n-Liªn Hoµn Tr¶m",
[523]="Néi ®¬n-L¹c §Þa Tr¶m",
[524]="Néi ®¬n-ThuÇn D­¬ng Hé ThÓ",
[525]="Néi ®¬n-Thiªn Qu©n Tr¶m",
[526]="Néi ®¬n-Khuynh Thµnh NhÊt KÝch",
[527]="Néi ®¬n-Kim Cang Chó",
[528]="Néi ®¬n-Th«i Th©n Chó",
[529]="Néi ®¬n-Bæ T©m Chó",
[530]="Néi ®¬n-C­êng C«ng Chó",
[531]="Néi ®¬n-Ph¸ Gi¸p Chó",
[532]="Néi ®¬n-Bå §Ò Chó",
[533]="Néi ®¬n-Tr¶m T©m Chó",
[534]="Néi ®¬n-TËt Phong Chó",
[535]="Néi ®¬n-V¹n Cèt Toµn Kh«",
[536]="Néi ®¬n-Lùc SÜ TÕ",
[537]="Néi ®¬n-Tr­êng Cung TÕ",
[538]="Néi ®¬n-Thiªn Vò TÕ",
[539]="Néi ®¬n-Liªn Nâ TÕ",
[540]="Néi ®¬n-Háa L«i TÕ",
[541]="Néi ®¬n-To¸i Cèt TÕ",
[542]="Néi ®¬n-L­u Tinh TÕ",
[543]="Néi ®¬n-Truy Hån TÕ",
[544]="Néi ®¬n-Phong QuyÓn Tµn V©n",
[545]="BiÖn Thøc KÝnh",
[546]="Ph¸ Thñy Hoµn",
[547]="Ph¸ Thñy ThÊt Tinh Hoµn",
[548]="CÊn Thæ Hoµn",
[549]="CÊn Thæ ThËp Ph­¬ng Hoµn",
[550]="Tèn Phong Hoµn",
[551]="Tèn Phong B¸t VËn Hoµn",
[552]="Ly Háa Hoµn",
[553]="Ly Háa Cöu Cung Hoµn",
[554]="Néi §¬n (thÊp)",
[555]="Néi §¬n (trung)",
[556]="Néi §¬n (cao)",
[1012]="Bét Néi ®¬n",
[5108]="Ph¸ Thñy Hoµn (tinh chÕ)",
[5109]="Ph¸ Thñy ThÊt Tinh Hoµn (tinh chÕ)",
[5110]="CÊn Thæ Hoµn (tinh chÕ)",
[5111]="CÊn Thæ ThËp Ph­¬ng Hoµn (tinh chÕ)",
[5112]="Tèn Phong Hoµn (tinh chÕ)",
[5113]="Tèn Phong B¸t VËn Hoµn (tinh chÕ)",
[5114]="Ly Háa Hoµn (tinh chÕ)",
[5115]="Ly Háa Cöu Cung Hoµn (tinh chÕ)",
}
PT_PET_TRAINING_IB_NAMES={
[415]="Hãa Linh T¸n",
[1026]="Linh Quang KÝnh",
[1027]="Thiªn Tiªn Qu¶",
[1028]="Thiªn Quang Thñy",
[1409]="Phi Th¨ng Ng­ng Lé",
[5744]="Phi Th¨ng Ng­ng Lé (cao cÊp)",
}
PT_PET_SKILL_RECIPES={
{material=487,name="Néi ®¬n-Ch­ëng T©m L«i",book="Ch­ëng T©m L«i"},
{material=488,name="Néi ®¬n-L­u Tinh Th¹ch",book="L­u Tinh Th¹ch"},
{material=489,name="Néi ®¬n-B¨ng TuyÕt §¹n",book="B¨ng TuyÕt ®¹n"},
{material=490,name="Néi ®¬n-TÝch LÞch Háa",book="TÝch LÞch Háa"},
{material=491,name="Néi ®¬n-Tinh Th«ng L«i HÖ",book="Tinh Th«ng L«i HÖ"},
{material=492,name="Néi ®¬n-Thiªn Phong §Þa NhÉn",book="Thiªn Phong §Þa NhËn"},
{material=493,name="Néi ®¬n-B¨ng C¬ TuyÕt Cèt",book="B¨ng C¬ TuyÕt Cèt"},
{material=494,name="Néi ®¬n-Phong L©m Háa S¬n",book="Phong l©m háa s¬n"},
{material=495,name="Néi ®¬n-H¹n §Þa L«i",book="H¹n §Þa L«i"},
{material=496,name="Néi ®¬n-Tinh Th«ng Thæ HÖ",book="Tinh Th«ng Thæ HÖ"},
{material=497,name="Néi ®¬n-ThiÕt M· B¨ng Qua",book="ThiÕt M· B¨ng Qua"},
{material=498,name="Néi ®¬n-Tinh Th«ng Háa HÖ",book="Tinh Th«ng Háa HÖ"},
{material=499,name="Néi ®¬n-Phong V©n L«i §éng",book="Phong V©n L«i §éng"},
{material=500,name="Néi ®¬n-Ngò Nh¹c TriÒu T«ng",book="Ngò Nh¹c TriÒu T«ng"},
{material=501,name="Néi ®¬n-Tinh Th«ng B¨ng HÖ",book="Tinh Th«ng B¨ng HÖ"},
{material=502,name="Néi ®¬n-ThËp Ph­¬ng LiÖt Háa",book="ThËp Ph­¬ng LiÖt Háa"},
{material=503,name="Néi ®¬n-L«i Phong Gi¸p",book="L«i Phong Gi¸p"},
{material=504,name="Néi ®¬n-Thiªn B¨ng §Þa LiÖt",book="Thiªn B¨ng ®Þa liÖt"},
{material=505,name="Néi ®¬n-B¨ng Phong B¹o",book="B¨ng Phong B¹o"},
{material=506,name="Néi ®¬n-Chóc Dung Ch©n KhÝ",book="Chóc Dung Ch©n KhÝ"},
{material=507,name="Néi ®¬n-L«i §éng Cöu Thiªn",book="L«i §éng Cöu thiªn"},
{material=508,name="Néi ®¬n-HuyÒn N÷ Bæ Thiªn",book="HuyÒn N÷ Bæ Thiªn"},
{material=509,name="Néi ®¬n-B¨ng Phong V¹n Lý",book="B¨ng Phong V¹n Lý"},
{material=510,name="Néi ®¬n-Tam Muéi Ch©n Háa",book="Tam Muéi Ch©n Háa"},
{material=511,name="Néi ®¬n-TÕ HuyÕt Tr¶m",book="TÕ HuyÕt tr¶m"},
{material=512,name="Néi ®¬n-L¨ng Ba Vi Bé",book="L¨ng Ba Vi Bé"},
{material=513,name="Néi ®¬n-Khai S¬n Tr¶m",book="Khai s¬n tr¶m"},
{material=514,name="Néi ®¬n-Håi Phong Tr¶m",book="Håi Phong Tr¶m"},
{material=515,name="Néi ®¬n-§iÖn Quang Tr¶m",book="§iÖn Quang Tr¶m"},
{material=516,name="Néi ®¬n-Hoành Kh«ng Tr¶m",book="Hoành Kh«ng Tr¶m"},
{material=517,name="Néi ®¬n-Tinh th«ng ®o¶n ®ao",book="Tinh Th«ng §o¶n §ao"},
{material=518,name="Néi ®¬n-Tinh th«ng tr­êng ®ao",book="Tinh Th«ng Tr­êng §ao"},
{material=519,name="Néi ®¬n-Tam §Çu Lôc Thñ",book="Tam §Çu Lôc Thñ"},
{material=520,name="Néi ®¬n-HuyÒn B¨ng Tr¶m",book="HuyÒn B¨ng tr¶m"},
{material=521,name="Néi ®¬n-Háa Quang Tr¶m",book="Háa Quang Tr¶m"},
{material=522,name="Néi ®¬n-Liªn Hoàn Tr¶m",book="Liªn Hoàn Tr¶m"},
{material=523,name="Néi ®¬n-L¹c §Þa Tr¶m",book="L¹c §Þa Tr¶m"},
{material=524,name="Néi ®¬n-ThuÇn D­¬ng Hé ThÓ",book="ThuÇn D­¬ng Hé ThÓ"},
{material=525,name="Néi ®¬n-Thiªn Qu©n Tr¶m",book="Thiªn Qu©n Tr¶m"},
{material=526,name="Néi ®¬n-Khuynh Thành NhÊt KÝch",book="Khuynh Thành NhÊt KÝch"},
{material=527,name="Néi ®¬n-Kim Cang Chó",book="Kim Cang chó"},
{material=528,name="Néi ®¬n-Th«i Th©n Chó",book="Th«i Th©n Chó"},
{material=529,name="Néi ®¬n-Bæ T©m Chó",book="Bæ T©m Chó"},
{material=530,name="Néi ®¬n-C­êng C«ng Chó",book="C­êng C«ng chó"},
{material=531,name="Néi ®¬n-Ph¸ Gi¸p Chó",book="Ph¸ Gi¸p chó"},
{material=532,name="Néi ®¬n-Bå §Ò Chó",book="Bå §Ò chó"},
{material=533,name="Néi ®¬n-Tr¶m T©m Chó",book="Tr¶m T©m Chó"},
{material=534,name="Néi ®¬n-TËt Phong Chó",book="TËt Phong chó"},
{material=535,name="Néi ®¬n-V¹n Cèt Toàn Kh«",book="V¹n Cèt Toàn Kh«"},
{material=536,name="Néi ®¬n-Lùc SÜ TÕ",book="Lùc SÜ tÕ"},
{material=537,name="Néi ®¬n-Tr­êng Cung TÕ",book="Tr­êng Cung tÕ"},
{material=538,name="Néi ®¬n-Thiªn Vò TÕ",book="Thiªn Vò tÕ"},
{material=539,name="Néi ®¬n-Liªn Nâ TÕ",book="Liªn Nç TÕ"},
{material=540,name="Néi ®¬n-Háa L«i TÕ",book="Háa L«i TÕ"},
{material=541,name="Néi ®¬n-To¸i Cèt TÕ",book="To¸i Cèt tÕ"},
{material=542,name="Néi ®¬n-L­u Tinh TÕ",book="L­u Tinh tÕ"},
{material=543,name="Néi ®¬n-Truy Hån TÕ",book="Truy Hån tÕ"},
{material=544,name="Néi ®¬n-Phong QuyÓn Tàn V©n",book="Phong QuyÓn Tàn V©n"},
}
-- Reconstructed Lua 4 NPC entry. Observed original menu; original table text.
function pt_pet_close() CloseDialog() end
function pt_pet_beauty()
    local owned = PetGetInfo()
    if owned ~= 1 then Talk(1,"pt_pet_menu","B¹n ch­a nhËn nu«i Linh thó."); return end
    local cost = PetCosmeticCost(1)
    if cost < 0 then return end
    SayButtons("Chän kiÓu h×nh Linh thó. CÇn " .. PTDialogMaterial(cost,"T­íng Qu©n LÖnh") .. ".",5,
        "Lôc Phi Phi/pt_pet_beauty_confirm(1)", "TuyÕt Linh Thö/pt_pet_beauty_confirm(2)",
        "Tø BÊt T­îng/pt_pet_beauty_confirm(21)", "Ng©n Long/pt_pet_beauty_confirm(29)", "Quay l¹i/pt_pet_menu")
end
function pt_pet_beauty_confirm(kind)
    local cost = PetCosmeticCost(1)
    if cost < 0 then return end
    SayButtons("Làm ®Ñp Linh thó cÇn " .. PTDialogMaterial(cost,"T­íng Qu©n LÖnh") .. ". B¹n ®ång ý kh«ng?",2,
        "§ång ý/pt_pet_beauty_apply(" .. kind .. ")", "Quay l¹i/pt_pet_beauty")
end
function pt_pet_beauty_apply(kind)
    if PetCosmetic(1,kind,0) == 1 then
        Talk(1,"pt_pet_menu","§· ®æi kiÓu h×nh Linh thó. NhÊn F3 ®Ó xem.")
    else
        Talk(1,"pt_pet_beauty","Ch­a thÓ làm ®Ñp. KiÓm tra nguyªn liÖu, khãa hành trang và ®øng gÇn ThÇy t­íng sè.")
    end
end
function pt_pet_color()
    local owned = PetGetInfo()
    if owned ~= 1 then Talk(1,"pt_pet_menu","B¹n ch­a nhËn nu«i Linh thó."); return end
    local cost = PetCosmeticCost(2)
    if cost < 0 then return end
    SayButtons("Chän s¾c t­íng cho Linh thó.\nChi phÝ: " .. PTDialogMaterial(cost,"T­íng Qu©n LÖnh") .. ".",8,
        "Màu gèc/pt_pet_color_confirm(0,0)", "S¾c 60/pt_pet_color_confirm(60,0)",
        "S¾c 120/pt_pet_color_confirm(120,0)", "S¾c 180/pt_pet_color_confirm(180,0)",
        "S¾c 240/pt_pet_color_confirm(240,0)", "S¾c 300/pt_pet_color_confirm(300,0)",
        "Trong suèt/pt_pet_color_confirm(0,128)", "Quay l¹i/pt_pet_menu")
end
function pt_pet_color_confirm(hue,alpha)
    local cost = PetCosmeticCost(2)
    if cost < 0 then return end
    SayButtons("§æi màu Linh thó cÇn " .. PTDialogMaterial(cost,"T­íng Qu©n LÖnh") .. ". B¹n ®ång ý kh«ng?",2,
        "§ång ý/pt_pet_color_apply(" .. hue .. "," .. alpha .. ")", "Quay l¹i/pt_pet_color")
end
function pt_pet_color_apply(hue,alpha)
    if PetCosmetic(2,hue,alpha) == 1 then
        Talk(1,"pt_pet_menu","§· ®æi màu Linh thó. NhÊn F3 ®Ó xem.")
    else
        Talk(1,"pt_pet_color","Ch­a thÓ ®æi màu. KiÓm tra nguyªn liÖu, khãa hành trang và ®øng gÇn ThÇy t­íng sè.")
    end
end
function pt_pet_say_choices(text, choices)
    local args = {text, getn(choices)}
    for i=1,getn(choices) do tinsert(args,choices[i]) end
    call(SayButtons,args)
end
function pt_pet_menu()
    local message = "Muèn t×m hiÓu vÒ linh thó cø ®Õn t×m ta, xem kü phÇn <color=green>Giíi thiÖu linh thó<color> nhÐ!\n(NhÊn phÝm F3 ®Ó xem tr¹ng th¸i Linh thó)"
    if pt_pet_original_main and (GetTask(51) == 1 or GetTask(91) == 1 or GetTask(91) == 3) then
        SayButtons(message, 7, "Nu«i d­ìng/pt_pet_nurture", "Thuéc tÝnh Linh Sñng/pt_pet_attributes", "Làm ®Ñp pet/pt_pet_beauty", "§æi màu pet/pt_pet_color", "BiÕn h×nh thó triÖu håi/pt_summon_morph_menu", "TÞnh hãa và Néi §¬n/pt_pet_training_menu", "NhiÖm vô/pt_pet_original_main")
    else
        SayButtons(message, 6, "Nu«i d­ìng/pt_pet_nurture", "Thuéc tÝnh Linh Sñng/pt_pet_attributes", "Làm ®Ñp pet/pt_pet_beauty", "§æi màu pet/pt_pet_color", "BiÕn h×nh thó triÖu håi/pt_summon_morph_menu", "TÞnh hãa và Néi §¬n/pt_pet_training_menu")
    end
end
function pt_pet_nurture()
    SayButtons("Linh thó hÊp thu linh khÝ trêi ®Êt mà thành. NÕu quyÕt t©m nu«i d­ìng, ph¶i hÕt lßng b¶o vÖ nã!", 2,
        "NhËn nu«i/pt_pet_adopt", "Giíi thiÖu/pt_pet_intro")
end
function pt_pet_adopt()
    local owned = PetGetInfo()
    if owned < 0 then Talk(1,"pt_pet_nurture","Ch­a ®äc ®­îc th«ng tin Linh thó. H·y thö l¹i sau."); return end
    if owned == 1 then pt_pet_info(); return end
    local amount, silver = PetAdoptCost()
    if amount < 0 then Talk(1,"pt_pet_nurture","HiÖn ch­a thÓ nhËn nu«i Linh thó."); return end
    SayButtons("NhËn nu«i Linh thó cÇn:\n" .. PTDialogMaterial(amount,"Linh NguyÖn") .. "\n<color=yellow>" .. PTDialogMoney(silver) .. "<color>\nSau khi nhËn nu«i, nhÊn <color=green>F3<color> ®Ó ch¨m sãc Linh thó.", 2,
        "§ång ý nhËn nu«i/pt_pet_adopt_confirm", "Quay l¹i/pt_pet_nurture")
end
function pt_pet_adopt_confirm()
    if AdoptPet() == 1 then
        Talk(1,"pt_pet_info","§· nhËn nu«i Linh thó. NhÊn F3 ®Ó xem và ch¨m sãc Linh thó.")
    else
        Talk(1,"pt_pet_nurture","Ch­a thÓ nhËn nu«i. H·y kiÓm tra <color=cyan>Linh NguyÖn<color>, <color=yellow>ng©n l­îng<color>, khãa hành trang và ®øng gÇn <color=green>ThÇy t­íng sè<color>.")
    end
end
function pt_pet_info()
    local owned, kind, health, spirit, age, protect, strength, dexterity, constitution, intellect, aptitude, sex, name = PetGetInfo()
    if owned ~= 1 then Talk(1,"pt_pet_nurture","B¹n ch­a nhËn nu«i Linh thó."); return end
    local text = name .. "\nSøc kháe: " .. floor(health / 3600) .. "/100\nLinh lùc: " .. spirit .. "/300\nTr­ëng thành: " .. floor(age / 3600) .. " giê\nHé chñ: " .. protect .. "\nSøc m¹nh: " .. strength .. "  Th©n ph¸p: " .. dexterity .. "\nThÓ chÊt: " .. constitution .. "  Linh ho¹t: " .. intellect .. "\nT­ chÊt: " .. aptitude
    SayButtons(text, 2, "Giíi thiÖu/pt_pet_intro", "Quay l¹i/pt_pet_nurture")
end
function pt_pet_intro()
    SayButtons("Linh thó là linh vËt hÊp thu tinh hoa cña trêi ®Êt, h×nh d¸ng nhá nh¾n, chØ ¨n thøc ¨n quý hiÕm. Muèn cho ¨n, ph¶i hiÓu râ tÝnh khÝ cña nã!", 3,
        "Cho ¨n/pt_pet_intro_food", "B¶n lÜnh/pt_pet_intro_ability", "Làm ®Ñp/pt_pet_intro_beauty")
end
function pt_pet_intro_food()
    SayButtons("NhÊn F3, chän « thøc ¨n råi bÊm Cho ¨n ®Ó ch¨m sãc Linh thó.", 4,
        PT_PET_FOOD_NAMES[414] .. "/pt_pet_food_414", PT_PET_FOOD_NAMES[415] .. "/pt_pet_food_415",
        PT_PET_FOOD_NAMES[416] .. "/pt_pet_food_416", PT_PET_FOOD_NAMES[417] .. "/pt_pet_food_417")
end
function pt_pet_food_414() SayButtons(PT_PET_FOOD_DESCS[414],1,"Quay l¹i/pt_pet_intro_food") end
function pt_pet_food_415() SayButtons(PT_PET_FOOD_DESCS[415],1,"Quay l¹i/pt_pet_intro_food") end
function pt_pet_food_416() SayButtons(PT_PET_FOOD_DESCS[416],1,"Quay l¹i/pt_pet_intro_food") end
function pt_pet_food_417() SayButtons(PT_PET_FOOD_DESCS[417],1,"Quay l¹i/pt_pet_intro_food") end
function pt_pet_intro_ability()
    SayButtons("NhÊn F3 ®Ó chän chøc n¨ng hç trî cña Linh thó. §iÒu kiÖn tr­ëng thành, søc kháe và linh lùc ®­îc ghi t¹i tõng lùa chän.",2,"Xem Linh thó/pt_pet_info","Quay l¹i/pt_pet_intro")
end
function pt_pet_intro_beauty()
    SayButtons("Cã thÓ ®æi tªn Linh thó trong <color=green>F3<color>. §æi tªn cÇn " .. PTDialogMaterial(2,"T­íng Qu©n LÖnh") .. ".",1,"Quay l¹i/pt_pet_intro")
end
function pt_pet_attributes()
    SayButtons("Mçi Linh Sñng Thuéc TÝnh ®Òu së h÷u kü n¨ng siªu m¹nh, cã thÓ trî gióp anh hïng ph¸t triÓn nhanh h¬n, tung hoành ThÕ Giíi Phong ThÇn.",3,
        "Linh Sñng Thuéc TÝnh/pt_pet_species", "Kü n¨ng tæ hîp/pt_pet_combos", "Hép Linh Sñng B¸ch BiÕn/pt_pet_box")
end
function pt_pet_species(page)
    if page == nil then page = 1 end
    if page < 1 or page > getn(PT_PET_SPECIES) then return end
    local choices = {}
    local last = page + 7
    if last > getn(PT_PET_SPECIES) then last = getn(PT_PET_SPECIES) end
    for i=page,last do tinsert(choices,PT_PET_SPECIES[i].name .. "/pt_pet_species_info(" .. i .. ")") end
    if last < getn(PT_PET_SPECIES) then tinsert(choices,"Trang sau/pt_pet_species(" .. (last+1) .. ")") end
    tinsert(choices,"Quay l¹i/pt_pet_attributes")
    pt_pet_say_choices("Linh Sñng Thuéc TÝnh hiÕm cã, chØ khi ®¸p øng ®iÒu kiÖn liªn quan míi cã thÓ nhËn.",choices)
end
function pt_pet_species_info(index)
    if index == nil or index < 1 or index > getn(PT_PET_SPECIES) then return end
    local pet = PT_PET_SPECIES[index]
    local level = GetTaskByte(pet.values[1],2)
    local text = pet.name .. "\nCÊp: " .. level
    if pet.description then text = text .. "\n" .. pet.description end
    if level > 0 and level < getn(pet.steps) then
        local step = pet.steps[level]
        text = text .. "\nYªu cÇu qu¸i <color=yellow>cÊp " .. step.monster .. " trë lªn<color>: " .. GetTask(pet.values[2]) .. "/" .. step.num
    elseif level == 0 then
        text = text .. "\nH·y mang theo <color=cyan>QuyÓn Trôc<color> hoÆc <color=cyan>Hép Linh Sñng B¸ch BiÕn<color> t­¬ng øng."
    end
    SayButtons(text,1,"Quay l¹i/pt_pet_species")
end
function pt_pet_combos()
    local choices = {}
    for i=1,getn(PTAP_L_PETCOMBOS) do tinsert(choices,PTAP_L_PETCOMBOS[i].name .. "/pt_pet_combo_info(" .. i .. ")") end
    tinsert(choices,"Quay l¹i/pt_pet_attributes")
    pt_pet_say_choices("Kü n¨ng tæ hîp Linh Sñng",choices)
end
function pt_pet_combo_info(index)
    if index == nil or index < 1 or index > getn(PTAP_L_PETCOMBOS) then return end
    local combo = PTAP_L_PETCOMBOS[index]
    local text = combo.name .. "\n" .. combo.petname1 .. " + " .. combo.petname2 .. "\n" .. combo.info
    if GetTaskBit(PTAP_PetRoleCombosTask,combo.bit) == 1 then text = text .. "\n§· cã quyÒn sö dông." else text = text .. "\nCh­a cã quyÒn sö dông." end
    SayButtons(text,1,"Quay l¹i/pt_pet_combos")
end
function pt_pet_box()
    SayButtons("<color=cyan>Hép Linh Sñng B¸ch BiÕn<color> dïng cïng <color=cyan>QuyÓn Trôc Linh Sñng<color>. Chän Linh Sñng Thuéc TÝnh ®Ó xem cÊp và tiÕn ®é cña tõng Linh Sñng.",2,
        "Linh Sñng Thuéc TÝnh/pt_pet_species", "Quay l¹i/pt_pet_attributes")
end

-- Phuc dung Lua 4. Ten vat pham va ky nang lay tu bang PC goc.
function pt_pet_training_menu()
    SayButtons("TÞnh hãa và Néi §¬n Linh thó.\n<color=yellow>C¸c tû lÖ ngÉu nhiªn ®ang dïng cÊu h×nh phôc dùng t¹i server.<color>",8,
        "Tinh chÕ Hoàn/pt_pet_refine_menu", "TÞnh hãa lÇn 1/pt_pet_purify_menu",
        "Ph©n biÖt Âm D­¬ng/pt_pet_training_confirm(3,0,0,0)", "§æi t­ chÊt/pt_pet_training_confirm(4,0,0,0)",
        "Gi¸m ®Þnh Néi §¬n/pt_pet_identify_menu", "Häc kü n¨ng/pt_pet_skill_menu(1)", "TÞnh hãa lÇn 2/pt_pet_pair_confirm", "Quay l¹i/pt_pet_menu")
end
function pt_pet_pair_confirm()
    SayButtons("Tæ ®éi ®óng <color=green>2 ng­êi<color>, c¶ hai ®øng gÇn ThÇy T­íng Sè, cïng lo¹i Linh thó ®· tÞnh hãa lÇn 1, kh¸c Âm/D­¬ng và ®ñ <color=green>300 ®iÓm tÞnh hãa<color>.\nMçi ng­êi ph¶i tù bÊm x¸c nhËn trong vßng 60 gi©y. Sau thành c«ng: më « 4, ®iÓm tÞnh hãa vÒ 0, thuéc tÝnh vÒ ban ®Çu; gi÷ kü n¨ng ®· häc.",2,
        "X¸c nhËn tÞnh hãa lÇn 2/pt_pet_training_apply(8,0,0,0)","Quay l¹i/pt_pet_training_menu")
end
function pt_pet_refine_menu()
    local choices={}
    for i=0,3 do
        tinsert(choices,PT_PET_MATERIAL_NAMES[546+i*2] .. "/pt_pet_training_confirm(1," .. (i*2) .. ",0,0)")
    end
    tinsert(choices,"Quay l¹i/pt_pet_training_menu")
    pt_pet_say_choices("Tinh chÕ: " .. PTDialogMaterial(10,"Hoàn t­¬ng øng") .. " và <color=yellow>100 v¹n l­îng<color>. ThÊt b¹i vÉn tiªu hao nguyªn liÖu và tiÒn.",choices)
end
function pt_pet_purify_menu()
    SayButtons("LÇn 1 cÇn pet ®ang ho¹t ®éng, ®ñ <color=green>300 linh lùc<color>.\n" .. PTDialogMaterial(10,"Hoàn tinh chÕ t­¬ng øng") .. ", " .. PTDialogMaterial(1,PT_PET_TRAINING_IB_NAMES[415]) .. ", <color=yellow>500 v¹n l­îng<color>.\nCã thÓ thÊt b¹i và mÊt nguyªn liÖu, tiÒn.",5,
        "Tinh Minh Hå/pt_pet_training_confirm(2,0,0,0)", "§Ëu §Ëu Quy/pt_pet_training_confirm(2,1,0,0)",
        "Tra Tra §iÓu/pt_pet_training_confirm(2,2,0,0)", "BiÕn S¾c Long/pt_pet_training_confirm(2,3,0,0)", "Quay l¹i/pt_pet_training_menu")
end
function pt_pet_identify_menu()
    SayButtons("Gi¸m ®Þnh Néi §¬n cÇn <color=yellow>10 v¹n l­îng<color>. NhËn ngÉu nhiªn Néi §¬n kü n¨ng cïng bËc. §Ó hoàn nguyªn, chän Néi §¬n ®· gi¸m ®Þnh trong môc Häc kü n¨ng.",4,
        PT_PET_MATERIAL_NAMES[554] .. "/pt_pet_training_confirm(6,1,0,0)",
        PT_PET_MATERIAL_NAMES[555] .. "/pt_pet_training_confirm(6,2,0,0)",
        PT_PET_MATERIAL_NAMES[556] .. "/pt_pet_training_confirm(6,3,0,0)", "Quay l¹i/pt_pet_training_menu")
end
function pt_pet_skill_menu(page)
    if page==nil or page<1 then page=1 end
    if page>getn(PT_PET_SKILL_RECIPES) then return end
    local choices={}
    local last=page+7
    if last>getn(PT_PET_SKILL_RECIPES) then last=getn(PT_PET_SKILL_RECIPES) end
    for i=page,last do tinsert(choices,PT_PET_SKILL_RECIPES[i].name .. "/pt_pet_skill_slots(" .. i .. ")") end
    if page>1 then tinsert(choices,"Trang tr­íc/pt_pet_skill_menu(" .. (page-8) .. ")") end
    if last<getn(PT_PET_SKILL_RECIPES) then tinsert(choices,"Trang sau/pt_pet_skill_menu(" .. (last+1) .. ")") end
    tinsert(choices,"Quay l¹i/pt_pet_training_menu")
    pt_pet_say_choices("Chän Néi §¬n cã trong hành trang chÝnh. Server kiÓm tra kü n¨ng nh©n vËt ®· häc, « ®· më và nguyªn liÖu tr­íc khi trõ.",choices)
end
function pt_pet_skill_slots(index)
    if not PT_PET_SKILL_RECIPES[index] then return end
    local material=PT_PET_SKILL_RECIPES[index].material
    SayButtons(PT_PET_SKILL_RECIPES[index].name .. "\nChän « cÇn häc hoÆc n©ng cÊp. Kh«ng thÓ ghi ®Ì kü n¨ng kh¸c hay häc trïng «.",6,
        "¤ 1/pt_pet_skill_boost(" .. index .. ",0)","¤ 2/pt_pet_skill_boost(" .. index .. ",1)",
        "¤ 3/pt_pet_skill_boost(" .. index .. ",2)","¤ 4/pt_pet_skill_boost(" .. index .. ",3)",
        "Hoàn nguyªn/pt_pet_training_confirm(7," .. material .. ",0,0)","Quay l¹i/pt_pet_skill_menu(1)")
end
function pt_pet_skill_boost(index,slot)
    if not PT_PET_SKILL_RECIPES[index] then return end
    local material=PT_PET_SKILL_RECIPES[index].material
    SayButtons("Chän vËt phÈm hç trî häc kü n¨ng. Sè l­îng b»ng cÊp kü n¨ng ®Ých.\nThÊt b¹i vÉn tiªu hao tiÒn và nguyªn liÖu.",4,
        "Kh«ng hç trî/pt_pet_training_confirm(5," .. material .. "," .. slot .. ",0)",
        PT_PET_TRAINING_IB_NAMES[1409] .. "/pt_pet_training_confirm(5," .. material .. "," .. slot .. ",1)",
        PT_PET_TRAINING_IB_NAMES[5744] .. "/pt_pet_training_confirm(5," .. material .. "," .. slot .. ",2)","Quay l¹i/pt_pet_skill_slots(" .. index .. ")")
end
function pt_pet_training_confirm(action,value,slot,boost)
    local text=""
    if action==1 then
        text=PTDialogMaterial(10,PT_PET_MATERIAL_NAMES[546+value]) .. "\n<color=yellow>100 v¹n l­îng<color>"
    elseif action==2 then
        text=PTDialogMaterial(10,PT_PET_MATERIAL_NAMES[5108+value*2]) .. "\n" .. PTDialogMaterial(1,PT_PET_TRAINING_IB_NAMES[415]) .. "\n<color=yellow>500 v¹n l­îng<color>"
    elseif action==3 then
        text=PTDialogMaterial(10,PT_PET_MATERIAL_NAMES[41]) .. "\n" .. PTDialogMaterial(1,PT_PET_TRAINING_IB_NAMES[415]) .. "\n<color=yellow>500 v¹n l­îng<color>"
    elseif action==4 then
        text=PTDialogMaterial(1,PT_PET_TRAINING_IB_NAMES[1027]) .. "\nT­ chÊt cã thÓ t¨ng hoÆc gi¶m."
    elseif action==5 then
        local stage,purity,sex,s0,s1,s2,s3=PetTrainingInfo()
        local skills={s0,s1,s2,s3}
        local level=floor(skills[slot+1]/65536)+1
        local recipe=nil
        for i=1,getn(PT_PET_SKILL_RECIPES) do if PT_PET_SKILL_RECIPES[i].material==value then recipe=PT_PET_SKILL_RECIPES[i] end end
        if recipe==nil or level>4 then return end
        text="¤ " .. (slot+1) .. ", cÊp ®Ých " .. level .. "\n" .. PTDialogMaterial(level,recipe.name) .. "\n" .. PTDialogMaterial(level,recipe.book) .. "\n<color=yellow>" .. PTDialogMoney(2500000*level) .. "<color>"
        if boost>0 then
            local detail=1409
            if boost==2 then detail=5744 end
            text=text .. "\n" .. PTDialogMaterial(level,PT_PET_TRAINING_IB_NAMES[detail])
        end
    elseif action==6 then
        text=PTDialogMaterial(1,PT_PET_MATERIAL_NAMES[553+value]) .. "\n<color=yellow>10 v¹n l­îng<color>"
    elseif action==7 then
        text=PTDialogMaterial(1,PT_PET_MATERIAL_NAMES[value]) .. "\n" .. PTDialogMaterial(1,PT_PET_TRAINING_IB_NAMES[1026])
    else return end
    SayButtons(text .. "\nX¸c nhËn thùc hiÖn?",2,"§ång ý/pt_pet_training_apply(" .. action .. "," .. value .. "," .. slot .. "," .. boost .. ")","Quay l¹i/pt_pet_training_menu")
end
function pt_pet_training_apply(action,value,slot,boost)
    local result=PetTraining(action,value,slot,boost)
    if result==3 then Talk(1,"pt_pet_pair_confirm","§· x¸c nhËn. Ng­êi cßn l¹i trong tæ ®éi cÇn tù x¸c nhËn t¹i ThÇy T­íng Sè trong 60 gi©y. Ch­a thay ®æi Linh thó.")
    elseif result==1 then Talk(1,"pt_pet_training_menu","Thành c«ng. NhÊn F3 ®Ó xem Linh thó.")
    elseif result==2 then Talk(1,"pt_pet_training_menu","LÇn thùc hiÖn này thÊt b¹i. Nguyªn liÖu và ng©n l­îng ®· tiªu hao.")
    elseif result==-4 then Talk(1,"pt_pet_training_menu","Ch­a ®ñ nguyªn liÖu hoÆc ng©n l­îng trong hành trang chÝnh; còng cÇn chç trèng khi nhËn vËt phÈm. Ch­a tiªu hao.")
    elseif result==-6 then Talk(1,"pt_pet_training_menu","Nh©n vËt cÇn häc kü n¨ng t­¬ng øng tr­íc khi dïng Néi §¬n. Ch­a tiªu hao.")
    else Talk(1,"pt_pet_training_menu","Ch­a ®ñ ®iÒu kiÖn. KiÓm tra giai ®o¹n tÞnh hãa, linh lùc, « kü n¨ng, khãa hành trang và ®øng gÇn ThÇy T­íng Sè. Ch­a tiªu hao.") end
end

function pt_pet_craft_menu()
    SayButtons("GhÐp Néi §¬n t¹i XÝch Tïng Tö. Dïng Thiªn Quang Thñy sÏ b¶o ®¶m thành c«ng; kh«ng dïng cã thÓ thÊt b¹i và mÊt nguyªn liÖu, tiÒn.\n<color=yellow>Tû lÖ kh«ng dïng vËt phÈm hç trî là cÊu h×nh phôc dùng.<color>",4,
        "Néi §¬n thÊp/pt_pet_craft_boost(1)","Néi §¬n trung/pt_pet_craft_boost(2)","Néi §¬n cao/pt_pet_craft_boost(3)","Quay l¹i/pt_xich_menu")
end
function pt_pet_craft_boost(tier)
    if tier<1 or tier>3 then return end
    local material=552+tier
    if tier==1 then material=1012 end
    local money={2000000,10000000,50000000}
    SayButtons(PTDialogMaterial(5,PT_PET_MATERIAL_NAMES[material]) .. "\n<color=yellow>" .. PTDialogMoney(money[tier]) .. "<color>\nHç trî: " .. PTDialogMaterial(tier,PT_PET_TRAINING_IB_NAMES[1028]),3,
        "Dïng Thiªn Quang Thñy/pt_pet_craft_apply(" .. tier .. ",1)","Kh«ng dïng, cã thÓ thÊt b¹i/pt_pet_craft_apply(" .. tier .. ",0)","Quay l¹i/pt_pet_craft_menu")
end
function pt_pet_craft_apply(tier,boost)
    local result=PetTraining(9,tier,0,boost)
    if result==1 then Talk(1,"pt_pet_craft_menu","GhÐp Néi §¬n thành c«ng.")
    elseif result==2 then Talk(1,"pt_pet_craft_menu","GhÐp thÊt b¹i. Nguyªn liÖu và ng©n l­îng ®· tiªu hao.")
    else Talk(1,"pt_pet_craft_menu","Ch­a ®ñ ®iÒu kiÖn hoÆc nguyªn liÖu. CÇn ®øng gÇn XÝch Tïng Tö, më khãa hành trang và ®ñ chç nhËn vËt phÈm. Ch­a tiªu hao.") end
end

function pt_summon_morph_menu()
    SayButtons("§æi ngo¹i h×nh thó triÖu håi DÞ Nh©n. CÇn <color=yellow>9000 l­îng<color>. Ngo¹i h×nh ®­îc l­u riªng theo kü n¨ng triÖu håi; gi÷ tªn chñ, màu tªn và søc chiÕn ®Êu.",4,
        "Hoàng Kim Cù Nh©n/pt_summon_morph_confirm(359)", "ThiÕt Thè/pt_summon_morph_confirm(408)", "Anh Vò/pt_summon_morph_confirm(409)", "Quay l¹i/pt_pet_menu")
end
function pt_summon_morph_confirm(kind)
    SayButtons("CÇn <color=yellow>9000 l­îng<color>. B¹n ®ång ý ®æi h×nh thó triÖu håi ®ang gäi kh«ng?",2,
        "§ång ý/pt_summon_morph_apply(" .. kind .. ")", "Quay l¹i/pt_summon_morph_menu")
end
function pt_summon_morph_apply(kind)
    if SummonMorphAtSage(kind)==1 then Talk(1,"pt_summon_morph_menu","§· l­u ngo¹i h×nh thó triÖu håi. Gäi l¹i thó vÉn gi÷ h×nh ®· chän.")
    else Talk(1,"pt_summon_morph_menu","Ch­a thÓ ®æi h×nh. CÇn nh©n vËt DÞ Nh©n ®ang gäi thó, ®ñ ng©n l­îng, hành trang kh«ng khãa và ®øng gÇn ThÇy T­íng Sè.") end
end
