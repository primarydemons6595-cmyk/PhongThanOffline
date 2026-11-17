TN_STATE={}
TN_SLOTS={"M·o","Yªu ®¸i","Giµy","Phi phong","Kh¶i gi¸p"}
function tn_state()
 local key=PlayerIndex or GetName()
 local s=TN_STATE[key]
 if not s or s.owner~=GetName() then s={owner=GetName()};TN_STATE[key]=s end
 return s
end
function tn_buttons(text,options)
 local args={text,getn(options)}
 for i=1,getn(options) do tinsert(args,options[i]) end
 return call(SayButtons,args)
end
function tn_pages(text,callback)
 local pages={};local page="";local at=1
 while at<=strlen(text) do
  local stop=strfind(text,"\n",at,1) or strlen(text)+1
  local line=strsub(text,at,stop-1)
  if strlen(page)+strlen(line)>160 and page~="" then tinsert(pages,page);page="" end
  page=page..line.."\n";at=stop+1
 end
 if page~="" then tinsert(pages,page) end
 local args={getn(pages),callback};for i=1,getn(pages) do tinsert(args,pages[i]) end
 call(Talk,args)
end
function main()
 local q=TranNguyenInfo()
 if not q.phase then return end
 tn_state().expires=SystemTime()+120
 if q.phase==0 then
  if GetLevel()<42 then Talk(1,"no",12827);return end
  tn_buttons(12823,{"ThÝ luyÖn/tn_trial","T×m hiÓu/tn_help"})
 elseif q.phase==1 then
  local options={"TiÕn ®é/tn_progress","Hñy bá/tn_cancel"}
  if q.mask==63 then tinsert(options,1,"Tr¶ thÝ luyÖn/tn_claim") end
  tn_buttons(12836,options)
 elseif q.slot==0 then
  tn_buttons(12828,{"Chän lo¹i chÕ t¹o/tn_choose_slot","T×m hiÓu/tn_help"})
 else
  tn_buttons(12823,{"ChÕ t¹o/tn_craft","Häc ®å phæ/tn_learn","Danh väng/tn_reputation","Linh Nguyªn/tn_ling","Cam cÊp 3/tn_admission","T×m hiÓu/tn_help"})
 end
end
function no() CloseDialog() end
function tn_result(ok)
 if ok==1 then main() else Talk(1,"no","Ch­a ®ñ ®iÒu kiÖn, nguyªn liÖu hoÆc chç trèng. H·y kiÓm tra hµnh trang vµ tr¹ng th¸i nhiÖm vô.") end
end
function tn_trial() tn_buttons(12836,{"X¸c ®Þnh/tn_take","Quay l¹i/main"}) end
function tn_take() if TranNguyenAction(1)==1 then tn_progress() else tn_result(0) end end
function tn_progress()
 local q=TranNguyenInfo();TranNguyenAction(11)
 local names={"Thùc KhÝ Quû - Hiªn Viªn ®éng tÇng 1 (204,178)","Thùc Thñy Quû - §«ng H¶i Thñy Vùc (241,199)","TËt Hµnh Quû - Sa M¹c Thæ Thµnh (208,186)","KhÝ Nguyªn: Th¶o Tiªn - Du Hån (219,194)","Thñy Nguyªn: YÓm Ho¶ - T©y C«n L«n (225,212)","Lùc Nguyªn: Cèt Tinh - Tam S¬n (193,197)"}
 local text="<color=yellow>ThÝ luyÖn TrÊn Nguyªn<color>\n"
 for i=1,6 do
  local done=mod(floor(q.mask/2^(i-1)),2)==1
  text=text..(done and "<color=green>" or "<color=white>")..names[i]..(done and " [§· ®ñ]" or " [Ch­a ®ñ]").."<color>\n"
 end
 tn_pages(text,"main")
end
function tn_cancel() tn_buttons("Hñy thÝ luyÖn vµ xãa tiÕn ®é hiÖn t¹i?",{"X¸c ®Þnh/tn_cancel_yes","Quay l¹i/main"}) end
function tn_cancel_yes() tn_result(TranNguyenAction(10)) end
function tn_claim() tn_result(TranNguyenAction(2)) end
function tn_choose_slot()
 tn_buttons("Chän mét lo¹i chÕ t¹o dïng chung cho ba hÖ ph¸i. §· x¸c ®Þnh sÏ kh«ng tù ®æi sang lo¹i kh¸c.",{"M·o/tn_slot1","Yªu ®¸i/tn_slot2","Giµy/tn_slot3","Phi phong/tn_slot4","Kh¶i gi¸p/tn_slot5"})
