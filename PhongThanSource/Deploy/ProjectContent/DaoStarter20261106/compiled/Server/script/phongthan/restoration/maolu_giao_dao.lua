--description: ‘”ªı…Ã
--author: yichuan
--date: 2004/5/12

function main(sel)
	if(songxin()==0)then
		MsgBox(10217,"yes_1","no")
	end
end;

function songxin()
	local task_id = 870
	local map_id = 2

	local task_val = GetTask(task_id)
	local type1 = GetByte(task_val,1) 
	local type2 = GetByte(task_val,2)
	local finish = GetByte(task_val,3)

	local setbit = 0
	if (type1 == map_id) then
		setbit = 7
	elseif (type2 == map_id)then
		setbit = 8
	end

	if (setbit ~= 0) then
	 	if (GetBit(finish, setbit) == 0)then
	 		Talk(1,"no","Ng≠¨i v t v∂ qu∏!")
			SetTask(task_id, SetByte(task_val,3,SetBit(finish, setbit, 1)))
	 		return 1
	 	end
	end
	return 0
end


function yes_1()
		CloseDialog()
		Sale(6);
end;

function no()
		CloseDialog()
end;
Include("\\script\\phongthan\\restoration\\dao_maolu.lua")
PTMAO_NPC_CLASS=1
PTMAO_NPC_ROLE="giao"
PTMAO_OLD_MAIN=main
PTMAO_OLD_MARK=GetPlayerTaskState
PTMAO_OLD_CATEGORY=GetPlayerTaskCategory
function main()
 if PTMaoMenu()==1 then return end
 if PTMAO_OLD_MAIN then return PTMAO_OLD_MAIN() end
 CloseDialog()
end
function GetPlayerTaskState() return PTMaoMarker() end
function GetPlayerTaskCategory()
 if PTMaoReady() then return 1 end
 if PTMAO_OLD_CATEGORY then return PTMAO_OLD_CATEGORY() end
 if PTGMarker then local stage=PTGMarker();if stage>0 then return 1 end end
 if PT2Marker then local stage=PT2Marker();if stage>0 then return 1 end end
 return 0
end

-- PT_GIAO_HUAN_MERCHANT_20261030
PT_GH_MAP=1003
PT_GH_FACTION=1
PT_GH_HOME="Ng‰c H≠ Cung (216,201)"
PT_GH_NAMES={[1]="Tuy’t Qu∏i",[4]="B®ng Lang",[2]="H·a Di÷n",[5]="LÙc Qu∏i",[6]="CuÂng ßi”u",[9]="Th∂o Ti™n",[18]="Hæc Phong"}
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
  PtTwoFactionNote("3053",1,"<color=yellow>Gi∏o Hu n<color><enter>Di÷t <color=red>"..PT_GH_NAMES[GetTask(4554)].."<color> quanh ph∏i: "..GetTask(4555).."/40.<enter>V“ Tπp H„a Th≠¨ng Nh©n, "..PT_GH_HOME..".",GetTask(4555))
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
 local options={"Gi∏o Hu n/pt_gh_view","Giao dﬁch, nhi÷m vÙ kh∏c/pt_gh_original","ß„ng/pt_gh_close"}
 SayButtons("Tπp H„a Th≠¨ng Nh©n",getn(options),options)
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
  local options={"Quay lπi/main"}
  if GetTask(4555)>=40 then tinsert(options,1,"NhÀn th≠Îng/pt_gh_claim") end
  SayButtons("<color=yellow>Gi∏o Hu n<color><enter>Di÷t "..PT_GH_NAMES[GetTask(4554)]..": "..GetTask(4555).."/40.",getn(options),options)
 elseif pt_gh_ready()==1 then
  SayButtons("<color=yellow>Gi∏o Hu n<color><enter>Di÷t 40 qu∏i quanh ph∏i.<enter>Ph«n th≠Îng: 4600 kinh nghi÷m, 3000 l≠Óng.",2,"NhÀn nhi÷m vÙ/pt_gh_take","Quay lπi/main")
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
 SayButtons("ß∑ nhÀn 4600 kinh nghi÷m vµ 3000 l≠Óng.",1,"Quay lπi/main")
end
function pt_gh_close() CloseDialog() end
