-- This file is appended only to explicitly selected quest NPC/item scripts.
-- Old menus remain available; no client-wide dialogue rendering changes.
function PT2Class() local f=GetPlayerType();return f==1 or f==2 end
function PT2MainTask() if GetPlayerType()==1 then return 1 end return 2 end
function PT2GiaoRole() return (GetPlayerType()==1 and PT2_ROLE=="giao_dao") or (GetPlayerType()==2 and PT2_ROLE=="giao_di") end
function PT2Have(rows)
 for i=1,getn(rows) do local r=rows[i];if HaveNormalItem(r[1],r[2],r[3],r[4])<r[7] then return 0 end end
 return 1
end
function PT2Ready(a)
 return PT2Class() and a.f==GetPlayerType() and a.role==PT2_ROLE and GetContentWorldPos()+1000==PT2_MAP and
 GetLevel()>=a.level and GetTask(a.task)==a.from and (not a.mainPhase or GetTask(PT2MainTask())==a.mainPhase)
end
function PT2Journal()
 if not PT2Class() then return end
 local phase=GetTask(PT2MainTask())
 local title=GetPlayerType()==1 and "Chủ tuyến Đạo Sĩ" or "Chủ tuyến Dị Nhân"
 local note=PT2_NOTES[GetPlayerType()][phase]
 local id=GetPlayerType()==1 and "3040" or "3041"
 if note then PtTwoFactionNote(id,1,"<color=yellow>"..title.."<color><enter>"..note,phase)
 else PtTwoFactionNote(id,2," ",phase) end
 PtTwoFactionNote(GetPlayerType()==1 and "3041" or "3040",2," ",0)
 for i=1,getn(PT2_UNITS) do local q=PT2_UNITS[i]
  if q.f==GetPlayerType() then
   local state=GetTask(q.task);local row=q.rows[state]
   if row then
    local text="<color=yellow>"..q.title.."<color><enter>"..row
    local needs=q.needs[state]
    if needs then for j=1,getn(needs) do local need=needs[j];local count=HaveNormalItem(need[1],need[2],need[3],need[4]);if count>need[7] then count=need[7] end;text=text.."<enter>Tiến độ: "..count.."/"..need[7] end end
    if q.task==35 and state>=3 and state<=6 then text=text.."<enter>Mảnh thần khí: "..HaveEventItem(28).."/3" end
    PtTwoFactionNote(q.id,1,text,state)
   else PtTwoFactionNote(q.id,2," ",state) end
  else PtTwoFactionNote(q.id,2," ",0) end
 end
 local state=GetTask(4550)
 if state>0 and state<6 then
 local progress=""
 if state==1 then progress="<enter>Hạp Đằng: "..HaveEventItem(191).."/5"
 elseif state==3 then progress="<enter>Nguyên Sâm: "..HaveEventItem(192).."/10"
 elseif state==5 then progress="<enter>Liễu Mộc: "..HaveEventItem(201).."/5" end
 PtTwoFactionNote("3042",1,"<color=yellow>Trừ Họa<color><enter>"..PT2_HARM_NOTES[state]..progress,state)
 else PtTwoFactionNote("3042",2," ",state) end
 if GetPlayerType()==1 and GetTask(4770)==1 then PtTwoFactionNote("3052",1,"<color=yellow>Băng Lang<color><enter>Tây Côn Lôn: diệt <color=red>Băng Lang<color> "..GetTask(4771).."/20; <color=red>Băng Lang Vương<color> "..GetTask(4772).."/3.",GetTask(4771)+GetTask(4772)*21)
 elseif GetPlayerType()==1 and GetTask(4770)==2 then PtTwoFactionNote("3052",1,"Về <color=green>Nam Cực Tiên Ông<color>, Ngọc Hư Cung (209,191) phục mệnh.",2)
 else PtTwoFactionNote("3052",2," ",0) end
 if GetTask(4554)>0 then
  local home=GetPlayerType()==1 and "Ngọc Hư Cung (217,202)" or "Xi Vưu Mộ (200,200)"
  PtTwoFactionNote("3053",1,"<color=yellow>Giáo Huấn<color><enter>Diệt <color=red>"..PT2_LOCAL_NAMES[GetTask(4554)].."<color> quanh phái: "..GetTask(4555).."/40.<enter>Về Tạp Hóa Thương Nhân, "..home..".",GetTask(4555))
 else PtTwoFactionNote("3053",2," ",0) end
