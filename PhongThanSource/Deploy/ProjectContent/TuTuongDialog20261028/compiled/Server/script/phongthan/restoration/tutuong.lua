PTQUEST_SEEN=PTQUEST_SEEN or {}
-- Shared authoritative Tu Tuong collection projection. Task 55 retains legacy target IDs.
PTTT_NAMES={[22]="§Şa t©m",[23]="Phong lÖ",[24]="Thñy hån",[25]="Háa linh"}
PTTT_PLACES={[22]="Mª cung sa m¹c",[23]="B¨ng xuyªn mª cung",[24]="c¸c khu vùc Long Cung",[25]="Hiªn Viªn §éng"}
function PTTTDay() return floor((SystemTime()+25200)/86400) end
function PTTTCount() if GetTask(4961)~=PTTTDay() then return 0 end;return GetTask(4962) end
function PTTTActive() return GetTask(55)>=22 and GetTask(55)<=25 and HaveIBBuff(293)==1 end
function PTTTJournal()
 local target=GetTask(55)
 if not PTTT_NAMES[target] then AddNote("3026",2," ",0);return end
 local count=HaveNormalItem(3,target,0,0);if count>10 then count=10 end
 local text="<color=yellow>Tø T­îng<color><enter>Thu thËp <color=yellow>"..PTTT_NAMES[target].."<color> t¹i <color=cyan>"..PTTT_PLACES[target].."<color>, råi giao Thñ khè TriÒu Ca hoÆc T©y Kú.<enter>TiÕn ®é: "..count.."/10."
 if HaveIBBuff(293)~=1 and count<10 then text=text.."<enter>Truy t×m Tam Nguyªn ®· hÕt hiÖu lùc. GÆp Thñ khè ®Ó nhËn l¹i l­ît t×m." end
 AddNote("3026",1,text,count)
end
function pts_elements()
 if GetPlayerType()<0 or GetPlayerType()>2 or GetLevel()<31 or (PTS_ROLE~="store_west" and PTS_ROLE~="store_east") or PTSMap()~=PTS_MAP then return end
 local target=GetTask(55)
 if PTTT_NAMES[target] and HaveNormalItem(3,target,0,0)>=10 then
  local gifts={};local roll=RANDOM(1,100000);local green=0
  if roll<=10000 then gifts={{0,4,RANDOM(6,13),3,0,0,1}}
  elseif roll<=15000 then local part=RANDOM(3,5);gifts={{0,4+RANDOM(1,3),part,4,2+5*(part-3),0,1}};green=1 end
  if PTSExchange(55,target,0,{{3,target,0,0,0,0,10}},gifts,1200,nil)==1 then
   AddCredit(5);SetTask(54,0);RemoveIBBuff(293);if green==1 then SetTask(811,GetTask(811)+1) end;PTTTJournal();PTSNotice("§· giao ®ñ nguyªn liÖu Tø T­îng vµ nhËn th­ëng.")
  end
  return
 end
 if PTTTActive() then PTTTJournal();return end
 local paid=target==0 and HaveIBBuff(293)==1
 if PTTTCount()>=5 and not paid then PTSNotice("§· nhËn ®ñ 5 l­ît miÔn phİ h«m nay. Ngµy mai cã thÓ tiÕp tôc; vËt phÈm T×m nguyªn liÖu dïng theo ®iÒu kiÖn cña vËt phÈm.");return end
 if target~=0 and not PTTT_NAMES[target] then return end
 local next=21+RANDOM(1,4)
 if target~=0 then next=target end
 -- Create the original visible status first; failure never spends a daily turn.
 if not paid and AddIBBuff(293,1800)~=1 then return end
 if target==0 and PTSExchange(55,0,next,{},{},nil,nil)~=1 then RemoveIBBuff(293);return end
 if not paid then local count=PTTTCount();SetTask(4961,PTTTDay());SetTask(4962,count+1) end
 SetTask(54,1)
 PTTTJournal();PTSNotice("§· nhËn <color=yellow>Truy t×m Tam Nguyªn<color> trong 30 phót. §¸nh qu¸i t¹i "..PTTT_PLACES[next].."; nguyªn liÖu t×m ®­îc tù vµo hµnh trang.")
end
function PTTTKill(npc,entity,template,map)
 if not PTTTActive() or GetTrade()~=0 or GetLockState()~=0 then return end
 local target=GetTask(55);local area=(target==22 and map>=1023 and map<=1026) or (target==23 and map>=1032 and map<=1036) or (target==24 and map>=1038 and map<=1041) or (target==25 and map>=1027 and map<=1031)
 if not area or HaveNormalItem(3,target,0,0)>=10 then return end
 -- Native death hooks may be replayed; keep a bounded per-player entity ring.
 local p=PlayerIndex or 0;local owner=PlayerId or 0
 local seen=PTQUEST_SEEN[p]
 if not seen or seen.owner~=owner then seen={owner=owner,ids={},next=1};PTQUEST_SEEN[p]=seen end
 for i=1,getn(seen.ids) do if seen.ids[i]==entity then return end end
 seen.ids[seen.next]=entity;seen.next=seen.next+1;if seen.next>16 then seen.next=1 end

 -- Collection rate is a separate server policy; never changes monster loot.
 if RANDOM(1,10)~=1 then return end
 if AddNormalItem(3,target,0,0,0,0)>0 then PTTTJournal() end
end
-- Includes share one Lua state. The daily NPC includes this file through
-- daily/runtime.lua and again at its tail; never wrap our wrapper again.
if PTSJournal and PTSJournal~=PTTT_WRAPPED_JOURNAL then
 local previous=PTSJournal
 function PTSJournal() %previous();PTTTJournal() end
 PTTT_WRAPPED_JOURNAL=PTSJournal
end
