-- Article recipe inputs; six new families explicitly use Loose trial combat formulas.
function main() pttk_page(1) end
function pttk_page(page)
 local options={}
 if page==1 then
 tinsert(options,"Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n)".."/pttk_confirm_1")
 tinsert(options,"Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167\181o)".."/pttk_confirm_2")
 tinsert(options,"Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Ng\249 S\203u)".."/pttk_confirm_3")
 tinsert(options,"Th\199n K\252 Tri\214u H\229i".."/pttk_confirm_4")
 tinsert(options,"Th\199n K\252 Kh\171 M\233c Ph\239ng Xu\169n".."/pttk_confirm_5")
 tinsert(options,"Th\199n K\252 \167o\182n Binh Giao Ti\213p".."/pttk_confirm_6")
 end
 if page==2 then
 tinsert(options,"Th\199n K\252 H\228a \167\222a Vi Lao".."/pttk_confirm_7")
 tinsert(options,"Th\199n K\252 Xung Phong H\183m Tr\203n".."/pttk_confirm_8")
 tinsert(options,"Th\199n K\252 Nam Minh Ly H\225a".."/pttk_confirm_9")
 tinsert(options,"Th\199n K\252 Thi\170n To\181n \167\222a Chuy\211n".."/pttk_confirm_10")
 tinsert(options,"Th\199n K\252 B\168ng H\229n Tuy\213t Ph\184ch".."/pttk_confirm_11")
 tinsert(options,"Th\199n K\252 Huy\213t Nguy\214t Tr\183m (Loose th\246 nghi\214m)".."/pttk_confirm_12")
 end
 if page==3 then
 tinsert(options,"Th\199n K\252 Huy\210n \182nh T\181n Hoa (Loose th\246 nghi\214m)".."/pttk_confirm_13")
 tinsert(options,"Th\199n K\252 Ph\203t N\233 H\225a Li\170n (Loose th\246 nghi\214m)".."/pttk_confirm_14")
 tinsert(options,"Th\199n K\252 L\171i H\225a T\181n Phong (Loose th\246 nghi\214m)".."/pttk_confirm_15")
 tinsert(options,"Th\199n K\252 Truy H\229n T\184n Hoa (Loose th\246 nghi\214m)".."/pttk_confirm_16")
 tinsert(options,"Th\199n K\252 V\185n Ki\213m Quy T\171ng (Loose th\246 nghi\214m)".."/pttk_confirm_17")
 tinsert(options,"Th\199n K\252 Huy\213t Nguy\214t Tr\183m C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_18")
 end
 if page==4 then
 tinsert(options,"Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_19")
 tinsert(options,"Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_20")
 tinsert(options,"Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_21")
 tinsert(options,"Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_22")
 tinsert(options,"Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2 - n\169ng b\187ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_23")
 tinsert(options,"Th\199n K\252 Huy\213t Nguy\214t Tr\183m C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_24")
 end
 if page==5 then
 tinsert(options,"Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_25")
 tinsert(options,"Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_26")
 tinsert(options,"Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_27")
 tinsert(options,"Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_28")
 tinsert(options,"Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2 - n\169ng kh\171ng m\182nh (Loose th\246 nghi\214m)".."/pttk_confirm_29")
 end
 if page>1 then tinsert(options,"Trang tr\173\237c".."/pttk_page("..(page-1)..")") end
 if page<5 then tinsert(options,"Trang sau".."/pttk_page("..(page+1)..")") end
 tinsert(options,"\167\227ng".."/pttk_close")
 SayButtons("H\229 L\171 \167\187ng - \174\230i Th\199n K\252 t\245 c\202p 140, \174\243ng h\214 ph\184i. S\184u nh\227m m\237i d\239ng c\171ng th\248c Loose th\246 nghi\214m, ch\216nh trong Studio. Ch\228n s\184ch \174\211 xem nguy\170n li\214u.",getn(options),options)
end
function pttk_confirm_1()
 SayButtons("Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n)\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_1","Quay l\185i".."/main")
end
function pttk_buy_1()
 local result=PtThanKyExchange(1)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_2()
 SayButtons("Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167\181o)\010C\199n: 4 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_2","Quay l\185i".."/main")
end
function pttk_buy_2()
 local result=PtThanKyExchange(2)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_3()
 SayButtons("Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Ng\249 S\203u)\010C\199n: 6 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_3","Quay l\185i".."/main")
end
function pttk_buy_3()
 local result=PtThanKyExchange(3)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_4()
 SayButtons("Th\199n K\252 Tri\214u H\229i\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_4","Quay l\185i".."/main")
end
function pttk_buy_4()
 local result=PtThanKyExchange(4)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_5()
 SayButtons("Th\199n K\252 Kh\171 M\233c Ph\239ng Xu\169n\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_5","Quay l\185i".."/main")
end
function pttk_buy_5()
 local result=PtThanKyExchange(5)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_6()
 SayButtons("Th\199n K\252 \167o\182n Binh Giao Ti\213p\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_6","Quay l\185i".."/main")
end
function pttk_buy_6()
 local result=PtThanKyExchange(6)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_7()
 SayButtons("Th\199n K\252 H\228a \167\222a Vi Lao\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_7","Quay l\185i".."/main")
