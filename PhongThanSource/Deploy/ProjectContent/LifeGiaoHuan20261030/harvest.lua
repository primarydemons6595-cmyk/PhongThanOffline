function main()
 local result=LifeGather(0)
 if result==1 then return end
 local text="Chưa thể thu thập tại đây."
 if result==-2 then text="Chưa học nghề hoặc cấp nghề chưa đủ. Hãy đến Sinh Hoạt Sư."
 elseif result==-3 then text="Không đủ thể lực để thu thập."
 elseif result==-4 then text="Hãy đứng gần nguyên liệu hơn."
 elseif result==-5 then text="Chưa thể thu thập khi đang bận, giao dịch hoặc khóa hành trang." end
 if result==-7 then text="Đào đồng thau cần học Bàn Cổ Khai Thiên và cầm Cuốc chim trong ô vũ khí." end
 if result==-8 then text="Cần trang bị Cần câu trong ô vũ khí. Có thể đứng trên bờ để câu dưới nước." end
 Talk(1,"no",text)
end
function pt_life_gather_finish()
 local result=LifeGather(1)
 if result==1 then Msg2Player("Thu thập thành công. Nguyên liệu đã vào túi.")
 elseif result==-6 then Msg2Player("Túi không đủ chỗ; nguyên liệu và thể lực chưa bị tiêu hao.") end
end
function no() CloseDialog() end
