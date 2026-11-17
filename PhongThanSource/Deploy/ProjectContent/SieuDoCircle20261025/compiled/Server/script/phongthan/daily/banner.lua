Include("\\script\\phongthan\\daily\\runtime.lua")
function main(item)
 if PTDailyGet(2,0)~=1 or PTDailyGet(2,3)<SystemTime() then return end
 local p=PT_DAILY_DATA.targets[PTDailyGet(2,6)];local map=GetWorldPos()
 if not p or map~=p.map then return end
 local variant=random(1,4)
 local names={"Chiªu Hån trËn (L«i)","Chiªu Hån trËn (Thæ)","Chiªu Hån trËn (B¨ng)","Chiªu Hån trËn (Háa)"}
 if DailySoulCircle(variant,names[variant])>0 then
  for i=262,265 do RemoveIBBuff(i) end
  AddIBBuff(261+variant,200);PTDailyJournal(2)
 end
end
