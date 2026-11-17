-- Original VNG source payload; provenance in deployment report.
--description: ×£ÈÚÍ¼ÌÚ-½ÌÁ·-ò¿ÓÈÄ¹10¼¶ÈÎÎñ
--author: yichuan
--date: 2004/5/15

function main(sel)
	tasks = 
	{
		{"Khai TrÝ","renwu1";show=0},
		{"ThÇn KhÝ","renwu2";show=0}
	}

	UTask_20 = GetTask(30);
	UTask_25 = GetTask(35);
	if (UTask_20==1) or (UTask_20==3)or (UTask_20==5)or(UTask_20==7)then
			tasks[1].show=1;
	end;

	if (UTask_25==7) and (HaveEventItem(27)>=1) then
				tasks[2].show=1;
	end;

	if (UTask_25==0) and(GetPlayerType()==2)and(GetLevel()>=7) then
				tasks[2].show=1;
	end;

	SayTask(10218,tasks)
end;

function   renwu1()
    if GetPlayerType() ~= 2 then return end
	UTask_20 = GetTask(30);
	if(UTask_20==1)then
			Talk(1,"no",10219)
			Msg2Player("§èi tho¹i víi Chóc Dung, nhËn ®­îc nh÷ng chØ dÉn quý gi¸!")
			TaskNote(13,2)
			SetTask(30,UTask_20+8)
	end;
	if(UTask_20==3)then
			Talk(1,"no",10219)
			Msg2Player("§èi tho¹i víi Chóc Dung, nhËn ®­îc nh÷ng chØ dÉn quý gi¸!")
			TaskNote(13,6)
			SetTask(30,UTask_20+8)
	end;
	if(UTask_20==5)then
			Talk(1,"no",10219)
			Msg2Player("§èi tho¹i víi Chóc Dung, nhËn ®­îc nh÷ng chØ dÉn quý gi¸!")
			TaskNote(13,4)
			SetTask(30,UTask_20+8)
	end;
	if(UTask_20==7)then
			Talk(1,"no",10219)
			Msg2Player("§èi tho¹i víi Chóc Dung, nhËn ®­îc nh÷ng chØ dÉn quý gi¸!")
			TaskNote(13,7)
			SetTask(30,UTask_20+8)
	end;
end;

function   renwu2()
    if GetPlayerType() ~= 2 then return end
	UTask_25 = GetTask(35);
	if (UTask_25==7) and (HaveEventItem(27)>=1) then
			Talk(1,"no",10220)
			DelEventItem(27)
			local n=random(0,5);				
			AddNormalItem(0,4,n,1,0,0)
			AddCredit(10)
			SetTask(35,8)
			Msg2Player("NhËn ®­îc mét ph¸p b¶o cÊp 10.")
			TaskNote(17,7)
	elseif (UTask_25==0) and(GetPlayerType()==2)and(GetLevel()>=7) then
			MsgBox(10221,"yes_1","no")
	end;
end;

function yes_1()
    if GetPlayerType() ~= 2 then return end
	Talk(1,"no",10222)
	SetTask(35,1)
	Msg2Player("§i t×m Céng C«ng hái tin tøc cña  ThÇn KhÝ.")
	TaskNote(17,0)
end;

function no()
		CloseDialog()
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1004_03; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1004 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("Totem Chóc Dung t¹i Xi V­u Mé. Dïng Lua theo totem cña map nµy, kh«ng dïng Lua Chóc Dung ë ViÔn Cæ.", 2, "Quay l¹i/main", "§ãng/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Cong Cong (192/204); Ho Tro Tan Thu (194/205); Phong Ba (195/204); Chuyen Sinh Lao Lao (196/202); Hinh Thien (194/200)", 2, "Quay l¹i/main", "§ãng/pt_close")
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

-- Read-only quest marker projection of the bound dialogue predicates.
function GetPlayerTaskState()
    if GetPlayerType() ~= 2 then return 0,0 end
    if GetTask(30)==1 or GetTask(30)==3 or GetTask(30)==5 or GetTask(30)==7 then return 3,1 end
    if GetTask(35)==7 and HaveEventItem(27)>=1 then return 3,7 end
    if GetTask(35)==0 and GetPlayerType()==2 and GetLevel()>=7 then return 1,7 end
    if GetTask(35)>0 and GetTask(35)<8 then return 2,7 end
    return 0,0
end

-- Enforce class ownership when projecting the original NPC's quest menu.
pt_class_original_say_task=SayTask
pt_class_quest_callbacks={renwu1=2, renwu2=2, yes_1=2}
function SayTask(text,choices)
    for i=1,getn(choices) do
        local expected=pt_class_quest_callbacks[choices[i][2]]
        if expected~=nil and GetPlayerType()~=expected then choices[i].show=0 end
    end
    return pt_class_original_say_task(text,choices)
end

