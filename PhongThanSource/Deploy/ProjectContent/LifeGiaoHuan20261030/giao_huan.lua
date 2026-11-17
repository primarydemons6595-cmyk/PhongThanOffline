-- Appended to the actual general merchants, preserving their existing services.
PT_GH_OLD_MAIN=main
PT_GH_OLD_MARK=GetPlayerTaskState
PT_GH_OLD_CATEGORY=GetPlayerTaskCategory
function pt_gh_guard()
 return GetPlayerType()==PT_GH_FACTION and GetContentWorldPos()+1000==PT_GH_MAP
end
function pt_gh_ready()
 if not pt_gh_guard() or GetLevel()<20 or GetTask(4554)~=0 then return 0 end
 if GetLevel()>=29 then return GetTask(4558)==0 and 1 or 0 end
 local today=floor((SystemTime()+25200)/86400)
 return (GetTask(4556)~=today or GetTask(4557)<3) and 1 or 0
end
function pt_gh_journal()
 if GetTask(4554)>0 then
  PtTwoFactionNote("3053",1,"<color=yellow>Giáo Huấn<color><enter>Diệt <color=red>"..PT_GH_NAMES[GetTask(4554)].."<color> quanh phái: "..GetTask(4555).."/40.<enter>Về Tạp Hóa Thương Nhân, "..PT_GH_HOME..".",GetTask(4555))
 else PtTwoFactionNote("3053",2," ",0) end
end
function pt_gh_original() if PT_GH_OLD_MAIN then return PT_GH_OLD_MAIN() end CloseDialog() end
function pt_gh_state()
 if not pt_gh_guard() or GetLevel()<20 then return 0 end
 if GetTask(4554)>0 then return GetTask(4555)>=40 and 3 or 2 end
 return pt_gh_ready()
end
function main()
 if pt_gh_state()==0 then return pt_gh_original() end
 pt_gh_journal()
 local options={"Giáo Huấn/pt_gh_view","Giao dịch, nhiệm vụ khác/pt_gh_original","Đóng/pt_gh_close"}
 SayButtons("Tạp Hóa Thương Nhân",getn(options),options)
end
function GetPlayerTaskState()
 local state=pt_gh_state()
 if state>0 then return state,20 end
 if PT_GH_OLD_MARK then return PT_GH_OLD_MARK() end
 return 0,0
end
function GetPlayerTaskCategory()
 if pt_gh_state()>0 then return 1 end
 if PT_GH_OLD_CATEGORY then return PT_GH_OLD_CATEGORY() end
 return 0
end
function pt_gh_view()
 if not pt_gh_guard() then return end
 if GetTask(4554)>0 then
  local options={"Quay lại/main"}
  if GetTask(4555)>=40 then tinsert(options,1,"Nhận thưởng/pt_gh_claim") end
  SayButtons("<color=yellow>Giáo Huấn<color><enter>Diệt "..PT_GH_NAMES[GetTask(4554)]..": "..GetTask(4555).."/40.",getn(options),options)
 elseif pt_gh_ready()==1 then
  SayButtons("<color=yellow>Giáo Huấn<color><enter>Diệt 40 quái quanh phái.<enter>Phần thưởng: 4600 kinh nghiệm, 3000 lượng.",2,"Nhận nhiệm vụ/pt_gh_take","Quay lại/main")
 end
end
function pt_gh_take()
 if pt_gh_ready()~=1 or GetTrade()~=0 or GetLockState()~=0 then return end
 local pool=PT_GH_FACTION==1 and {1,4} or {2,5,6,9,18}
 local target=pool[RANDOM(1,getn(pool))]
 if QuestExchange(4554,0,target,{},{},1)~=1 then return end
 local today=floor((SystemTime()+25200)/86400)
 if GetTask(4556)~=today then SetTask(4556,today);SetTask(4557,0) end
 SetTask(4557,GetTask(4557)+1);SetTask(4555,0)
 SetTask(4559,PT_GH_FACTION==1 and 1008 or 1011)
 if GetLevel()>=29 then SetTask(4558,1) end
 pt_gh_journal();pt_gh_view()
end
function pt_gh_claim()
 if not pt_gh_guard() or GetTask(4554)<=0 or GetTask(4555)<40 or GetTrade()~=0 or GetLockState()~=0 then return end
 local before=GetCash();if before>1999997000 then return end
 Earn(3000);if GetCash()~=before+3000 then return end
 if QuestExchange(4554,GetTask(4554),0,{},{},1)~=1 then Pay(3000);return end
 SetTask(4555,0);AddOwnExp(4600);pt_gh_journal()
 SayButtons("Đã nhận 4600 kinh nghiệm và 3000 lượng.",1,"Quay lại/main")
end
function pt_gh_close() CloseDialog() end
