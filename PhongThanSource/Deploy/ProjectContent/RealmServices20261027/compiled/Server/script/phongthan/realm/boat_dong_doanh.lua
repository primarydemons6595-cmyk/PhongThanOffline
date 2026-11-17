PT_REALM_HAS_OLD=0
function main() CloseDialog() end
PT_REALM_PROMPT=11125

-- PT_REALM_PERSONAL_BEGIN: reconstructed personal VNG progression only.
PT_REALM_OLD_MAIN=main
PT_REALM_OLD_MARK=GetPlayerTaskState
function ptrq_op(s,r,c)
 if r==1 then
  local mainTask=3
  if GetPlayerType()==1 then mainTask=1 elseif GetPlayerType()==2 then mainTask=2 end
  if s==0 and GetLevel()>=100 and GetTask(mainTask)>=90 then return 1,"Phong Håi Lé ChuyÓn" end
  if s==2 then return 3,"Thİ luyÖn D­¬ng TiÔn" end
  if s==4 then return 4,"Phôc mÖnh" end
  if s==5 and GetLevel()>=110 then return 5,"Cöu Lai" end
 elseif r==2 and s==1 then return 2,"Phong Håi Lé ChuyÓn"
 elseif r==3 or r==4 then
  if s==6 then return 6,"T×m hiÓu Tiªn Ma Giíi" end
  if s==7 then return 7,"Thö th¸ch Cöu Lai" end
  if r==c+2 then
   if s==9 then return 8,"Phôc mÖnh Cöu Lai" end
   if s==10 and GetLevel()>=121 then return 9,"Th¨ng Tiªn NhËp Ma" end
   if s==12 then return 11,"Phôc mÖnh" end
  end
 elseif r==5 and s==11 then return 10,"T×m hiÓu Tiªn Ma Giíi"
 elseif r==6 and s>=11 then return 12,"§Õn BÊt Chu Thiªn Quan" end
 return 0,""
end
function ptrq_show(text,t)
 local n=getn(t)
 if n==1 then SayButtons(text,1,t[1])
 elseif n==2 then SayButtons(text,2,t[1],t[2])
 elseif n==3 then SayButtons(text,3,t[1],t[2],t[3])
 elseif n==4 then SayButtons(text,4,t[1],t[2],t[3],t[4])
 elseif n==5 then SayButtons(text,5,t[1],t[2],t[3],t[4],t[5])
 elseif n==6 then SayButtons(text,6,t[1],t[2],t[3],t[4],t[5],t[6])
 else Talk(1,"ptrq_close",text) end
end
function main()
 local ok,s,r,c=PtRealmQuest(0)
 if ok~=1 then PT_REALM_OLD_MAIN();return end
 local op,label=ptrq_op(s,r,c)
 local valid,world,days,reputation,mask,registered=PtRealmWorld(0)
 if op==12 and world~=3 then op=0 end
 local t={}
 if op>0 then tinsert(t,label.."/ptrq_preview") end
 if r==3 or r==4 then
  local ready,state,runs,stage,missing=PtRealmReputation(0)
  if ready==1 and stage>=10 then
   if state==0 and runs<4 then tinsert(t,"Danh väng Nh©n Giíi/ptrq_rep")
   elseif state==(r-2)*10+5 then tinsert(t,"Phôc mÖnh danh väng/ptrq_rep") end
   if state>0 and missing==1 then tinsert(t,"NhËn l¹i ph¸p khİ/ptrq_reissue") end
  end
 elseif r==7 and (world==1 or world==2) and registered==0 and GetLevel()>=121 then tinsert(t,"Ghi danh më cæng/ptrq_register")
 elseif r>=8 then
  local ready,lure,whisker=PtRealmIsland(0)
  if r==8 and ready==1 and (lure==0 or lure==4) and whisker==0 and GetLevel()>=120 then tinsert(t,"Thu phôc Giao Long/ptrq_lure_start") end
  if (r==8 or r==10) and lure==3 then tinsert(t,"NhËn R©u ThÇn Long/ptrq_whisker") end
  if r==9 then tinsert(t,"§«ng Doanh ®¶o/ptrq_boat_east") end
  if r==10 then
   tinsert(t,"Ph­¬ng Tr­îng/ptrq_boat_phuong")
   tinsert(t,"VÒ TrÇn §­êng/ptrq_boat_back")
   if lure==1 then tinsert(t,"Dô Giao Long/ptrq_lure") end
  end
 end
 if r>=3 then tinsert(t,"TiÕn ®é më cæng/ptrq_world") end
 if getn(t)==0 then PT_REALM_OLD_MAIN();return end
 if PT_REALM_HAS_OLD~=0 then tinsert(t,"Trß chuyÖn/ptrq_old") end
 ptrq_show(PT_REALM_PROMPT or "Tiªn Nh©n Ma tam giíi. Ng­¬i cã ®iÒu g× muèn hái?",t)
