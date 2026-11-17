
function pt_id() local map=GetContentWorldPos();return map-66 end
function main()
 if GetTask(408)==1 then
  SetTask(408,100000)
  MsgBox("Ta ch­a biÕt tung tİch <color=green>Lôc Hån Phiªn<color>. H·y quay vÒ gÆp <color=green>Thiªn Hïng<color>!","pt_close")
  return
 end
 local id=pt_id()
 if id<1 or id>4 then return end
 local state=EventVttQuest(id,0)
 local text="<color=yellow>§¹i Phu - V¹n Tiªn TrËn<color><enter>NhËn nhiÖm vô, tiªu diÖt ®ñ <color=red>4 Hé Ph¸p<color>, sau ®ã h¹ <color=red>Th«ng Thiªn Gi¸o Chñ<color>. HÕt thêi gian mà ch­a hoàn thành: lÇn sau ph¶i hñy nhiÖm vô cò råi nhËn l¹i."
 if state==0 then Say(text,3,"NhËn nhiÖm vô V¹n Tiªn TrËn/pt_accept","Mua thuèc/pt_shop","KÕt thóc/pt_close")
 elseif state==3 then Say(text.."<enter><color=green>§· hoàn thành. §Ó trèng F4 råi nhËn th­ëng.<color>",4,"NhËn th­ëng/pt_claim","Hñy nhiÖm vô/pt_cancel","Mua thuèc/pt_shop","KÕt thóc/pt_close")
 elseif state==4 then Say(text.."<enter><color=red>NhiÖm vô cò ®· hÕt h¹n; tiÕp tôc ®¸nh kh«ng ®­îc tİnh.<color>",3,"Hñy nhiÖm vô cò/pt_cancel","Mua thuèc/pt_shop","KÕt thóc/pt_close")
 else Say(text.."<enter>§· h¹ "..EventVttQuest(id,4).."/4 Hé Ph¸p.",3,"Hñy nhiÖm vô/pt_cancel","Mua thuèc/pt_shop","KÕt thóc/pt_close") end
end
function pt_accept()
 local result=EventVttQuest(pt_id(),1)
 if result==1 then Msg2Player("§· nhËn nhiÖm vô V¹n Tiªn TrËn. H¹ ®ñ bèn Hé Ph¸p råi tiªu diÖt Th«ng Thiªn Gi¸o Chñ.")
 elseif result==-3 then Msg2Player("CÇn hñy nhiÖm vô cò tr­íc khi nhËn l­ît míi.")
 elseif result==-5 then Msg2Player("TrËn này ®· cã Hé Ph¸p bŞ h¹. H·y nhËn nhiÖm vô tõ ®Çu l­ît trËn sau.")
 else Msg2Player("Ch­a thÓ nhËn nhiÖm vô. H·y ®øng gÇn §¹i Phu trong mét l­ît trËn ®ang ho¹t ®éng.") end
 main()
end
function pt_claim() EventVttQuest(pt_id(),3);main() end
function pt_cancel() Say("Hñy nhiÖm vô V¹n Tiªn TrËn hiÖn t¹i?",2,"§ång ı/pt_cancel_yes","Quay l¹i/main") end
function pt_cancel_yes() EventVttQuest(pt_id(),2);main() end
function pt_shop() CloseDialog();Sale(24) end
function pt_close() CloseDialog() end

function GetPlayerTaskState() return EventVttQuest(pt_id(),5),0 end
