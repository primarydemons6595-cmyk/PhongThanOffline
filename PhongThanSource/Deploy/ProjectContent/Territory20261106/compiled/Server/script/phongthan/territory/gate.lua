function pt_no() CloseDialog() end
function pt_result(result)
 if result==1 then Talk(1,"pt_no","§· thùc hiÖn thµnh c«ng.")
 elseif result==2 then Talk(1,"pt_no","Giao dÞch ®ang chê l­u. Kh«ng thùc hiÖn l¹i; server sÏ tù hoµn tÊt.")
 else Talk(1,"pt_no","Kh«ng thùc hiÖn ®­îc. H·y kiÓm tra quyÒn, vËt phÈm mang theo, ng©n l­îng, nh©n khÝ vµ ®iÒu kiÖn cña thµnh.") end
end
function main()
 local tasks={}
 for i=0,24 do
  local map,name,level=TerritoryCityAt(i)
  if map>0 then tinsert(tasks,{name.." - CÊp "..level,"pt_enter"..i;show=1}) end
 end
 if getn(tasks)==0 then Talk(1,"pt_no","Ch­a cã thµnh thÞ nµo ®­îc lËp.") else SayTask("Chän thµnh thÞ muèn ®Õn:",tasks) end
end
function pt_enter0() local map,name,level=TerritoryCityAt(0); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter1() local map,name,level=TerritoryCityAt(1); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter2() local map,name,level=TerritoryCityAt(2); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter3() local map,name,level=TerritoryCityAt(3); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter4() local map,name,level=TerritoryCityAt(4); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter5() local map,name,level=TerritoryCityAt(5); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter6() local map,name,level=TerritoryCityAt(6); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter7() local map,name,level=TerritoryCityAt(7); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter8() local map,name,level=TerritoryCityAt(8); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter9() local map,name,level=TerritoryCityAt(9); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter10() local map,name,level=TerritoryCityAt(10); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter11() local map,name,level=TerritoryCityAt(11); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter12() local map,name,level=TerritoryCityAt(12); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter13() local map,name,level=TerritoryCityAt(13); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter14() local map,name,level=TerritoryCityAt(14); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter15() local map,name,level=TerritoryCityAt(15); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter16() local map,name,level=TerritoryCityAt(16); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter17() local map,name,level=TerritoryCityAt(17); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter18() local map,name,level=TerritoryCityAt(18); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter19() local map,name,level=TerritoryCityAt(19); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter20() local map,name,level=TerritoryCityAt(20); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter21() local map,name,level=TerritoryCityAt(21); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter22() local map,name,level=TerritoryCityAt(22); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter23() local map,name,level=TerritoryCityAt(23); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
function pt_enter24() local map,name,level=TerritoryCityAt(24); if TerritoryAction(1,map)~=1 then Talk(1,"pt_no","Kh«ng thÓ vµo thµnh. H·y ®øng gÇn cæng thµnh ë Phong ThÇn §µi vµ thö l¹i.") end end