end
function ptrq_old() PT_REALM_OLD_MAIN() end
function ptrq_preview()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok~=1 or op==0 then CloseDialog();return end
 local text=""
 if op==1 then text="Ta nhê ng­¬i ®Õn gÆp §¾c Kû lóc nhá t¹i Ngäc H­ Cung 10 n¨m tr­íc (208,203)."
 elseif op==2 then text="H·y quay l¹i Kh­¬ng Tö Nha lóc trÎ ®Ó nhËn nhiÖm vô thİ luyÖn D­¬ng TiÔn."
 elseif op==3 then text="§Õn ¶i TrÇn §­êng thö søc víi <color=red>D­¬ng TiÔn<color>, chiÕn th¾ng råi trë vÒ phôc mÖnh."
 elseif op==4 then text="NhËn <color=yellow>§å phæ B¹ch Liªn ph¸p khİ<color> vµ <color=yellow>2.000 v¹n kinh nghiÖm<color>."
 elseif op==5 then text="LÇn l­ît ®Õn Ngäc H­ Cung (213,197) gÆp Xİch Tinh Tö vµ Xi V­u Mé (206,198) gÆp Cao Minh ®Ó t×m hiÓu Tiªn Ma Giíi."
 elseif op==6 then text="Tiªn Ma kh¸c lèi. Sau khi gÆp c¶ hai vŞ tiÕp dÉn s­, ng­¬i h·y chän mét vŞ ®Ó tiÕp nhËn thö th¸ch Cöu Lai."
 elseif op==7 then text="§Õn §«ng Doanh tiªu diÖt <color=red>Giao Long<color>, sau ®ã trë vÒ gÆp ta phôc mÖnh."
 elseif op==8 then text="Ng­¬i ®· v­ît qua thö th¸ch Cöu Lai. Khi ®¹t cÊp 121 cã thÓ tiÕp tôc Th¨ng Tiªn NhËp Ma."
 elseif op==9 then text="Qua cæng t¹i Diªu Tr× (200,196), ®Õn BÊt Chu Thiªn Quan gÆp HuyÒn §« §¹i Ph¸p S­ (205,242), råi quay vÒ phôc mÖnh."
 elseif op==10 then text="Sau khi t×m hiÓu Tiªn Ma Giíi, h·y trë l¹i vŞ tiÕp dÉn s­ cña m×nh ®Ó phôc mÖnh."
 elseif op==11 then text="Hoµn thµnh Th¨ng Tiªn NhËp Ma, nhËn cÊp Tiªn Ma 1 vµ mét <color=yellow>vò khİ Tiªn Ma<color> theo hÖ ph¸i."
 elseif op==12 then text="§Õn BÊt Chu Thiªn Quan?" end
 MsgBox(text,"ptrq_confirm","ptrq_close")
end
function ptrq_confirm()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok~=1 or op==0 then CloseDialog();return end
 local result=PtRealmQuest(op)
 if result~=1 then Talk(1,"main","Ch­a thÓ thùc hiÖn. KiÓm tra ®iÒu kiÖn nhiÖm vô vµ chç trèng trong F4.");return end
 if op==12 then CloseDialog();return end
 main()