end
function pt2_close() CloseDialog() end
function PT2Notice(text) SayButtons(text,1,"Quay lại/main") end
function PT2Preview(i)
 local a=PT2_STEPS[i];if not PT2Ready(a) then return end
 local text="<color=yellow>"..a.title.."<color><enter>"..a.note
 if a.xp>0 then text=text.."<enter><color=green>Kinh nghiệm: "..a.xp.."<color>" end
 if a.money>0 then text=text.."<enter><color=green>Lượng: "..a.money.."<color>" end
 SayButtons(text,2,"Tiếp tục/pt2_yes"..i,"Quay lại/main")
end
function PT2Apply(i)
 local a=PT2_STEPS[i]
 if not PT2Ready(a) or GetTrade()~=0 or GetLockState()~=0 then return end
 if PT2Have(a.needs)~=1 then PT2Notice("Chưa đủ vật phẩm.<enter>"..a.note);return end
 local needs=a.needs;local gifts=a.gifts
 if a.keepNeeds then needs={} end
 if a.ensureBoard then gifts={};local count=HaveEventItem(9);if count<3 then gifts={{4,9,0,0,0,0,3-count}} end end
 if a.randomBook then gifts={PTG_BOOKS[RANDOM(1,getn(PTG_BOOKS))]} end
 if a.randomTransform then gifts={PT2_TRANSFORMS[RANDOM(1,getn(PT2_TRANSFORMS))]} end
 if a.randomFabao10 then gifts={{0,4,RANDOM(0,3),1,0,0,1}} end
 if a.randomFabao70 then gifts={{0,4,RANDOM(14,17),1,0,0,1}} end
 local old=GetCash()
 if a.money>0 then if old>2000000000-a.money then return end;Earn(a.money);if GetCash()~=old+a.money then return end end
 if QuestExchange(a.task,a.from,a.to,needs,gifts,1)~=1 then
  if a.money>0 then Pay(a.money) end
  PT2Notice("Chưa thể giao nhiệm vụ. Hãy mở khóa vật phẩm và dành đủ ô trống trong hành trang.");return
 end
 if a.xp>0 then AddOwnExp(a.xp) end
 if a.credit>0 then AddCredit(a.credit) end
 if a.camp then SetCamp(a.camp) end
 if a.task==PT2MainTask() and a.to==84 then SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0) end
 if a.task==4770 and a.to==1 then SetTask(4771,0);SetTask(4772,0) end
 PT2Journal()
 if a.travel then NewWorld(a.travel[1],a.travel[2]*8+4,a.travel[3]*16+8) end
 PT2Notice("<color=yellow>"..a.title.."<color><enter>"..a.note)
end
function pt2_services() if PT2_OLD_MAIN then return PT2_OLD_MAIN() end CloseDialog() end
function PT2Menu()
 if not PT2Class() then return PT2_OLD_MAIN and PT2_OLD_MAIN() end
 PT2Journal();local options={}
 for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i]
  if PT2Ready(a) then tinsert(options,a.label.."/pt2_view"..i) end
 end
 if PT2_ROLE=="thong" and GetTask(PT2MainTask())==88 then tinsert(options,"Khiêu chiến/pt2_challenge") end
 if PT2GiaoRole() and GetLevel()>=20 then
  if PT2GiaoReady()==1 then tinsert(options,"Giáo Huấn/pt2_giao_preview") end
  if GetTask(4554)>0 and GetTask(4555)>=40 then tinsert(options,"Nhận thưởng/pt2_giao_claim") end
 end
 if getn(options)==0 then return pt2_services() end
 if PT2_OLD_MAIN then tinsert(options,"Dịch vụ/pt2_services") end
 if getn(options)==0 then CloseDialog();return end
 SayButtons("<color=yellow>Nhiệm vụ<color><enter>Chọn nhiệm vụ để xem mục tiêu và phần thưởng.",getn(options),options)
end
function PT2Marker()
 if not PT2Class() then if PT2_OLD_MARK then return PT2_OLD_MARK() end return 0,0 end
 local best,level=0,0
 for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i];if PT2Ready(a) then
  local state=PT2Have(a.needs)==1 and (a.from==0 and 1 or 3) or 2
  if state>best then best=state;level=a.level end
 end end
 if PT2GiaoRole() and GetLevel()>=20 then
  if GetTask(4554)>0 then return GetTask(4555)>=40 and 3 or 2,20 end
  if PT2GiaoReady()==1 then return 1,20 end
 end
 if PT2_OLD_MARK then
  local state,oldLevel=PT2_OLD_MARK()
  if state and state>best then return state,oldLevel end
 end
 return best,level
end
function PT2GiaoReady()
 if not PT2Class() or GetLevel()<20 or GetTask(4554)~=0 then return 0 end
 if GetLevel()>=29 then return GetTask(4558)==0 and 1 or 0 end
 local today=floor((SystemTime()+25200)/86400)
 return (GetTask(4556)~=today or GetTask(4557)<3) and 1 or 0
