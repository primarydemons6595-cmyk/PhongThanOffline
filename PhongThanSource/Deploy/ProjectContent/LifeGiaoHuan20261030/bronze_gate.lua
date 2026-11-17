-- General weapons shop and original delivery callback remain available.
PT_BRONZE_OLD_MAIN=main
function main()
 if GetContentWorldPos()~=52 then return PT_BRONZE_OLD_MAIN() end
 if songxin and songxin()==1 then return end
 SayButtons("Thợ Đồng Diêu Trì",3,"Mua bán vũ khí/yes","Khai Khoáng/pt_bronze_info","Đóng/no")
end
function pt_bronze_info()
 if GetContentWorldPos()~=52 then return end
 SayButtons("Đến Thanh Đồng Sơn để đào đồng thau. Cần học Bàn Cổ Khai Thiên và cầm Cuốc chim. Chọn hố Thanh Đồng Khoáng để khai thác; có cơ hội nhận thêm Hoàng đồng, Đồng đỏ hoặc Xích đồng.",3,
 "Vào Thanh Đồng Sơn/pt_bronze_enter","Mua Cuốc chim/yes","Quay lại/main")
end
function pt_bronze_enter()
 if GetContentWorldPos()~=52 or GetTrade()~=0 or GetLockState()~=0 then return end
 NewWorld(57,1608,3094);CloseDialog()
end
