checks=0;tasks={};items={};career=0;level=10;card=0;trade=0;locked=0;near=1;capacity=1000;choice=1;message="";menus=0
function check(v) checks=checks+1;if not v then error("LevelGift check #"..checks) end end
function GetTask(k) return tasks[k] or 0 end
function SetTask(k,v) tasks[k]=v end
function GetSeries() return career end
function GetLevel() return level end
function GetItemCount(g,d,l,s,p) check(g==8 and d==9700 and l==0 and p==3);return card end
function GetTrade() return trade end
function GetLockState() return locked end
function NearLeQuan() return near end
function Msg2Player(t) message=t end
function Talk(n,cb,t) message=t end
function CloseDialog() end
function SayButtons() menus=menus+1 end
function random(a,b) return choice end
function LockItem(id,t,s,d,unused) check(items[id] and t==1 and s==0 and d==0);items[id].trade=1 end
function AddNormalItem(g,d,p,l,s,luck,bind)
 if getn(items)>=capacity then return 0 end
 tinsert(items,{g,d,p,l,s,luck;bind=bind});return getn(items)
end
function QuestExchange(t,a,b,needs,rewards,stack)
 check(t==4704 and a==0 and b==1 and getn(needs)==0 and rewards[1][1]==8 and rewards[1][2]==9700 and rewards[1][3]==2)
 if GetTask(t)~=a or capacity==0 then return 0 end
 card=1;SetTask(t,b);return 1
end
function reset(c)
 tasks={};items={};career=c;level=10;card=0;trade=0;locked=0;near=1;capacity=1000;message=""
end
for c=0,2 do
 for w=1,2 do
  reset(c);choice=w
  capacity=0;check(not PTLGCard() and GetTask(4704)==0 and card==0)
  capacity=1000;check(PTLGCard()==1 and card==1 and GetTask(4704)==1)
  check(PTLGCard()==1 and card==1)
  for i=1,5 do main(1) end;check(getn(items)==0 and card==1)
  for stage=1,7 do
   local before=getn(items);level=PTLG_LEVELS[stage]-1
   PTLGPreview(stage);PTLGClaim(stage);check(getn(items)==before and GetTask(4709+stage)==0)
   level=PTLG_LEVELS[stage]
   trade=1;PTLGClaim(stage);check(getn(items)==before);trade=0
   locked=1;PTLGClaim(stage);check(getn(items)==before);locked=0
   card=0;PTLGClaim(stage);check(getn(items)==before);card=1
   PTLGPreview(stage);check(getn(items)==before and card==1)
   local rows=PTLGRewards(stage,c,w);local count=PTLGTotal(rows)
   -- Inventory failure at EVERY unit, retrying the persisted receipt each time.
   for unit=0,count-1 do
    capacity=before+unit;PTLGClaim(stage)
    check(GetTask(4709+stage)==unit and getn(items)==before+unit and card==1)
   end
   capacity=1000;PTLGClaim(stage);check(GetTask(4709+stage)==count and getn(items)==before+count)
   local at=before
   for row=1,getn(rows) do local r=rows[row]
    for unit=1,r[7] do at=at+1;local actual=items[at]
     for k=1,6 do check(actual[k]==r[k]) end
     if r[8]==1 then check(actual.bind==1)
     elseif r[8]==2 then check(actual.trade==1 and actual.bind==0) end
    end
   end
   PTLGClaim(stage);check(getn(items)==before+count and card==1)
   if stage==7 and c==0 then check(GetTask(4706)==w) end
  end
  check(PTLGAllDone() and card==1 and message==PTLG_DONE)
  for stage=1,7 do PTLGClaim(stage) end
  main(1);check(message==PTLG_DONE and card==1)
  career=mod(c+1,3);check(not PTLGReady());career=c
 end
end
reset(0);card=1;level=90;capacity=0;choice=2;PTLGClaim7();check(GetTask(4706)==2)
choice=1;capacity=1000;PTLGClaim7();check(GetTask(4706)==2 and items[2][3]==28)
-- Milestones may be claimed in any order at level 90; no earlier tier is lost.
reset(2);card=1;level=90
for stage=7,1,-1 do PTLGClaim(stage) end
check(PTLGAllDone() and card==1)
print("PASS LEVEL_GIFT checks="..checks.." classes=3 tiers=7 retained=1 duplicate_grants=0 partial_resume=all_units")