end
function tn_slot(n)
 tn_state().slot=n
 tn_buttons("Chän chÕ t¹o <color=orange>"..TN_SLOTS[n].."<color> vµ häc ba c«ng thøc c¬ b¶n t­¬ng øng ba hÖ ph¸i?",{"X¸c ®Þnh/tn_slot_yes","Quay l¹i/tn_choose_slot"})
end
function tn_slot_yes() tn_result(TranNguyenAction(3,tn_state().slot)) end
function tn_slot1() tn_slot(1) end
function tn_slot2() tn_slot(2) end
function tn_slot3() tn_slot(3) end
function tn_slot4() tn_slot(4) end
function tn_slot5() tn_slot(5) end
function tn_reputation()
 local q=TranNguyenInfo()
 tn_buttons("<color=yellow>"..TN_SLOTS[q.slot]..": "..q.credit.." danh väng<color>. Cßn "..q.remaining.." l­ît h«m nay. Th­êng +2, dïng KÝch phï hîp +10.",{"Cèng hiÕn/tn_donate","§æi Thä S¬n Th¹ch/tn_stone","Quay l¹i/main"})
end
function tn_donate() tn_list(1,0,1) end
function tn_craft() tn_list(2,0,1) end
function tn_learn() tn_list(4,0,1) end
function tn_admission()
 local q=TranNguyenInfo()
 if q.tier3==1 then tn_buttons("§· cã t­ c¸ch häc ®å phæ trang bÞ cam cÊp 3.",{"Häc ®å phæ/tn_learn","Quay l¹i/main"});return end
 tn_buttons("CÇn <color=yellow>500 danh väng<color> vµ ®ñ <color=orange>bé 5 mãn cam cÊp 2 cïng lo¹i, cïng hÖ ph¸i<color>. Chän bé ®Ó xem tr­íc khi giao nép.",{"Chän bé/tn_sets","Quay l¹i/main"})
end
function tn_sets() tn_list(5,0,1) end
function tn_stone() tn_buttons(12824,{"X¸c ®Þnh/tn_stone_yes","Quay l¹i/tn_reputation"}) end
function tn_stone_yes() tn_result(TranNguyenAction(8)) end
function tn_ling()
 local q=TranNguyenInfo()
 tn_buttons("HiÖn cã <color=yellow>"..q.ling.." Linh Nguyªn<color>. Giao mét tr¹ng th¸i ®­îc <color=green>"..q.lingvalue.." ®iÓm danh väng<color> cho lo¹i chÕ t¹o ®· chän.",{"Giao Linh Nguyªn/tn_ling_yes","Quay l¹i/main"})
end
function tn_ling_yes() tn_result(TranNguyenAction(5)) end
function tn_list(mode,id,page)
 local s=tn_state();s.mode=mode;s.id=id;s.page=page;s.rows=TranNguyenList(mode,id);s.expires=SystemTime()+120
 local n=getn(s.rows)
 if n==0 then Talk(1,"no",mode==5 and "Ch­a cã ®ñ mét bé 5 mãn cam cÊp 2 cïng lo¹i trong hµnh trang." or "Kh«ng cã lùa chän phï hîp. H·y kiÓm tra vËt phÈm, lo¹i chÕ t¹o vµ ®å phæ ®· häc.");return end
 local options={};local first=(page-1)*8+1
 for i=first,min(first+7,n) do local r=s.rows[i];local label=r.name
  if strsub(label,1,1)=="#" then label=strsub(label,2);r.name=label end
  if mode==4 then label=label.." ["..r.require.." danh väng]" end
  tinsert(options,label.."/tn_pick"..(i-first+1))
 end
 if page>1 then tinsert(options,"Trang tr­íc/tn_previous") end
 if first+7<n then tinsert(options,"Trang sau/tn_next") end
 tinsert(options,"Quay l¹i/main")
 local args={"H·y chän "..(mode==1 and "trang bÞ cèng hiÕn" or mode==3 and "trang bÞ tr¾ng ®Ó chÕ t¹o" or mode==5 and "bé trang bÞ giao nép" or "®å phæ")..". ["..page.."/"..ceil(n/8).."]",getn(options)}
 for i=1,getn(options) do tinsert(args,options[i]) end
 call(Say,args)
