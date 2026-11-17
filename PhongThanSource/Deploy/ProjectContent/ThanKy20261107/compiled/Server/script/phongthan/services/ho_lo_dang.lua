-- VNG item identities/recipe materials; atomic server-owned exchange.
function main()
 pttk_page(1)
end
function pttk_page(page)
 local options={}
 if page==1 then
 tinsert(options,"<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n)<c>".."/pttk_confirm_1")
 tinsert(options,"<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167\181o)<c>".."/pttk_confirm_2")
 tinsert(options,"<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Ng\249 S\203u)<c>".."/pttk_confirm_3")
 tinsert(options,"<c=pk>Th\199n K\252 Tri\214u H\229i<c>".."/pttk_confirm_4")
 tinsert(options,"<c=pk>Th\199n K\252 Kh\171 M\233c Ph\239ng Xu\169n<c>".."/pttk_confirm_5")
 tinsert(options,"<c=orange>Th\199n K\252 \167o\182n Binh Giao Ti\213p<c>".."/pttk_confirm_6")
 end
 if page==2 then
 tinsert(options,"<c=orange>Th\199n K\252 H\228a \167\222a Vi Lao<c>".."/pttk_confirm_7")
 tinsert(options,"<c=orange>Th\199n K\252 Xung Phong H\183m Tr\203n<c>".."/pttk_confirm_8")
 tinsert(options,"<c=cyan>Th\199n K\252 Nam Minh Ly H\225a<c>".."/pttk_confirm_9")
 tinsert(options,"<c=cyan>Th\199n K\252 Thi\170n To\181n \167\222a Chuy\211n<c>".."/pttk_confirm_10")
 tinsert(options,"<c=cyan>Th\199n K\252 B\168ng H\229n Tuy\213t Ph\184ch<c>".."/pttk_confirm_11")
 end
 if page==1 then tinsert(options,"Trang sau".."/pttk_page(2)") else tinsert(options,"Trang tr\173\237c".."/pttk_page(1)") end
 tinsert(options,"\167\227ng".."/pttk_close")
 SayButtons("H\229 L\171 \167\187ng - \174\230i Th\199n K\252 \174\243ng h\214, t\245 c\202p 140. Ch\228n s\184ch \174\211 xem nguy\170n li\214u v\181 x\184c nh\203n.",getn(options),options)
end
function pttk_confirm_1()
 SayButtons("<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n)<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_1","Quay l\185i".."/main")
end
function pttk_buy_1()
 local result=PtThanKyExchange(1)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_2()
 SayButtons("<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167\181o)<c>\010C\199n 4 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_2","Quay l\185i".."/main")
end
function pttk_buy_2()
 local result=PtThanKyExchange(2)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_3()
 SayButtons("<c=pk>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Ng\249 S\203u)<c>\010C\199n 6 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_3","Quay l\185i".."/main")
end
function pttk_buy_3()
 local result=PtThanKyExchange(3)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_4()
 SayButtons("<c=pk>Th\199n K\252 Tri\214u H\229i<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_4","Quay l\185i".."/main")
end
function pttk_buy_4()
 local result=PtThanKyExchange(4)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_5()
 SayButtons("<c=pk>Th\199n K\252 Kh\171 M\233c Ph\239ng Xu\169n<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_5","Quay l\185i".."/main")
end
function pttk_buy_5()
 local result=PtThanKyExchange(5)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_6()
 SayButtons("<c=orange>Th\199n K\252 \167o\182n Binh Giao Ti\213p<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_6","Quay l\185i".."/main")
end
function pttk_buy_6()
 local result=PtThanKyExchange(6)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_7()
 SayButtons("<c=orange>Th\199n K\252 H\228a \167\222a Vi Lao<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_7","Quay l\185i".."/main")
end
function pttk_buy_7()
 local result=PtThanKyExchange(7)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_8()
 SayButtons("<c=orange>Th\199n K\252 Xung Phong H\183m Tr\203n<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_8","Quay l\185i".."/main")
end
function pttk_buy_8()
 local result=PtThanKyExchange(8)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_9()
 SayButtons("<c=cyan>Th\199n K\252 Nam Minh Ly H\225a<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_9","Quay l\185i".."/main")
end
function pttk_buy_9()
 local result=PtThanKyExchange(9)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_10()
 SayButtons("<c=cyan>Th\199n K\252 Thi\170n To\181n \167\222a Chuy\211n<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_10","Quay l\185i".."/main")
end
function pttk_buy_10()
 local result=PtThanKyExchange(10)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_confirm_11()
 SayButtons("<c=cyan>Th\199n K\252 B\168ng H\229n Tuy\213t Ph\184ch<c>\010C\199n 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. \167\230i nh\203n 1 quy\211n s\184ch. K\221ch ho\185t s\184ch t\232n th\170m 1 Linh Th\185ch.",2,"X\184c nh\203n \174\230i".."/pttk_buy_11","Quay l\185i".."/main")
end
function pttk_buy_11()
 local result=PtThanKyExchange(11)
 if result==1 then Msg2Player("\167\230i Th\199n K\252 th\181nh c\171ng. B\202m chu\233t ph\182i v\181o s\184ch \174\211 k\221ch ho\185t.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p 140, h\214 ph\184i, ch\231 tr\232ng v\181 \174\248ng g\199n H\229 L\171 \167\187ng.") end
 main()
end
function pttk_close() CloseDialog() end
