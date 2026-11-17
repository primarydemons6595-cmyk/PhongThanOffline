-- Original VNG source payload; provenance in deployment report.
--description: ·çÁÖ-Ö÷Ïß?Îñ
--author: yichuan
--date:2004/5/13

function main()
	tasks = 
	{
		{"BÊt Tóy ","renwu1";show=0},
		{"Vµo s¬n cèc ","come";show=1}
	}
	UTask_Druid = GetTask(2);
	if(GetPlayerType()==2)and(GetLevel()>=35)  and  (UTask_Druid ==10 )then
			tasks[1].show=1
	end;


	SayTask(10348,tasks)
end;

function   renwu1()
			Talk(3,"func_leave",10349,10350,10351)
end;

function  func_leave()
	MsgBox(10352,"yes","no")
end;

function  yes()
	CloseDialog()
	NewWorld(15,1687,3106)--´«ËÍ½ø?É½¹È
	SetTask(2,11)
	TaskNote(29,3)
	Msg2Player("Vµo s¬n cèc, cøu bän DÞ nh©n say r­îu.")
end;

function  no()
		CloseDialog()
end;

function  come()
		NewWorld(15,1687,3106)--´«ËÍ½ø?É½¹È
		CloseDialog()
end;

pt_original_main = main
-- Original callbacks use content map IDs; guards retain runtime IDs.
pt_runtime_world_pos = GetWorldPos
GetWorldPos = GetContentWorldPos
SubWorldIdx2ID = SubWorldIdx2ContentID

-- Authored restoration menu begins here.
-- AUTHORED NPC 1015_01; not a claim of original appearance.
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
    Say("Dau moi Bat Tuy o Manh Tan; huong dan tim Ngo Long trong coc. Con lien quan Van Luong, nhung khong tu khoi tao xe luong.", 2, "Quay lai/main", "Dong/pt_close")
end
function pt_routes()
    if pt_guard() == 0 then return end
    Say("Dai Phu (192/212); Thuong Hao (206/195); Ngo Long (209/195)", 2, "Quay lai/main", "Dong/pt_close")
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

function pt_escort_old() return pt_original_main() end

-- Connected escort adapter. Original payload above remains byte-for-byte.
function main()
    Say("Van Luong - Truong Que Phuong - Tuyet Long Linh (164/196)", 7, "Nhan chuyen/pt_escort_take","Nhan phan thuong chuyen da giao/pt_escort_claim","Tinh trang chuyen/pt_escort_info","Huong dan/pt_escort_help","Huy chuyen/pt_escort_cancel","Nhiem vu chinh tuyen/pt_escort_old","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_close() CloseDialog() end
function no() CloseDialog() end
function pt_escort_help()
    Say("Cap 55, gia nhap lanh dia. 20 chuyen/ngay: chuyen dau mien phi, cac chuyen sau can Khao Quan Lenh. Giao xe roi ve Phong Lam nhan thuong. Xe di theo khi ban o gan. Khong bo xe qua xa; roi xe 2 phut, xe bi pha hoac het han thi chuyen that bai. Thoi han va thuong do Studio cai dat.",2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_info()
    local phase,left,cart,runs,limit=EventEscortInfo(1)
    local status="Chua nhan chuyen"
    if phase==1 then status="Dang ho tong xe; con "..floor(left/60).." phut "..mod(left,60).." giay"
    elseif phase==2 then status="Da giao xe. Ve Phong Lam o Manh Tan (191/212) nhan thuong" end
    Say(status..". Hom nay: "..runs.."/"..limit.." chuyen.",2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_result(result)
    local message="Chua the thuc hien. Hay de trong hanh trang F4, kiem tra tien va trang thai giao dich. Quyen nhan thuong van duoc giu."
    if result==1 then message="Thanh cong. Hay xem tinh trang chuyen de tiep tuc."
    elseif result==-1 then message="Thieu ve nhan them chuyen (Khao Quan Lenh) trong F4. Chua tru vat pham."
    elseif result==-2 then message="Can doi thoai dung NPC va dung buoc nhiem vu."
    elseif result==-3 then message="Event dang tat nhan chuyen moi hoac app Event Phong Than chua chay. Chuyen da nhan van duoc tra va nhan thuong."
    elseif result==-4 then message="Chua du cap/lanh dia/uy danh, het luot trong ngay, hoac dang co chuyen chua hoan tat."
    elseif result==-5 then message="Xe khong con hoac chuyen da het han. Hay kiem tra lai tinh trang chuyen."
    elseif result==-6 then message="Can dua xe den sat NPC tra nhiem vu. Chi chu xe moi duoc giao xe." end
    Say(message,2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_take() pt_escort_result(EventEscortAction(1,1)) end
function pt_escort_deliver() pt_escort_result(EventEscortAction(1,2)) end
function pt_escort_claim() pt_escort_result(EventEscortAction(1,3)) end
function pt_escort_cancel()
    Say("Huy chuyen se mat xe va luot da nhan. Ban co chac chan?",2,"Dong y huy/pt_escort_cancel_yes","Quay lai/main")
end
function pt_escort_cancel_yes() pt_escort_result(EventEscortAction(1,4)) end
