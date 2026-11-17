-- Original VNG source payload; provenance in deployment report.
--description: ÎâÁú-Òì?Ö÷Ïß?Îñ
--author: yichuan
--date:2004/5/13

function  main()
	tasks = 
	{
		{"BÊt Tóy ","renwu1";show=0}
	}
			UTask_Druid = GetTask(2);
		if(GetPlayerType()==2)and (UTask_Druid==11)then
					tasks[1].show=1;
		end;
		if(GetLevel()>=35)  and  (UTask_Druid==13)  and  (HaveEventItem(16)>=1)then
					tasks[1].show=1;
		end;
		SayTask(10354,tasks)
end;

function   renwu1()
		UTask_Druid = GetTask(2);
		if(GetPlayerType()==2)and (UTask_Druid==11)then
						Talk(4,"no",10355,10356,10357,10358)
						SetTask(2,12)
						Msg2Player("Mau ®Õn töu ®iÕm trong TriÒu Ca ®Ó mua canh tØnh r­îu!")
						TaskNote(29,4)
		end;
		if(GetLevel()>=35)  and  (UTask_Druid==13)  and  (HaveEventItem(16)>=1)then
						Talk(3,"no",10359,10360,10361)
						DelEventItem(16)
						AddNormalItem(7,59,128,1,0,0)
						Msg2Player("Cøu ®­îc Ng« Long, nhËn ®­îc s¸ch kü n¨ng Ban M«n Léng Phñ. TiÕp tôc ®i cøu nh÷ng ng­êi kh¸c.")
						SetTask(2,20)
						TaskNote(29,6)
		end;
end;

function   no()
		CloseDialog()
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1015_02; not a claim of original appearance.
function pt_guard()
    local w = pt_runtime_world_pos()
    if w ~= 1015 then CloseDialog(); return 0 end
    return 1
end
function main()
    if pt_guard() == 0 then return end
    return pt_original_main()
end
function pt_about()
    if pt_guard() == 0 then return end
    Say("Nhan vat trong Bat Tuy. Tim chu tuu diem tai Trieu Ca theo loi chi dan, sau do quay lai Ngo Long.", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Thuong Hao (206/195); Dai Phu (192/212); Phong Lam (192/212)", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_status()
    if pt_guard() == 0 then return end
    Say("Co nhanh Lua VNG goc trong PAK. Cac dieu kien va giao dich do nhanh goc kiem tra; chua nghiem thu toan bo nhiem vu.", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_close()
    CloseDialog()
end
function pt_original()
    if pt_guard() == 0 then return end
    pt_original_main()
end
