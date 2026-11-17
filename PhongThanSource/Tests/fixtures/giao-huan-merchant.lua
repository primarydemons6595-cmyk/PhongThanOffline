tasks={}
playerType=1
playerMap=3
playerLevel=20
cash=10000
experience=0
failed=0
oldOpened=0
function GetTask(k) return tasks[k] or 0 end
function SetTask(k,v) tasks[k]=v end
function GetPlayerType() return playerType end
function GetContentWorldPos() return playerMap end
function GetLevel() return playerLevel end
function SystemTime() return 1793366400 end
function GetCash() return cash end
function Earn(n) cash=cash+n end
function Pay(n) cash=cash-n end
function AddOwnExp(n) experience=experience+n end
function GetTrade() return 0 end
function GetLockState() return 0 end
function RANDOM(a,b) return a end
function QuestExchange(task,before,after,needs,gifts,flag)
 if failed==1 or GetTask(task)~=before then return 0 end
 SetTask(task,after);return 1
end
function PtTwoFactionNote(id,state,text,count) noteText=text end
function SayButtons(text,count,a,b,c) end
function CloseDialog() end
function main() oldOpened=oldOpened+1 end
function GetPlayerTaskState() return 0,0 end
function TestGiaoMerchant()
 playerType=PT_GH_FACTION;playerMap=PT_GH_MAP-1000
 main();assert(GetTask(4554)==0)
 pt_gh_take();assert(GetTask(4554)>0 and GetTask(4555)==0)
 local target=GetTask(4554);pt_gh_take();assert(GetTask(4554)==target and GetTask(4557)==1)
 SetTask(4555,31);main();assert(GetTask(4555)==31) -- existing progress preserved
 pt_gh_claim();assert(cash==10000 and experience==0)
 SetTask(4555,40);failed=1;pt_gh_claim();assert(cash==10000 and GetTask(4554)==target)
 failed=0;pt_gh_claim();assert(cash==13000 and experience==4600 and GetTask(4554)==0)
 pt_gh_claim();assert(cash==13000 and experience==4600)
 pt_gh_original();assert(oldOpened==1) -- shop/Mao Lu service remains accessible
 playerMap=1;pt_gh_take();assert(GetTask(4554)==0)
 playerMap=PT_GH_MAP-1000;playerType=0;pt_gh_take();assert(GetTask(4554)==0)
 playerType=PT_GH_FACTION;SetTask(4557,3);pt_gh_take();assert(GetTask(4554)==0)
 playerLevel=29;SetTask(4558,0);pt_gh_take();assert(GetTask(4554)>0 and GetTask(4558)==1)
 SetTask(4554,0);pt_gh_take();assert(GetTask(4554)==0)
 print('PASS merchant Giao Huan: correct faction/map, preserve active progress, 40 kills, rollback, single rewards, daily quota, level-29 quota, old services')
end
