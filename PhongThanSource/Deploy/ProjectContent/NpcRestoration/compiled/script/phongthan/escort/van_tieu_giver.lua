--description:ÉñÃØïÚÍ· ----- ÔËïÚ½ÓÈÎÎñNPC£¨³ç³Ç´óÓª£©
--author:ÈÙ½¯·É
--data:2004.7.25


function main()
    local playerlevel,carriagenum,playername,guardindex
    playerlevel=GetLevel()
	carriagenum=GetTGuardNum()                           --È¡µÃµ±Ç°ÓÎÏ·ÖĞÕıÔÚÔËĞĞïÚ³µ×ÜÊıÁ¿
    playername=GetName()
	guardindex=GetTGuardIndexByPlayerName(playername)
    if (GetTeam()~=0) then				    --¼ì²âÊÇ·ñÔÚ¶ÓÎéÀï
        Talk(1,"no",11405)
    elseif (GetCamp()==0) then		                     --¼ì²âÊÇ·ñÎªĞÂÊÖ
    	Talk(1,"no",11406)
    elseif (GetTask(60)~=0) then			     --¼ì²âÊÇ·ñÎªÅÜÉÌ×´Ì¬
    	Talk(1,"no",11407)
    elseif (playerlevel <30 ) then
		Talk(1,"no",11408)
    elseif (guardindex >0) then
	     Talk(1,"no",11409)
	elseif (GetGlobalValue(422) == 0) then --±íÊ¾Ã»µ½ÏµÍ³ÊÇ·ñ´¥·¢ÁË¿ªÊ¼ÔËïÚ»î¶¯
		 Talk(1,"no",11410)
	elseif (GetGlobalValue(423) == 1) then --±íÊ¾ÊÇ·ñÒÑ¾­·¢ËÍÁËÒ»ÌËïÚ³µ
	     Talk(1,"no",11411)
	else
	     Talk(1,"che",11412)
    end;
end;

function  che()
	MsgBox(11413,"ok","no")
end;

function no()
	CloseDialog()
end;

function ok()
    local mapid,x,y,playercredit,playergold,playername,carriagelevel,carriageindex,lasttime,playerlevel
    playerlevel=GetLevel()
	playercredit=GetCredit()
    mapid,x,y=GetWorldPos()
    playername=GetName()
	x=32*x   --ĞŞ¸Äby Rocker 2004-9-4 17:35 °ÑÂí³µÖ±½ÓÉú³Éµ½ÈËÉíÉÏ
	y=32*y
    lasttime=1800

	--Rocker ĞŞ¸Ä¿ªÊ¼ ·ÅÔÚÉñÃØïÚÍ·Éí±ß×ÜÊÇ°²È«µÄ
	local mapid2,x2,y2
	mapid2,x2,y2 = GetNpcWorldPos(DialogNpcIdx)
	x2 = 32 * x2
	y2 = 32 * y2
	x2 = x2 - random(100,200)
	y2 = y2 + random(120,150)
	--Rocker ĞŞ¸Ä½áÊø

	if (playerlevel>=30) and (playercredit>=10) then
		 if (mapid2 == 0) then
			carriageindex=NewSiegeWeapon(mapid,x,y,363)
		 else
			carriageindex=NewSiegeWeapon(mapid2,x2,y2,363)
		 end;
		 carriagelevel=1
         --ÉèÖÃïÚ³µ½Å±¾
		 carriagenpcindex = GetSiegeWeaponNpcIndex(carriageindex)
		 SetNpcScript(carriagenpcindex, "\\script\\ÔËïÚ\\ïÚ³µ.lua" )
		 SendCarriage(carriageindex,playername,carriagelevel,lasttime)
		 DelNpc(DialogNpcIdx)
		 SetGlobalValue(426,0)
		 --±äÕóÓª
		 if (carriagenpcindex>0 )then
		if(GetPK()>=88) then
		      SetNpcCurCamp ( carriagenpcindex , 8 )
		      SetCamp(8)   --Rocker ĞŞ¸Ä³ÉÓÀ¾ÃÕóÓª2004-9-4 17:37
			SetCurCamp(8)
		else
		      SetNpcCurCamp ( carriagenpcindex , 2 )
		      SetCamp(2)   --Rocker ĞŞ¸Ä³ÉÓÀ¾ÃÕóÓª2004-9-4 17:37
			SetCurCamp(2)
	end;
			  lasttime = lasttime / 60
			  AddGlobalCountNews("<color=green>"..playername.."<color> nhËn nhiÖm vô ¸p tiªu TuyÖt Long lÜnh, ®­îc Tiªu ®Çu thÇn bİ tin t­ëng",10)
			  CloseDialog()
		 end;
		 SetGlobalValue(423,1)
		 SetGlobalValue(424,0) --ïÚ³µËÀÍö´ÎÊı
	elseif((playerlevel>=30) and (playercredit<10)) then
	     Talk(1,"no",11414)
    end;
end;

-- Connected escort adapter. Original payload above remains byte-for-byte.
function main()
    Say("Van Tieu - Tong tieu dau - Trieu Ca (233/198)", 5, "Nhan chuyen/pt_escort_take","Tinh trang chuyen/pt_escort_info","Huong dan/pt_escort_help","Huy chuyen/pt_escort_cancel","Ket thuc doi thoai/pt_escort_close")
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