end
function pttk_buy_7()
 local result=PtThanKyExchange(7)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_8()
 SayButtons("Th\199n K\252 Xung Phong H\183m Tr\203n\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_8","Quay l\185i".."/main")
end
function pttk_buy_8()
 local result=PtThanKyExchange(8)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_9()
 SayButtons("Th\199n K\252 Nam Minh Ly H\225a\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_9","Quay l\185i".."/main")
end
function pttk_buy_9()
 local result=PtThanKyExchange(9)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_10()
 SayButtons("Th\199n K\252 Thi\170n To\181n \167\222a Chuy\211n\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_10","Quay l\185i".."/main")
end
function pttk_buy_10()
 local result=PtThanKyExchange(10)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_11()
 SayButtons("Th\199n K\252 B\168ng H\229n Tuy\213t Ph\184ch\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.",2,"X\184c nh\203n".."/pttk_buy_11","Quay l\185i".."/main")
end
function pttk_buy_11()
 local result=PtThanKyExchange(11)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_12()
 SayButtons("Th\199n K\252 Huy\213t Nguy\214t Tr\183m\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_12","Quay l\185i".."/main")
end
function pttk_buy_12()
 local result=PtThanKyExchange(12)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_13()
 SayButtons("Th\199n K\252 Huy\210n \182nh T\181n Hoa\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_13","Quay l\185i".."/main")
end
function pttk_buy_13()
 local result=PtThanKyExchange(13)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_14()
 SayButtons("Th\199n K\252 Ph\203t N\233 H\225a Li\170n\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_14","Quay l\185i".."/main")
end
function pttk_buy_14()
 local result=PtThanKyExchange(14)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_15()
 SayButtons("Th\199n K\252 L\171i H\225a T\181n Phong\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_15","Quay l\185i".."/main")
end
function pttk_buy_15()
 local result=PtThanKyExchange(15)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_16()
 SayButtons("Th\199n K\252 Truy H\229n T\184n Hoa\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_16","Quay l\185i".."/main")
end
function pttk_buy_16()
 local result=PtThanKyExchange(16)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_17()
 SayButtons("Th\199n K\252 V\185n Ki\213m Quy T\171ng\010C\199n: 2 m\182nh t\173\172ng \248ng + 1 \167a B\182o L\214nh B\181i + 1000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_17","Quay l\185i".."/main")
end
function pttk_buy_17()
 local result=PtThanKyExchange(17)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_18()
 SayButtons("Th\199n K\252 Huy\213t Nguy\214t Tr\183m C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_18","Quay l\185i".."/main")
end
function pttk_buy_18()
 local result=PtThanKyExchange(18)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_19()
 SayButtons("Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_19","Quay l\185i".."/main")
end
function pttk_buy_19()
 local result=PtThanKyExchange(19)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_20()
 SayButtons("Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_20","Quay l\185i".."/main")
end
function pttk_buy_20()
 local result=PtThanKyExchange(20)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_21()
 SayButtons("Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_21","Quay l\185i".."/main")
end
function pttk_buy_21()
 local result=PtThanKyExchange(21)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_22()
 SayButtons("Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_22","Quay l\185i".."/main")
end
function pttk_buy_22()
 local result=PtThanKyExchange(22)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_23()
 SayButtons("Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 m\182nh t\173\172ng \248ng + 2 \167a B\182o L\214nh B\181i + 2000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_23","Quay l\185i".."/main")
end
function pttk_buy_23()
 local result=PtThanKyExchange(23)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_24()
 SayButtons("Th\199n K\252 Huy\213t Nguy\214t Tr\183m C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_24","Quay l\185i".."/main")
end
function pttk_buy_24()
 local result=PtThanKyExchange(24)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_25()
 SayButtons("Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_25","Quay l\185i".."/main")
end
function pttk_buy_25()
 local result=PtThanKyExchange(25)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_26()
 SayButtons("Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_26","Quay l\185i".."/main")
end
function pttk_buy_26()
 local result=PtThanKyExchange(26)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_27()
 SayButtons("Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_27","Quay l\185i".."/main")
end
function pttk_buy_27()
 local result=PtThanKyExchange(27)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_28()
 SayButtons("Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_28","Quay l\185i".."/main")
end
function pttk_buy_28()
 local result=PtThanKyExchange(28)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_confirm_29()
 SayButtons("Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2\010C\199n: 1 s\184ch c\239ng nh\227m c\202p 1 + 4 \167a B\182o L\214nh B\181i + 4000 Linh Th\185ch. Nh\203n 1 s\184ch; ph\182i k\221ch ho\185t \174\211 nh\203n c\202p chi\170u m\237i.\010C\171ng th\248c chi\213n \174\202u: Loose th\246 nghi\214m, kh\171ng ph\182i th\171ng s\232 VNG \174\183 x\184c minh.",2,"X\184c nh\203n".."/pttk_buy_29","Quay l\185i".."/main")
end
function pttk_buy_29()
 local result=PtThanKyExchange(29)
 if result==1 then Msg2Player("\167\230i th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 thay c\202p chi\170u c\242.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra s\184ch c\202p 1/nguy\170n li\214u, c\202p/ph\184i, ch\231 tr\232ng, kh\227a/giao d\222ch v\181 c\202u h\215nh th\246 nghi\214m.") end
 main()
end
function pttk_close() CloseDialog() end