end
function ptrq_close() CloseDialog() end
function ptrq_rep()
 local ok,state,runs,stage=PtRealmReputation(0)
 if ok~=1 or stage<10 then CloseDialog();return end
 if state==0 and runs<4 then
  local text="NhËn nhiÖm vô Danh väng Nh©n Giíi lÇn "..(runs+1).." h«m nay. "
  if runs==0 then text=text.."LÇn nµy miÔn phİ." else text=text.."Phİ: <color=yellow>"..runs.." Xu<color>." end
  MsgBox(text,"ptrq_rep_start","ptrq_close")
 elseif state==15 or state==25 then MsgBox("§· dÉn hån vÒ §¹i Phu. Phôc mÖnh nhiÖm vô?","ptrq_rep_finish","ptrq_close")
 else Talk(1,"main","§Õn Khæn Tiªn cung tÇng 1, ngoµi khu an toµn gÇn §¹i Phu phİa ®«ng b¾c. Dïng ph¸p khİ gäi hån ®Ó hiÖn trËn; dô qu¸i ®Õn gÇn trËn vµ tiªu diÖt ®ñ 5 hån. Dïng vËt phÈm dÉn hån ®­a hån vÒ §¹i Phu, råi trë vÒ phôc mÖnh. Mçi giai ®o¹n cã thêi h¹n 5 phót.") end
end
function ptrq_rep_start() if PtRealmReputation(1)~=1 then Talk(1,"main","Ch­a thÓ nhËn. KiÓm tra ®iÒu kiÖn, sè Xu vµ chç trèng trong F4.");return end main() end
function ptrq_rep_finish() if PtRealmReputation(2)~=1 then Talk(1,"main","Ch­a thÓ phôc mÖnh. H·y kiÓm tra tiÕn ®é nhiÖm vô.");return end main() end
function ptrq_reissue() if PtRealmReputation(3)~=1 then Talk(1,"main","Ch­a thÓ bæ sung ph¸p khİ. KiÓm tra chç trèng trong F4.");return end main() end
function ptrq_world()
 local ok,phase,days,reputation,final,registered,mask=PtRealmWorld(0)
 if ok~=1 then Talk(1,"main","Ch­a ®äc ®­îc tiÕn ®é më cæng. H·y thö l¹i sau.");return end
 local text=""
 if phase==0 then text="Gi¶i cøu Linh Quang ®¹o nh©n: mçi ngµy tiªu diÖt Giao Long ë §«ng Doanh, Di Long ë Ph­¬ng Tr­îng vµ Ly Long ë Bång Lai, liªn tiÕp trong 3 ngµy. §· hoµn thµnh "..days.."/3 ngµy liªn tiÕp."
 elseif phase==1 then text="Linh Quang ®¹o nh©n ®· ®­îc gi¶i tho¸t, t¹i Diªu Tr× (196,203). Cïng hoµn thµnh nhiÖm vô Danh väng Nh©n Giíi: "..reputation.."/1500 l­ît. Nh©n vËt cÊp 121 trë lªn cã thÓ ghi danh më cæng."
 elseif phase==2 then text="§· ®ñ 1500 nhiÖm vô Danh väng Nh©n Giíi. C¸c kú sÜ ®· ghi danh cÇn tiªu diÖt Giao Long, Di Long vµ Ly Long trong cïng mét ngµy ®Ó më cæng."
 else text="Cæng BÊt Chu Thiªn Quan ®· më t¹i Diªu Tr× (200,196). H·y hoµn thµnh chuçi nhiÖm vô c¸ nh©n Th¨ng Tiªn NhËp Ma ®Ó b­íc vµo Tiªn Ma Giíi." end
 if phase==0 or phase==2 then
  local m=mask;if phase==2 then m=final end
  local ly,di,giao="ch­a","ch­a","ch­a"
  if m>=4 then ly="®· h¹";m=m-4 end
  if m>=2 then di="®· h¹";m=m-2 end
  if m>=1 then giao="®· h¹" end
  text=text.."<enter>H«m nay: Giao Long "..giao..", Di Long "..di..", Ly Long "..ly.."."
 end
 Talk(1,"main",text)
