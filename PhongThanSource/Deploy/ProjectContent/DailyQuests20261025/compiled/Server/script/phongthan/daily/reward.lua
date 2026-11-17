-- Shared only by the ten audited daily completion paths. No NPC menu text.
function PTDailyNextReceipt(kind)
 local slot=4370+kind-1
 local value=GetTask(slot)+1
 SetTask(slot,value)
 return value
end
function PTDailyHope(kind,receipt)
 local accepted,given=DailyHope(kind,receipt)
 if given>0 then
  TopMessage(14423)
  local count=GetIBBuffTimes(418)
  if count<30 then TaskNote(1048,0,count) else TaskNote(1048,1) end
 end
 return accepted
end
