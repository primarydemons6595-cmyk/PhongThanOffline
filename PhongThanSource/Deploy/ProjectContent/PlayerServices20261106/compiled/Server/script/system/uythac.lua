function pt_off_close() CloseDialog() end
function main()
 PT_OFF_MODE=PtOfflineState(3)
 local text="<color=yellow>ñy th¸c rêi m¹ng<color><enter>EXP: "..PtOfflineState(0).." giê chê; Lam D­¬ng: "..PtOfflineState(1).." giê chê; Th¸i Cùc: "..PtOfflineState(2).." giê chê.<enter>NhËt NguyÖt §¬n ph¶i ®­îc kÝch ho¹t b»ng chuét ph¶i vµ cßn h¹n khi ®¨ng nhËp. Th¸i Cùc/Lam D­¬ng chØ trõ khi nhËn th­ëng thµnh c«ng. Mçi phiªn tÝnh tèi ®a 4 giê trän vÑn."
 SayButtons(text,7,"Chän nhËn EXP/pt_off_exp","Chän luyÖn kü n¨ng/pt_off_skill","Chän luyÖn b¶o th¹ch/pt_off_gem","¸p dông lùa chän/pt_off_apply","NhËn th­ëng ®ang chê/pt_off_rewards","T¾t ñy th¸c/pt_off_disable","KÕt thóc/pt_off_close")
end
function pt_off_has(bit) return mod(floor(PT_OFF_MODE/bit),2)==1 end
function pt_off_toggle(bit) if pt_off_has(bit) then PT_OFF_MODE=PT_OFF_MODE-bit else PT_OFF_MODE=PT_OFF_MODE+bit end;pt_off_choice() end
function pt_off_exp() pt_off_toggle(1) end
function pt_off_skill() pt_off_toggle(4) end
function pt_off_gem() pt_off_toggle(2) end
function pt_off_choice()
 local text="<color=yellow>Lùa chän ñy th¸c<color><enter>EXP: "..(pt_off_has(1) and "BËt" or "T¾t").."<enter>Kü n¨ng chuét ph¶i: "..(pt_off_has(4) and "BËt" or "T¾t").."<enter>B¶o th¹ch: "..(pt_off_has(2) and "BËt" or "T¾t").."<enter>H·y dïng lùa chän ¸p dông tr­íc khi tho¸t b×nh th­êng. Kü n¨ng ®­îc ghi nhËn lóc më ñy th¸c tõ ESC."
 SayButtons(text,5,"§æi EXP/pt_off_exp","§æi kü n¨ng/pt_off_skill","§æi b¶o th¹ch/pt_off_gem","¸p dông/pt_off_apply","KÕt thóc/pt_off_close")
end
function pt_off_apply()
 if PtOfflineConfigure(PT_OFF_MODE)==1 then Talk(1,"pt_off_close","§· l­u lùa chän. H·y tho¸t game b×nh th­êng ®Ó b¾t ®Çu tÝnh giê. §¨ng nhËp l¹i råi më ESC - ñy th¸c ®Ó nhËn th­ëng.")
 else Talk(1,"pt_off_close","Ch­a ®Æt ®­îc ñy th¸c. NhËn hÕt giê ®ang chê tr­íc; EXP cÇn NhËt NguyÖt §¬n cßn h¹n ®· kÝch ho¹t, kü n¨ng cÇn chän bªn chuét ph¶i vµ d­íi cÊp 10. KÕt thóc khãa/giao dÞch råi thö l¹i.") end
end
function pt_off_disable() PT_OFF_MODE=0;pt_off_apply() end
function pt_off_rewards() SayButtons("Chän tõng giê cÇn nhËn. PhÇn kh«ng nhËn ®­îc do thiÕu ®¬n, hÕt h¹n hoÆc ®Çy tói vÉn ®­îc gi÷, kh«ng tiªu hao vËt phÈm. EXP tèi ®a 4 giê/ngµy vµ kh«ng dïng ®Ó v­ît cÊp 200. Kü n¨ng tèi ®a cÊp 10.",5,"NhËn 1 giê EXP/pt_off_claim_exp","NhËn 1 giê b¶o th¹ch/pt_off_claim_gem","NhËn 1 giê kü n¨ng/pt_off_claim_skill","Bá giê chê kh«ng dïng/pt_off_discard_confirm","KÕt thóc/pt_off_close") end
function pt_off_discard_confirm() SayButtons("Bá toµn bé giê th­ëng ®ang chê? Kh«ng trõ vËt phÈm, nh­ng sè giê nµy kh«ng thÓ lÊy l¹i.",2,"§ång ý bá/pt_off_discard","Gi÷ l¹i/pt_off_rewards") end
function pt_off_discard() if PtOfflineDiscard()==1 then Talk(1,"pt_off_close","§· bá giê th­ëng ®ang chê.") else Talk(1,"pt_off_close","H·y kÕt thóc khãa/giao dÞch råi thö l¹i.") end end
function pt_off_result(result) if result==1 then Talk(1,"pt_off_rewards","§· nhËn 1 giê ñy th¸c.") else Talk(1,"pt_off_rewards","Kh«ng nhËn ®­îc giê nµy. KiÓm tra giê ®ang chê, h¹n NhËt NguyÖt §¬n, sè ®¬n trong bèn tói, kü n¨ng vµ « trèng.") end end
function pt_off_claim_exp() pt_off_result(PtOfflineClaim(0)) end
function pt_off_claim_gem() pt_off_result(PtOfflineClaim(1)) end
function pt_off_claim_skill() pt_off_result(PtOfflineClaim(2)) end
