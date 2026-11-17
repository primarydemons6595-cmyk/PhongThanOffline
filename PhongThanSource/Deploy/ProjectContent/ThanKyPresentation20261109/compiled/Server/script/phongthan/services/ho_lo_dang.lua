-- Dialog presentation only; recipe indices remain the native transaction contract.
function main() pttk_page(1) end
function pttk_page(page)
 local options={}
 if page==1 then
  tinsert(options,"<color=yellow>H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n) - D\222 Nh\169n<color>".."/pttk_confirm_1")
  tinsert(options,"<color=yellow>H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167ao) - D\222 Nh\169n<color>".."/pttk_confirm_2")
  tinsert(options,"<color=yellow>H\215nh Thi\170n Chi\213n \253 (Ng\249 S\184u) - D\222 Nh\169n<color>".."/pttk_confirm_3")
  tinsert(options,"<color=yellow>Tri\214u H\229i - D\222 Nh\169n<color>".."/pttk_confirm_4")
  tinsert(options,"<color=yellow>Kh\171 M\233c Ph\239ng Xu\169n - D\222 Nh\169n<color>".."/pttk_confirm_5")
  tinsert(options,"<color=yellow>\167o\182n Binh Giao Ti\213p - Gi\184p S\220<color>".."/pttk_confirm_6")
  tinsert(options,"<color=yellow>H\228a \167\222a Vi Lao - Gi\184p S\220<color>".."/pttk_confirm_7")
  tinsert(options,"<color=yellow>Xung Phong H\183m Tr\203n - Gi\184p S\220<color>".."/pttk_confirm_8")
 end
 if page==2 then
  tinsert(options,"<color=yellow>Nam Minh Ly H\225a - \167\185o S\220<color>".."/pttk_confirm_9")
  tinsert(options,"<color=yellow>Thi\170n To\181n \167\222a Chuy\211n - \167\185o S\220<color>".."/pttk_confirm_10")
  tinsert(options,"<color=yellow>B\168ng H\229n Tuy\213t Ph\184ch - \167\185o S\220<color>".."/pttk_confirm_11")
  tinsert(options,"<color=yellow>Huy\213t Nguy\214t Tr\182m - Gi\184p S\220<color>".."/pttk_confirm_12")
  tinsert(options,"<color=yellow>Huy\210n \182nh T\181n Hoa - D\222 Nh\169n<color>".."/pttk_confirm_13")
  tinsert(options,"<color=yellow>Ph\203t N\233 H\225a Li\170n - \167\185o S\220<color>".."/pttk_confirm_14")
  tinsert(options,"<color=yellow>L\171i H\225a To\181n Phong - \167\185o S\220<color>".."/pttk_confirm_15")
  tinsert(options,"<color=yellow>Truy H\229n T\184n Hoa - \167\185o S\220<color>".."/pttk_confirm_16")
  tinsert(options,"<color=yellow>V\185n Ki\213m Quy T\171ng - \167\185o S\220<color>".."/pttk_confirm_17")
 end
 if page==3 then
  tinsert(options,"<color=yellow>Huy\213t Nguy\214t Tr\182m C\202p 2 - Gi\184p S\220 (gh\208p m\182nh)<color>".."/pttk_confirm_18")
  tinsert(options,"<color=yellow>Huy\210n \182nh T\181n Hoa C\202p 2 - D\222 Nh\169n (gh\208p m\182nh)<color>".."/pttk_confirm_19")
  tinsert(options,"<color=yellow>Ph\203t N\233 H\225a Li\170n C\202p 2 - \167\185o S\220 (gh\208p m\182nh)<color>".."/pttk_confirm_20")
  tinsert(options,"<color=yellow>L\171i H\225a To\181n Phong C\202p 2 - \167\185o S\220 (gh\208p m\182nh)<color>".."/pttk_confirm_21")
  tinsert(options,"<color=yellow>Truy H\229n T\184n Hoa C\202p 2 - \167\185o S\220 (gh\208p m\182nh)<color>".."/pttk_confirm_22")
  tinsert(options,"<color=yellow>V\185n Ki\213m Quy T\171ng C\202p 2 - \167\185o S\220 (gh\208p m\182nh)<color>".."/pttk_confirm_23")
 end
 if page==4 then
  tinsert(options,"<color=yellow>Huy\213t Nguy\214t Tr\182m C\202p 2 - Gi\184p S\220 (kh\171ng m\182nh)<color>".."/pttk_confirm_24")
  tinsert(options,"<color=yellow>Huy\210n \182nh T\181n Hoa C\202p 2 - D\222 Nh\169n (kh\171ng m\182nh)<color>".."/pttk_confirm_25")
  tinsert(options,"<color=yellow>Ph\203t N\233 H\225a Li\170n C\202p 2 - \167\185o S\220 (kh\171ng m\182nh)<color>".."/pttk_confirm_26")
  tinsert(options,"<color=yellow>L\171i H\225a To\181n Phong C\202p 2 - \167\185o S\220 (kh\171ng m\182nh)<color>".."/pttk_confirm_27")
  tinsert(options,"<color=yellow>Truy H\229n T\184n Hoa C\202p 2 - \167\185o S\220 (kh\171ng m\182nh)<color>".."/pttk_confirm_28")
  tinsert(options,"<color=yellow>V\185n Ki\213m Quy T\171ng C\202p 2 - \167\185o S\220 (kh\171ng m\182nh)<color>".."/pttk_confirm_29")
 end
 if page>1 then tinsert(options,"Trang tr\173\237c".."/pttk_page("..(page-1)..")") end
 if page<4 then tinsert(options,"Trang sau".."/pttk_page("..(page+1)..")") end
 tinsert(options,"\167\227ng".."/pttk_close")
 Say("\167\230i s\184ch Th\199n K\252 - y\170u c\199u c\202p 140.\010<color=yellow>Nguy\170n li\214u: m\182nh Th\199n K\252, \167a B\182o L\214nh B\181i, Linh Th\185ch.<color>\010Ch\228n s\184ch \174\211 xem s\232 l\173\238ng v\181 \174\230i.",getn(options),options)
