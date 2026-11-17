characters={};active=1;trainer=1003;busy=0;failPay=0;failAdd=0;bagFailure=0;roll=1;dialogs=0
function reset()
 characters[active]={state={},levels={},bag={},cash=1000000000,xp=0,transactions=0}
 characters[active].state[5141]=14900;characters[active].state[5142]=14900
 busy=0;failPay=0;failAdd=0;bagFailure=0;roll=1
end
function char() return characters[active] end
function PtLifeState(k,v) if v~=nil then char().state[k]=v end return char().state[k] or 0 end
function GetLiveSkillLevel(id) return char().levels[id] or 0 end
function GetLiveSkillProficiency(id) return PtLifeState(5120+id) end
function GetLevel() return playerLevel or 200 end
function GetCash() return char().cash end
function Pay(n) if failPay==1 or char().cash<n then return 0 end char().cash=char().cash-n;return 1 end
function Earn(n) char().cash=char().cash+n;return 1 end
function LifeBusy() return busy end
function LifeTrainerNear(skill)
 if trainer==1003 and (skill==nil or skill==0 or skill==1 or skill==3) then return 1 end
 if trainer==1021 and (skill==nil or skill==0 or skill==4 or skill==8 or skill==6) then return 1 end
 if trainer==1020 and (skill==nil or skill==0 or skill==10 or skill==9 or skill==7 or skill==5) then return 1 end
 return 0
end
function AddLiveSkill(id,rank)
 if failAdd==1 then return 0 end
 assert(LifeTrainerNear(id)==1 and GetLiveSkillLevel(id)+1==rank)
 char().levels[id]=rank;PtLifeState(5120+id,0);return 1
end
function key(t) return t[1]..":"..t[2]..":"..t[3] end
function quantity(t) return char().bag[key(t)] or 0 end
function give(t,n) char().bag[key(t)]=quantity(t)+n end
function QuestExchange(task,before,after,needs,gifts,stack)
 if bagFailure==1 or PtLifeState(task)~=before then return 0 end
 local totals={}
 for i=1,getn(needs) do local k=key(needs[i]);totals[k]=(totals[k] or 0)+needs[i][7] end
 for k,n in totals do if (char().bag[k] or 0)<n then return 0 end end
 for k,n in totals do char().bag[k]=char().bag[k]-n end
 for i=1,getn(gifts) do give(gifts[i],gifts[i][7]) end
 PtLifeState(task,after);char().transactions=char().transactions+1;return 1
end
function RANDOM(a,b) if roll<a then return a end if roll>b then return b end return roll end
function CloseDialog() end
function SayButtons(text,count,a,b,c,d,e,f)
 assert(strlen(text)<=480 and count>=1 and count<=6)
 dialogs=dialogs+1
end
function TestLifeSkills()
 local ids={1,3,4,5,6,7,8,9,10}
 reset()
 for i=1,getn(ids) do local skill=ids[i]
  trainer=(skill==1 or skill==3) and 1003 or ((skill==4 or skill==8 or skill==6) and 1021 or 1020)
  pt_life_main();pt_life_open(skill)
  for rank=0,9 do
   PtLifeState(5120+skill,10000);pt_life_learn_preview()
   local before=GetCash();local cost=rank==0 and 1000 or 10000*2^rank
   pt_life_learn();assert(GetLiveSkillLevel(skill)==rank+1 and GetCash()==before-cost)
   pt_life_learn();assert(GetLiveSkillLevel(skill)==rank+1 and GetCash()==before-cost)
  end
 end
 active=2;reset();trainer=1003;pt_life_open(5);pt_life_learn_preview();pt_life_learn();assert(GetLiveSkillLevel(5)==0)
 trainer=1021;pt_life_open(10);pt_life_learn_preview();pt_life_learn();assert(GetLiveSkillLevel(10)==0)
 trainer=1020;pt_life_open(4);pt_life_learn_preview();pt_life_learn();assert(GetLiveSkillLevel(4)==0)
 trainer=1003;pt_life_open(1);playerLevel=19;pt_life_learn_preview();pt_life_learn();assert(GetLiveSkillLevel(1)==0);playerLevel=200
 pt_life_learn_preview();failPay=1;pt_life_learn();assert(GetLiveSkillLevel(1)==0);failPay=0
 failAdd=1;pt_life_learn();assert(GetCash()==1000000000 and GetLiveSkillLevel(1)==0);failAdd=0
 pt_life_learn();assert(GetLiveSkillLevel(1)==1);pt_life_learn_preview();pt_life_learn();assert(GetLiveSkillLevel(1)==1)
 active=1;assert(GetLiveSkillLevel(1)==10)
 local tested=0
 for id=1,128 do local r=PT_LIFE_RECIPES[id]
  if r then
   reset();char().levels[r.skill]=r.rank;PtLifeState(5200+id,10);pt_life_open(r.skill)
   pt_life_recipe(id);local before=GetCash();bagFailure=1;pt_life_make(id)
   assert(GetCash()==before and char().transactions==0 and PtLifeState(5141)==14900);bagFailure=0
   for i=1,getn(r.needs) do give(r.needs[i],r.needs[i][7]) end
   PtLifeState(5120+r.skill,9999);pt_life_recipe(id);pt_life_make(id)
   assert(char().transactions==1 and GetCash()==before-r.cost and PtLifeState(5141)==14900-r.energy and PtLifeState(5142)==14900-r.strength)
   assert(quantity(r.outputs[1].item)>=1)
   assert(GetLiveSkillProficiency(r.skill)<=10000)
   if r.default==0 then assert(PtLifeState(5200+id)==9) end
   pt_life_make(id);assert(char().transactions==1)
   -- Failure and alternate-output branches use the actual recipe probabilities.
   reset();char().levels[r.skill]=r.rank;PtLifeState(5200+id,10);pt_life_open(r.skill)
   for i=1,getn(r.needs) do give(r.needs[i],r.needs[i][7]) end
   roll=100;pt_life_recipe(id);pt_life_make(id);assert(char().transactions==1)
   if r.chance<100 then assert(quantity(r.outputs[1].item)==0)
   elseif getn(r.outputs)==2 then assert(quantity(r.outputs[2].item)==1) end
   reset();char().levels[r.skill]=r.rank;PtLifeState(5200+id,10);pt_life_open(r.skill)
   pt_life_recipe(id);pt_life_make(id);assert(char().transactions==0 and GetCash()==1000000000)
   tested=tested+1
  end
 end
 assert(tested==128)
 reset();for i=1,getn(ids) do char().levels[ids[i]]=10;pt_life_open(ids[i]);pt_life_page0();pt_life_locate() end
 print('PASS actual nine-profession Lua4: 90 learning steps, town restrictions, level/proficiency/coin guards, refund, isolated players, 128 recipes success/failure/alternate outputs/missing/full-bag/replay, formula charges, resource costs, menus')
end
