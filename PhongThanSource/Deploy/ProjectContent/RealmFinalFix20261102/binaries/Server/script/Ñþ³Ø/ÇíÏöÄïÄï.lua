--description:npc
--author: zhujialiang
--date:2005/4/13

function  main()
		Talk(1,"no",11241)
end;

function  no()
	CloseDialog()
end;

-- PT_ISLAND_FAME_BEGIN
PT_QT_OLD_MAIN=main
function main()
 local ok,balance,rate=PtIslandFame(0)
 if ok~=1 then PT_QT_OLD_MAIN();return end
 SayButtons("Quúnh Tiªu - danh väng Ph­¬ng Tr­îng: "..balance..". Mçi Tam Tiªm Xoa ®æi "..rate.." ®iÓm danh väng.",5,"§æi danh väng/qt_exchange","Xem danh väng/qt_view","§æi ®å phæ (1000 ®iÓm)/qt_recipe","§æi phi phong cÊp 100 (3000 ®iÓm)/qt_cloak","Trß chuyÖn/qt_old")
end
function qt_old() PT_QT_OLD_MAIN() end
function qt_exchange() SayButtons("Chän sè Tam Tiªm Xoa muèn giao. VËt phÈm ph¶i ch­a khãa vµ n»m trong hµnh trang.",4,"Giao 1/qt_one","Giao 10/qt_ten","Giao 50/qt_fifty","Quay l¹i/main") end
function qt_give(n) local ok,balance=PtIslandFame(1,n) if ok~=1 then Talk(1,"main","Ch­a ®æi ®­îc. KiÓm tra sè Tam Tiªm Xoa ch­a khãa trong hµnh trang.");return end Talk(1,"main","§æi thµnh c«ng. Danh väng Ph­¬ng Tr­îng hiÖn cã: "..balance..".") end
function qt_one() qt_give(1) end
function qt_ten() qt_give(10) end
function qt_fifty() qt_give(50) end
function qt_view() local ok,balance=PtIslandFame(0) if ok~=1 then CloseDialog();return end Talk(1,"main","Danh väng Ph­¬ng Tr­îng: "..balance..". Dïng 1000 ®iÓm ®æi ®å phæ Ph¸ Qu©n hoÆc 3000 ®iÓm ®æi phi phong lôc cÊp 100 theo hÖ ph¸i.") end
function qt_recipe() MsgBox("§æi 1000 danh väng lÊy ®å phæ Ph¸ Qu©n phï hîp hÖ ph¸i?","qt_recipe_yes","main") end
function qt_recipe_yes() qt_reward(2) end
function qt_cloak() MsgBox("§æi 3000 danh väng lÊy phi phong lôc cÊp 100 phï hîp hÖ ph¸i?","qt_cloak_yes","main") end
function qt_cloak_yes() qt_reward(3) end
function qt_reward(op) local ok,balance=PtIslandFame(op) if ok~=1 then Talk(1,"main","Ch­a ®æi ®­îc. KiÓm tra ®ñ danh väng, chç trèng hµnh trang vµ hÖ ph¸i phï hîp.");return end Talk(1,"main","§æi th­ëng thµnh c«ng. Danh väng cßn l¹i: "..balance..".") end
