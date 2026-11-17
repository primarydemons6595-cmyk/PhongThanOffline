Include("\\script\\phongthan\\daily\\runtime.lua")
function main(item)
 if PTDailyGet(2,0)~=1 or PTDailyGet(2,3)<SystemTime() then return end
 local p=PT_DAILY_DATA.targets[PTDailyGet(2,6)];local map=DailyWorldPos()
 if not p then return end
 local variant=random(1,4)
 local names={"Chiªu Hån trËn (L«i)","Chiªu Hån trËn (Thæ)","Chiªu Hån trËn (B¨ng)","Chiªu Hån trËn (Háa)"}
 local result=DailySoulCircle(variant,names[variant],p.map,p.x,p.y)
 if result>0 then PTDailyJournal(2)
 elseif result==-1 then Talk(1,"","Ch­a ph¶i n¬i cÇn siªu ®é.")
 else Talk(1,"","Ch­a thÓ më trËn. H·y kiÓm tra nhiÖm vô, ph­ín vµ trËn ®ang cã.") end
end
