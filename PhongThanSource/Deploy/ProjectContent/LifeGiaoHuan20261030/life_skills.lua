-- Nine-profession runtime. Recipe costs, ingredients and probabilities are
-- compiled from the existing tables; learning/site policy is authored for Loose.
function pt_ls(k,v) return PtLifeState(k,v) end
function pt_life_ok() return LifeBusy()==0 end
function pt_life_close() CloseDialog() end
function pt_life_notice(text) SayButtons(text,1,"Quay lại/pt_life_current") end
function pt_life_current() return pt_life_open(pt_ls(5145)) end
function pt_life_main()
 if LifeTrainerNear(1)==1 then
  SayButtons("Thu Thập, Khai Khoáng là kỹ năng cần thiết để thu thập thảo dược và khoáng vật. Khi tăng cấp nhân vật và đạt đủ độ thuần thục, bạn có thể trở lại nâng nghề.",4,
   "Thu Thập/pt_life_skill1","Khai Khoáng/pt_life_skill3","Sinh Hoạt Sư/pt_life_other_teachers","Kết thúc/pt_life_close")
 elseif LifeTrainerNear(4)==1 then
  SayButtons("Sau khi học Câu Cá, bạn có thể câu cá ở các nơi thủy vực; chế biến cá tôm thành nguyên liệu, rồi Nấu Nướng thành thức ăn.",4,
   "Câu Cá/pt_life_skill4","Chế Biến Thức Ăn/pt_life_skill8","Nấu Nướng/pt_life_skill6","Kết thúc/pt_life_close")
 elseif LifeTrainerNear(10)==1 then
  SayButtons("Gia Công Thuốc và Khoáng Vật biến sản vật sơ cấp thành nguyên liệu. Chế Luyện, Luyện Đơn dùng nguyên liệu đã gia công tạo ra thành phẩm.",5,
   "Gia Công Thuốc/pt_life_skill10","Gia Công Khoáng Vật/pt_life_skill9","Chế Luyện/pt_life_skill7","Luyện Đơn/pt_life_skill5","Kết thúc/pt_life_close")
 else
  SayButtons("Chọn nhóm nghề để xem công thức hoặc địa điểm lấy nguyên liệu.",4,
   "Thu thập nguyên liệu/pt_life_raw","Gia công nguyên liệu/pt_life_process","Chế tạo vật phẩm/pt_life_craft","Đóng/pt_life_close")
 end
end
function pt_life_other_teachers() SayButtons("Sinh Hoạt Sư Triều Ca dạy Câu Cá, Chế Biến Thức Ăn và Nấu Nướng. Sinh Hoạt Sư Tây Kỳ dạy Gia Công Thuốc, Gia Công Khoáng Vật, Chế Luyện và Luyện Đơn.",1,"Quay lại/main") end
function pt_life_raw() SayButtons("Thu thập nguyên liệu",4,"Thu Thập/pt_life_skill1","Khai Khoáng/pt_life_skill3","Câu Cá/pt_life_skill4","Quay lại/main") end
function pt_life_process() SayButtons("Gia công nguyên liệu",4,"Dược Liệu/pt_life_skill10","Khoáng Vật/pt_life_skill9","Thực Phẩm/pt_life_skill8","Quay lại/main") end
function pt_life_craft() SayButtons("Chế tạo vật phẩm",4,"Luyện Đan/pt_life_skill5","Nấu Ăn/pt_life_skill6","Luyện Chế/pt_life_skill7","Quay lại/main") end
function pt_life_skill1() pt_life_open(1) end
function pt_life_skill3() pt_life_open(3) end
function pt_life_skill4() pt_life_open(4) end
function pt_life_skill5() pt_life_open(5) end
function pt_life_skill6() pt_life_open(6) end
function pt_life_skill7() pt_life_open(7) end
function pt_life_skill8() pt_life_open(8) end
function pt_life_skill9() pt_life_open(9) end
function pt_life_skill10() pt_life_open(10) end
function pt_life_open(skill)
 if not PT_LIFE_NAMES[skill] then return end
 pt_ls(5145,skill);pt_ls(5144,0)
 local rank=GetLiveSkillLevel(skill)
 local text="<color=yellow>"..PT_LIFE_NAMES[skill].."<color><enter>Cấp nghề: "..rank.."/10; thuần thục: "..GetLiveSkillProficiency(skill).."/10000.<enter>Tinh lực: "..pt_ls(5141).."; Thể lực: "..pt_ls(5142).."."
 local options={}
 if LifeTrainerNear(skill)==1 then tinsert(options,"Học, nâng nghề/pt_life_learn_preview") end
 if skill==1 or skill==3 or skill==4 then tinsert(options,"Nơi lấy nguyên liệu/pt_life_locate")
 else tinsert(options,"Công thức/pt_life_page0") end
 tinsert(options,"Chọn nghề khác/main");tinsert(options,"Đóng/pt_life_close")
 SayButtons(text,getn(options),options)
