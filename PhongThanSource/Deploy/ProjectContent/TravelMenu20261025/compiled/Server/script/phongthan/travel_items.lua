PT_TRAVEL_OPTIONS={{1,1001,"Phong ThÇn ®µi"},{1,1052,"Diªu Tr×"},{1,1020,"T©y Kú"},{1,1021,"TriÒu Ca"},{1,1002,"Sïng thµnh ®¹i doanh"},{1,1003,"Ngäc H­ cung"},{1,1004,"Xi V­u Mé"},{1,1005,"Sïng thµnh"},{1,1006,"B¾c H¶i"},{1,1007,"YÕn S¬n"},{1,1008,"Ch©n nói C«n L«n"},{1,1009,"T©y C«n L«n"},{1,1010,"Thñ D­¬ng s¬n"},{1,1011,"Du Hån"},{1,1012,"Miªu C­¬ng"},{1,1013,"Cù Léc"},{1,1014,"®ång quan"},{1,1015,"M¹nh T©n"},{1,1016,"Tam S¬n "},{1,1017,"Kú S¬n"},{1,1018,"Môc D·"},{1,1019,"TuyÖt Long lÜnh"},{1,1065,"TrÇn ®­êng"},{1,1064,"ChiÕn tr­êng ViÔn Cæ "},{1,1057,"Thanh ®ång s¬n"},{2,1022,"Hoang m¹c"},{2,1023,"Sa M¹c Thæ Thµnh"},{2,1024,"Sa M¹c Phong Than"},{2,1025,"Sa M¹c Lôc Ch©u"},{2,1026,"Sa M¹c chÕt"},{3,1027,"Hiªn Viªn ®éng tÇng 1"},{3,1028,"Hiªn Viªn ®éng tÇng 2"},{3,1029,"Hiªn Viªn ®éng tÇng 3"},{3,1030,"Hiªn Viªn ®éng tÇng 4"},{3,1031,"Hiªn Viªn ®éng tÇng 5"},{4,1032,"Ngäc TuyÒn B¨ng Xuyªn"},{4,1033,"B¨ng Xuyªn TuyÕt Cèc"},{4,1034,"®¹i Phong B¨ng Xuyªn"},{4,1035,"®¹i Tr¹ch B¨ng Xuyªn"},{4,1036,"B¨ng Xuyªn Chi Cùc"},{5,1037,"®«ng H¶i Long Vùc"},{5,1038,"®«ng H¶i Long Cung"},{5,1039,"®«ng H¶i H¶i C©u"},{5,1040,"®«ng H¶i Long Vùc"},{5,1041,"®«ng H¶i Long Uyªn"},{6,1042,"BÝch Du cung tÇng 1"},{6,1043,"BÝch Du cung tÇng 2"},{6,1044,"BÝch Du cung tÇng 3"},{6,1045,"BÝch Du cung tÇng 4"},{6,1046,"BÝch Du cung tÇng 5"},{7,1047,"Khæn Tiªn cung tÇng 1"},{7,1048,"Khæn Tiªn cung tÇng 2"},{7,1049,"Khæn Tiªn cung tÇng 3"},{7,1050,"Khæn Tiªn cung tÇng 4"},{7,1051,"Khæn Tiªn cung tÇng 5"}}

function no() CloseDialog() end
function main(item)
 if GetMorphType()==364 or IsPlayerInsideWeapon(PlayerIndex)>0 then return end
 local profile=TravelItem(0,item)
 if profile==0 then return end
 if profile==8 then TravelItem(1,0);return end
 local options={}
 local maps={1002,1003,1004,1021,1020,1024,1038,1029,1035}
 for i=1,getn(PT_TRAVEL_OPTIONS) do
  local d=PT_TRAVEL_OPTIONS[i]
  local show=nil
  if d[1]==profile then show=1 end
  if profile==9 then if d[1]>=2 and d[1]<=5 then show=1 end end
  if profile==10 then if d[1]>=6 and d[1]<=7 then show=1 end end
  if profile==11 then show=GetTask(333)>=1 and GetTask(333)<=9 and d[2]==maps[GetTask(333)] end
  if show then tinsert(options,d[3].."/pt_travel_"..i) end
 end
 if getn(options)>0 then Say(10286,getn(options),options) end
end
function pt_travel_choose(option)
 if GetMorphType()==364 or IsPlayerInsideWeapon(PlayerIndex)>0 then return end
 if TravelItem(1,option)==1 then CloseDialog() end
end
function pt_travel_1() pt_travel_choose(1) end
function pt_travel_2() pt_travel_choose(2) end
function pt_travel_3() pt_travel_choose(3) end
function pt_travel_4() pt_travel_choose(4) end
function pt_travel_5() pt_travel_choose(5) end
function pt_travel_6() pt_travel_choose(6) end
function pt_travel_7() pt_travel_choose(7) end
function pt_travel_8() pt_travel_choose(8) end
function pt_travel_9() pt_travel_choose(9) end
function pt_travel_10() pt_travel_choose(10) end
function pt_travel_11() pt_travel_choose(11) end
function pt_travel_12() pt_travel_choose(12) end
function pt_travel_13() pt_travel_choose(13) end
function pt_travel_14() pt_travel_choose(14) end
function pt_travel_15() pt_travel_choose(15) end
function pt_travel_16() pt_travel_choose(16) end
function pt_travel_17() pt_travel_choose(17) end
function pt_travel_18() pt_travel_choose(18) end
function pt_travel_19() pt_travel_choose(19) end
function pt_travel_20() pt_travel_choose(20) end
function pt_travel_21() pt_travel_choose(21) end
function pt_travel_22() pt_travel_choose(22) end
function pt_travel_23() pt_travel_choose(23) end
function pt_travel_24() pt_travel_choose(24) end
function pt_travel_25() pt_travel_choose(25) end
function pt_travel_26() pt_travel_choose(26) end
function pt_travel_27() pt_travel_choose(27) end
function pt_travel_28() pt_travel_choose(28) end
function pt_travel_29() pt_travel_choose(29) end
function pt_travel_30() pt_travel_choose(30) end
function pt_travel_31() pt_travel_choose(31) end
function pt_travel_32() pt_travel_choose(32) end
function pt_travel_33() pt_travel_choose(33) end
function pt_travel_34() pt_travel_choose(34) end
function pt_travel_35() pt_travel_choose(35) end
function pt_travel_36() pt_travel_choose(36) end
function pt_travel_37() pt_travel_choose(37) end
function pt_travel_38() pt_travel_choose(38) end
function pt_travel_39() pt_travel_choose(39) end
function pt_travel_40() pt_travel_choose(40) end
function pt_travel_41() pt_travel_choose(41) end
function pt_travel_42() pt_travel_choose(42) end
function pt_travel_43() pt_travel_choose(43) end
function pt_travel_44() pt_travel_choose(44) end
function pt_travel_45() pt_travel_choose(45) end
function pt_travel_46() pt_travel_choose(46) end
function pt_travel_47() pt_travel_choose(47) end
function pt_travel_48() pt_travel_choose(48) end
function pt_travel_49() pt_travel_choose(49) end
function pt_travel_50() pt_travel_choose(50) end
function pt_travel_51() pt_travel_choose(51) end
function pt_travel_52() pt_travel_choose(52) end
function pt_travel_53() pt_travel_choose(53) end
function pt_travel_54() pt_travel_choose(54) end
function pt_travel_55() pt_travel_choose(55) end