end
function ptrq_register() MsgBox("Ghi danh tham gia më cæng BÊt Chu Thiªn Quan?","ptrq_register_yes","ptrq_close") end
function ptrq_register_yes() if PtRealmWorld(1)~=1 then Talk(1,"main","Ch­a thÓ ghi danh. KiÓm tra cÊp ®é vµ tiÕn ®é më cæng.");return end main() end
function ptrq_lure_start() MsgBox("Mang 10 <color=yellow>ChÊn Thiªn TiÔn<color> ®Õn ThuyÒn Phu t¹i §«ng Doanh (223,197) ®Ó dô Giao Long. Tiªu diÖt nã råi nhËn <color=yellow>R©u ThÇn Long<color>.","ptrq_lure_start_yes","ptrq_close") end
function ptrq_lure_start_yes() if PtRealmIsland(1)~=1 then Talk(1,"main","Ch­a thÓ nhËn nhiÖm vô. KiÓm tra ®iÒu kiÖn.");return end main() end
function ptrq_lure() MsgBox("Dïng 10 <color=yellow>ChÊn Thiªn TiÔn<color> ®Ó dô Giao Long?","ptrq_lure_yes","ptrq_close") end
function ptrq_lure_yes() if PtRealmIsland(2)~=1 then Talk(1,"main","CÇn ®ñ 10 ChÊn Thiªn TiÔn vµ ®· nhËn lêi V©n Tiªu.");return end main() end
function ptrq_whisker() if PtRealmIsland(3)~=1 then Talk(1,"main","Ch­a thÓ nhËn R©u ThÇn Long. KiÓm tra chç trèng trong F4.");return end main() end
function ptrq_boat_east() MsgBox("§Õn §«ng Doanh ®¶o, phİ <color=yellow>200 v¹n l­îng<color>. §ång ı lªn thuyÒn?","ptrq_boat_east_yes","ptrq_close") end
function ptrq_boat_east_yes() if PtRealmIsland(4)~=1 then Talk(1,"main","Ch­a thÓ lªn thuyÒn. CÇn ®ñ 200 v¹n l­îng.");return end CloseDialog() end
function ptrq_boat_phuong() MsgBox("§Õn Ph­¬ng Tr­îng (V©n Long ®¶o) cÇn cÊp <color=yellow>120<color> vµ cã <color=yellow>R©u ThÇn Long<color>. §ång ı lªn thuyÒn?","ptrq_boat_phuong_yes","ptrq_close") end
function ptrq_boat_phuong_yes() if PtRealmIsland(5)~=1 then Talk(1,"main","CÇn cÊp 120 trë lªn vµ cã R©u ThÇn Long trong hµnh trang.");return end CloseDialog() end
function ptrq_boat_back() if PtRealmIsland(6)~=1 then Talk(1,"main","Ch­a thÓ rêi ®¶o, h·y thö l¹i sau.");return end CloseDialog() end
function GetPlayerTaskState()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok==1 and op>0 then
  if op==4 or op==8 or op==11 then return 3,0 end
  return 1,0
 end
 if r==3 or r==4 then
  local ready,state,runs,stage=PtRealmReputation(0)
  if ready==1 and stage>=10 then
   if state==(r-2)*10+5 then return 3,110 end
   if state==0 and runs<4 then return 1,110 end
  end
 elseif r==7 then
  local ready,phase,days,reputation,mask,registered=PtRealmWorld(0)
  if ready==1 and (phase==1 or phase==2) and registered==0 and GetLevel()>=121 then return 1,121 end
 elseif r==8 or r==10 then
  local ready,lure,whisker=PtRealmIsland(0)
  if ready==1 and lure==3 then return 3,120 end
  if r==8 and ready==1 and (lure==0 or lure==4) and whisker==0 then return 1,120 end
 end
 if PT_REALM_OLD_MARK then return PT_REALM_OLD_MARK() end
 return 0,0
end
