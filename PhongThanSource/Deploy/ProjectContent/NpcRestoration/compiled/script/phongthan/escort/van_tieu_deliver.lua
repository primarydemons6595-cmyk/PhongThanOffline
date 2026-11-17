--description:Ô⁄ ¶ ----- ‘ÀÔ⁄Ωª»ŒŒÒNPC£®Úø”»ƒπ£©
--author:»ŸΩØ∑…
--data:2004.7.27

function main()
    local playername,guardindex
	playername=GetName()
	guardindex=GetTGuardIndexByPlayerName(playername)
	if (guardindex==0) then
	     Talk(1,"no",11402)
	else
         local  bounty,a,b,c,carriageindex=GetTGuardInfo(guardindex)
		 local  timeparam=GetTGuardTimeScale()/100
		 local  getcredit=10
		 -- ≈–∂œ «∑Ò‘⁄Ô⁄≥µ¿Ô
		 local insideindex=IsPlayerInsideWeapon(PlayerIndex)
		 if (insideindex==carriageindex) and (carriageindex ~= 0) then
			Talk(1,"no",11403)
			Earn(bounty)
			Msg2Player("Bπn nhÀn Æ≠Óc ph«n th≠Îng 50w.")
			AddCredit(getcredit) -- ’‚¿Ôµ√…˘Õ˚
			AddNormalItem(3,83,0,1,0,0)
			Msg2Player("Bπn nhÀn Æ≠Óc mÈt Ng‰c t˚.")
			AddGlobalCountNews("<color=green>"..playername.."<color> nhÀn Æ≠Óc ph«n th≠Îng Ng‰c t˚",1)
			DeleteSiegeWeapon(carriageindex)
			SetGlobalValue(422,0)
			SetGlobalValue(423,0)
			SetGlobalValue(424,0)
		 else
		      Talk(1,"no",11404)
		 end;
	end;
end;
		      

function no()
		CloseDialog()
end;

-- Connected escort adapter. Original payload above remains byte-for-byte.
function main()
    Say("Van Tieu - Tong tieu dau - Trieu Ca (233/198)", 5, "Giao xe/pt_escort_deliver","Tinh trang chuyen/pt_escort_info","Huong dan/pt_escort_help","Huy chuyen/pt_escort_cancel","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_close() CloseDialog() end
function no() CloseDialog() end
function pt_escort_help()
    Say("Cap 30, it nhat 10 uy danh. Ho tong xe tu Tuyet Long Linh den Trieu Ca. Thuong goc: 50 van luong, 10 uy danh va 1 Ngoc ty. Xe di theo khi ban o gan. Khong bo xe qua xa; roi xe 2 phut, xe bi pha hoac het han thi chuyen that bai. Thoi han va thuong do Studio cai dat.",2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_info()
    local phase,left,cart,runs,limit=EventEscortInfo(2)
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
function pt_escort_take() pt_escort_result(EventEscortAction(2,1)) end
function pt_escort_deliver() pt_escort_result(EventEscortAction(2,2)) end
function pt_escort_claim() pt_escort_result(EventEscortAction(2,3)) end
function pt_escort_cancel()
    Say("Huy chuyen se mat xe va luot da nhan. Ban co chac chan?",2,"Dong y huy/pt_escort_cancel_yes","Quay lai/main")
end
function pt_escort_cancel_yes() pt_escort_result(EventEscortAction(2,4)) end