end
function tn_previous() local s=tn_state();tn_list(s.mode,s.id,s.page-1) end
function tn_next() local s=tn_state();tn_list(s.mode,s.id,s.page+1) end
function tn_pick(n)
 local s=tn_state()
 if not s.expires or s.expires<SystemTime() then main();return end
 local r=s.rows and s.rows[(s.page-1)*8+n]
 if not r then return end
 s.selected=r
 if s.mode==2 then s.recipe=r;s.recipeid=r.id;tn_list(3,r.id,1)
 elseif s.mode==1 then tn_buttons("Cèng hiÕn <color=100,149,255>"..r.name.."<color> (bËc "..r.level..")? Trang bÞ sÏ ®­îc thu håi sau khi x¸c ®Þnh.",{"Th­êng +2/tn_donate_normal","Dïng KÝch +10/tn_donate_boost","Quay l¹i/tn_donate"})
 elseif s.mode==3 then tn_pages("<color=orange>"..s.recipe.name.."<color>\nNguyªn liÖu:\n"..s.recipe.detail.."\nPhÝ: <color=yellow>"..s.recipe.fee.." l­îng<color>.","tn_craft_confirm")
 elseif s.mode==4 then tn_buttons("Häc <color=orange>"..r.name.."<color> cÇn "..r.require.." ®iÓm danh väng vµ ®å phæ t­¬ng øng nÕu lµ c«ng thøc cao cÊp.",{"X¸c ®Þnh/tn_learn_yes","Quay l¹i/tn_learn"})
 elseif s.mode==5 then tn_pages("Bé trang bÞ sÏ giao nép:\n<color=orange>"..r.detail.."<color>","tn_admission_confirm") end
end
function tn_craft_confirm()
 local s=tn_state();if not tn_selected() then return end
 tn_buttons("ChÕ t¹o <color=orange>"..s.recipe.name.."<color>? PhÝ <color=yellow>"..s.recipe.fee.." l­îng<color>.",{"X¸c ®Þnh/tn_craft_yes","Quay l¹i/tn_craft"})
end
function tn_admission_confirm()
 if not tn_selected() then return end
 tn_buttons("X¸c nhËn giao nép ®óng bé trang bÞ võa xem ®Ó më t­ c¸ch cam cÊp 3?",{"X¸c ®Þnh/tn_admission_yes","Quay l¹i/tn_sets"})
end
function tn_pick1() tn_pick(1) end
function tn_pick2() tn_pick(2) end
function tn_pick3() tn_pick(3) end
function tn_pick4() tn_pick(4) end
function tn_pick5() tn_pick(5) end
function tn_pick6() tn_pick(6) end
function tn_pick7() tn_pick(7) end
function tn_pick8() tn_pick(8) end
function tn_selected()
 local s=tn_state();if not s.expires or s.expires<SystemTime() then return nil end
 return s.selected
end
function tn_donate_normal() local r=tn_selected();if r then tn_result(TranNguyenAction(4,r.index,0,r.serial)) end end
function tn_donate_boost() local r=tn_selected();if r then tn_result(TranNguyenAction(4,r.index,1,r.serial)) end end
function tn_craft_yes() local r=tn_selected();if r then tn_result(TranNguyenAction(7,tn_state().recipeid,r.index,r.serial)) end end
function tn_learn_yes() local r=tn_selected();if r then tn_result(TranNguyenAction(6,r.id)) end end
function tn_admission_yes() local r=tn_selected();if r then tn_result(TranNguyenAction(9,r.id,0,r.stamp)) end end
function tn_help()
 tn_buttons(12823,{"Tam Quû/tn_help_ghost","Tam Nguyªn/tn_help_elements","ChÕ t¹o vµ ®å phæ/tn_help_craft","Quay l¹i/main"})
end
function tn_help_ghost() Talk(3,"main",12840,12841,12842) end
function tn_help_elements() tn_pages("<color=yellow>KhÝ Nguyªn<color>: ®¸nh <color=red>Th¶o Tiªn<color> ë Du Hån (219,194).\n<color=yellow>Thñy Nguyªn<color>: ®¸nh <color=red>YÓm Ho¶<color> ë T©y C«n L«n (225,212).\n<color=yellow>Lùc Nguyªn<color>: ®¸nh <color=red>Cèt Tinh<color> ë Tam S¬n (193,197).", "main") end
function tn_help_craft() Talk(2,"main",12837,12838) end

function GetPlayerTaskState() if GetLevel()<42 then return 0,42 end;local phase=GetTask(4746);return phase==0 and 1 or (phase==1 and (GetTask(4747)==63 and 3 or 2) or 0),42 end
