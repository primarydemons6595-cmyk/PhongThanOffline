function main()
 local phase,left,cart,runs,limit,rider=EventEscortInfo(1)
 if phase~=1 and phase~=3 or cart==0 then return CloseDialog() end
 if rider==1 then SayButtons("Ng­¬i muèn xuèng xe? Xe sÏ dõng l¹i khi kh«ng cã ng­êi ®iÒu khiÓn.",2,"Xuèng xe/pt_cart_leave","TiÕp tôc ®iÒu khiÓn/no")
 else SayButtons("Ng­¬i muèn vµo xe ®Ó ®iÒu khiÓn? Khi ë trong xe kh«ng thÓ dïng vâ c«ng hoÆc phï di chuyÓn.",2,"Vµo xe/pt_cart_enter","Hñy/no") end
end
function pt_cart_enter()
 if EventEscortRide(1)==1 then CloseDialog()
 else SayButtons("ChØ chñ xe ®øng gÇn xe míi cã thÓ vµo xe.",1,"§ãng/no") end
end
function pt_cart_leave()
 if EventEscortRide(0)==1 then CloseDialog()
 else SayButtons("Kh«ng thÓ xuèng xe lóc nµy.",1,"§ãng/no") end
end
function no() CloseDialog() end

pt_daily_original_cart=main
function main()
 local n=GetNpcIdx();local tid=GetNpcTemplateID(n)
 if tid~=648 and tid~=649 then return pt_daily_original_cart() end
 SayButtons("Xe chuyÓn r­îu",2,"Vµo xe/pt_daily_wine_enter","Ra khái xe/pt_daily_wine_exit")
end
function pt_daily_wine_enter() if DailyWineRide(1)==1 then CloseDialog() end end
function pt_daily_wine_exit() if DailyWineRide(0)==1 then CloseDialog() end end
