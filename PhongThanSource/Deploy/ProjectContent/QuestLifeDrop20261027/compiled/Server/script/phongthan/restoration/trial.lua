PTQUEST_SEEN=PTQUEST_SEEN or {}
PTTR_NAMES={[0]="KiÕm Nh©n",[1]="TuyÕt Qu¸i",[2]="Háa DiÖn"}
PTTR_HOME={[0]=1002,[1]=1003,[2]=1004}
PTTR_RETURN={[0]="T¹p Hãa Th­¬ng, Sïng Thµnh (207,198)",[1]="Thî §ång, Ngäc H­ Cung (220,199)",[2]="Thî §ång, Xi V­u Mé (191,202)"}
function PTTrialHere()
 local f=GetPlayerType()
 return PTTR_NAMES[f] and PTTR_ROLE==f and GetContentWorldPos()+1000==PTTR_HOME[f]
end
function PTTrialNote()
 local f=GetPlayerType();if not PTTR_NAMES[f] then return end
 local phase=GetTask(4963)
 if phase==1 or phase==2 then
  local text="<color=yellow>T©n Thñ ThÝ LuyÖn<color><enter>Tiªu diÖt 8 <color=red>"..PTTR_NAMES[f].."<color> bªn ngoµi t©n thñ th«n. TiÕn ®é: "..GetTask(4964).."/8.<enter>Tr¶ nhiÖm vô: <color=green>"..PTTR_RETURN[f].."<color>."
  AddNote("3121",1,text,GetTask(4964))
 elseif phase==3 then AddNote("3121",2," ",3) end
end
function pt_trial_take()
 if not PTTrialHere() or GetLevel()<1 or GetTrade()~=0 or GetLockState()~=0 then return end
 if QuestExchange(4963,0,1,{},{},1)==1 then SetTask(4964,0);PTTrialNote() end
end
function pt_trial_claim()
 if not PTTrialHere() or GetTask(4963)~=2 or GetTask(4964)~=8 or GetTrade()~=0 or GetLockState()~=0 then return end
 local f=GetPlayerType();local detail=f==2 and 6 or 7
 local gifts={{0,detail,f,1,0,0,1}}
 if QuestExchange(4963,2,3,{},gifts,1)~=1 then
  Msg2Player("H·y dµnh chç trèng trong hµnh trang ®Ó nhËn trang bÞ t©n thñ.");return
 end
 AddOwnExp(f==2 and 200 or 150);PTTrialNote()
 Msg2Player("Hoµn thµnh T©n Thñ ThÝ LuyÖn. §· nhËn kinh nghiÖm vµ trang bÞ t©n thñ.")
end
function PTTrialKill(npc,entity,template,map)
 local f=GetPlayerType();if not PTTR_NAMES[f] or GetTask(4963)~=1 then return end
 if GetTrade()~=0 or GetLockState()~=0 then return end
 local target=f==0 and 0 or (f==1 and 1 or 2)
 local allowed=(f==0 and map>=1005 and map<=1007) or (f==1 and map>=1008 and map<=1010) or (f==2 and map>=1011 and map<=1013)
 if not allowed or template~=target then return end
 -- Native death hooks may be replayed; keep a bounded per-player entity ring.
 local p=PlayerIndex or 0;local owner=PlayerId or 0
 local seen=PTQUEST_SEEN[p]
 if not seen or seen.owner~=owner then seen={owner=owner,ids={},next=1};PTQUEST_SEEN[p]=seen end
 for i=1,getn(seen.ids) do if seen.ids[i]==entity then return end end
 seen.ids[seen.next]=entity;seen.next=seen.next+1;if seen.next>16 then seen.next=1 end

 local count=GetTask(4964);if count<0 or count>=8 then return end
 SetTask(4964,count+1)
 if count+1==8 then
  if QuestExchange(4963,1,2,{},{},1)~=1 then SetTask(4964,count);return end
  Msg2Player("T©n Thñ ThÝ LuyÖn ®· ®ñ 8 qu¸i. VÒ gÆp "..PTTR_RETURN[f]..".")
 end
 PTTrialNote()
end
function pt_trial_services() if PTTR_OLD_MAIN then return PTTR_OLD_MAIN() end;CloseDialog() end
function PTTrialMenu()
 if not PTTrialHere() then return pt_trial_services() end
 PTTrialNote();local phase=GetTask(4963);local options={}
 if phase==0 then tinsert(options,"NhËn T©n Thñ ThÝ LuyÖn/pt_trial_take")
 elseif phase==2 and GetTask(4964)==8 then tinsert(options,"Tr¶ T©n Thñ ThÝ LuyÖn/pt_trial_claim") end
 if phase==3 then return pt_trial_services() end
 tinsert(options,"Héi tho¹i kh¸c/pt_trial_services")
 tinsert(options,"KÕt thóc/pt_trial_close")
 SayButtons("<color=yellow>T©n Thñ ThÝ LuyÖn<color><enter>Ra ngoµi t©n thñ th«n tiªu diÖt 8 <color=red>"..PTTR_NAMES[GetPlayerType()].."<color> ®Ó chøng tá thùc lùc. TiÕn ®é: "..GetTask(4964).."/8.",getn(options),options)
end
function pt_trial_close() CloseDialog() end
function PTTrialMarker()
 if PTTrialHere() then
  if GetTask(4963)==0 then return 1,1 end
  if GetTask(4963)==1 then return 2,1 end
  if GetTask(4963)==2 then return 3,1 end
 end
 if PTTR_OLD_MARK then return PTTR_OLD_MARK() end;return 0,0
end
