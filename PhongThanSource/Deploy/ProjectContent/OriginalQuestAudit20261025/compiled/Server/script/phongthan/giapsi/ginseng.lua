Include("\\script\\phongthan\\giapsi\\herb_0.lua")
function main() GiapGather(0) end
function pt_ginseng_finish()
 local result=GiapGather(1)
 if result==1 then Msg2Player("NhËn ®­îc 1 Nguyªn S©m.");PTGJournal()
 elseif result==2 then Msg2Player("NhËn ®­îc 1 S¬n Xuyªn LiÔu.");PTGJournal() end
end
