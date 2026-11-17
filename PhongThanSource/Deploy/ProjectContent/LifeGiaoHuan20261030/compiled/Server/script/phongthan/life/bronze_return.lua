function main()
 if GetContentWorldPos()~=57 then return end
 SayButtons("§iÓm chuyÓn tiÕp Thanh §ång S¬n",2,"VÒ Diªu Tr×/pt_bronze_return","§ãng/no")
end
function pt_bronze_return()
 if GetContentWorldPos()~=57 or GetTrade()~=0 or GetLockState()~=0 then return end
 NewWorld(52,1632,3067);CloseDialog()
end
function no() CloseDialog() end