end
function pt2_giao_preview()
 if not PT2GiaoRole() or PT2GiaoReady()~=1 then return end
 SayButtons("<color=yellow>Giáo Huấn<color><enter>Diệt 40 quái quanh phái theo chỉ dẫn.<enter><color=green>4600 kinh nghiệm, 3000 lượng.<color>",2,"Nhận nhiệm vụ/pt2_giao_take","Quay lại/main")
end
function pt2_giao_take()
 if not PT2GiaoRole() or PT2GiaoReady()~=1 or GetContentWorldPos()+1000~=PT2_MAP or GetTrade()~=0 or GetLockState()~=0 then return end
 local pool=GetPlayerType()==1 and {1,4} or {2,5,6,9,18};local target=pool[RANDOM(1,getn(pool))]
 if QuestExchange(4554,0,target,{},{},1)~=1 then return end
 local today=floor((SystemTime()+25200)/86400)
 if GetTask(4556)~=today then SetTask(4556,today);SetTask(4557,0) end
 SetTask(4557,GetTask(4557)+1);SetTask(4555,0);SetTask(4559,GetPlayerType()==1 and 1008 or 1011)
 if GetLevel()>=29 then SetTask(4558,1) end
 PT2Journal();PT2Notice("Diệt <color=red>"..PT2_LOCAL_NAMES[target].."<color> quanh phái theo chỉ dẫn nhiệm vụ.")
end
function pt2_giao_claim()
 if not PT2GiaoRole() or GetContentWorldPos()+1000~=PT2_MAP or GetTask(4554)<=0 or GetTask(4555)<40 or GetTrade()~=0 or GetLockState()~=0 then return end
 local before=GetCash();if before>1999997000 then return end;Earn(3000);if GetCash()~=before+3000 then return end
 if QuestExchange(4554,GetTask(4554),0,{},{},1)~=1 then Pay(3000);return end
 SetTask(4555,0);AddOwnExp(4600);PT2Journal();PT2Notice("Đã nhận 4600 kinh nghiệm và 3000 lượng.")
end
function PT2Event(id,limit) if HaveEventItem(id)>=limit then return 0 end return AddEventItem(id) end
function PT2Death(template)
 if not PT2Class() then return end
 local task=PT2MainTask();local phase=GetTask(task);local map=GetContentWorldPos()+1000
 for i=1,getn(PT2_KILLS) do local k=PT2_KILLS[i]
  if k[1]==GetPlayerType() and k[2]==template and k[3]==map and phase==k[4] and PT2Event(k[6],1)>0 and k[4]~=k[5] then SetTask(task,k[5]) end
 end
 if GetTask(4550)==1 and map==1015 then
  if template==7 then if AddIBBuff(335,10)>0 then SetTask(4551,SystemTime()+10);TopMessage(11631) else SetTask(4551,0) end end
  if template==13 and GetTask(4551)>SystemTime() and HaveIBBuff(335)==1 then PT2Event(191,5) end
 end
 -- Ngưu Sát is original template 12. Its bounded native reward hook
 -- grants Tĩnh Hóa Phù; template 18 is Hắc Phong, not this objective.
 if phase==87 and map==1070 and template>=541 and template<=543 then
  local bit=2^(template-541);local mask=GetTask(4562)
  if mod(floor(mask/bit),2)==0 then SetTask(4562,mask+bit) end
  if GetTask(4562)==7 then SetTask(task,88);SetTask(4563,SystemTime()+180) end
 end
 if template==2703 and map==1070 and phase==88 and SystemTime()<=GetTask(4563) and PT2Event(174,1)>0 then SetTask(task,89) end
 PT2Journal()
end
function pt2_challenge()
 if not PT2Class() or PT2_ROLE~="thong" or GetContentWorldPos()+1000~=1070 or GetTask(PT2MainTask())~=88 then return end
 if SystemTime()>GetTask(4563) then
  for id=176,178 do for n=1,2 do if HaveEventItem(id)>0 then DelEventItem(id) end end end
  SetTask(PT2MainTask(),83);SetTask(4560,0);SetTask(4561,0);SetTask(4562,0);SetTask(4563,0);PT2Journal();return
 end
 local previous=GetTask(4567)
 if previous>0 and GetNpcTemplateID(previous)==2703 and GetNpcWorldPos(previous)==1070 and GetNpcLife(previous)>0 then return end
 local npc=AddNpc(2703,95,SubWorld,204*256+128,210*512+256,1)
 if npc and npc>0 then if SetNpcDeathScript(npc,"\\script\\phongthan\\giapsi\\death_2703.lua")==1 then SetTask(4567,npc) else DelNpc(npc) end end
 CloseDialog()
end
