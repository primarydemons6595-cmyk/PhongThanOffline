PTMAO_TEXT={
[0]="Lµ mét t©n thñ cña Sïng thµnh, b¹n ph¶i nç lùc hÕt m×nh. H·y ®Õn gÆp <color=red>T¹p Hãa Th­¬ng<color> b¸o cho «ng Êy biÕt s¾p cã m­a lín, h·y che ®Ëy hµng hãa cho kü!",
[1]="Lµ mét t©n thñ cña Ngäc H­ Cung, b¹n ph¶i luyÖn tËp ch¨m chØ. H·y ®Õn t×m <color=red>Tõ Hµng §¹o Nh©n<color> xem cã chuyÖn g× kh«ng?",
[2]="Lµ t©n thñ cña Xi V­u Mé ph¶i nç lùc hÕt m×nh. H·y ®Õn t×m <color=red>ThiÕu H¹o<color> xem cã chuyÖn g× kh«ng?"}
function PTMaoNote()
 local f=GetPlayerType();if not PTMAO_TEXT[f] then return end
 if GetTask(4960)==1 then AddNote("3120",1,"<color=yellow>Mao L­<color><enter>"..PTMAO_TEXT[f],1)
 elseif GetTask(4960)>=2 then AddNote("3120",2," ",2) end
end
function pt_mao_take()
 if GetLevel()~=1 or not PTMAO_TEXT[GetPlayerType()] then return end
 if QuestExchange(4960,0,1,{},{})==1 then PTMaoNote() end
end
function pt_mao_later() CloseDialog() end
function PTMaoOffer()
 if GetLevel()==1 and GetTask(4960)==0 and PTMAO_TEXT[GetPlayerType()] then
  MsgBox("<color=green>Hoan nghªnh b¹n ®Õn víi thÕ giíi Phong ThÇn!<color><enter><enter><color=yellow>Mao L­:<color> "..PTMAO_TEXT[GetPlayerType()],"pt_mao_take","pt_mao_later")
 else PTMaoNote() end
end
function PTMaoMeet()
 local f=GetPlayerType();local map=GetContentWorldPos()+1000
 local target=(f==0 and PTG_ROLE=="giao" and map==1002) or (f==1 and PT2_ROLE=="tuhang" and map==1003) or (f==2 and PT2_ROLE=="thieuhao" and map==1004)
 if target and GetTask(4960)==1 and QuestExchange(4960,1,2,{},{})==1 then PTMaoNote();Msg2Player("Mao L­: ®· t×m ®­îc ng­êi cÇn gÆp.") end
end
