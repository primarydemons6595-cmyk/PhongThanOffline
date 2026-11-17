function main() Say("§¹i Phu",3,"Cöa hµng thuèc/pt_doctor_shop","TrÞ th­¬ng/pt_doctor_heal","KÕt thóc/pt_doctor_close") end
function pt_doctor_shop() CloseDialog();Sale(23) end
function pt_doctor_heal()
 if SystemTime()-GetTask(763)<1800 then Talk(1,"pt_doctor_close","Ta ch­a håi phôc nguyªn khÝ. H·y quay l¹i sau.");return end
 RestoreLife();RestoreMana();SetTask(763,SystemTime());CloseDialog();Msg2Player("Sinh lùc vµ néi lùc ®· hoµn toµn håi phôc.")
end
function pt_doctor_close() CloseDialog() end
