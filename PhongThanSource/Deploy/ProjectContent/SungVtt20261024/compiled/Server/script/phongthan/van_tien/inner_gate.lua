function main()
 local map=GetContentWorldPos();local id=map-66
 local state=EventVttGate(id,0)
 if state<0 then CloseDialog();return end
 if state==1 then Say("<color=yellow>Cöa V¹n Tiªn TrËn<color> ®· më. Ng­¬i cã thÓ vµo gÆp <color=red>Th«ng Thiªn Gi¸o Chñ<color>.",2,"Vµo cöa trËn/pt_enter","KÕt thóc/pt_close")
 else Say("Muèn më cöa trËn, h·y h¹ bèn ®Çu lÜnh vµ mang ®ñ <color=green>Thanh Long LÖnh<color>, <color=green>B¹ch Hæ LÖnh<color>, <color=green>Chu T­íc LÖnh<color> vµ <color=green>HuyÒn Vò LÖnh<color> ®Õn ®©y.",2,"Dïng bèn LÖnh Bµi më cöa/pt_open","KÕt thóc/pt_close") end
end
function pt_open()
 local map=GetContentWorldPos();local result=EventVttGate(map-66,1)
 if result==1 then main()
 elseif result==-2 then Talk(1,"pt_close","TrËn chiÕn ch­a b¾t ®Çu.")
 elseif result==-3 then Talk(1,"pt_close","Bèn ®Çu lÜnh vÉn ch­a bÞ tiªu diÖt hÕt.")
 else Talk(1,"pt_close","Ng­¬i ch­a mang ®ñ <color=green>bèn LÖnh Bµi<color> trong hµnh trang. Ch­a trõ vËt phÈm.") end
end
function pt_enter()
 local map=GetContentWorldPos();if EventVttGate(map-66,2)==1 then CloseDialog() else Talk(1,"pt_close","Ch­a thÓ vµo cöa trËn. H·y ®øng gÇn cöa vµ thö l¹i.") end
end
function pt_close() CloseDialog() end

pt_legacy_gate_main=main
pt_legacy_gate_open=pt_open
function main()
 local id=GetContentWorldPos()-66
 if id==4 then return pt_legacy_gate_main() end
 local state=EventVttGate(id,0)
 if state<0 then CloseDialog();return end
 SayButtons("<color=yellow>Cæng thµnh V¹n Tiªn TrËn<color><enter>H¹ ®ñ bèn Hé VÖ, mang bèn LÖnh Bµi ®Õn më cæng. Khi cæng më, h·y ®i qua lèi nµy ®Ó gÆp Th«ng Thiªn Gi¸o Chñ.",1,"Më cæng/pt_open")
end
function pt_open()
 local id=GetContentWorldPos()-66
 if id==4 then return pt_legacy_gate_open() end
 local result=EventVttGate(id,1)
 if result==1 then SayButtons("<color=green>Cæng thµnh ®· më.<color><enter>H·y ®i qua cöa vµo gÆp Th«ng Thiªn Gi¸o Chñ.",0,{})
 elseif result==-2 then SayButtons("Ch­a ®Õn giê chiÕn ®Êu.",0,{})
 elseif result==-3 then SayButtons("Cßn Hé VÖ ch­a bÞ tiªu diÖt.",0,{})
 else SayButtons("CÇn ®ñ bèn LÖnh Bµi trong hµnh trang vµ ®øng gÇn cæng. VËt phÈm ch­a bÞ trõ.",0,{}) end
end
