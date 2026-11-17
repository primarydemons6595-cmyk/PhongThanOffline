--description: ¨ªªQ¤l
--author:yichuan
--date: 2004/6/10
--modify:liuying 2005/4/5(¥[¤J¤F¥L¤s¤§¥Ûªº¥æ´«¥ô°È)

function main(sel)
			tasks = 
			{
				{"Hîp thµnh vËt phÈm","dz";show=1},
				{"Tha S¬n th¹ch","tszs";show=1},
                                {"b¶o ®iÓn","hcbd";show=1}  
			}
			SayTask(10499,tasks)
end;

function no()
		CloseDialog()
end;

function   dz()
	CloseDialog()
	EnchaseItem();
end;

function  tszs()		--Án±æ´«¥L¤s¤§¥Û
	MsgBox(11256,"change","no")
end;

function  change()
	if(GetCredit()>=10)then
		DecCredit(10)
		AddNormalItem(3,82,0,1,0,0)
		MsgBox(11257,"no")
	else
		MsgBox(11258,"no")
	end;
end;

function   hcbd()              --¦X¦¨Ä_¨å
			tasks1 =
			{
				{"Th¨ng cÊp vò khİ","wqsj";show=1},
				{"Th¨ng cÊp trang bŞ","zbsj";show=1},
				{"Ph¸p b¶o","fbhc";show=1},
				{"B¸ L¹c","blhc";show=1},
				{"B¶o th¹ch","bshc";show=1},
				{"Lo¹i kh¸c","qthc";show=1}
                        }
                        SayTask("Ta cã bé B¶o ®iÓn hîp thµnh, chia lµm 6 quyÓn! Cã muèn xem thö kh«ng?",tasks1)
	
end;

function   wqsj()
		Talk(1,"wqsj1","<color=green>Th¨ng cÊp phæ th«ng<color>: Mçi lÇn th¨ng cÊp vò khİ yªu cÇu ®¼ng cÊp t¨ng 4 cÊp\nNgò Quang th¹ch+Lam B¶o th¹ch+Vò khİ=Vò khİ+STCB\nHçn Thiªn L¨ng+Lam B¶o th¹ch+Vò khİ=Vò khİ+Thæ s¸t\nCµn Kh«n xİch+Lam B¶o th¹ch+Vò khİ=Vò khİ+L«i s¸t\nHáa Long tiªu+Lam B¶o th¹ch+Vò khİ=Vò khİ+Háa s¸t\nB×nh L­u Ly+Lam B¶o th¹ch+Vò khİ=Vò khİ+B¨ng s¸t")
end;     
function   wqsj1()
                Talk(1,"wqsj2","<color=green>Vò khİ<color>: Mçi lÇn th¨ng cÊp vò khİ yªu cÇu ®¼ng cÊp t¨ng 4 cÊp\nNgò Quang th¹ch+3 Lam B¶o th¹ch+Vò khİ=Vò khİ+STCB\nHçn Thiªn L¨ng+3 Lam B¶o th¹ch+Vò khİ=Vò khİ+Thæ s¸t\nCµn Kh«n xİch+3 Lam B¶o th¹ch+Vò khİ=Vò khİ+L«i s¸t\nHáa Long tiªu+3 Lam B¶o th¹ch+Vò khİ=Vò khİ+Háa s¸t\nB×nh L­u Ly+3 Lam B¶o th¹ch+Vò khİ=Vò khİ+B¨ng s¸t")
end;
function   wqsj2() 
                Talk(1,"wqsj3","<color=green>Th¨ng cÊp tinh chÕ<color>: Mçi lÇn th¨ng cÊp vò khİ yªu cÇu kh«ng cÇn ®¼ng cÊp t¨ng\nNgò Quang th¹ch+Lam B¶o th¹ch+Vò khİ+TrÇm §iÖn §ång=Vò khİ+STCB\nHçn Thiªn L¨ng+Lam B¶o th¹ch+Vò khİ+TrÇm §iÖn §ång=Vò khİ+Thæ s¸t\nCµn Kh«n xİch+Lam B¶o th¹ch+Vò khİ+TrÇm §iÖn §ång=Vò khİ+L«i s¸t\nHáa Long tiªu+Lam B¶o th¹ch+Vò khİ+TrÇm §iÖn §ång=Vò khİ+Háa s¸t\nB×nh L­u Ly+Lam B¶o th¹ch+Vò khİ+TrÇm §iÖn §ång=Vò khİ+B¨ng s¸t") 
end;
function   wqsj3() 
                Talk(1,"wqsj4","<color=green>Tû lÖ th¨ng cÊp<color>: \n3 lÇn th¨ng cÊp ®Çu tiªn nhÊt ®Şnh thµnh c«ng, 3 lÇn kÕ tiÕp tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vò khİ sÏ th¨ng cÊp 3 lÇn, 3 lÇn kÕ tiÕp tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vò khİ sÏ mÊt ®i, th¨ng cÊp thµnh c«ng 9 lÇn kh«ng thÓ tiÕp tôc th¨ng cÊp\n Chó ı: <color=red>Th¨ng cÊp phæ th«ng <color>vµ  <color=red>th¨ng cÊp tinh chÕ<color> kh«ng thÓ phèi hîp sö dông, vò khİ lóc ®Çu ®· cã thuéc tİnh s¸t th­¬ng t­¬ng øng")                
end;
function   wqsj4() 
                Talk(1,"hcbd","<color=green>Hoµn nguyªn vò khİ<color>: \nVò khİ ®· th¨ng cÊp+Qui Ch©n th¹ch=Vò khİ ban ®Çu")                
end;
function  zbsj()
		Talk(1,"zbsj1"," <color=green>Th¨ng cÊp phæ th«ng<color>: Mçi lÇn th¨ng cÊp trang bŞ yªu cÇu ®¼ng cÊp t¨ng 4 cÊp\nH×nh Thiªn Ên+Lam B¶o th¹ch+Kh¶i gi¸p=Kh¶i gi¸p+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Ngäc béi=Ngäc béi+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Giµy=Giµy+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Yªu ®¸i=Yªu ®¸i+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+§Çu kh«i=§Çu kh«i+Søc phßng thñ")
end;  
function  zbsj1()
		Talk(1,"zbsj2"," <color=green>Th¨ng cÊp tinh chÕ<color>: Mçi lÇn th¨ng cÊp trang bŞ kh«ng cÇn ®¼ng cÊp t¨ng\nH×nh Thiªn Ên+Lam B¶o th¹ch+Kh¶i gi¸p+TrÇm §iÖn §ång=Kh¶i gi¸p+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Ngäc béi+TrÇm §iÖn §ång=Ngäc béi+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Giµy+TrÇm §iÖn §ång=Giµy+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+Yªu ®¸i+TrÇm §iÖn §ång=Yªu ®¸i+Søc phßng thñ\nH×nh Thiªn Ên+Lam B¶o th¹ch+§Çu kh«i+TrÇm §iÖn §ång=§Çu kh«i+Søc phßng thñ")   
end;
function  zbsj2()
		Talk(1,"zbsj3","<color=green>Tû lÖ th¨ng cÊp<color>: \n3 lÇn th¨ng cÊp ®Çu nhÊt ®Şnh thµnh c«ng, 3 lÇn th¨ng cÊp kÕ tiÕp tû lÖ thµnh c«ng t­¬ng ®èi! sau 9 lÇn n©ng cÊp thµnh c«ng sÏ kh«ng thÓ tiÕp tôc.")  
end;
function  zbsj3()
		Talk(1,"hcbd","<color=green>Hoµn nguyªn trang bŞ<color>: \nTrang bŞ ®· th¨ng cÊp+Qui Ch©n th¹ch=Trang bŞ ban ®Çu")  
end;
function  fbhc()
		Talk(2,"fbhc1","<color=green>hîp thµnh Ph¸p b¶o song thuéc tİnh<color>: \nHçn Thiªn L¨ng+Tói Ng« Phong+Hång thñy tinh=Toµn T©m ®inh\nCµn Kh«n xİch+¢m D­¬ng kİnh+Hång thñy tinh=L¹c Hån chung\nHáa Long tiªu+Kim Cang ph¸ch+Hång thñy tinh=Hång hå l«\nB×nh L­u Ly+Bİch Tú Bµ+Hång thñy tinh=Ngäc H­ phï\n Chó ı: Tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vËt phÈm sÏ mÊt ®i","<color=green>hîp thµnh Ph¸p b¶o song thuéc tİnh<color>: \nBµn Cæ ph­ín+Tói Ng« Phong+Hång thñy tinh=Cäc §én Long\nTh¸i D­¬ng ch©m+¢m D­¬ng kİnh+Hång thñy tinh=ChiÕu Yªu kİnh\nÊm V¹n Nha+Kim Cang ph¸ch+Hång thñy tinh=Phong Háa lu©n\nD©y Ph­îc Long+Bİch Tú Bµ+Hång thñy tinh=Kim Quang táa\n Chó ı: Tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vËt phÈm sÏ mÊt ®i.")
              
end;
function  fbhc1()
		Talk(1,"hcbd"," <color=green>hîp thµnh Ph¸p b¶o song thuéc tİnh<color>: \nHçn Thiªn L¨ng+Bµn Cæ ph­ín+Hång thñy tinh=H¹nh Hoµng kú\nTh¸i D­¬ng ch©m+Cµn Kh«n xİch+Hång thñy tinh=Cµn Kh«n khuyªn\nHáa Long tiªu+Êm V¹n Nha+Hång thñy tinh=B¹ch Cèt ph­ín\nB×nh L­u Ly+D©y Ph­îc Long+Hång thñy tinh=§Şnh Phong ch©u\n Chó ı: Tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vËt phÈm sÏ mÊt ®i.")
end;
function   blhc()
		Talk(1,"blhc1","<color=green>hîp thµnh B¸ L¹c nh·n<color>: \nB¸ L¹c nh·n cÊp 1+B¸ L¹c nh·n cÊp 1+Tr­êng sinh cèc=B¸ L¹c nh·n cÊp 2\nB¸ L¹c nh·n cÊp 2+B¸ L¹c nh·n cÊp 2+Tr­êng sinh cèc=B¸ L¹c nh·n cÊp 3\nB¸ L¹c nh·n cÊp 3+B¸ L¹c nh·n cÊp 3+Tr­êng sinh cèc=B¸ L¹c nh·n cÊp 4\nB¸ L¹c nh·n cÊp 4+B¸ L¹c nh·n cÊp 4+Tr­êng sinh cèc=B¸ L¹c nh·n cÊp 5\n Chó ı: NhÊt ®Şnh thµnh c«ng")
end;
function   blhc1()
		Talk(1,"blhc2","<color=green> hîp thµnh B¸ L¹c nh·n<color>: \nB¸ L¹c nh·n cÊp 5+B¸ L¹c nh·n cÊp 5+Hång thñy tinh=B¸ L¹c nh·n cÊp 6\nB¸ L¹c nh·n cÊp 6+B¸ L¹c nh·n cÊp 6+Hång thñy tinh=B¸ L¹c nh·n cÊp 7\nB¸ L¹c nh·n cÊp 7+B¸ L¹c nh·n cÊp 7+Hång thñy tinh=B¸ L¹c nh·n cÊp 8\nB¸ L¹c nh·n cÊp 8+B¸ L¹c nh·n cÊp 8+Hång thñy tinh=B¸ L¹c nh·n cÊp 9\n Chó ı: NhÊt ®Şnh thµnh c«ng")
end;
function   blhc2()
		Talk(1,"hcbd","<color=green>hîp thµnh B¸ L¹c nh·n<color>: \nB¸ L¹c nh·n cÊp 9+B¸ L¹c nh·n cÊp 9+Lam B¶o th¹ch=B¸ L¹c nh·n cÊp 10\nB¸ L¹c nh·n cÊp 10+B¸ L¹c nh·n cÊp 10+Lam B¶o th¹ch=1B¸ L¹c nh·n cÊp 1\n1B¸ L¹c nh·n cÊp 1+B¸ L¹c nh·n cÊp 11+Lam B¶o th¹ch=1B¸ L¹c nh·n cÊp 2\n1B¸ L¹c nh·n cÊp 2+B¸ L¹c nh·n cÊp 12+Lam B¶o th¹ch=1B¸ L¹c nh·n cÊp 3\n Chó ı: NhÊt ®Şnh thµnh c«ng")
end;
function   bshc()
		Talk(1,"hcbd","<color=green>hîp thµnh M¶nh b¶o th¹ch<color>: \n3 m¶nh Hång thñy tinh+Tha S¬n th¹ch=1 Hång thñy tinh\n3 Hång thñy tinh+Tha S¬n th¹ch=1 Hång b¶o th¹ch\n3 m¶nh Lam thñy tinh+Tha S¬n th¹ch=1 Lam thñy tinh\n3 Lam thñy tinh+Tha S¬n th¹ch=1 Lam B¶o th¹ch\n Chó ı: Tû lÖ thµnh c«n cao, sau khi thÊt b¹i vËt phÈm sÏ mÊt ®i")
end;

function   qthc()
		Talk(1,"hcbd","<color=green> hîp thµnh V¹n Tiªn phï<color>: \n2 Thæ Linh Phï = 1 Thñy Linh Phï\n2 Thñy Linh Phï = 1 Háa Linh phï\n2 Háa Linh Phï = 1 Phong Linh Phï\n Chó ı: Tû lÖ thµnh c«ng cao, sau khi thÊt b¹i vËt phÈm sÏ mÊt ®i")
end;
-- Appendix to the exact active VNG script; keep all original help callbacks.
-- Only the two mutating services are routed through validated native APIs.
function dz()
    CloseDialog()
    if XichTungTuService(0) ~= 1 then
        MsgBox("Hay dung gan Xich Tung Tu va mo khoa hanh trang truoc khi hop thanh.", "no")
    end
end

function change()
    local result = XichTungTuService(1)
    if result == 1 then
        MsgBox(11257, "no")
    elseif result == -3 then
        MsgBox(11258, "no")
    elseif result == -1 then
        MsgBox("Hay tro lai gan Xich Tung Tu de doi Tha Son Thach.", "no")
    elseif result == -2 then
        MsgBox("Hay mo khoa hanh trang va ket thuc giao dich truoc khi doi.", "no")
    else
        MsgBox("Ch­a nhËn ®­îc Tha S¬n Th¹ch. CÇn chç trèng F4. Danh väng ch­a bŞ trõ.", "no")
    end
end

-- Add button navigation while retaining all original composition/help callbacks.
pt_xich_original_menu=main
Include("\\script\\phongthan\\pet_service.lua")
function pt_xich_menu()
 SayButtons("Xİch Tïng Tö: chän chøc n¨ng cÇn sö dông.",7,
  "Hîp thµnh vËt phÈm/dz", "GhĞp Néi §¬n Linh thó/pt_pet_craft_menu",
  "B¶o ®iÓn hîp thµnh/hcbd", "Tha S¬n th¹ch/tszs", "§æi Hån/pt_xich_soul", "Ph¸p B¶o/pt_fabao_menu", "GhĞp Linh Phï/pt_vtt_merge_menu")
end
function pt_xich_soul()
 SayButtons("§æi <color=yellow>3 Ph¸ch<color> vµ <color=yellow>50 §ång Thau<color> lÊy <color=yellow>1 Hån<color>.",1,"§æi Hån/pt_xich_soul_confirm")
end
function pt_xich_soul_confirm()
 if XichTungTuService(2)==1 then CloseDialog()
 else Talk(1,"no","CÇn 3 Ph¸ch, 50 §ång Thau trong tói vµ chç trèng F4. H·y ®øng gÇn Xİch Tïng Tö, më khãa hµnh trang vµ kÕt thóc giao dŞch.") end
end
function main() return pt_xich_menu() end

-- PT_FABAO_RESTORE_BEGIN
function pt_fabao_menu()
 SayButtons("Hîp thµnh vµ n©ng cÊp Ph¸p B¶o Nh©n Giíi. Cöa sæ ghĞp sÏ kiÓm tra nguyªn liÖu vµ phİ tr­íc khi x¸c nhËn.",4,
 "Hîp thµnh/dz","Th¨ng cÊp/dz","C«ng thøc/pt_fabao_book(1)","Tam Sinh, ThÊt TrÇn Trai, Kim S¬n/pt_fb_extra")
end
PtFbBook={}
tinsert(PtFbBook,{"Toµn T©m §inh",{"Nguyªn liÖu hîp thµnh <color=green>Toµn T©m §inh<color>:\n<color=yellow>1 Hçn Thiªn L¨ng<color>\n<color=yellow>1 Tói Ng« Phong<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"L¹c Hån Chung",{"Nguyªn liÖu hîp thµnh <color=green>L¹c Hån Chung<color>:\n<color=yellow>1 Cµn Kh«n Xİch<color>\n<color=yellow>1 ¢m D­¬ng Kİnh<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Hång Hå L«",{"Nguyªn liÖu hîp thµnh <color=green>Hång Hå L«<color>:\n<color=yellow>1 Háa Long Tiªu<color>\n<color=yellow>1 Kim Cang Ph¸ch<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>25%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Ngäc H­ Phï",{"Nguyªn liÖu hîp thµnh <color=green>Ngäc H­ Phï<color>:\n<color=yellow>1 B×nh L­u Ly<color>\n<color=yellow>1 Bİch Tú Bµ<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Cäc §én Long",{"Nguyªn liÖu hîp thµnh <color=green>Cäc §én Long<color>:\n<color=yellow>1 Tói Ng« Phong<color>\n<color=yellow>1 Bµn Cæ ph­ín<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Kİnh ChiÕu Yªu",{"Nguyªn liÖu hîp thµnh <color=green>Kİnh ChiÕu Yªu<color>:\n<color=yellow>1 ¢m D­¬ng Kİnh<color>\n<color=yellow>1 Th¸i D­¬ng Ch©m<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Phong Háa Lu©n",{"Nguyªn liÖu hîp thµnh <color=green>Phong Háa Lu©n<color>:\n<color=yellow>1 Kim Cang Ph¸ch<color>\n<color=yellow>1 Êm V¹n Nha<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>25%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Kim Quang Táa",{"Nguyªn liÖu hîp thµnh <color=green>Kim Quang Táa<color>:\n<color=yellow>1 Bİch Tú Bµ<color>\n<color=yellow>1 D©y Ph­îc Long<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"H¹nh Hoµng Kú",{"Nguyªn liÖu hîp thµnh <color=green>H¹nh Hoµng Kú<color>:\n<color=yellow>1 Hçn Thiªn L¨ng<color>\n<color=yellow>1 Bµn Cæ ph­ín<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Cµn Kh«n Khuyªn",{"Nguyªn liÖu hîp thµnh <color=green>Cµn Kh«n Khuyªn<color>:\n<color=yellow>1 Cµn Kh«n Xİch<color>\n<color=yellow>1 Th¸i D­¬ng Ch©m<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"B¹ch Cèt Ph­ín",{"Nguyªn liÖu hîp thµnh <color=green>B¹ch Cèt Ph­ín<color>:\n<color=yellow>1 Háa Long Tiªu<color>\n<color=yellow>1 Êm V¹n Nha<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>25%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"§Şnh Phong Ch©u",{"Nguyªn liÖu hîp thµnh <color=green>§Şnh Phong Ch©u<color>:\n<color=yellow>1 B×nh L­u Ly<color>\n<color=yellow>1 D©y Ph­îc Long<color>\n<color=yellow>1 Hång thñy tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Hçn nguyªn ch©u",{"Nguyªn liÖu hîp thµnh <color=green>Hçn nguyªn ch©u<color>:\n<color=yellow>4 Tø t­îng tinh th¹ch<color>\n<color=yellow>8 T­íng Qu©n lÖnh<color>\n<color=yellow>10 Dung tinh lé<color>","Phİ hîp thµnh: <color=yellow>1000 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Thanh V©n KiÕm",{"Nguyªn liÖu hîp thµnh <color=green>Thanh V©n KiÕm<color>:\n<color=yellow>1 Cµn Kh«n Xİch<color>\n<color=yellow>1 Ngò Quang th¹ch<color>\n<color=yellow>1 Cµn Kh«n Khuyªn<color>","Nguyªn liÖu hîp thµnh <color=green>Thanh V©n KiÕm<color>:\n<color=yellow>1 ChÊn Thiªn Cung<color>\n<color=yellow>20 S¬n th¹ch<color>","Phİ hîp thµnh: <color=yellow>80 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Ngäc Nh­ ı ",{"Nguyªn liÖu hîp thµnh <color=green>Ngäc Nh­ ı <color>:\n<color=yellow>1 B×nh L­u Ly<color>\n<color=yellow>1 H×nh Thiªn Ên<color>\n<color=yellow>1 D©y Khæn Tiªn<color>","Nguyªn liÖu hîp thµnh <color=green>Ngäc Nh­ ı <color>:\n<color=yellow>1 §Şnh Phong Ch©u<color>\n<color=yellow>20 S¬n th¹ch<color>","Phİ hîp thµnh: <color=yellow>80 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"Háa Tú Bµ",{"Nguyªn liÖu hîp thµnh <color=green>Háa Tú Bµ<color>:\n<color=yellow>1 Háa Long Tiªu<color>\n<color=yellow>1 Ngò Quang th¹ch<color>\n<color=yellow>1 B¹ch Cèt Ph­ín<color>","Nguyªn liÖu hîp thµnh <color=green>Háa Tú Bµ<color>:\n<color=yellow>1 Linh Lung Th¸p<color>\n<color=yellow>20 S¬n th¹ch<color>","Phİ hîp thµnh: <color=yellow>80 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>25%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"An MÖnh Phï",{"Nguyªn liÖu hîp thµnh <color=green>An MÖnh Phï<color>:\n<color=yellow>1 Hçn Thiªn L¨ng<color>\n<color=yellow>1 H×nh Thiªn Ên<color>\n<color=yellow>1 H¹nh Hoµng Kú<color>","Nguyªn liÖu hîp thµnh <color=green>An MÖnh Phï<color>:\n<color=yellow>1 Kim B¸t Vu<color>\n<color=yellow>20 S¬n th¹ch<color>","Phİ hîp thµnh: <color=yellow>80 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>26%<color>.\nThÊt b¹i mÊt nguyªn liÖu vµ phİ."}})
tinsert(PtFbBook,{"ThÊt B¶o Kim Liªn#Gi¸p SÜ",{"Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#Gi¸p SÜ<color>:\n<color=yellow>1 §å Phæ ThÊt B¶o Kim Liªn (Gi¸p SÜ)<color>\n<color=yellow>50 Tø Tinh<color>\n<color=yellow>30 HuyÔn Linh Nh·n<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#Gi¸p SÜ<color>:\n<color=yellow>10 T­íng Qu©n lÖnh<color>","Phİ hîp thµnh: <color=yellow>5000 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"ThÊt B¶o Kim Liªn#§¹o SÜ",{"Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#§¹o SÜ<color>:\n<color=yellow>1 §å Phæ ThÊt B¶o Kim Liªn (§¹o SÜ)<color>\n<color=yellow>50 Tø Tinh<color>\n<color=yellow>30 HuyÔn Linh Nh·n<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#§¹o SÜ<color>:\n<color=yellow>10 T­íng Qu©n lÖnh<color>","Phİ hîp thµnh: <color=yellow>5000 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"ThÊt B¶o Kim Liªn#DŞ Nh©n",{"Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#DŞ Nh©n<color>:\n<color=yellow>1 §å Phæ ThÊt B¶o Kim Liªn (DŞ Nh©n)<color>\n<color=yellow>50 Tø Tinh<color>\n<color=yellow>30 HuyÔn Linh Nh·n<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt B¶o Kim Liªn#DŞ Nh©n<color>:\n<color=yellow>10 T­íng Qu©n lÖnh<color>","Phİ hîp thµnh: <color=yellow>5000 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"QuÇn Anh Héi 11",{"Nguyªn liÖu hîp thµnh <color=green>QuÇn Anh Héi 11<color>:\n<color=yellow>1 QuÇn Anh Héi 9<color>\n<color=yellow>1 LÖnh Bµi Gi¶m Thêi Gian §ãng B¨ng<color>\n<color=yellow>1 LÖnh Bµi Gi¶m Thêi Gian Tróng Th­¬ng<color>","Nguyªn liÖu hîp thµnh <color=green>QuÇn Anh Héi 11<color>:\n<color=yellow>1 LÖnh Bµi T¨ng Thêi Gian Th?Th­¬ng<color>\n<color=yellow>10 T­íng Qu©n LÖnh Siªu CÊp<color>","Phİ hîp thµnh: <color=yellow>200 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"QuÇn Anh Héi 11 (Siªu CÊp)",{"Nguyªn liÖu hîp thµnh <color=green>QuÇn Anh Héi 11 (Siªu CÊp)<color>:\n<color=yellow>1 QuÇn Anh Héi 11<color>\n<color=yellow>1 LÖnh Bµi T?Tinh CÊp 1<color>\n<color=yellow>1 LÖnh Bµi Sinh Lùc % <color>","Nguyªn liÖu hîp thµnh <color=green>QuÇn Anh Héi 11 (Siªu CÊp)<color>:\n<color=yellow>1 LÖnh Bµi T¨ng Thêi Gian Th?Th­¬ng<color>\n<color=yellow>15 T­íng Qu©n LÖnh Siªu CÊp<color>","Phİ hîp thµnh: <color=yellow>300 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"ThÊt TrÇn Trai Ph¸p B¶o(Ma)(khãa)",{"Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(Ma)(khãa)<color>:\n<color=yellow>1 Khu«n Ph¸p B¶o Tiªn Ma 40<color>\n<color=yellow>250 M¶nh ThÊt TrÇn Trai (Ma)<color>\n<color=yellow>250 HuyÕt Ma Linh Lé Tinh<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(Ma)(khãa)<color>:\n<color=yellow>250 Thiªn Tiªn Lé Tinh<color>\n<color=yellow>10 §Çu Th©n C«ng B¸o<color>\n<color=yellow>10 §Çu Minh Sa Ma<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(Ma)(khãa)<color>:\n<color=yellow>10 §Çu Ngò §éc Xµ<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"ThÊt TrÇn Trai Ph¸p B¶o(ThÇn)(khãa)",{"Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(ThÇn)(khãa)<color>:\n<color=yellow>1 Khu«n Ph¸p B¶o Tiªn Ma 40<color>\n<color=yellow>250 M¶nh ThÊt TrÇn Trai (Tiªn)<color>\n<color=yellow>250 TuyÕt Linh Tiªn Lé Tinh<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(ThÇn)(khãa)<color>:\n<color=yellow>250 Thiªn Tiªn Lé Tinh<color>\n<color=yellow>10 §Çu Th©n C«ng B¸o<color>\n<color=yellow>10 §Çu Minh Sa Ma<color>","Nguyªn liÖu hîp thµnh <color=green>ThÊt TrÇn Trai Ph¸p B¶o(ThÇn)(khãa)<color>:\n<color=yellow>10 §Çu Ngò §éc Xµ<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *GS",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *GS<color>:\n<color=yellow>1 Lôc Hån Phiªn *GS<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *§S",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *§S<color>:\n<color=yellow>1 Lôc Hån Phiªn *§S<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>10 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>20 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})
tinsert(PtFbBook,{"Lôc Hån Phiªn *DN",{"Nguyªn liÖu hîp thµnh <color=green>Lôc Hån Phiªn *DN<color>:\n<color=yellow>1 Lôc Hån Phiªn *DN<color>\n<color=yellow>500 Canh Tinh<color>","Phİ hîp thµnh: <color=yellow>0 v¹n<color>.\nTû lÖ thµnh c«ng: <color=yellow>100%<color>."}})

function pt_fabao_book(page)
 local options={}
 local first=(page-1)*6+1
 local last=min(getn(PtFbBook),first+5)
 for i=first,last do tinsert(options,PtFbBook[i][1].."/pt_fabao_formula("..i..","..page..")") end
 if page>1 then tinsert(options,"Trang tr­íc/pt_fabao_book("..(page-1)..")") end
 if last<getn(PtFbBook) then tinsert(options,"Trang sau/pt_fabao_book("..(page+1)..")") end
 tinsert(options,"Quay l¹i/main")
 Say("Chän c«ng thøc Ph¸p B¶o muèn xem.",getn(options),options)
end
function pt_fabao_formula(i,page)
 if not PtFbBook[i] then return end
 local pages=PtFbBook[i][2]
 local callback="pt_fabao_book("..page..")"
 if getn(pages)==4 then Talk(4,callback,pages[1],pages[2],pages[3],pages[4])
 elseif getn(pages)==3 then Talk(3,callback,pages[1],pages[2],pages[3])
 elseif getn(pages)==2 then Talk(2,callback,pages[1],pages[2])
 else Talk(1,callback,pages[1]) end
end

function pt_vtt_merge_menu()
 SayButtons("GhĞp 2 Linh Phï cïng lo¹i thµnh 1 Linh Phï bËc kÕ tiÕp.",4,
 "2 Thæ -> 1 Thñy/pt_vtt_merge(1)","2 Thñy -> 1 Háa/pt_vtt_merge(2)","2 Háa -> 1 Phong/pt_vtt_merge(3)","Quay l¹i/main")
end
function pt_vtt_merge(tier)
 if EventVttMerge(tier)==1 then Msg2Player("GhĞp Linh Phï thµnh c«ng.")
 else Msg2Player("CÇn 2 Linh Phï ®óng lo¹i trong tói vµ chç trèng F4. Ch­a trõ nguyªn liÖu.") end
end

-- PT_FABAO_SUPPLEMENT_20261102
PtFbExtra={
 {"Tam Sinh Th¹ch","An MÖnh Phï"},
 {"Kim S¬n","Háa Tú Bµ"},
 {"ThÊt TrÇn Trai (vò khİ)","Thanh V©n KiÕm"},
 {"ThÊt TrÇn Trai (bïa ph¸p)","Ngäc Nh­ ı"}
}
function pt_fb_extra()
 SayButtons("Ph¸p B¶o Nh©n Giíi: c«ng thøc bæ sung, dïng chØ sè gèc. Chän lo¹i muèn hîp thµnh hoÆc n©ng cÊp.",5,
 "Tam Sinh Th¹ch/pt_fb_family(1)","Kim S¬n/pt_fb_family(2)","ThÊt TrÇn Trai (vò khİ)/pt_fb_family(3)","ThÊt TrÇn Trai (bïa ph¸p)/pt_fb_family(4)","Quay l¹i/pt_fabao_menu")
end
function pt_fb_family(i)
 if not PtFbExtra[i] then return end
 PtFbExtraChoice=i
 SayButtons(PtFbExtra[i][1]..": chän chøc n¨ng. C«ng thøc hîp thµnh míi kh«ng thay ®æi thuéc tİnh gèc.",4,
 "C«ng thøc hîp thµnh/pt_fb_craft_info","Nguyªn liÖu n©ng cÊp/pt_fb_upgrade_info","Më cöa sæ ghĞp/dz","Quay l¹i/pt_fb_extra")
end
function pt_fb_craft_info()
 local f=PtFbExtra[PtFbExtraChoice]
 if not f then return end
 Talk(2,"pt_fb_extra","Hîp thµnh "..f[1]..": 1 "..f[2].." cÊp 1 + 50 Tø Tinh + 30 HuyÔn Linh Nh·n + 10 T­íng Qu©n LÖnh.","Phİ: 5.000 v¹n l­îng. Thµnh c«ng: 100%. NhËn ph¸p b¶o cÊp 1 víi thuéc tİnh gèc; gi÷ tr¹ng th¸i khãa cña nguyªn liÖu. Më cöa sæ ghĞp ®Ó xem vµ x¸c nhËn.")
end
function pt_fb_upgrade_info()
 Talk(3,"pt_fb_extra","N©ng cÊp tõ 0 ®Õn 12 lÇn, dïng ph¸p b¶o cÇn n©ng cÊp vµ 5 Hång b¶o th¹ch mçi lÇn.","LÇn 1-3: 2 Lß tinh luyÖn s¬ cÊp + 2 Dung tinh lé. LÇn 4-6: 2 Lß tinh luyÖn trung cÊp + 3 Dung tinh lé.","LÇn 7-9: 2 Lß tinh luyÖn cao cÊp + 4 Dung tinh lé. LÇn 10-12: 5 Dung tinh lé + 2 Lß luyÖn Tr©n Hùu. Phİ vµ tªn nguyªn liÖu chİnh x¸c ®­îc kiÓm tra trong cöa sæ ghĞp tr­íc khi x¸c nhËn.")
end
