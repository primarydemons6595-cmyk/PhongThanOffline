function main()
 if GetContentWorldPos()~=57 then return end
 SayButtons("Điểm chuyển tiếp Thanh Đồng Sơn",2,"Về Diêu Trì/pt_bronze_return","Đóng/no")
end
function pt_bronze_return()
 if GetContentWorldPos()~=57 or GetTrade()~=0 or GetLockState()~=0 then return end
 NewWorld(52,1632,3067);CloseDialog()
end
function no() CloseDialog() end
