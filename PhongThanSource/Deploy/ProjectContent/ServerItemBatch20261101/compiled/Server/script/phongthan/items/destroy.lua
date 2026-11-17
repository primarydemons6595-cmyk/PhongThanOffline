function main()
 if DestroyItems(0)~=1 then return end
 GiveItemUI("Tiªu Hñy §å","§Æt nh÷ng mãn kh«ng cÇn vµo ®©y. NhÊn X¸c nhËn sÏ tiªu hñy vÜnh viÔn, kÓ c¶ ®å khãa/b¶o hiÓm vÜnh viÔn. NhÊn Hñy ®Ó nhËn l¹i ®å.","pt_destroy_finish")
end
function pt_destroy_finish()
 local count=DestroyItems(1)
 if count>0 then Msg2Player("§· tiªu hñy "..count.." vËt phÈm.") end
end
