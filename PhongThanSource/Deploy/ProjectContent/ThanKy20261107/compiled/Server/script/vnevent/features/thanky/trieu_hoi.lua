-- Original skill formulas; user-approved Loose carried-book lease.
function main(item)
 local result=PtThanKyUseBook(item)
 if result==1 then Msg2Player("\167\183 k\221ch ho\185t Th\199n K\252. Gi\247 \174\243ng s\184ch trong h\181nh trang \174\211 d\239ng chi\170u. S\184ch m\237i c\199n k\221ch ho\185t l\185i.")
 elseif result==2 then Msg2Player("Quy\211n s\184ch n\181y \174\183 \174\173\238c k\221ch ho\185t; kh\171ng ti\170u hao th\170m Linh Th\185ch.")
 elseif result==-1 then Msg2Player("C\199n 1 Linh Th\185ch trong h\181nh trang \174\211 k\221ch ho\185t.")
 else Msg2Player("Ch\173a th\211 k\221ch ho\185t: ki\211m tra c\202p 140, h\214 ph\184i, kh\227a v\203t ph\200m v\181 b\182n \174\229 Nh\169n Gi\237i.")
 end
end