end
function pt_life_learn_preview()
 local skill=pt_ls(5145);local rank=GetLiveSkillLevel(skill)
 if not PT_LIFE_NAMES[skill] or not pt_life_ok() or LifeTrainerNear(skill)~=1 then return end
 if rank>=10 then pt_life_notice("Nghề đã đạt cấp tối đa.");return end
 local need=(rank+2)*10;if rank==9 then need=120 end
 if GetLevel()<need then pt_life_notice("Cần nhân vật cấp "..need..".");return end
 if rank>0 and GetLiveSkillProficiency(skill)<10000 then pt_life_notice("Cần đạt 10000 độ thuần thục trước khi nâng cấp.");return end
 local cost=rank==0 and 1000 or 10000*2^rank
 pt_ls(5146,rank)
 SayButtons("Học "..PT_LIFE_NAMES[skill].." cấp "..(rank+1).." cần "..cost.." lượng.",2,"Xác nhận/pt_life_learn","Quay lại/pt_life_current")
end
function pt_life_learn()
 local skill=pt_ls(5145);local rank=GetLiveSkillLevel(skill)
 if not PT_LIFE_NAMES[skill] or rank~=pt_ls(5146) or rank>=10 or not pt_life_ok() or LifeTrainerNear(skill)~=1 then return end
 local need=(rank+2)*10;if rank==9 then need=120 end
 if GetLevel()<need or (rank>0 and GetLiveSkillProficiency(skill)<10000) then return end
 local cost=rank==0 and 1000 or 10000*2^rank
 if GetCash()<cost then pt_life_notice("Không đủ lượng.");return end
 if Pay(cost)~=1 then return end
 if AddLiveSkill(skill,rank+1)~=1 then Earn(cost);return end
 pt_ls(5146,-1);pt_life_notice("Đã học "..PT_LIFE_NAMES[skill].." cấp "..(rank+1)..".")
end
function pt_life_locate()
 local skill=pt_ls(5145);local rank=GetLiveSkillLevel(skill);local text="Nguyên liệu có thể thu thập:"
 local count=0
 for grade=rank,1,-1 do
  for i=1,getn(PT_LIFE_LOCATIONS) do local n=PT_LIFE_LOCATIONS[i]
   if n.skill==skill and n.rank==grade and count<4 then
    text=text.."<enter>"..n.name.." - "..PT_LIFE_MAP_NAMES[n.map].." ("..floor(n.x/256)..","..floor(n.y/512)..")";count=count+1
   end
  end
 end
 if count==0 then text="Hãy học nghề tại Sinh Hoạt Sư trước. Bấm vào cây, mỏ hoặc bãi cá tại địa điểm được chỉ dẫn để thu thập." end
 pt_life_notice(text)
end
function pt_life_known(r)
 return r.default==1 or pt_ls(5200+r.id)>0
