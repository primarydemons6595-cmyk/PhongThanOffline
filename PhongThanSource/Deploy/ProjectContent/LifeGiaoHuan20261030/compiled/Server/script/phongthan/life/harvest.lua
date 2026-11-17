function main()
 local result=LifeGather(0)
 if result==1 then return end
 local text="Ch­a thÓ thu thËp t¹i ®©y."
 if result==-2 then text="Ch­a häc nghÒ hoÆc cÊp nghÒ ch­a ®ñ. H·y ®Õn Sinh Ho¹t S­."
 elseif result==-3 then text="Kh«ng ®ñ thÓ lùc ®Ó thu thËp."
 elseif result==-4 then text="H·y ®øng gÇn nguyªn liÖu h¬n."
 elseif result==-5 then text="Ch­a thÓ thu thËp khi ®ang bËn, giao dŞch hoÆc khãa hµnh trang." end
 if result==-7 then text="§µo ®ång thau cÇn häc Bµn Cæ Khai Thiªn vµ cÇm Cuèc chim trong « vò khİ." end
 Talk(1,"no",text)
end
function pt_life_gather_finish()
 local result=LifeGather(1)
 if result==1 then Msg2Player("Thu thËp thµnh c«ng. Nguyªn liÖu ®· vµo tói.")
 elseif result==-6 then Msg2Player("Tói kh«ng ®ñ chç; nguyªn liÖu vµ thÓ lùc ch­a bŞ tiªu hao.") end
end
function no() CloseDialog() end
