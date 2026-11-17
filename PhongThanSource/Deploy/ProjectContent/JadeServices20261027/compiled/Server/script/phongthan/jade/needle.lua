-- Native transaction validates the equipped serial and every ingredient.
function main()
 local id=JadeService(0)
 if id==0 then Talk(1,"no","B¹n ch­a trang bÞ Ngäc Béi.") return end
 SayButtons("Chän hÖ Ngäc Tñy muèn chó nhËp. Mçi lÇn dïng 1 DÉn Tñy Ch©m vµ 5 Ngäc Tñy cïng hÖ. TÈy l¹i hÖ ®· cã cÇn thªm 1 TÈy Tñy Ch©u.",5,
 "Háa Ngäc Tñy/jade_choose(1,"..id..")","B¨ng Ngäc Tñy/jade_choose(2,"..id..")","Thæ Ngäc Tñy/jade_choose(3,"..id..")","L«i Ngäc Tñy/jade_choose(4,"..id..")","Kim Ngäc Tñy/jade_choose(5,"..id..")")
end
function jade_choose(group,id)
 SayButtons("TÈy tñy Ngäc Béi ®ang ®eo víi 5 Ngäc Tñy cïng hÖ vµ 1 DÉn Tñy Ch©m. NÕu hÖ nµy ®· cã dßng th× dïng thªm 1 TÈy Tñy Ch©u. §ång ý?",2,
 "X¸c ®Þnh/jade_refine("..group..","..id..")","Quay l¹i/main")
end
function jade_refine(group,id) jade_result(JadeService(1,group,id)) end
function no() CloseDialog() end

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