end
function pttk_confirm_1()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh H\215nh Thi\170n Chi\213n \253<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Phong Uy\211n)<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_1","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_1()
 local result=PtThanKyExchange(1)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_2()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>4 m\182nh H\215nh Thi\170n Chi\213n \253<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Cu\229ng \167ao)<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_2","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_2()
 local result=PtThanKyExchange(2)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_3()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>6 m\182nh H\215nh Thi\170n Chi\213n \253<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 H\215nh Thi\170n Chi\213n \253 (Ng\249 S\184u)<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_3","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_3()
 local result=PtThanKyExchange(3)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_4()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Tri\214u H\229i<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Tri\214u H\229i<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_4","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_4()
 local result=PtThanKyExchange(4)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_5()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Kh\171 M\233c Ph\239ng Xu\169n<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Kh\171 M\233c Ph\239ng Xu\169n<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_5","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_5()
 local result=PtThanKyExchange(5)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_6()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh \167o\182n Binh Giao Ti\213p<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 \167o\182n Binh Giao Ti\213p<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_6","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_6()
 local result=PtThanKyExchange(6)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_7()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh H\228a \167\222a Vi Lao<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 H\228a \167\222a Vi Lao<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_7","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_7()
 local result=PtThanKyExchange(7)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_8()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Xung Phong H\183m Tr\203n<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Xung Phong H\183m Tr\203n<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_8","Quay l\185i".."/pttk_page(1)")
end
function pttk_buy_8()
 local result=PtThanKyExchange(8)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(1)