-- PT_TWO_FACTION_BEGIN
PTG_BOOKS={{7,16,19,1,0,0,1},{7,17,20,1,0,0,1},{7,18,21,1,0,0,1},{7,19,22,1,0,0,1},{7,33,36,1,0,0,1},{7,34,37,1,0,0,1},{7,35,38,1,0,0,1},{7,36,39,1,0,0,1},{7,45,48,1,0,0,1},{7,46,49,1,0,0,1},{7,55,456,1,0,0,1},{7,60,1481,1,0,0,1},{7,66,1487,1,0,0,1},{7,81,1548,1,0,0,1},{7,83,1551,1,0,0,1}}
PT2_STEPS={{f=1,role="hoanglong",title="Chinh §å",level=25,task=1,from=0,to=1,note="Mang <color=yellow>th­ tiÕn cö<color> ®Õn <color=green>Kh­¬ng Tö Nha<color>, T©y Kú (158,189).",needs={},gifts={{4,0,0,0,0,0,1}},xp=0,money=0,credit=0,label="Chinh §å"},{f=1,role="khuong",title="Chinh §å",level=25,task=1,from=1,to=2,note="Hoµn thµnh Chinh §å.",needs={{4,0,0,0,0,0,1}},gifts={},xp=20000,money=30000,credit=0,label="Chinh §å"},{f=1,role="lei_tay",title="ThÕ Së",level=35,task=1,from=2,to=10,note="LÇn l­ît gÆp <color=green>Hoµng Phi Hæ<color>, <color=green>Lý TÞnh<color> vµ <color=green>§Æng Cöu C«ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="hoang",title="ThÕ Së",level=35,task=1,from=10,to=11,note="§· hái chuyÖn <color=green>Hoµng Phi Hæ<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="ly",title="ThÕ Së",level=35,task=1,from=10,to=12,note="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="dang",title="ThÕ Së",level=35,task=1,from=10,to=14,note="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="ly",title="ThÕ Së",level=35,task=1,from=11,to=13,note="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="dang",title="ThÕ Së",level=35,task=1,from=11,to=15,note="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="hoang",title="ThÕ Së",level=35,task=1,from=12,to=13,note="§· hái chuyÖn <color=green>Hoµng Phi Hæ<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="dang",title="ThÕ Së",level=35,task=1,from=12,to=16,note="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="dang",title="ThÕ Së",level=35,task=1,from=13,to=17,note="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="hoang",title="ThÕ Së",level=35,task=1,from=14,to=15,note="§· hái chuyÖn <color=green>Hoµng Phi Hæ<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="ly",title="ThÕ Së",level=35,task=1,from=14,to=16,note="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="ly",title="ThÕ Së",level=35,task=1,from=15,to=17,note="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="hoang",title="ThÕ Së",level=35,task=1,from=16,to=17,note="§· hái chuyÖn <color=green>Hoµng Phi Hæ<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÕ Së"},{f=1,role="lei_tay",title="ThÕ Së",level=35,task=1,from=17,to=20,note="Hoµn thµnh ThÕ Së.",needs={},gifts={{7,59,128,1,0,0,1}},xp=80000,money=0,credit=0,label="ThÕ Së"},{f=1,role="khuong",title="ThÇn Oanh",level=45,task=1,from=20,to=21,note="GÆp <color=green>Hoµng Thiªn Hãa<color>, TriÒu Ca (214,184).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Oanh"},{f=1,role="hoangthien",title="ThÇn Oanh",level=45,task=1,from=21,to=22,note="DiÖt <color=red>ThÇn Oanh<color> ë B¾c H¶i (213,204), (209,200), (228,203).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Oanh"},{f=1,role="thohanh",title="ThÇn Oanh",level=45,task=1,from=23,to=24,note="Mang <color=red>ThÇn Oanh<color> vÒ <color=green>Hoµng Thiªn Hãa<color>.",needs={{4,1,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="ThÇn Oanh"},{f=1,role="hoangthien",title="ThÇn Oanh",level=45,task=1,from=24,to=30,note="Hoµn thµnh <color=red>ThÇn Oanh<color>.",needs={},gifts={{0,0,33,4,1,0,1}},xp=240000,money=0,credit=0,label="ThÇn Oanh"},{f=1,role="thohanh",title="ThÇn Méc",level=55,task=1,from=30,to=31,note="Hái <color=green>Hå Hû MÞ<color>, TriÒu Ca (205,184).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Méc"},{f=1,role="ho",title="ThÇn Méc",level=55,task=1,from=31,to=32,note="DiÖt <color=red>BÊt Tö ThÇn Méc<color> ë Sa M¹c Phong ThÇn (212,181).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn Méc"},{f=1,role="ho",title="ThÇn Méc",level=55,task=1,from=33,to=40,note="Hoµn thµnh <color=yellow>ThÇn Méc<color>.",needs={{4,2,0,0,0,0,1}},gifts={{3,46,0,0,0,0,1}},xp=600000,money=0,credit=0,label="ThÇn Méc"},{f=2,role="hinh",title="Mao L­",level=25,task=2,from=0,to=1,note="GÆp <color=green>Hå Hû MÞ<color>, TriÒu Ca (205,184) nhËn <color=yellow>thiÕp mêi<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="Mao L­"},{f=2,role="ho",title="Mao L­",level=25,task=2,from=1,to=2,note="Mang <color=yellow>thiÕp mêi<color> vÒ <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",needs={},gifts={{4,15,0,0,0,0,1}},xp=0,money=0,credit=0,label="Mao L­"},{f=2,role="hinh",title="Mao L­",level=25,task=2,from=2,to=10,note="Hoµn thµnh Mao L­.",needs={{4,15,0,0,0,0,1}},gifts={},xp=20000,money=30000,credit=0,label="Mao L­"},{f=2,role="phonglam",title="BÊt Tóy",level=35,task=2,from=10,to=11,note="GÆp <color=green>Ng« Long<color> ë M¹nh T©n (209,195).",needs={},gifts={},xp=0,money=0,credit=0,label="BÊt Tóy"},{f=2,role="ngolong",title="BÊt Tóy",level=35,task=2,from=11,to=12,note="GÆp chñ töu ®iÕm, TriÒu Ca (227,195) lÊy <color=yellow>thuèc tØnh r­îu<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="BÊt Tóy"},{f=2,role="wine",title="BÊt Tóy",level=35,task=2,from=12,to=13,note="Mang <color=yellow>thuèc tØnh r­îu<color> vÒ <color=green>Ng« Long<color>.",needs={},gifts={{4,16,0,0,0,0,1}},xp=0,money=0,credit=0,label="BÊt Tóy"},{f=2,role="ngolong",title="BÊt Tóy",level=35,task=2,from=13,to=20,note="Hoµn thµnh BÊt Tóy.",needs={{4,16,0,0,0,0,1}},gifts={{7,59,128,1,0,0,1}},xp=80000,money=0,credit=0,label="BÊt Tóy"},{f=2,role="thuonghao",title="Cæ §ao",level=45,task=2,from=20,to=21,note="DiÖt <color=red>HiÖu óy Th­¬ng<color> ë M¹nh T©n (209,195), cøu téc nh©n.",needs={},gifts={},xp=0,money=0,credit=0,label="Cæ §ao"},{f=2,role="thuonghao",title="Cæ §ao",level=45,task=2,from=22,to=30,note="Hoµn thµnh Cæ §ao. Gi÷ chiÕc nhÉn cho nhiÖm vô <color=yellow>Ma HuyÕt<color>.",needs={},gifts={{4,17,0,0,0,0,1},{0,0,34,4,2,0,1}},xp=240000,money=0,credit=0,label="Cæ §ao"},{f=2,role="van",title="Ma HuyÕt",level=55,task=2,from=30,to=31,note="§Õn Tam S¬n diÖt <color=red>Háa Thó<color> (180,211), lÊy <color=yellow>Kim Qu¸n<color>.",needs={{4,17,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Ma HuyÕt"},{f=2,role="dabao",title="Ma HuyÕt",level=55,task=2,from=32,to=33,note="Mang <color=yellow>Ma HuyÕt<color> vÒ <color=green>V¨n Th¸i S­<color>, §ång Quan (193,207).",needs={{4,18,0,0,0,0,1}},gifts={{4,19,0,0,0,0,1}},xp=0,money=0,credit=0,label="Ma HuyÕt"},{f=2,role="van",title="Ma HuyÕt",level=55,task=2,from=33,to=34,note="GÆp <color=green>Hå Hû MÞ<color> ë TriÒu Ca (205,184).",needs={{4,19,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Ma HuyÕt"},{f=2,role="ho",title="Ma HuyÕt",level=55,task=2,from=34,to=40,note="Hoµn thµnh <color=yellow>Ma HuyÕt<color>.",needs={},gifts={{3,46,0,0,0,0,1}},xp=600000,money=0,credit=0,label="Ma HuyÕt"},{role="dac",title="B×nh An",level=65,task=1,from=40,to=41,note="GÆp <color=green>T©y V­¬ng MÉu<color> t¹i Diªu Tr×.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=1,from=41,to=42,note="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=1,from=43,to=50,note="Hoµn thµnh B×nh An.",needs={{4,3,0,0,0,0,1}},gifts={},xp=1400000,money=0,credit=0,randomBook=1,f=1,label="B×nh An"},{role="hienvien",title="ThÇn Dô KÝnh",level=70,task=1,from=50,to=51,note="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="ThÇn Dô KÝnh"},{role="hienvien",title="ThÇn Dô KÝnh",level=70,task=1,from=52,to=60,note="Hoµn thµnh <color=yellow>ThÇn Dô KÝnh<color>. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",needs={{4,4,0,0,0,0,1}},gifts={{0,0,33,7,1,0,1}},xp=2000000,money=0,credit=0,keepNeeds=1,f=1,label="ThÇn Dô KÝnh"},{role="dac",title="Tam S¸ch",level=75,task=1,from=60,to=61,note="Mang hai <color=yellow>b¶ng Phong ThÇn<color> tíi <color=green>Kh­¬ng Tö Nha<color>. Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={},gifts={},xp=0,money=0,credit=0,ensureBoard=1,f=1,label="Tam S¸ch"},{role="khuong",title="Tam S¸ch",level=75,task=1,from=61,to=62,note="Mang <color=yellow>ThÇn Dô KÝnh<color> tíi <color=green>Vâ V­¬ng<color>.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=1,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=1,from=62,to=63,note="LÊy ba bé s¸ch t¹i Khæn Tiªn Cung tÇng 2, 3 vµ 4.",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=1,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=1,from=63,to=70,note="Hoµn thµnh Tam S¸ch.",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},gifts={},xp=3000000,money=0,credit=0,f=1,randomFabao70=1,label="Tam S¸ch"},{role="tay",title="Cæ Kim",level=80,task=1,from=70,to=71,note="GÆp <color=green>§¾c Kû<color> t¹i TriÒu Ca m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1063,198,200},f=1,label="Cæ Kim"},{role="dac_future",title="Cæ Kim",level=80,task=1,from=71,to=72,note="GÆp <color=green>Nguyªn Thñy Thiªn T«n<color> t¹i Ngäc H­ Cung m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1062,203,198},f=1,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=1,from=72,to=73,note="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=1,from=74,to=80,note="Hoµn thµnh Cæ Kim. Dïng <color=yellow>Khø Lai Phï<color> ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",needs={{4,10,0,0,0,0,1}},gifts={{3,41,0,0,0,0,1},{6,1,34,1,0,0,1}},xp=5000000,money=0,credit=0,f=1,label="Cæ Kim"},{role="dac_young",title="Thiªn Duyªn",level=85,task=1,from=80,to=81,note="Nãi chuyÖn víi <color=green>Kh­¬ng Tö Nha<color> thêi niªn thiÕu.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=1,label="Thiªn Duyªn"},{role="khuong_young",title="Thiªn Duyªn",level=85,task=1,from=81,to=82,note="Trë l¹i gÆp <color=green>§¾c Kû<color> thêi niªn thiÕu.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=1,from=82,to=85,note="DiÖt <color=red>Bµn Cæ<color> ë Khæn Tiªn Cung tÇng 5, lÊy <color=yellow>Bµn Cæ ThÇn KhÝ<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=1,from=85,to=83,note="Hoµn thµnh Thiªn Duyªn.",needs={{4,175,0,0,0,0,1}},gifts={{0,5,7,8,9,0,1}},xp=6500000,money=0,credit=0,f=1,label="Thiªn Duyªn"},{role="khuong_young",title="Th«ng Thiªn",level=92,task=1,from=83,to=84,note="T×m Tham Lam t¹i Th­¬ng Nh©n §¹i Phong, V« Tri t¹i Cöu Di Tam S¬n; dïng MËt §å ë B¾c H¶i t×m Tµn NhÉn.",needs={},gifts={{6,1,142,1,0,0,1},{6,1,181,1,0,0,1}},xp=0,money=0,credit=0,f=1,label="Th«ng Thiªn"},{role="merchant",title="Th«ng Thiªn",level=92,task=4560,from=0,to=1,note="NhËn Tham Lam.",needs={},gifts={{4,176,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,f=1,label="Th«ng Thiªn"},{role="cuudi",title="Th«ng Thiªn",level=92,task=4561,from=0,to=1,note="NhËn V« Tri.",needs={},gifts={{4,177,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,f=1,label="Th«ng Thiªn"},{role="thong",title="Th«ng Thiªn",level=92,task=1,from=84,to=86,note="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=1,label="Th«ng Thiªn"},{role="khuong_young",title="Th«ng Thiªn: nhËn giµy",level=92,task=1,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,5,7,9,10,0,1}},xp=0,money=0,credit=0,rewardChoice=1,f=1,label="NhËn giµy"},{role="khuong_young",title="Th«ng Thiªn: nhËn yªu ®¸i",level=92,task=1,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,6,7,9,10,0,1}},xp=0,money=0,credit=0,rewardChoice=2,f=1,label="NhËn yªu ®¸i"},{role="khuong_young",title="Th«ng Thiªn: nhËn m·o",level=92,task=1,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,7,7,9,10,0,1}},xp=0,money=0,credit=0,rewardChoice=3,f=1,label="NhËn m·o"},{role="dac",title="B×nh An",level=65,task=2,from=40,to=41,note="GÆp <color=green>T©y V­¬ng MÉu<color> t¹i Diªu Tr×.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=2,from=41,to=42,note="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="B×nh An"},{role="tay",title="B×nh An",level=65,task=2,from=43,to=50,note="Hoµn thµnh B×nh An.",needs={{4,3,0,0,0,0,1}},gifts={},xp=1400000,money=0,credit=0,randomBook=1,f=2,label="B×nh An"},{role="xivuu",title="ThÇn Dô KÝnh",level=70,task=2,from=50,to=51,note="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="ThÇn Dô KÝnh"},{role="xivuu",title="ThÇn Dô KÝnh",level=70,task=2,from=52,to=60,note="Hoµn thµnh <color=yellow>ThÇn Dô KÝnh<color>. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",needs={{4,4,0,0,0,0,1}},gifts={{0,0,34,7,2,0,1}},xp=2000000,money=0,credit=0,keepNeeds=1,f=2,label="ThÇn Dô KÝnh"},{role="dac",title="Tam S¸ch",level=75,task=2,from=60,to=61,note="Mang hai <color=yellow>b¶ng Phong ThÇn<color> tíi <color=green>Kh­¬ng Tö Nha<color>. Gi÷ b¶ng thËt cho Thiªn Duyªn.",needs={},gifts={},xp=0,money=0,credit=0,ensureBoard=1,f=2,label="Tam S¸ch"},{role="khuong",title="Tam S¸ch",level=75,task=2,from=61,to=62,note="Mang <color=yellow>ThÇn Dô KÝnh<color> tíi <color=green>Vâ V­¬ng<color>.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=2,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=2,from=62,to=63,note="LÊy ba bé s¸ch t¹i Khæn Tiªn Cung tÇng 2, 3 vµ 4.",needs={{4,4,0,0,0,0,1},{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=2,label="Tam S¸ch"},{role="vo",title="Tam S¸ch",level=75,task=2,from=63,to=70,note="Hoµn thµnh Tam S¸ch.",needs={{4,6,0,0,0,0,1},{4,7,0,0,0,0,1},{4,8,0,0,0,0,1}},gifts={},xp=3000000,money=0,credit=0,f=2,randomFabao70=1,label="Tam S¸ch"},{role="tay",title="Cæ Kim",level=80,task=2,from=70,to=71,note="GÆp <color=green>§¾c Kû<color> t¹i TriÒu Ca m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1063,198,200},f=2,label="Cæ Kim"},{role="dac_future",title="Cæ Kim",level=80,task=2,from=71,to=72,note="GÆp <color=green>Nguyªn Thñy Thiªn T«n<color> t¹i Ngäc H­ Cung m­êi n¨m sau.",needs={},gifts={},xp=0,money=0,credit=0,travel={1062,203,198},f=2,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=2,from=72,to=73,note="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Cæ Kim"},{role="nguyen",title="Cæ Kim",level=80,task=2,from=74,to=80,note="Hoµn thµnh Cæ Kim. Dïng <color=yellow>Khø Lai Phï<color> ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",needs={{4,10,0,0,0,0,1}},gifts={{3,41,0,0,0,0,1},{6,1,34,1,0,0,1}},xp=5000000,money=0,credit=0,f=2,label="Cæ Kim"},{role="dac_young",title="Thiªn Duyªn",level=85,task=2,from=80,to=81,note="Nãi chuyÖn víi <color=green>Kh­¬ng Tö Nha<color> thêi niªn thiÕu.",needs={{4,9,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=2,label="Thiªn Duyªn"},{role="khuong_young",title="Thiªn Duyªn",level=85,task=2,from=81,to=82,note="Trë l¹i gÆp <color=green>§¾c Kû<color> thêi niªn thiÕu.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=2,from=82,to=85,note="DiÖt <color=red>Bµn Cæ<color> ë Khæn Tiªn Cung tÇng 5, lÊy <color=yellow>Bµn Cæ ThÇn KhÝ<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Thiªn Duyªn"},{role="dac_young",title="Thiªn Duyªn",level=85,task=2,from=85,to=83,note="Hoµn thµnh Thiªn Duyªn.",needs={{4,175,0,0,0,0,1}},gifts={{0,5,8,8,14,0,1}},xp=6500000,money=0,credit=0,f=2,label="Thiªn Duyªn"},{role="khuong_young",title="Th«ng Thiªn",level=92,task=2,from=83,to=84,note="T×m Tham Lam t¹i Th­¬ng Nh©n §¹i Phong, V« Tri t¹i Cöu Di Tam S¬n; dïng MËt §å ë B¾c H¶i t×m Tµn NhÉn.",needs={},gifts={{6,1,142,1,0,0,1},{6,1,181,1,0,0,1}},xp=0,money=0,credit=0,f=2,label="Th«ng Thiªn"},{role="merchant",title="Th«ng Thiªn",level=92,task=4560,from=0,to=1,note="NhËn Tham Lam.",needs={},gifts={{4,176,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,f=2,label="Th«ng Thiªn"},{role="cuudi",title="Th«ng Thiªn",level=92,task=4561,from=0,to=1,note="NhËn V« Tri.",needs={},gifts={{4,177,0,0,0,0,1}},xp=0,money=0,credit=0,mainPhase=84,f=2,label="Th«ng Thiªn"},{role="thong",title="Th«ng Thiªn",level=92,task=2,from=84,to=86,note="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",needs={{4,176,0,0,0,0,1},{4,177,0,0,0,0,1},{4,178,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,f=2,label="Th«ng Thiªn"},{role="khuong_young",title="Th«ng Thiªn: nhËn giµy",level=92,task=2,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,5,8,9,15,0,1}},xp=0,money=0,credit=0,rewardChoice=1,f=2,label="NhËn giµy"},{role="khuong_young",title="Th«ng Thiªn: nhËn yªu ®¸i",level=92,task=2,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,6,8,9,15,0,1}},xp=0,money=0,credit=0,rewardChoice=2,f=2,label="NhËn yªu ®¸i"},{role="khuong_young",title="Th«ng Thiªn: nhËn m·o",level=92,task=2,from=89,to=90,note="Hoµn thµnh Th«ng Thiªn.",needs={{4,174,0,0,0,0,1}},gifts={{0,7,8,9,15,0,1}},xp=0,money=0,credit=0,rewardChoice=3,f=2,label="NhËn m·o"},{role="doctor_manh",title="Trõ Häa: M¹nh T©n",level=18,task=4550,from=0,to=1,note="DiÖt <color=red>Hoµn CÈu<color> lÊy Ên ký, trong 10 gi©y diÖt <color=red>Gi¸p Cèt<color> lÊy ®ñ 5 <color=yellow>H¹p §»ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: tr¶ H¹p §»ng",level=18,task=4550,from=1,to=2,note="§Õn <color=green>§¹i Phu<color> t¹i Tam S¬n.",needs={{4,191,0,0,0,0,5}},gifts={},xp=6000,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: Tam S¬n",level=18,task=4550,from=2,to=3,note="H¸i 10 <color=yellow>Nguyªn S©m<color> t¹i Tam S¬n. CÈn thËn <color=red>Nguyªn Tham Tinh<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: tr¶ Nguyªn S©m",level=18,task=4550,from=3,to=4,note="§Õn <color=green>§¹i Phu<color> t¹i §ång Quan.",needs={{4,192,0,0,0,0,10}},gifts={},xp=8000,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: §ång Quan",level=18,task=4550,from=4,to=5,note="DiÖt <color=red>Ng­u S¸t<color>, dïng TÞnh Hãa Phï råi tÞnh hãa n¨m <color=yellow>S¬n Xuyªn LiÔu<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: tr¶ S¬n Xuyªn LiÔu",level=18,task=4550,from=5,to=6,note="Hoµn thµnh Trõ Häa.",needs={{4,201,0,0,0,0,5}},gifts={{0,2,7,2,6,0,1}},xp=10000,money=0,credit=0,f=1,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: M¹nh T©n",level=18,task=4550,from=0,to=1,note="DiÖt <color=red>Hoµn CÈu<color> lÊy Ên ký, trong 10 gi©y diÖt <color=red>Gi¸p Cèt<color> lÊy ®ñ 5 <color=yellow>H¹p §»ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Trõ Häa"},{role="doctor_manh",title="Trõ Häa: tr¶ H¹p §»ng",level=18,task=4550,from=1,to=2,note="§Õn <color=green>§¹i Phu<color> t¹i Tam S¬n.",needs={{4,191,0,0,0,0,5}},gifts={},xp=6000,money=0,credit=0,f=2,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: Tam S¬n",level=18,task=4550,from=2,to=3,note="H¸i 10 <color=yellow>Nguyªn S©m<color> t¹i Tam S¬n. CÈn thËn <color=red>Nguyªn Tham Tinh<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Trõ Häa"},{role="doctor_tam",title="Trõ Häa: tr¶ Nguyªn S©m",level=18,task=4550,from=3,to=4,note="§Õn <color=green>§¹i Phu<color> t¹i §ång Quan.",needs={{4,192,0,0,0,0,10}},gifts={},xp=8000,money=0,credit=0,f=2,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: §ång Quan",level=18,task=4550,from=4,to=5,note="DiÖt <color=red>Ng­u S¸t<color>, dïng TÞnh Hãa Phï råi tÞnh hãa n¨m <color=yellow>S¬n Xuyªn LiÔu<color>.",needs={},gifts={},xp=0,money=0,credit=0,f=2,label="Trõ Häa"},{role="doctor_dong",title="Trõ Häa: tr¶ S¬n Xuyªn LiÔu",level=18,task=4550,from=5,to=6,note="Hoµn thµnh Trõ Häa.",needs={{4,201,0,0,0,0,5}},gifts={{0,2,8,2,11,0,1}},xp=10000,money=0,credit=0,f=2,label="Trõ Häa"},{f=1,role="tuhang",title="B¸ch Lý",level=1,task=10,from=0,to=1,note="LÇn l­ît gÆp XÝch Tinh Tö, <color=green>Linh B¶o §¹i Ph¸p S­<color> vµ <color=green>Nam Cùc Tiªn ¤ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="xich",title="B¸ch Lý",level=1,task=10,from=1,to=9,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="linhbao",title="B¸ch Lý",level=1,task=10,from=1,to=3,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="namcuc",title="B¸ch Lý",level=1,task=10,from=1,to=5,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="xich",title="B¸ch Lý",level=1,task=10,from=3,to=11,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="namcuc",title="B¸ch Lý",level=1,task=10,from=3,to=7,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="xich",title="B¸ch Lý",level=1,task=10,from=5,to=13,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="linhbao",title="B¸ch Lý",level=1,task=10,from=5,to=7,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="xich",title="B¸ch Lý",level=1,task=10,from=7,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="linhbao",title="B¸ch Lý",level=1,task=10,from=9,to=11,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="namcuc",title="B¸ch Lý",level=1,task=10,from=9,to=13,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="namcuc",title="B¸ch Lý",level=1,task=10,from=11,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="linhbao",title="B¸ch Lý",level=1,task=10,from=13,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=1,role="tuhang",title="B¸ch Lý",level=1,task=10,from=15,to=16,note="Hoµn thµnh B¸ch Lý.",needs={},gifts={{1,0,1,1,0,0,3},{1,3,1,1,0,0,3}},xp=0,money=0,credit=0,label="B¸ch Lý"},{f=2,role="thieuhao",title="Khai TrÝ",level=1,task=30,from=0,to=1,note="LÇn l­ît gÆp <color=green>Phong B¸<color>, <color=green>Chóc Dung<color> vµ <color=green>Khoa Phô<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="phongba",title="Khai TrÝ",level=1,task=30,from=1,to=3,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="chucdung",title="Khai TrÝ",level=1,task=30,from=1,to=9,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="khoaphu",title="Khai TrÝ",level=1,task=30,from=1,to=5,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="chucdung",title="Khai TrÝ",level=1,task=30,from=3,to=11,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="khoaphu",title="Khai TrÝ",level=1,task=30,from=3,to=7,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="phongba",title="Khai TrÝ",level=1,task=30,from=5,to=7,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="chucdung",title="Khai TrÝ",level=1,task=30,from=5,to=13,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="chucdung",title="Khai TrÝ",level=1,task=30,from=7,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="phongba",title="Khai TrÝ",level=1,task=30,from=9,to=11,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="khoaphu",title="Khai TrÝ",level=1,task=30,from=9,to=13,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="khoaphu",title="Khai TrÝ",level=1,task=30,from=11,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="phongba",title="Khai TrÝ",level=1,task=30,from=13,to=15,note="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",needs={},gifts={},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=2,role="thieuhao",title="Khai TrÝ",level=1,task=30,from=15,to=16,note="Hoµn thµnh Khai TrÝ.",needs={},gifts={{1,0,1,1,0,0,3},{1,3,1,1,0,0,3}},xp=0,money=0,credit=0,label="Khai TrÝ"},{f=1,role="namcuc",title="Ngò ThÊt",level=3,task=11,from=0,to=1,note="GÆp <color=green>Hoµng Long Ch©n Nh©n<color>, Ngäc H­ Cung (204,195).",needs={},gifts={},xp=0,money=0,credit=0,label="Ngò ThÊt"},{f=1,role="hoanglong",title="Ngò ThÊt",level=3,task=11,from=1,to=2,note="§¸nh <color=red>TuyÕt Qu¸i<color> quanh ch©n nói C«n L«n, thu thËp 10 <color=yellow>B¨ng C¬<color> vÒ <color=green>Nam Cùc Tiªn ¤ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="Ngò ThÊt"},{f=1,role="namcuc",title="Ngò ThÊt",level=3,task=11,from=2,to=3,note="GÆp <color=green>Nhiªn §¨ng §¹o Nh©n<color>, Ngäc H­ Cung (206,192).",needs={{3,13,0,0,0,0,10}},gifts={},xp=0,money=0,credit=0,label="Ngò ThÊt"},{f=1,role="nhiendang",title="Ngò ThÊt",level=3,task=11,from=3,to=4,note="Mang Kh«ng Trung Háa vÒ <color=green>Nam Cùc Tiªn ¤ng<color>.",needs={},gifts={{4,20,0,0,0,0,1}},xp=0,money=0,credit=0,label="Ngò ThÊt"},{f=1,role="namcuc",title="Ngò ThÊt",level=3,task=11,from=4,to=5,note="Hoµn thµnh Ngò ThÊt.",needs={{4,20,0,0,0,0,1}},gifts={},xp=500,money=600,credit=0,label="Ngò ThÊt"},{f=1,role="vantrung",title="Kh¶o NghiÖm",level=7,task=15,from=0,to=1,note="§Õn <color=green>Linh B¶o §¹i Ph¸p S­<color> tr¶ lêi ba c©u hái: 2 - 1 - 3.",needs={},gifts={},xp=0,money=0,credit=0,label="Kh¶o NghiÖm"},{f=1,role="vantrung",title="Kh¶o NghiÖm",level=7,task=15,from=2,to=5,note="Tiªu diÖt <color=red>YÕm Háa<color> ë Thñ D­¬ng S¬n (168,192), råi vÒ <color=green>V©n Trung Tö<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="Kh¶o NghiÖm"},{f=1,role="vantrung",title="Kh¶o NghiÖm",level=7,task=15,from=6,to=7,note="Hoµn thµnh Kh¶o NghiÖm.",needs={},gifts={},xp=0,money=0,credit=10,randomFabao10=1,label="Kh¶o NghiÖm"},{f=1,role="nhiendang",title="Linh Lùc",level=14,task=14,from=0,to=1,note="§¸nh <color=red>B¨ng Lang<color> quanh ch©n nói, thu thËp 10 <color=yellow>Ngäc Cèt<color> vÒ <color=green>Nhiªn §¨ng §¹o Nh©n<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="Linh Lùc"},{f=1,role="nhiendang",title="Linh Lùc",level=14,task=14,from=1,to=2,note="Hoµn thµnh Linh Lùc.",needs={{3,9,0,0,0,0,10}},gifts={{7,58,62,1,0,0,1},{5,0,0,0,0,0,5}},xp=3000,money=0,credit=0,camp=7,label="Linh Lùc"},{f=2,role="hautho",title="T©n Thøc",level=3,task=31,from=0,to=1,note="GÆp <color=green>Cao Gi¸c<color>, <color=green>Xi V­u<color> Mé (205,197).",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{f=2,role="caogiac",title="T©n Thøc",level=3,task=31,from=1,to=2,note="GÆp <color=green>Cao Minh<color>, <color=green>Xi V­u<color> Mé (206,198).",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{f=2,role="cao",title="T©n Thøc",level=3,task=31,from=2,to=3,note="Mang <color=yellow>mËt th­<color> ®Õn <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",needs={},gifts={{4,29,0,0,0,0,1}},xp=0,money=0,credit=0,label="T©n Thøc"},{f=2,role="hinh",title="T©n Thøc",level=3,task=31,from=3,to=4,note="Trë vÒ gÆp <color=green>Cao Minh<color>.",needs={{4,29,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{f=2,role="cao",title="T©n Thøc",level=3,task=31,from=4,to=5,note="DiÖt <color=red>Háa DiÖn<color> quanh <color=green>Xi V­u<color> Mé, lÊy 10 <color=yellow>MÆt Quû<color> vÒ <color=green>HËu Thæ<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="T©n Thøc"},{f=2,role="hautho",title="T©n Thøc",level=3,task=31,from=5,to=6,note="Hoµn thµnh T©n Thøc.",needs={{3,12,0,0,0,0,10}},gifts={},xp=500,money=600,credit=0,label="T©n Thøc"},{f=2,role="chucdung",title="ThÇn KhÝ",level=7,task=35,from=0,to=1,note="GÆp <color=green>Céng C«ng<color>, <color=green>Xi V­u<color> Mé (192,204).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn KhÝ"},{f=2,role="congcong",title="ThÇn KhÝ",level=7,task=35,from=1,to=2,note="GÆp <color=green>Cao Minh<color>, <color=green>Xi V­u<color> Mé (206,198).",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn KhÝ"},{f=2,role="cao",title="ThÇn KhÝ",level=7,task=35,from=2,to=3,note="DiÖt <color=red>Th¶o Tiªn Bµ Bµ<color> ë Miªu C­¬ng (199,206), (187,197), (219,209), lÊy ba m¶nh thÇn khÝ vÒ <color=green>Céng C«ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="ThÇn KhÝ"},{f=2,role="congcong",title="ThÇn KhÝ",level=7,task=35,from=6,to=7,note="Mang <color=green>Xi V­u<color> ThÇn KhÝ vÒ <color=green>Chóc Dung<color>.",needs={{4,28,0,0,0,0,3}},gifts={{4,27,0,0,0,0,1}},xp=0,money=0,credit=0,label="ThÇn KhÝ"},{f=2,role="chucdung",title="ThÇn KhÝ",level=7,task=35,from=7,to=8,note="Hoµn thµnh ThÇn KhÝ.",needs={{4,27,0,0,0,0,1}},gifts={},xp=0,money=0,credit=10,randomFabao10=1,label="ThÇn KhÝ"},{f=2,role="phongba",title="Cøu TÕ",level=14,task=34,from=0,to=1,note="GÆp <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",needs={},gifts={},xp=0,money=0,credit=0,label="Cøu TÕ"},{f=2,role="hinh",title="Cøu TÕ",level=14,task=34,from=1,to=2,note="§Õn Cù Léc t×m <color=green>§Çu LÜnh<color> (239,192).",needs={},gifts={},xp=0,money=0,credit=0,label="Cøu TÕ"},{f=2,role="tocnhan",title="Cøu TÕ",level=14,task=34,from=2,to=3,note="DiÖt <color=red>§éc Lam Qu¸i<color> ë Cù Léc, lÊy l¹i l­¬ng thùc cho téc nh©n.",needs={},gifts={},xp=0,money=0,credit=0,label="Cøu TÕ"},{f=2,role="tocnhan",title="Cøu TÕ",level=14,task=34,from=4,to=5,note="Mang l­¬ng thùc vÒ <color=green>H×nh Thiªn<color>.",needs={{4,30,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,keepNeeds=1,label="Cøu TÕ"},{f=2,role="hinh",title="Cøu TÕ",level=14,task=34,from=5,to=6,note="Trë vÒ <color=green>Phong B¸<color> nhËn s¸ch kü n¨ng.",needs={{4,30,0,0,0,0,1}},gifts={},xp=0,money=0,credit=0,label="Cøu TÕ"},{f=2,role="phongba",title="Cøu TÕ",level=14,task=34,from=6,to=7,note="Hoµn thµnh Cøu TÕ.",needs={},gifts={{7,58,62,1,0,0,1},{5,0,0,0,0,0,5}},xp=2000,money=0,credit=0,camp=7,label="Cøu TÕ"},{f=1,role="namcuc",title="B¨ng Lang",level=14,task=4770,from=0,to=1,note="§Õn T©y C«n L«n ®¸nh <color=red>B¨ng Lang<color>, ®ñ 20 con sÏ dô ®­îc <color=red>B¨ng Lang V­¬ng<color>. DiÖt ®ñ 3 <color=red>B¨ng Lang V­¬ng<color> råi vÒ <color=green>Nam Cùc Tiªn ¤ng<color>.",needs={},gifts={},xp=0,money=0,credit=0,label="B¨ng Lang"},{f=1,role="namcuc",title="B¨ng Lang",level=14,task=4770,from=2,to=3,note="Hoµn thµnh <color=red>B¨ng Lang<color>.",needs={},gifts={},xp=1500,money=0,credit=0,randomTransform=1,label="B¨ng Lang"}}
PT2_NOTES={[1]={[1]="Mang <color=yellow>th­ tiÕn cö<color> ®Õn <color=green>Kh­¬ng Tö Nha<color>, T©y Kú (158,189).",[2]="Hoµn thµnh Chinh §å.",[10]="LÇn l­ît gÆp <color=green>Hoµng Phi Hæ<color>, <color=green>Lý TÞnh<color> vµ <color=green>§Æng Cöu C«ng<color>.",[11]="§· hái chuyÖn <color=green>Hoµng Phi Hæ<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[12]="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[14]="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[13]="§· hái chuyÖn <color=green>Lý TÞnh<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[15]="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[16]="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[17]="§· hái chuyÖn <color=green>§Æng Cöu C«ng<color>. GÆp ®ñ ba t­íng råi vÒ <color=green>L«i ChÊn Tö<color>.",[20]="Hoµn thµnh ThÕ Së.",[21]="GÆp <color=green>Hoµng Thiªn Hãa<color>, TriÒu Ca (214,184).",[22]="DiÖt <color=red>ThÇn Oanh<color> ë B¾c H¶i (213,204), (209,200), (228,203).",[24]="Mang <color=red>ThÇn Oanh<color> vÒ <color=green>Hoµng Thiªn Hãa<color>.",[30]="Hoµn thµnh <color=red>ThÇn Oanh<color>.",[31]="Hái <color=green>Hå Hû MÞ<color>, TriÒu Ca (205,184).",[32]="DiÖt <color=red>BÊt Tö ThÇn Méc<color> ë Sa M¹c Phong ThÇn (212,181).",[40]="Hoµn thµnh <color=yellow>ThÇn Méc<color>.",[41]="GÆp <color=green>T©y V­¬ng MÉu<color> t¹i Diªu Tr×.",[42]="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",[50]="Hoµn thµnh B×nh An.",[51]="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",[60]="Hoµn thµnh <color=yellow>ThÇn Dô KÝnh<color>. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",[61]="Mang hai <color=yellow>b¶ng Phong ThÇn<color> tíi <color=green>Kh­¬ng Tö Nha<color>. Gi÷ b¶ng thËt cho Thiªn Duyªn.",[62]="Mang <color=yellow>ThÇn Dô KÝnh<color> tíi <color=green>Vâ V­¬ng<color>.",[63]="LÊy ba bé s¸ch t¹i Khæn Tiªn Cung tÇng 2, 3 vµ 4.",[70]="Hoµn thµnh Tam S¸ch.",[71]="GÆp <color=green>§¾c Kû<color> t¹i TriÒu Ca m­êi n¨m sau.",[72]="GÆp <color=green>Nguyªn Thñy Thiªn T«n<color> t¹i Ngäc H­ Cung m­êi n¨m sau.",[73]="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",[80]="Hoµn thµnh Cæ Kim. Dïng <color=yellow>Khø Lai Phï<color> ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",[81]="Nãi chuyÖn víi <color=green>Kh­¬ng Tö Nha<color> thêi niªn thiÕu.",[82]="Trë l¹i gÆp <color=green>§¾c Kû<color> thêi niªn thiÕu.",[85]="DiÖt <color=red>Bµn Cæ<color> ë Khæn Tiªn Cung tÇng 5, lÊy <color=yellow>Bµn Cæ ThÇn KhÝ<color>.",[83]="Hoµn thµnh Thiªn Duyªn.",[84]="T×m Tham Lam t¹i Th­¬ng Nh©n §¹i Phong, V« Tri t¹i Cöu Di Tam S¬n; dïng MËt §å ë B¾c H¶i t×m Tµn NhÉn.",[86]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[23]="DiÖt <color=red>ThÇn Oanh<color> ë B¾c H¶i (213,204), (209,200), (228,203).",[33]="DiÖt <color=red>BÊt Tö ThÇn Méc<color> ë Sa M¹c Phong ThÇn (212,181).",[43]="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",[52]="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",[74]="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",[87]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[88]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[89]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån."},[2]={[1]="GÆp <color=green>Hå Hû MÞ<color>, TriÒu Ca (205,184) nhËn <color=yellow>thiÕp mêi<color>.",[2]="Mang <color=yellow>thiÕp mêi<color> vÒ <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",[10]="Hoµn thµnh Mao L­.",[11]="GÆp <color=green>Ng« Long<color> ë M¹nh T©n (209,195).",[12]="GÆp chñ töu ®iÕm, TriÒu Ca (227,195) lÊy <color=yellow>thuèc tØnh r­îu<color>.",[13]="Mang <color=yellow>thuèc tØnh r­îu<color> vÒ <color=green>Ng« Long<color>.",[20]="Hoµn thµnh BÊt Tóy.",[21]="DiÖt <color=red>HiÖu óy Th­¬ng<color> ë M¹nh T©n (209,195), cøu téc nh©n.",[30]="Hoµn thµnh Cæ §ao. Gi÷ chiÕc nhÉn cho nhiÖm vô <color=yellow>Ma HuyÕt<color>.",[31]="§Õn Tam S¬n diÖt <color=red>Háa Thó<color> (180,211), lÊy <color=yellow>Kim Qu¸n<color>.",[33]="Mang <color=yellow>Ma HuyÕt<color> vÒ <color=green>V¨n Th¸i S­<color>, §ång Quan (193,207).",[34]="GÆp <color=green>Hå Hû MÞ<color> ë TriÒu Ca (205,184).",[40]="Hoµn thµnh <color=yellow>Ma HuyÕt<color>.",[41]="GÆp <color=green>T©y V­¬ng MÉu<color> t¹i Diªu Tr×.",[42]="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",[50]="Hoµn thµnh B×nh An.",[51]="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",[60]="Hoµn thµnh <color=yellow>ThÇn Dô KÝnh<color>. Gi÷ kÝnh cho nhiÖm vô Tam S¸ch.",[61]="Mang hai <color=yellow>b¶ng Phong ThÇn<color> tíi <color=green>Kh­¬ng Tö Nha<color>. Gi÷ b¶ng thËt cho Thiªn Duyªn.",[62]="Mang <color=yellow>ThÇn Dô KÝnh<color> tíi <color=green>Vâ V­¬ng<color>.",[63]="LÊy ba bé s¸ch t¹i Khæn Tiªn Cung tÇng 2, 3 vµ 4.",[70]="Hoµn thµnh Tam S¸ch.",[71]="GÆp <color=green>§¾c Kû<color> t¹i TriÒu Ca m­êi n¨m sau.",[72]="GÆp <color=green>Nguyªn Thñy Thiªn T«n<color> t¹i Ngäc H­ Cung m­êi n¨m sau.",[73]="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",[80]="Hoµn thµnh Cæ Kim. Dïng <color=yellow>Khø Lai Phï<color> ®Õn Ngäc H­ Cung m­êi n¨m tr­íc.",[81]="Nãi chuyÖn víi <color=green>Kh­¬ng Tö Nha<color> thêi niªn thiÕu.",[82]="Trë l¹i gÆp <color=green>§¾c Kû<color> thêi niªn thiÕu.",[85]="DiÖt <color=red>Bµn Cæ<color> ë Khæn Tiªn Cung tÇng 5, lÊy <color=yellow>Bµn Cæ ThÇn KhÝ<color>.",[83]="Hoµn thµnh Thiªn Duyªn.",[84]="T×m Tham Lam t¹i Th­¬ng Nh©n §¹i Phong, V« Tri t¹i Cöu Di Tam S¬n; dïng MËt §å ë B¾c H¶i t×m Tµn NhÉn.",[86]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[22]="DiÖt <color=red>HiÖu óy Th­¬ng<color> ë M¹nh T©n (209,195), cøu téc nh©n.",[23]="DiÖt <color=red>HiÖu óy Th­¬ng<color> ë M¹nh T©n (209,195), cøu téc nh©n.",[32]="§Õn Tam S¬n diÖt <color=red>Háa Thó<color> (180,211), lÊy <color=yellow>Kim Qu¸n<color>.",[43]="DiÖt <color=red>Cù D·<color> ë BÝch Du Cung tÇng 4, lÊy <color=yellow>An C­ §å<color>.",[52]="DiÖt <color=red>Tr¹nh Nanh<color> ë Khæn Tiªn Cung tÇng 1, lÊy <color=yellow>ThÇn Dô KÝnh<color>.",[74]="DiÖt <color=red>Bé §Çu<color> qu©n Chu, lÊy <color=yellow>Tróc Gi¶n<color>.",[87]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[88]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån.",[89]="Dïng <color=yellow>Khø Lai Phï<color> gäi Thiªn Hån, §Þa Hån vµ MÖnh Hån."}}
PT2_UNITS={{f=1,task=10,id="3043",title="B¸ch Lý",rows={[1]="LÇn l­ît gÆp XÝch Tinh Tö, <color=green>Linh B¶o §¹i Ph¸p S­<color> vµ <color=green>Nam Cùc Tiªn ¤ng<color>.",[9]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[3]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[5]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[11]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[7]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[13]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[15]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh."},needs={}},{f=1,task=11,id="3044",title="Ngò ThÊt",rows={[1]="GÆp <color=green>Hoµng Long Ch©n Nh©n<color>, Ngäc H­ Cung (204,195).",[2]="§¸nh <color=red>TuyÕt Qu¸i<color> quanh ch©n nói C«n L«n, thu thËp 10 <color=yellow>B¨ng C¬<color> vÒ <color=green>Nam Cùc Tiªn ¤ng<color>.",[3]="GÆp <color=green>Nhiªn §¨ng §¹o Nh©n<color>, Ngäc H­ Cung (206,192).",[4]="Mang Kh«ng Trung Háa vÒ <color=green>Nam Cùc Tiªn ¤ng<color>."},needs={[2]={{3,13,0,0,0,0,10}},[4]={{4,20,0,0,0,0,1}}}},{f=1,task=15,id="3045",title="Kh¶o NghiÖm",rows={[1]="§Õn <color=green>Linh B¶o §¹i Ph¸p S­<color> tr¶ lêi ba c©u hái: 2 - 1 - 3.",[5]="Tiªu diÖt <color=red>YÕm Háa<color> ë Thñ D­¬ng S¬n (168,192), råi vÒ <color=green>V©n Trung Tö<color>.",[2]="Trë vÒ <color=green>V©n Trung Tö<color>, Ngäc H­ Cung (215,194) phôc mÖnh.",[6]="Tiªu diÖt <color=red>YÕm Háa<color> ë Thñ D­¬ng S¬n (168,192), råi vÒ <color=green>V©n Trung Tö<color>."},needs={}},{f=1,task=14,id="3046",title="Linh Lùc",rows={[1]="§¸nh <color=red>B¨ng Lang<color> quanh ch©n nói, thu thËp 10 <color=yellow>Ngäc Cèt<color> vÒ <color=green>Nhiªn §¨ng §¹o Nh©n<color>."},needs={[1]={{3,9,0,0,0,0,10}}}},{f=2,task=30,id="3047",title="Khai TrÝ",rows={[1]="LÇn l­ît gÆp <color=green>Phong B¸<color>, <color=green>Chóc Dung<color> vµ <color=green>Khoa Phô<color>.",[3]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[9]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[5]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[11]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[7]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[13]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh.",[15]="Nghe chØ dÉn cña ba vÞ råi trë vÒ phôc mÖnh."},needs={}},{f=2,task=31,id="3048",title="T©n Thøc",rows={[1]="GÆp <color=green>Cao Gi¸c<color>, <color=green>Xi V­u<color> Mé (205,197).",[2]="GÆp <color=green>Cao Minh<color>, <color=green>Xi V­u<color> Mé (206,198).",[3]="Mang <color=yellow>mËt th­<color> ®Õn <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",[4]="Trë vÒ gÆp <color=green>Cao Minh<color>.",[5]="DiÖt <color=red>Háa DiÖn<color> quanh <color=green>Xi V­u<color> Mé, lÊy 10 <color=yellow>MÆt Quû<color> vÒ <color=green>HËu Thæ<color>."},needs={[3]={{4,29,0,0,0,0,1}},[5]={{3,12,0,0,0,0,10}}}},{f=2,task=35,id="3049",title="ThÇn KhÝ",rows={[1]="GÆp <color=green>Céng C«ng<color>, <color=green>Xi V­u<color> Mé (192,204).",[2]="GÆp <color=green>Cao Minh<color>, <color=green>Xi V­u<color> Mé (206,198).",[3]="DiÖt <color=red>Th¶o Tiªn Bµ Bµ<color> ë Miªu C­¬ng (199,206), (187,197), (219,209), lÊy ba m¶nh thÇn khÝ vÒ <color=green>Céng C«ng<color>.",[7]="Mang <color=green>Xi V­u<color> ThÇn KhÝ vÒ <color=green>Chóc Dung<color>.",[4]="DiÖt <color=red>Th¶o Tiªn Bµ Bµ<color> ë Miªu C­¬ng (199,206), (187,197), (219,209), lÊy ba m¶nh thÇn khÝ vÒ <color=green>Céng C«ng<color>.",[5]="DiÖt <color=red>Th¶o Tiªn Bµ Bµ<color> ë Miªu C­¬ng (199,206), (187,197), (219,209), lÊy ba m¶nh thÇn khÝ vÒ <color=green>Céng C«ng<color>.",[6]="DiÖt <color=red>Th¶o Tiªn Bµ Bµ<color> ë Miªu C­¬ng (199,206), (187,197), (219,209), lÊy ba m¶nh thÇn khÝ vÒ <color=green>Céng C«ng<color>."},needs={[6]={{4,28,0,0,0,0,3}},[7]={{4,27,0,0,0,0,1}}}},{f=2,task=34,id="3050",title="Cøu TÕ",rows={[1]="GÆp <color=green>H×nh Thiªn<color>, <color=green>Xi V­u<color> Mé (194,200).",[2]="§Õn Cù Léc t×m <color=green>§Çu LÜnh<color> (239,192).",[3]="DiÖt <color=red>§éc Lam Qu¸i<color> ë Cù Léc, lÊy l¹i l­¬ng thùc cho téc nh©n.",[5]="Mang l­¬ng thùc vÒ <color=green>H×nh Thiªn<color>.",[6]="Trë vÒ <color=green>Phong B¸<color> nhËn s¸ch kü n¨ng.",[4]="DiÖt <color=red>§éc Lam Qu¸i<color> ë Cù Léc, lÊy l¹i l­¬ng thùc cho téc nh©n."},needs={[4]={{4,30,0,0,0,0,1}},[5]={{4,30,0,0,0,0,1}}}}}
PT2_HARM_NOTES={[1]="DiÖt <color=red>Hoµn CÈu<color> lÊy Ên ký, trong 10 gi©y diÖt <color=red>Gi¸p Cèt<color> lÊy ®ñ 5 <color=yellow>H¹p §»ng<color>.",[2]="§Õn <color=green>§¹i Phu<color> t¹i Tam S¬n.",[3]="H¸i 10 <color=yellow>Nguyªn S©m<color> t¹i Tam S¬n. CÈn thËn <color=red>Nguyªn Tham Tinh<color>.",[4]="§Õn <color=green>§¹i Phu<color> t¹i §ång Quan.",[5]="DiÖt <color=red>Ng­u S¸t<color>, dïng TÞnh Hãa Phï råi tÞnh hãa n¨m <color=yellow>S¬n Xuyªn LiÔu<color>.",[6]="Hoµn thµnh Trõ Häa."}
PT2_KILLS={}
PT2_TRANSFORMS={{6,1,42,1,0,0,1},{6,1,43,1,0,0,1},{6,1,44,1,0,0,1},{6,1,45,1,0,0,1},{6,1,46,1,0,0,1},{6,1,47,1,0,0,1},{6,1,48,1,0,0,1},{6,1,49,1,0,0,1},{6,1,50,1,0,0,1},{6,1,51,1,0,0,1},{6,1,52,1,0,0,1},{6,1,53,1,0,0,1},{6,1,54,1,0,0,1},{6,1,55,1,0,0,1},{6,1,56,1,0,0,1},{6,1,57,1,0,0,1},{6,1,58,1,0,0,1},{6,1,59,1,0,0,1},{6,1,60,1,0,0,1},{6,1,61,1,0,0,1},{6,1,62,1,0,0,1},{6,1,63,1,0,0,1},{6,1,64,1,0,0,1},{6,1,65,1,0,0,1},{6,1,66,1,0,0,1},{6,1,67,1,0,0,1},{6,1,68,1,0,0,1}}
PT2_LOCAL_NAMES={[1]="TuyÕt Qu¸i",[4]="B¨ng Lang",[2]="Háa DiÖn",[5]="Lôc Qu¸i",[6]="Cuång §iÓu",[9]="Th¶o Tiªn",[18]="H¾c Phong"}
-- This file is appended only to explicitly selected quest NPC/item scripts.
-- Old menus remain available; no client-wide dialogue rendering changes.
function PT2Class() local f=GetPlayerType();return f==1 or f==2 end
function PT2MainTask() if GetPlayerType()==1 then return 1 end return 2 end
function PT2GiaoRole() return (GetPlayerType()==1 and PT2_ROLE=="giao_dao") or (GetPlayerType()==2 and PT2_ROLE=="giao_di") end
function PT2Have(rows)
 for i=1,getn(rows) do local r=rows[i];if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end end
 return 1
end
function PT2Ready(a)
 return PT2Class() and a.f==GetPlayerType() and a.role==PT2_ROLE and GetContentWorldPos()+1000==PT2_MAP and
 GetLevel()>=a.level and GetTask(a.task)==a.from and (not a.mainPhase or GetTask(PT2MainTask())==a.mainPhase)
end
function PT2Journal()
 if not PT2Class() then return end
 local phase=GetTask(PT2MainTask())
 local title=GetPlayerType()==1 and "Chñ tuyÕn §¹o SÜ" or "Chñ tuyÕn DÞ Nh©n"
 local note=PT2_NOTES[GetPlayerType()][phase]
 local id=GetPlayerType()==1 and "3040" or "3041"
 if note then PtTwoFactionNote(id,1,"<color=yellow>"..title.."<color><enter>"..note,phase)
 else PtTwoFactionNote(id,2," ",phase) end
 PtTwoFactionNote(GetPlayerType()==1 and "3041" or "3040",2," ",0)
 for i=1,getn(PT2_UNITS) do local q=PT2_UNITS[i]
  if q.f==GetPlayerType() then
   local state=GetTask(q.task);local row=q.rows[state]
   if row then
    local text="<color=yellow>"..q.title.."<color><enter>"..row
    local needs=q.needs[state]
    if needs then for j=1,getn(needs) do local need=needs[j];local count=HaveNormalItem(need[1],need[2],need[3],need[4]);if count>need[7] then count=need[7] end;text=text.."<enter>TiÕn ®é: "..count.."/"..need[7] end end
    if q.task==35 and state>=3 and state<=6 then text=text.."<enter>M¶nh thÇn khÝ: "..HaveEventItem(28).."/3" end
    PtTwoFactionNote(q.id,1,text,state)
   else PtTwoFactionNote(q.id,2," ",state) end
  else PtTwoFactionNote(q.id,2," ",0) end
 end
 local state=GetTask(4550)
 if state>0 and state<6 then
 local progress=""
 if state==1 then progress="<enter>H¹p §»ng: "..HaveEventItem(191).."/5"
 elseif state==3 then progress="<enter>Nguyªn S©m: "..HaveEventItem(192).."/10"
 elseif state==5 then progress="<enter>LiÔu Méc: "..HaveEventItem(201).."/5" end
 PtTwoFactionNote("3042",1,"<color=yellow>Trõ Häa<color><enter>"..PT2_HARM_NOTES[state]..progress,state)
 else PtTwoFactionNote("3042",2," ",state) end
 if GetPlayerType()==1 and GetTask(4770)==1 then PtTwoFactionNote("3052",1,"<color=yellow>B¨ng Lang<color><enter>T©y C«n L«n: diÖt <color=red>B¨ng Lang<color> "..GetTask(4771).."/20; <color=red>B¨ng Lang V­¬ng<color> "..GetTask(4772).."/3.",GetTask(4771)+GetTask(4772)*21)
 elseif GetPlayerType()==1 and GetTask(4770)==2 then PtTwoFactionNote("3052",1,"VÒ <color=green>Nam Cùc Tiªn ¤ng<color>, Ngäc H­ Cung (209,191) phôc mÖnh.",2)
 else PtTwoFactionNote("3052",2," ",0) end
 if GetTask(4554)>0 then
  local home=GetPlayerType()==1 and "Ngäc H­ Cung (217,202)" or "Xi V­u Mé (200,200)"
  PtTwoFactionNote("3053",1,"<color=yellow>Gi¸o HuÊn<color><enter>DiÖt <color=red>"..PT2_LOCAL_NAMES[GetTask(4554)].."<color> quanh ph¸i: "..GetTask(4555).."/40.<enter>VÒ T¹p Hãa Th­¬ng Nh©n, "..home..".",GetTask(4555))
 else PtTwoFactionNote("3053",2," ",0) end
end
function pt2_close() CloseDialog() end
function PT2Notice(text) SayButtons(text,1,"Quay l¹i/main") end
function PT2Preview(i)
 local a=PT2_STEPS[i];if not PT2Ready(a) then return end
 local text="<color=yellow>"..a.title.."<color><enter>"..a.note
 if a.xp>0 then text=text.."<enter><color=green>Kinh nghiÖm: "..a.xp.."<color>" end
 if a.money>0 then text=text.."<enter><color=green>L­îng: "..a.money.."<color>" end
 SayButtons(text,2,"TiÕp tôc/pt2_yes"..i,"Quay l¹i/main")
end
function PT2Apply(i)
 local a=PT2_STEPS[i]
 if not PT2Ready(a) or GetTrade()~=0 or GetLockState()~=0 then return end
 if PT2Have(a.needs)~=1 then PT2Notice("Ch­a ®ñ vËt phÈm.<enter>"..a.note);return end
 local needs=a.needs;local gifts=a.gifts
 if a.keepNeeds then needs={} end
 if a.ensureBoard then gifts={};local count=HaveEventItem(9);if count<3 then gifts={{4,9,0,0,0,0,3-count}} end end
 if a.randomBook then gifts={PTG_BOOKS[RANDOM(1,getn(PTG_BOOKS))]} end
 if a.randomTransform then gifts={PT2_TRANSFORMS[RANDOM(1,getn(PT2_TRANSFORMS))]} end
 if a.randomFabao10 then gifts={{0,4,RANDOM(0,3),1,0,0,1}} end
 if a.randomFabao70 then gifts={{0,4,RANDOM(14,17),1,0,0,1}} end
 local old=GetCash()
 if a.money>0 then if old>2000000000-a.money then return end;Earn(a.money);if GetCash()~=old+a.money then return end end
 if QuestExchange(a.task,a.from,a.to,needs,gifts,1)~=1 then
  if a.money>0 then Pay(a.money) end
  PT2Notice("Ch­a thÓ giao nhiÖm vô. H·y më khãa vËt phÈm vµ dµnh ®ñ « trèng trong hµnh trang.");return
 end
 if a.xp>0 then AddOwnExp(a.xp) end
 if a.credit>0 then AddCredit(a.credit) end
 if a.camp then SetCamp(a.camp) end
 if a.task==PT2MainTask() and a.to==84 then SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0) end
 if a.task==4770 and a.to==1 then SetTask(4771,0);SetTask(4772,0) end
 PT2Journal()
 if a.travel then NewWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8) end
 PT2Notice("<color=yellow>"..a.title.."<color><enter>"..a.note)
end
function pt2_services() if PT2_OLD_MAIN then return PT2_OLD_MAIN() end CloseDialog() end
function PT2Menu()
 if not PT2Class() then return PT2_OLD_MAIN and PT2_OLD_MAIN() end
 PT2Journal();local options={}
 for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i]
  if PT2Ready(a) and PT2Have(a.needs)==1 then tinsert(options,a.label.."/pt2_view"..i) end
 end
 if PT2_ROLE=="thong" and GetTask(PT2MainTask())==88 then tinsert(options,"Khiªu chiÕn/pt2_challenge") end
 if PT2GiaoRole() and GetLevel()>=20 then
  if PT2GiaoReady()==1 then tinsert(options,"Gi¸o HuÊn/pt2_giao_preview") end
  if GetTask(4554)>0 and GetTask(4555)>=40 then tinsert(options,"NhËn th­ëng/pt2_giao_claim") end
 end
 if getn(options)==0 then return pt2_services() end
 if PT2_OLD_MAIN then tinsert(options,"DÞch vô/pt2_services") end
 if getn(options)==0 then CloseDialog();return end
 SayButtons("<color=yellow>NhiÖm vô<color><enter>Chän nhiÖm vô ®Ó xem môc tiªu vµ phÇn th­ëng.",getn(options),options)
end
function PT2Marker()
 if not PT2Class() then if PT2_OLD_MARK then return PT2_OLD_MARK() end return 0,0 end
 local best,level=0,0
 for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i];if PT2Ready(a) then
  local state=PT2Have(a.needs)==1 and (a.from==0 and 1 or 3) or 2
  if state>best then best=state;level=a.level end
 end end
 if PT2GiaoRole() and GetLevel()>=20 then
  if GetTask(4554)>0 then return GetTask(4555)>=40 and 3 or 2,20 end
  if PT2GiaoReady()==1 then return 1,20 end
 end
 if best==0 and PT2_OLD_MARK then return PT2_OLD_MARK() end
 return best,level
end
function PT2GiaoReady()
 if not PT2Class() or GetLevel()<20 or GetTask(4554)~=0 then return 0 end
 if GetLevel()>=29 then return GetTask(4558)==0 and 1 or 0 end
 local today=floor((SystemTime()+25200)/86400)
 return (GetTask(4556)~=today or GetTask(4557)<3) and 1 or 0
end
function pt2_giao_preview()
 if not PT2GiaoRole() or PT2GiaoReady()~=1 then return end
 SayButtons("<color=yellow>Gi¸o HuÊn<color><enter>DiÖt 40 qu¸i quanh ph¸i theo chØ dÉn.<enter><color=green>4600 kinh nghiÖm, 3000 l­îng.<color>",2,"NhËn nhiÖm vô/pt2_giao_take","Quay l¹i/main")
end
function pt2_giao_take()
 if not PT2GiaoRole() or PT2GiaoReady()~=1 or GetContentWorldPos()+1000~=PT2_MAP or GetTrade()~=0 or GetLockState()~=0 then return end
 local pool=GetPlayerType()==1 and {1,4} or {2,5,6,9,18};local target=pool[RANDOM(1,getn(pool))]
 if QuestExchange(4554,0,target,{},{},1)~=1 then return end
 local today=floor((SystemTime()+25200)/86400)
 if GetTask(4556)~=today then SetTask(4556,today);SetTask(4557,0) end
 SetTask(4557,GetTask(4557)+1);SetTask(4555,0);SetTask(4559,GetPlayerType()==1 and 1008 or 1011)
 if GetLevel()>=29 then SetTask(4558,1) end
 PT2Journal();PT2Notice("DiÖt <color=red>"..PT2_LOCAL_NAMES[target].."<color> quanh ph¸i theo chØ dÉn nhiÖm vô.")
end
function pt2_giao_claim()
 if not PT2GiaoRole() or GetContentWorldPos()+1000~=PT2_MAP or GetTask(4554)<=0 or GetTask(4555)<40 or GetTrade()~=0 or GetLockState()~=0 then return end
 local before=GetCash();if before>1999997000 then return end;Earn(3000);if GetCash()~=before+3000 then return end
 if QuestExchange(4554,GetTask(4554),0,{},{},1)~=1 then Pay(3000);return end
 SetTask(4555,0);AddOwnExp(4600);PT2Journal();PT2Notice("§· nhËn 4600 kinh nghiÖm vµ 3000 l­îng.")
end
function PT2Event(id,limit) if HaveEventItem(id)>=limit then return 0 end return AddEventItem(id) end
function PT2Death(template)
 if not PT2Class() then return end
 local task=PT2MainTask();local phase=GetTask(task);local map=GetContentWorldPos()+1000
 for i=1,getn(PT2_KILLS) do local k=PT2_KILLS[i]
  if k[1]==GetPlayerType() and k[2]==template and k[3]==map and phase==k[4] and PT2Event(k[6],1)>0 and k[4]~=k[5] then SetTask(task,k[5]) end
 end
 if GetTask(4550)==1 and map==1015 then
  if template==7 then if AddIBBuff(335,10)>0 then SetTask(4551,SystemTime()+10);TopMessage(11631) else SetTask(4551,0) end end
  if template==13 and GetTask(4551)>SystemTime() and HaveIBBuff(335)==1 then PT2Event(191,5) end
 end
 -- Ng­u S¸t is original template 12. Its bounded native reward hook
 -- grants TÜnh Hãa Phï; template 18 is H¾c Phong, not this objective.
 if phase==87 and map==1070 and template>=541 and template<=543 then
  local bit=2^(template-541);local mask=GetTask(4562)
  if mod(floor(mask/bit),2)==0 then SetTask(4562,mask+bit) end
  if GetTask(4562)==7 then SetTask(task,88);SetTask(4563,SystemTime()+180) end
 end
 if template==2703 and map==1070 and phase==88 and SystemTime()<=GetTask(4563) and PT2Event(174,1)>0 then SetTask(task,89) end
 PT2Journal()
end
function pt2_challenge()
 if not PT2Class() or PT2_ROLE~="thong" or GetContentWorldPos()+1000~=1070 or GetTask(PT2MainTask())~=88 then return end
 if SystemTime()>GetTask(4563) then
  for id=176,178 do for n=1,2 do if HaveEventItem(id)>0 then DelEventItem(id) end end end
  SetTask(PT2MainTask(),83);SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0);PT2Journal();return
 end
 local previous=GetTask(4567)
 if previous>0 and GetNpcTemplateID(previous)==2703 and GetNpcWorldPos(previous)==1070 and GetNpcLife(previous)>0 then return end
 local npc=AddNpc(2703,95,SubWorld,204*256+128,210*512+256,1)
 if npc and npc>0 then if SetNpcDeathScript(npc,"\\script\\phongthan\\giapsi\\death_2703.lua")==1 then SetTask(4567,npc) else DelNpc(npc) end end
 CloseDialog()
end

function pt2_view1() PT2Preview(1) end
function pt2_yes1() PT2Apply(1) end

function pt2_view2() PT2Preview(2) end
function pt2_yes2() PT2Apply(2) end

function pt2_view3() PT2Preview(3) end
function pt2_yes3() PT2Apply(3) end

function pt2_view4() PT2Preview(4) end
function pt2_yes4() PT2Apply(4) end

function pt2_view5() PT2Preview(5) end
function pt2_yes5() PT2Apply(5) end

function pt2_view6() PT2Preview(6) end
function pt2_yes6() PT2Apply(6) end

function pt2_view7() PT2Preview(7) end
function pt2_yes7() PT2Apply(7) end

function pt2_view8() PT2Preview(8) end
function pt2_yes8() PT2Apply(8) end

function pt2_view9() PT2Preview(9) end
function pt2_yes9() PT2Apply(9) end

function pt2_view10() PT2Preview(10) end
function pt2_yes10() PT2Apply(10) end

function pt2_view11() PT2Preview(11) end
function pt2_yes11() PT2Apply(11) end

function pt2_view12() PT2Preview(12) end
function pt2_yes12() PT2Apply(12) end

function pt2_view13() PT2Preview(13) end
function pt2_yes13() PT2Apply(13) end

function pt2_view14() PT2Preview(14) end
function pt2_yes14() PT2Apply(14) end

function pt2_view15() PT2Preview(15) end
function pt2_yes15() PT2Apply(15) end

function pt2_view16() PT2Preview(16) end
function pt2_yes16() PT2Apply(16) end

function pt2_view17() PT2Preview(17) end
function pt2_yes17() PT2Apply(17) end

function pt2_view18() PT2Preview(18) end
function pt2_yes18() PT2Apply(18) end

function pt2_view19() PT2Preview(19) end
function pt2_yes19() PT2Apply(19) end

function pt2_view20() PT2Preview(20) end
function pt2_yes20() PT2Apply(20) end

function pt2_view21() PT2Preview(21) end
function pt2_yes21() PT2Apply(21) end

function pt2_view22() PT2Preview(22) end
function pt2_yes22() PT2Apply(22) end

function pt2_view23() PT2Preview(23) end
function pt2_yes23() PT2Apply(23) end

function pt2_view24() PT2Preview(24) end
function pt2_yes24() PT2Apply(24) end

function pt2_view25() PT2Preview(25) end
function pt2_yes25() PT2Apply(25) end

function pt2_view26() PT2Preview(26) end
function pt2_yes26() PT2Apply(26) end

function pt2_view27() PT2Preview(27) end
function pt2_yes27() PT2Apply(27) end

function pt2_view28() PT2Preview(28) end
function pt2_yes28() PT2Apply(28) end

function pt2_view29() PT2Preview(29) end
function pt2_yes29() PT2Apply(29) end

function pt2_view30() PT2Preview(30) end
function pt2_yes30() PT2Apply(30) end

function pt2_view31() PT2Preview(31) end
function pt2_yes31() PT2Apply(31) end

function pt2_view32() PT2Preview(32) end
function pt2_yes32() PT2Apply(32) end

function pt2_view33() PT2Preview(33) end
function pt2_yes33() PT2Apply(33) end

function pt2_view34() PT2Preview(34) end
function pt2_yes34() PT2Apply(34) end

function pt2_view35() PT2Preview(35) end
function pt2_yes35() PT2Apply(35) end

function pt2_view36() PT2Preview(36) end
function pt2_yes36() PT2Apply(36) end

function pt2_view37() PT2Preview(37) end
function pt2_yes37() PT2Apply(37) end

function pt2_view38() PT2Preview(38) end
function pt2_yes38() PT2Apply(38) end

function pt2_view39() PT2Preview(39) end
function pt2_yes39() PT2Apply(39) end

function pt2_view40() PT2Preview(40) end
function pt2_yes40() PT2Apply(40) end

function pt2_view41() PT2Preview(41) end
function pt2_yes41() PT2Apply(41) end

function pt2_view42() PT2Preview(42) end
function pt2_yes42() PT2Apply(42) end

function pt2_view43() PT2Preview(43) end
function pt2_yes43() PT2Apply(43) end

function pt2_view44() PT2Preview(44) end
function pt2_yes44() PT2Apply(44) end

function pt2_view45() PT2Preview(45) end
function pt2_yes45() PT2Apply(45) end

function pt2_view46() PT2Preview(46) end
function pt2_yes46() PT2Apply(46) end

function pt2_view47() PT2Preview(47) end
function pt2_yes47() PT2Apply(47) end

function pt2_view48() PT2Preview(48) end
function pt2_yes48() PT2Apply(48) end

function pt2_view49() PT2Preview(49) end
function pt2_yes49() PT2Apply(49) end

function pt2_view50() PT2Preview(50) end
function pt2_yes50() PT2Apply(50) end

function pt2_view51() PT2Preview(51) end
function pt2_yes51() PT2Apply(51) end

function pt2_view52() PT2Preview(52) end
function pt2_yes52() PT2Apply(52) end

function pt2_view53() PT2Preview(53) end
function pt2_yes53() PT2Apply(53) end

function pt2_view54() PT2Preview(54) end
function pt2_yes54() PT2Apply(54) end

function pt2_view55() PT2Preview(55) end
function pt2_yes55() PT2Apply(55) end

function pt2_view56() PT2Preview(56) end
function pt2_yes56() PT2Apply(56) end

function pt2_view57() PT2Preview(57) end
function pt2_yes57() PT2Apply(57) end

function pt2_view58() PT2Preview(58) end
function pt2_yes58() PT2Apply(58) end

function pt2_view59() PT2Preview(59) end
function pt2_yes59() PT2Apply(59) end

function pt2_view60() PT2Preview(60) end
function pt2_yes60() PT2Apply(60) end

function pt2_view61() PT2Preview(61) end
function pt2_yes61() PT2Apply(61) end

function pt2_view62() PT2Preview(62) end
function pt2_yes62() PT2Apply(62) end

function pt2_view63() PT2Preview(63) end
function pt2_yes63() PT2Apply(63) end

function pt2_view64() PT2Preview(64) end
function pt2_yes64() PT2Apply(64) end

function pt2_view65() PT2Preview(65) end
function pt2_yes65() PT2Apply(65) end

function pt2_view66() PT2Preview(66) end
function pt2_yes66() PT2Apply(66) end

function pt2_view67() PT2Preview(67) end
function pt2_yes67() PT2Apply(67) end

function pt2_view68() PT2Preview(68) end
function pt2_yes68() PT2Apply(68) end

function pt2_view69() PT2Preview(69) end
function pt2_yes69() PT2Apply(69) end

function pt2_view70() PT2Preview(70) end
function pt2_yes70() PT2Apply(70) end

function pt2_view71() PT2Preview(71) end
function pt2_yes71() PT2Apply(71) end

function pt2_view72() PT2Preview(72) end
function pt2_yes72() PT2Apply(72) end

function pt2_view73() PT2Preview(73) end
function pt2_yes73() PT2Apply(73) end

function pt2_view74() PT2Preview(74) end
function pt2_yes74() PT2Apply(74) end

function pt2_view75() PT2Preview(75) end
function pt2_yes75() PT2Apply(75) end

function pt2_view76() PT2Preview(76) end
function pt2_yes76() PT2Apply(76) end

function pt2_view77() PT2Preview(77) end
function pt2_yes77() PT2Apply(77) end

function pt2_view78() PT2Preview(78) end
function pt2_yes78() PT2Apply(78) end

function pt2_view79() PT2Preview(79) end
function pt2_yes79() PT2Apply(79) end

function pt2_view80() PT2Preview(80) end
function pt2_yes80() PT2Apply(80) end

function pt2_view81() PT2Preview(81) end
function pt2_yes81() PT2Apply(81) end

function pt2_view82() PT2Preview(82) end
function pt2_yes82() PT2Apply(82) end

function pt2_view83() PT2Preview(83) end
function pt2_yes83() PT2Apply(83) end

function pt2_view84() PT2Preview(84) end
function pt2_yes84() PT2Apply(84) end

function pt2_view85() PT2Preview(85) end
function pt2_yes85() PT2Apply(85) end

function pt2_view86() PT2Preview(86) end
function pt2_yes86() PT2Apply(86) end

function pt2_view87() PT2Preview(87) end
function pt2_yes87() PT2Apply(87) end

function pt2_view88() PT2Preview(88) end
function pt2_yes88() PT2Apply(88) end

function pt2_view89() PT2Preview(89) end
function pt2_yes89() PT2Apply(89) end

function pt2_view90() PT2Preview(90) end
function pt2_yes90() PT2Apply(90) end

function pt2_view91() PT2Preview(91) end
function pt2_yes91() PT2Apply(91) end

function pt2_view92() PT2Preview(92) end
function pt2_yes92() PT2Apply(92) end

function pt2_view93() PT2Preview(93) end
function pt2_yes93() PT2Apply(93) end

function pt2_view94() PT2Preview(94) end
function pt2_yes94() PT2Apply(94) end

function pt2_view95() PT2Preview(95) end
function pt2_yes95() PT2Apply(95) end

function pt2_view96() PT2Preview(96) end
function pt2_yes96() PT2Apply(96) end

function pt2_view97() PT2Preview(97) end
function pt2_yes97() PT2Apply(97) end

function pt2_view98() PT2Preview(98) end
function pt2_yes98() PT2Apply(98) end

function pt2_view99() PT2Preview(99) end
function pt2_yes99() PT2Apply(99) end

function pt2_view100() PT2Preview(100) end
function pt2_yes100() PT2Apply(100) end

function pt2_view101() PT2Preview(101) end
function pt2_yes101() PT2Apply(101) end

function pt2_view102() PT2Preview(102) end
function pt2_yes102() PT2Apply(102) end

function pt2_view103() PT2Preview(103) end
function pt2_yes103() PT2Apply(103) end

function pt2_view104() PT2Preview(104) end
function pt2_yes104() PT2Apply(104) end

function pt2_view105() PT2Preview(105) end
function pt2_yes105() PT2Apply(105) end

function pt2_view106() PT2Preview(106) end
function pt2_yes106() PT2Apply(106) end

function pt2_view107() PT2Preview(107) end
function pt2_yes107() PT2Apply(107) end

function pt2_view108() PT2Preview(108) end
function pt2_yes108() PT2Apply(108) end

function pt2_view109() PT2Preview(109) end
function pt2_yes109() PT2Apply(109) end

function pt2_view110() PT2Preview(110) end
function pt2_yes110() PT2Apply(110) end

function pt2_view111() PT2Preview(111) end
function pt2_yes111() PT2Apply(111) end

function pt2_view112() PT2Preview(112) end
function pt2_yes112() PT2Apply(112) end

function pt2_view113() PT2Preview(113) end
function pt2_yes113() PT2Apply(113) end

function pt2_view114() PT2Preview(114) end
function pt2_yes114() PT2Apply(114) end

function pt2_view115() PT2Preview(115) end
function pt2_yes115() PT2Apply(115) end

function pt2_view116() PT2Preview(116) end
function pt2_yes116() PT2Apply(116) end

function pt2_view117() PT2Preview(117) end
function pt2_yes117() PT2Apply(117) end

function pt2_view118() PT2Preview(118) end
function pt2_yes118() PT2Apply(118) end

function pt2_view119() PT2Preview(119) end
function pt2_yes119() PT2Apply(119) end

function pt2_view120() PT2Preview(120) end
function pt2_yes120() PT2Apply(120) end

function pt2_view121() PT2Preview(121) end
function pt2_yes121() PT2Apply(121) end

function pt2_view122() PT2Preview(122) end
function pt2_yes122() PT2Apply(122) end

function pt2_view123() PT2Preview(123) end
function pt2_yes123() PT2Apply(123) end

function pt2_view124() PT2Preview(124) end
function pt2_yes124() PT2Apply(124) end

function pt2_view125() PT2Preview(125) end
function pt2_yes125() PT2Apply(125) end

function pt2_view126() PT2Preview(126) end
function pt2_yes126() PT2Apply(126) end

function pt2_view127() PT2Preview(127) end
function pt2_yes127() PT2Apply(127) end

function pt2_view128() PT2Preview(128) end
function pt2_yes128() PT2Apply(128) end

function pt2_view129() PT2Preview(129) end
function pt2_yes129() PT2Apply(129) end

function pt2_view130() PT2Preview(130) end
function pt2_yes130() PT2Apply(130) end

function pt2_view131() PT2Preview(131) end
function pt2_yes131() PT2Apply(131) end

function pt2_view132() PT2Preview(132) end
function pt2_yes132() PT2Apply(132) end

function pt2_view133() PT2Preview(133) end
function pt2_yes133() PT2Apply(133) end

function pt2_view134() PT2Preview(134) end
function pt2_yes134() PT2Apply(134) end

function pt2_view135() PT2Preview(135) end
function pt2_yes135() PT2Apply(135) end

function pt2_view136() PT2Preview(136) end
function pt2_yes136() PT2Apply(136) end

function pt2_view137() PT2Preview(137) end
function pt2_yes137() PT2Apply(137) end

function pt2_view138() PT2Preview(138) end
function pt2_yes138() PT2Apply(138) end

function pt2_view139() PT2Preview(139) end
function pt2_yes139() PT2Apply(139) end

function pt2_view140() PT2Preview(140) end
function pt2_yes140() PT2Apply(140) end

function pt2_view141() PT2Preview(141) end
function pt2_yes141() PT2Apply(141) end

function pt2_view142() PT2Preview(142) end
function pt2_yes142() PT2Apply(142) end

function pt2_view143() PT2Preview(143) end
function pt2_yes143() PT2Apply(143) end

function pt2_view144() PT2Preview(144) end
function pt2_yes144() PT2Apply(144) end

function pt2_view145() PT2Preview(145) end
function pt2_yes145() PT2Apply(145) end

function pt2_view146() PT2Preview(146) end
function pt2_yes146() PT2Apply(146) end

function pt2_view147() PT2Preview(147) end
function pt2_yes147() PT2Apply(147) end

function pt2_view148() PT2Preview(148) end
function pt2_yes148() PT2Apply(148) end

function pt2_view149() PT2Preview(149) end
function pt2_yes149() PT2Apply(149) end

function pt2_view150() PT2Preview(150) end
function pt2_yes150() PT2Apply(150) end

function pt2_view151() PT2Preview(151) end
function pt2_yes151() PT2Apply(151) end

function pt2_view152() PT2Preview(152) end
function pt2_yes152() PT2Apply(152) end

function pt2_view153() PT2Preview(153) end
function pt2_yes153() PT2Apply(153) end

PT2_ROLE="chucdung"
PT2_MAP=1004
PT2_REPLACED_TITLES={[1]={"B¸ch Lý","B×nh An","B¨ng Lang","Chinh §å","Cæ Kim","Kh¶o NghiÖm","Linh Lùc","Ngò ThÊt","Tam S¸ch","Thiªn Duyªn","Th«ng Thiªn","ThÇn Dô KÝnh","ThÇn Méc","ThÇn Oanh","ThÕ Së","Trõ Häa","Th¨m Dß","Linh lùc","Gi¸o HuÊn"},[2]={"B×nh An","BÊt Tóy","Cæ Kim","Cæ §ao","Cøu TÕ","Khai TrÝ","Ma HuyÕt","Mao L­","Tam S¸ch","Thiªn Duyªn","Th«ng Thiªn","ThÇn Dô KÝnh","ThÇn KhÝ","Trõ Häa","T©n Thøc","Ma huyÕt","BÊt Tóy ","Cæ ®ao","Gi¸o HuÊn"}}
PT2_OLD_MAIN=main
PT2_OLD_MARK=GetPlayerTaskState
PT2_OLD_SAY_TASK=SayTask
function SayTask(text,choices)
 if PT2Class() then
  for i=1,getn(choices) do local label=strlower(gsub(choices[i][1],"^%s*(.-)%s*$","%1"))
   for f=1,2 do for j=1,getn(PT2_REPLACED_TITLES[f]) do
    if label==strlower(PT2_REPLACED_TITLES[f][j]) and not (GetPlayerType()==1 and PT2_ROLE=="linhbao" and label==strlower("Th¨m Dß") and GetTask(15)==1) then choices[i].show=0 end
   end end
  end
 end
 return PT2_OLD_SAY_TASK(text,choices)
end
function main() return PT2Menu() end
function GetPlayerTaskState() return PT2Marker() end
