-- Original formula adapter or USER-APPROVED Loose trial; native identity/serial guards.
function main(item)
 local result=PtThanKyUseBook(item)
 if result==1 then Msg2Player("\167\183 k\221ch ho\185t Th\199n K\252. Gi\247 quy\211n s\184ch trong h\181nh trang \174\211 d\239ng; s\184ch n\169ng c\202p c\199n k\221ch ho\185t l\185i v\181 thay th\213 chi\170u c\242.")
 elseif result==2 then Msg2Player("Quy\211n s\184ch n\181y \174\183 k\221ch ho\185t, kh\171ng ti\170u hao th\170m Linh Th\185ch.")
 elseif result==-1 then Msg2Player("C\199n 1 Linh Th\185ch nguy\170n li\214u trong h\181nh trang.")
 elseif result==-3 then Msg2Player("Nh\227m Th\199n K\252 th\246 nghi\214m \174ang t\190t ho\198c c\202u h\215nh ch\173a h\238p l\214. S\184ch v\181 Linh Th\185ch \174\173\238c gi\247 nguy\170n.")
 else Msg2Player("Ch\173a k\221ch ho\185t: ki\211m tra c\202p 140, h\214 ph\184i, kh\227a/giao d\222ch v\181 b\182n \174\229 Nh\169n Gi\237i.")
 end
end