end
function pttk_confirm_9()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Nam Minh Ly H\225a<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Nam Minh Ly H\225a<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_9","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_9()
 local result=PtThanKyExchange(9)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_10()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Thi\170n To\181n \167\222a Chuy\211n<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Thi\170n To\181n \167\222a Chuy\211n<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_10","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_10()
 local result=PtThanKyExchange(10)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_11()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh B\168ng H\229n Tuy\213t Ph\184ch<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 B\168ng H\229n Tuy\213t Ph\184ch<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_11","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_11()
 local result=PtThanKyExchange(11)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_12()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Huy\213t Nguy\214t Tr\182m<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\213t Nguy\214t Tr\182m<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_12","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_12()
 local result=PtThanKyExchange(12)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_13()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Huy\210n \182nh T\181n Hoa<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\210n \182nh T\181n Hoa<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_13","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_13()
 local result=PtThanKyExchange(13)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_14()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Ph\203t N\233 H\225a Li\170n<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Ph\203t N\233 H\225a Li\170n<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_14","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_14()
 local result=PtThanKyExchange(14)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_15()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh L\171i H\225a To\181n Phong<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 L\171i H\225a To\181n Phong<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_15","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_15()
 local result=PtThanKyExchange(15)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_16()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh Truy H\229n T\184n Hoa<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Truy H\229n T\184n Hoa<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_16","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_16()
 local result=PtThanKyExchange(16)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_17()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>2 m\182nh V\185n Ki\213m Quy T\171ng<color>\010<color=green>1 \167a B\182o L\214nh B\181i<color>\010<color=green>1000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 V\185n Ki\213m Quy T\171ng<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_17","Quay l\185i".."/pttk_page(2)")
end
function pttk_buy_17()
 local result=PtThanKyExchange(17)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(2)
end
function pttk_confirm_18()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh Huy\213t Nguy\214t Tr\182m<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\213t Nguy\214t Tr\182m C\202p 2<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_18","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_18()
 local result=PtThanKyExchange(18)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_19()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh Huy\210n \182nh T\181n Hoa<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_19","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_19()
 local result=PtThanKyExchange(19)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_20()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh Ph\203t N\233 H\225a Li\170n<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_20","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_20()
 local result=PtThanKyExchange(20)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_21()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh L\171i H\225a To\181n Phong<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_21","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_21()
 local result=PtThanKyExchange(21)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_22()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh Truy H\229n T\184n Hoa<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_22","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_22()
 local result=PtThanKyExchange(22)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_23()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 m\182nh V\185n Ki\213m Quy T\171ng<color>\010<color=green>2 \167a B\182o L\214nh B\181i<color>\010<color=green>2000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_23","Quay l\185i".."/pttk_page(3)")
end
function pttk_buy_23()
 local result=PtThanKyExchange(23)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(3)
end
function pttk_confirm_24()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\213t Nguy\214t Tr\182m C\202p 2<color> - Gi\184p S\220",2,"\167\230i s\184ch".."/pttk_buy_24","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_24()
 local result=PtThanKyExchange(24)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_confirm_25()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Huy\210n \182nh T\181n Hoa C\202p 2<color> - D\222 Nh\169n",2,"\167\230i s\184ch".."/pttk_buy_25","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_25()
 local result=PtThanKyExchange(25)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_confirm_26()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Ph\203t N\233 H\225a Li\170n C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_26","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_26()
 local result=PtThanKyExchange(26)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_confirm_27()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 L\171i H\225a To\181n Phong C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_27","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_27()
 local result=PtThanKyExchange(27)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_confirm_28()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 Truy H\229n T\184n Hoa C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_28","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_28()
 local result=PtThanKyExchange(28)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_confirm_29()
 Say("<color=yellow>Nguy\170n li\214u c\199n thi\213t:<color>\010<color=green>1 s\184ch c\239ng nh\227m c\202p 1<color>\010<color=green>4 \167a B\182o L\214nh B\181i<color>\010<color=green>4000 Linh Th\185ch<color>\010<color=red>Th\199n K\252 V\185n Ki\213m Quy T\171ng C\202p 2<color> - \167\185o S\220",2,"\167\230i s\184ch".."/pttk_buy_29","Quay l\185i".."/pttk_page(4)")
end
function pttk_buy_29()
 local result=PtThanKyExchange(29)
 if result==1 then Msg2Player("\167\230i s\184ch th\181nh c\171ng. K\221ch ho\185t s\184ch m\237i \174\211 nh\203n k\252 n\168ng.") else Msg2Player("Ch\173a \174\230i \174\173\238c. Ki\211m tra nguy\170n li\214u, c\202p, h\214 ph\184i, ch\231 tr\232ng v\181 kh\227a v\203t ph\200m.") end
 pttk_page(4)
end
function pttk_close() CloseDialog() end