end
function pt_life_page(page)
 local skill=pt_ls(5145);local rank=GetLiveSkillLevel(skill);local list={}
 for id=1,128 do local r=PT_LIFE_RECIPES[id]
  if r and r.skill==skill and r.rank<=rank and pt_life_known(r) then tinsert(list,id) end
 end
 if page<0 then page=0 end
 if page*3>=getn(list) then page=0 end
 local options={}
 for i=page*3+1,min(page*3+3,getn(list)) do local r=PT_LIFE_RECIPES[list[i]]
  tinsert(options,strsub(r.name,1,17).."/pt_life_recipe"..r.id)
 end
 if page>0 then tinsert(options,"Trang trước/pt_life_page"..(page-1)) end
 if page*3+3<getn(list) then tinsert(options,"Trang sau/pt_life_page"..(page+1)) end
 tinsert(options,"Quay lại/pt_life_current")
 local text="<color=yellow>Công thức "..PT_LIFE_NAMES[skill].."<color>"
 if getn(list)==0 then text=text.."<enter>Chưa có công thức phù hợp. Cần học nghề, nâng cấp hoặc đọc sách công thức." end
 SayButtons(text,getn(options),options)
end
function pt_life_recipe(id)
 local r=PT_LIFE_RECIPES[id]
 if not r or pt_ls(5145)~=r.skill or GetLiveSkillLevel(r.skill)<r.rank or not pt_life_known(r) then return end
 local chance=min(100,r.chance+(GetLiveSkillLevel(r.skill)-r.rank)*r.bonus)
 local text="<color=yellow>"..r.name.."<color><enter>Nguyên liệu:"
 for i=1,getn(r.needs) do local t=r.needs[i];text=text.."<enter>"..PT_LIFE_ITEM_NAMES[t[1]..":"..t[2]].." x"..t[7] end
 text=text.."<enter>Thể lực: "..r.strength.."; Tinh lực: "..r.energy.."; Lượng: "..r.cost..".<enter>Thành công: "..chance.."%. Thất bại vẫn mất nguyên liệu và tài nguyên."
 pt_ls(5144,id)
 SayButtons(text,2,"Chế tạo/pt_life_make"..id,"Quay lại/pt_life_page0")
end
function pt_life_make(id)
 local r=PT_LIFE_RECIPES[id]
 if not r or pt_ls(5144)~=id or pt_ls(5145)~=r.skill or not pt_life_ok() then return end
 local rank=GetLiveSkillLevel(r.skill)
 if rank<r.rank or not pt_life_known(r) then return end
 local energy=pt_ls(5141);local strength=pt_ls(5142)
 if energy<r.energy or strength<r.strength then pt_life_notice("Không đủ tinh lực hoặc thể lực.");return end
 if GetCash()<r.cost then pt_life_notice("Không đủ lượng.");return end
 local outputs={};local success=RANDOM(1,100)<=min(100,r.chance+(rank-r.rank)*r.bonus)
 if success then
  local roll=RANDOM(1,100);local sum=0
  for i=1,getn(r.outputs) do local out=r.outputs[i];sum=sum+out.chance
   if roll<=sum and getn(outputs)==0 then tinsert(outputs,out.item) end
  end
 end
 if r.cost>0 and Pay(r.cost)~=1 then return end
 local serial=pt_ls(5140);local next=serial==0 and 1 or 0
 if QuestExchange(5140,serial,next,r.needs,outputs,1)~=1 then
  if r.cost>0 then Earn(r.cost) end
  pt_life_notice("Chưa đủ nguyên liệu hoặc túi không đủ chỗ. Giao dịch đã được giữ nguyên.");return
 end
 pt_ls(5141,energy-r.energy);pt_ls(5142,strength-r.strength);pt_ls(5144,0)
 if r.default==0 then pt_ls(5200+id,pt_ls(5200+id)-1) end
 local gain=PT_LIFE_GROWTH[id][rank] or 0
 pt_ls(5120+r.skill,min(10000,GetLiveSkillProficiency(r.skill)+gain))
 if success then pt_life_notice("Chế tạo thành công: "..r.name..".") else pt_life_notice("Chế tạo thất bại. Đã tiêu hao nguyên liệu và tài nguyên theo công thức.") end
end
