-- Read-only capability contract. Player callbacks remain the only writers.
function PTBotTuple(g,d,p,l,s,n) return {g,d,p,l,s,0,n} end
function PTBotGoal(ordinal,wantedGenre,wantedDetail,wantedPart)
 local goals={}
 if PTBOT_BANK then
  local opened=GetTask(13)==2 or GetTask(23)==2 or GetTask(33)==2
  if not opened then
   local phase=GetTask(PTBOT_BANK.task)
   local needs={}
   if phase==1 then needs={PTBotTuple(3,PTBOT_BANK.detail,0,0,0,5)} end
   tinsert(goals,{id=PTBOT_BANK.task,phase=phase,priority=10000,task=PTBOT_BANK.task,choices={"yes_1","renwu1"},needs=needs})
  end
 end
 if PTAttentionDailyMark then
  local state,level=PTAttentionDailyMark()
  -- The free daily spin and an unfinished spin are useful. Do not spend a
  -- paid extra-spin item just because one happens to be carried.
  if state>0 and (GetTask(4910)==1 or ceil(GetTask(73)/86400)~=ceil(SystemTime()/86400)) then
   tinsert(goals,{id=4910,phase=GetTask(4910),priority=850,choices={"yes_3","yes_2"},needs={}})
  end
 end
 if PTS_ACTIONS and PTS_ROLE and GetPlayerType()>=0 and GetPlayerType()<=2 then
  if GetTask(51)==3 and HaveEventItem(37)<1 then
   tinsert(goals,{id=30051,phase=3,priority=800,task=51,target=109,targetMap=1022,choices={},needs={PTBotTuple(4,37,0,0,0,1)}})
  end
  if GetTask(50)==2 and HaveEventItem(38)<1 then
   tinsert(goals,{id=30050,phase=2,priority=800,task=50,target=105,targetMap=1010,choices={},needs={PTBotTuple(4,38,0,0,0,1)}})
  elseif GetTask(50)==5 and HaveEventItem(40)<1 then
   tinsert(goals,{id=30050,phase=5,priority=800,task=50,target=115,targetMap=1043,choices={},needs={PTBotTuple(4,40,0,0,0,1)}})
  end
  for i=1,getn(PTS_ACTIONS) do
   local q=PTS_ACTIONS[i]
   if q.role==PTS_ROLE and GetLevel()>=q.level and GetTask(q.task)==q.from and not q.loan and (not q.morph or GetMorphType()==q.morph) and GetCash()>=(q.cost or 0) then
    local future={}
    for other=1,getn(PTS_ACTIONS) do local step=PTS_ACTIONS[other]
     if step.task==q.task then for j=1,getn(step.gifts) do tinsert(future,step.gifts[j]) end end
    end
    tinsert(goals,{id=10000+i,phase=q.from,priority=450,task=q.task,fee=q.cost or 0,choices={"pts_apply"..i,"pts_preview"..i},needs=q.needs,rewards=q.gifts,futureRewards=future})
   end
  end
  if PTS_ROLE=="west" and GetLevel()>=19 then
   for n=1,getn(PTS_MOUNTS) do local r=PTS_MOUNTS[n]
    if HaveNormalItem(0,10,r[1],r[2])>0 then
     tinsert(goals,{id=20000+n,phase=GetTask(4570),priority=700,task=4570,choices={"pts_mount_apply","pts_mount"},needs={PTBotTuple(4,39,0,0,0,3),PTBotTuple(0,10,r[1],r[2],0,1),PTBotTuple(3,r[3],0,0,0,1)},rewards={PTBotTuple(0,10,r[4],r[2],0,1)}})
    end
   end
  end
 end
 local goal=goals[ordinal+1]
 if goal and wantedGenre then
  goal.relevant=0
  local rewards={}
  local source=goal.futureRewards or goal.rewards or {}
  for i=1,getn(source) do tinsert(rewards,source[i]) end
  if goal.task then for i=1,getn(PTS_ACTIONS or {}) do local step=PTS_ACTIONS[i]
   if step.task==goal.task then for j=1,getn(step.gifts) do tinsert(rewards,step.gifts[j]) end end
  end end
  for i=1,getn(rewards) do local r=rewards[i]
   if r[1]==wantedGenre and r[2]==wantedDetail and (wantedPart<0 or r[3]==wantedPart) then goal.relevant=1 end
  end
 end
 return goal
end
