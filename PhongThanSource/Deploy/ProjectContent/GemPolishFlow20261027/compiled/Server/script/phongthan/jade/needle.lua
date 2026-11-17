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
 if mode==1 then text="Chän ngäc hoÆc Qu¸i Phï ch­a mµi trong tói. PhÝ mµi sÏ hiÖn tr­íc khi x¸c ®Þnh; mçi lÇn mµi 1 vËt phÈm." end
 if mode==2 then text="Chän ngäc ®· mµi trong tói ®Ó hoµn nguyªn. Thuéc tÝnh hiÖn t¹i sÏ mÊt, nhËn l¹i ®óng lo¹i ngäc ch­a mµi." end
 Say(text,getn(options),options)
end
function jade_extract() jade_list(0,1) end
function jade_polish() jade_list(1,1) end
function jade_revert() jade_list(2,1) end
function jade_confirm(mode,id)
 if mode==2 then jade_revert_confirm(id) return end
 local text="Thñ tñy sÏ mÊt Ngäc Béi ®· chän ®Ó nhËn 1 Ngäc Tñy ngÉu nhiªn. §ång ý?"
 local operation=2
 if mode==1 then
  local info=JadeService(5,id)
  if not info[1] then jade_list(1,1) return end
  text="Mµi s¸ng 1 <color=green>"..info[1].."<color>, cÇn <color=green>"..(info[2]/10000).." v¹n<color> b¹c, ®ång ý khai quang chø?"
  operation=3
 end
 SayButtons(text,2,"X¸c ®Þnh/jade_do("..operation..","..id..")","Hñy/jade_list("..mode..",1)")
end
function jade_do(operation,id)
 local result,output=JadeService(operation,id)
 if operation==3 and result==1 then
  local info=JadeService(5,output)
  if not info[1] then jade_result(result) return end
  local star=""
  if info[5]==1 then star="<pic=428>" end
  local text="Mµi s¸ng thµnh c«ng <color=green>"..info[1].."<color>! Thuéc tÝnh cña ngäc lµ:\n\n<color=99,101,255>"..star..info[3].."<color>\n\nCã ph¶i muèn tiÕn hµnh hoµn nguyªn cho nã?"
  SayButtons(text,2,"X¸c ®Þnh/jade_revert_confirm("..output..")","Hñy/main")
 else jade_result(result) end
end
function jade_revert_confirm(id)
 local info=JadeService(5,id)
 if not info[1] or info[4]~=1 then jade_list(2,1) return end
 SayButtons("Hoµn nguyªn <color=green>"..info[1].."<color> cÇn <color=green>1 Di Quang KÝnh<color> hoÆc <color=green>18 Xu<color>. X¸c ®Þnh muèn hoµn nguyªn chø?",2,
 "X¸c ®Þnh/jade_revert_payment("..id..")","Hñy/main")
end
function jade_revert_payment(id)
 SayButtons("Chän nguyªn liÖu muèn dïng ®Ó hoµn nguyªn. ChØ trõ lo¹i ®· chän khi hoµn nguyªn thµnh c«ng.",3,
 "Di Quang KÝnh/jade_revert_do("..id..",0)","18 Xu/jade_revert_do("..id..",1)","Hñy/main")
end
function jade_revert_do(id,payment) jade_result(JadeService(4,id,payment)) end
