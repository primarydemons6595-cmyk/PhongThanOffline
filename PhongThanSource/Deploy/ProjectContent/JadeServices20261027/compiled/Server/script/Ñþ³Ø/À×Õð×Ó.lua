-- Lei Zhen Zi, Dieu Tri only. Existing socket callbacks preserved.
function main()
 SayButtons("Ta cã thÓ ®ôc lç, kh¶m n¹m, trang trÝ, thñ tñy Ngäc Béi vµ mµi ngäc. Ng­¬i cÇn gióp ®iÒu g×?",5,
 "§ôc lç - Trang trÝ/socket","Kh¶m n¹m/inlay","Thñ tñy Ngäc Béi/jade_extract","Mµi ngäc/jade_polish","ThuyÕt minh/help")
end
function socket()
 CloseDialog()
 EnchaseItem(2)
end
function inlay()
 SayButtons("Ng­¬i muèn kh¶m vµo lç nµo? NÕu lç ®· cã ngäc, kh¶m thµnh c«ng sÏ thay thÕ ngäc cò.",4,
 "Lç thø nhÊt/slot1", "Lç thø hai/slot2", "Lç thø ba/slot3", "Lç thø t­/slot4")
end
function slot1() CloseDialog() EnchaseItem(3) end
function slot2() CloseDialog() EnchaseItem(4) end
function slot3() CloseDialog() EnchaseItem(5) end
function slot4() CloseDialog() EnchaseItem(6) end
function help()
 Talk(1,"main","§Æt mét trang bÞ cïng nguyªn liÖu vµo cöa sæ GhÐp. KiÓm tra phÝ tr­íc khi thùc hiÖn.\n§ôc lç: dïng Hoµng Thñy Tinh t­¬ng øng víi thø tù lç.\nTrang trÝ: dïng Tö Thñy Tinh t­¬ng øng; cã thÓ ®Æt thªm BÝ Ng©n. Ngäc ®· kh¶m ®­îc gi÷ l¹i khi trang trÝ lç.\nKh¶m: chän vÞ trÝ lç, ®Æt ngäc vµ nguyªn liÖu kh¶m t­¬ng øng.\nSè lç tèi ®a phô thuéc lo¹i, phÈm chÊt vµ ®¼ng cÊp trang bÞ. Trªn vËt phÈm, sè tr­íc dÊu / lµ sè lç ®· kh¶m, sè sau lµ sè lç ®· ®ôc.")
end

function jade_result(result)
 if result==1 then Talk(1,"main","§· hoµn thµnh.")
 elseif result==-2 then Talk(1,"main","Ngäc Béi ®· ®ñ sè hÖ thuéc tÝnh hoÆc d÷ liÖu ch­a phï hîp. Cã thÓ tÈy l¹i hÖ ®· cã b»ng TÈy Tñy Ch©u.")
 else Talk(1,"main","Ch­a thùc hiÖn ®­îc. KiÓm tra vËt phÈm ®· chän, khãa tói, chç trèng vµ nguyªn liÖu hoÆc tiÒn cÇn dïng.") end
end
function jade_list(mode,page)
 local items=JadeList(mode)
 local options={}
 local first=(page-1)*6+1
 local last=min(getn(items),first+5)
 for i=first,last do
  tinsert(options,items[i][2].."/jade_confirm("..mode..","..items[i][1]..")")
 end
 if page>1 then tinsert(options,"Trang tr­íc/jade_list("..mode..","..(page-1)..")") end
 if last<getn(items) then tinsert(options,"Trang sau/jade_list("..mode..","..(page+1)..")") end
 tinsert(options,"Quay l¹i/main")
 local text="Chän Ngäc Béi trong tói ®Ó thñ tñy. Th¸o Ngäc Béi khái ng­êi tr­íc. Sau khi x¸c nhËn, Ngäc Béi mÊt vµ nhËn 1 Ngäc Tñy ngÉu nhiªn."
 if mode==1 then text="Chän ngäc hoÆc Qu¸i Phï ch­a mµi trong tói. PhÝ mçi lÇn mµi lµ 5 v¹n; mçi lÇn mµi 1 vËt phÈm." end
 Say(text,getn(options),options)
end
function jade_extract() jade_list(0,1) end
function jade_polish() jade_list(1,1) end
function jade_confirm(mode,id)
 local text="Thñ tñy sÏ mÊt Ngäc Béi ®· chän ®Ó nhËn 1 Ngäc Tñy ngÉu nhiªn. §ång ý?"
 local operation=2
 if mode==1 then text="Dïng 5 v¹n ®Ó mµi 1 ngäc hoÆc Qu¸i Phï ®· chän. §ång ý?" operation=3 end
 SayButtons(text,2,"X¸c ®Þnh/jade_do("..operation..","..id..")","Quay l¹i/jade_list("..mode..",1)")
end
function jade_do(operation,id) jade_result(JadeService(operation,id)) end
