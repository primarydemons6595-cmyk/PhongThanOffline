from pathlib import Path
import sys
sys.path.insert(0,'PhongThanSource/Tools')
from extract_client_coordinate_dictionary import encode_tcvn3
def enc(text):
 out=bytearray()
 for c in text:
  try:out.extend(encode_tcvn3(c))
  except KeyError:out.extend(encode_tcvn3(c.lower()))
 return bytes(out)
ROOT=Path(__file__).resolve().parents[2]
import os
os.chdir(ROOT)
B=ROOT/'analysis/realm_equipment_20261027';S=B/'compiled';R=ROOT/'PhongThanRuntime-Loose'
texts=['',
 'Đến gặp <color=green>Đắc Kỷ lúc nhỏ<color>, Ngọc Hư 10 năm trước (208,203).',
 'Quay lại gặp <color=green>Khương Tử Nha lúc trẻ<color> (198,201) để nhận thí luyện.',
 'Đánh bại <color=red>Dương Tiễn<color> tại Ải Trần Đường.',
 'Trở về <color=green>Khương Tử Nha lúc trẻ<color> (198,201), nhận <color=yellow>Đồ phổ Bạch Liên pháp khí<color> và 2.000 vạn kinh nghiệm.',
 '',
 'Lần lượt đối thoại với <color=green>Xích Tinh Tử<color>, Ngọc Hư Cung (213,197) và <color=green>Cao Minh<color>, Xi Vưu Mộ (206,198).',
 'Chọn một tiếp dẫn sư: <color=green>Xích Tinh Tử<color> hoặc <color=green>Cao Minh<color> để nhận thử thách.',
 'Tiêu diệt <color=red>Giao Long<color> tại Đông Doanh rồi trở về tiếp dẫn sư đã chọn.',
 'Trở về tiếp dẫn sư đã chọn để hoàn thành <color=yellow>Cửu Lai<color>.',
 '',
 'Qua cổng ở Diêu Trì (200,196), đến gặp <color=green>Huyền Đô Đại Pháp Sư<color> tại Bất Chu Thiên Quan (205,242).',
 'Quay về tiếp dẫn sư đã chọn để phục mệnh Thăng Tiên Nhập Ma.',
 '']
header=['#ifndef PT_REALM_QUEST_TEXT_H','#define PT_REALM_QUEST_TEXT_H','static const char* PT_REALM_STEPS[]={']
for t in texts:header.append('"'+''.join('\\x%02x'%v for v in enc(t))+'",')
header+=['};']
for key,text in {
 'PT_REALM_RESCUED':'Linh Quang đạo nhân đã được giải thoát và xuất hiện tại Diêu Trì (196,203). Các kỳ sĩ cùng hoàn thành 1.500 nhiệm vụ Danh vọng Nhân Giới để mở thiên lộ!',
 'PT_REALM_GATE_OPEN':'Cổng Bất Chu Thiên Quan tại Diêu Trì (200,196) đã mở! Kỳ sĩ hoàn thành Thăng Tiên Nhập Ma có thể bước vào Tiên Ma Giới.',
 'PT_REALM_CIRCLE_NAME':'Tụ Hồn Trận','PT_REALM_TIEN_SOUL':'Tiên Hồn','PT_REALM_MA_SOUL':'Ma Phách',
 'PT_REALM_LINH_QUANG':'Linh Quang đạo nhân','PT_REALM_PORTAL':'Cổng Bất Chu Thiên Quan',
 'PT_REALM_BOAT_NAME':'Thuyền Phu','PT_REALM_VAN_NAME':'Vân Tiêu',
 'PT_REALM_REP_PROGRESS':'Khổn Tiên cung tầng 1, gần <color=green>Đại Phu<color> phía đông bắc (%d,%d). Dùng <color=yellow>pháp khí gọi hồn<color> ngoài khu an toàn, dẫn quái đến gần trận và tiêu diệt: %d/5 hồn. Sau đó dùng <color=yellow>vật phẩm dẫn hồn<color>, dẫn hồn về Đại Phu rồi trở về tiếp dẫn sư.',
 'PT_REALM_REP_RETURN':'Đã đưa hồn về Đại Phu. Trở lại tiếp dẫn sư để phục mệnh.',
 'PT_REALM_LURE_STEPS':'Gặp <color=green>Thuyền Phu<color>, Đông Doanh (223,197), giao <color=yellow>10 Chấn Thiên Tiễn<color> để dụ <color=red>Giao Long<color>. Tiêu diệt Giao Long đã dụ rồi trở lại Thuyền Phu hoặc <color=green>Vân Tiêu<color>, Diêu Trì (195,203) nhận <color=yellow>Râu Thần Long<color>.',
}.items():header.append('static const char '+key+'[]="'+''.join('\\x%02x'%v for v in enc(text))+'";')
header+=['#endif'];Path('PhongThanSource/Headers/PhongThanRealmQuestText.h').write_text('\n'.join(header)+'\n','ascii')
lua=r'''
-- PT_REALM_PERSONAL_BEGIN: reconstructed personal VNG progression only.
PT_REALM_OLD_MAIN=main
PT_REALM_OLD_MARK=GetPlayerTaskState
function ptrq_op(s,r,c)
 if r==1 then
  local mainTask=3
  if GetPlayerType()==1 then mainTask=1 elseif GetPlayerType()==2 then mainTask=2 end
  if s==0 and GetLevel()>=100 and GetTask(mainTask)>=90 then return 1,"Phong Hồi Lộ Chuyển" end
  if s==2 then return 3,"Thí luyện Dương Tiễn" end
  if s==4 then return 4,"Phục mệnh" end
  if s==5 and GetLevel()>=110 then return 5,"Cửu Lai" end
 elseif r==2 and s==1 then return 2,"Phong Hồi Lộ Chuyển"
 elseif r==3 or r==4 then
  if s==6 then return 6,"Tìm hiểu Tiên Ma Giới" end
  if s==7 then return 7,"Thử thách Cửu Lai" end
  if r==c+2 then
   if s==9 then return 8,"Phục mệnh Cửu Lai" end
   if s==10 and GetLevel()>=121 then return 9,"Thăng Tiên Nhập Ma" end
   if s==12 then return 11,"Phục mệnh" end
  end
 elseif r==5 and s==11 then return 10,"Tìm hiểu Tiên Ma Giới"
 elseif r==6 and s>=11 then return 12,"Đến Bất Chu Thiên Quan" end
 return 0,""
end
function ptrq_show(text,t)
 local n=getn(t)
 if n==1 then SayButtons(text,1,t[1])
 elseif n==2 then SayButtons(text,2,t[1],t[2])
 elseif n==3 then SayButtons(text,3,t[1],t[2],t[3])
 elseif n==4 then SayButtons(text,4,t[1],t[2],t[3],t[4])
 elseif n==5 then SayButtons(text,5,t[1],t[2],t[3],t[4],t[5])
 elseif n==6 then SayButtons(text,6,t[1],t[2],t[3],t[4],t[5],t[6])
 else Talk(1,"ptrq_close",text) end
end
function main()
 local ok,s,r,c=PtRealmQuest(0)
 if ok~=1 then PT_REALM_OLD_MAIN();return end
 local op,label=ptrq_op(s,r,c)
 local valid,world,days,reputation,mask,registered=PtRealmWorld(0)
 if op==12 and world~=3 then op=0 end
 local t={}
 if op>0 then tinsert(t,label.."/ptrq_preview") end
 if r==3 or r==4 then
  local ready,state,runs,stage,missing=PtRealmReputation(0)
  if ready==1 and stage>=10 then
   if state==0 and runs<4 then tinsert(t,"Danh vọng Nhân Giới/ptrq_rep")
   elseif state==(r-2)*10+5 then tinsert(t,"Phục mệnh danh vọng/ptrq_rep") end
   if state>0 and missing==1 then tinsert(t,"Nhận lại pháp khí/ptrq_reissue") end
  end
 elseif r==7 and (world==1 or world==2) and registered==0 and GetLevel()>=121 then tinsert(t,"Ghi danh mở cổng/ptrq_register")
 elseif r>=8 then
  local ready,lure,whisker=PtRealmIsland(0)
  if r==8 and ready==1 and (lure==0 or lure==4) and whisker==0 and GetLevel()>=120 then tinsert(t,"Thu phục Giao Long/ptrq_lure_start") end
  if (r==8 or r==10) and lure==3 then tinsert(t,"Nhận Râu Thần Long/ptrq_whisker") end
  if r==9 then tinsert(t,"Đông Doanh đảo/ptrq_boat_east") end
  if r==10 then
   tinsert(t,"Phương Trượng/ptrq_boat_phuong")
   tinsert(t,"Về Trần Đường/ptrq_boat_back")
   if lure==1 then tinsert(t,"Dụ Giao Long/ptrq_lure") end
  end
 end
 if r>=3 then tinsert(t,"Tiến độ mở cổng/ptrq_world") end
 if getn(t)==0 then PT_REALM_OLD_MAIN();return end
 if PT_REALM_HAS_OLD~=0 then tinsert(t,"Trò chuyện/ptrq_old") end
 ptrq_show(PT_REALM_PROMPT or "Tiên Nhân Ma tam giới. Ngươi có điều gì muốn hỏi?",t)
end
function ptrq_old() PT_REALM_OLD_MAIN() end
function ptrq_preview()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok~=1 or op==0 then CloseDialog();return end
 local text=""
 if op==1 then text="Ta nhờ ngươi đến gặp Đắc Kỷ lúc nhỏ tại Ngọc Hư Cung 10 năm trước (208,203)."
 elseif op==2 then text="Hãy quay lại Khương Tử Nha lúc trẻ để nhận nhiệm vụ thí luyện Dương Tiễn."
 elseif op==3 then text="Đến Ải Trần Đường thử sức với <color=red>Dương Tiễn<color>, chiến thắng rồi trở về phục mệnh."
 elseif op==4 then text="Nhận <color=yellow>Đồ phổ Bạch Liên pháp khí<color> và <color=yellow>2.000 vạn kinh nghiệm<color>."
 elseif op==5 then text="Lần lượt đến Ngọc Hư Cung (213,197) gặp Xích Tinh Tử và Xi Vưu Mộ (206,198) gặp Cao Minh để tìm hiểu Tiên Ma Giới."
 elseif op==6 then text="Tiên Ma khác lối. Sau khi gặp cả hai vị tiếp dẫn sư, ngươi hãy chọn một vị để tiếp nhận thử thách Cửu Lai."
 elseif op==7 then text="Đến Đông Doanh tiêu diệt <color=red>Giao Long<color>, sau đó trở về gặp ta phục mệnh."
 elseif op==8 then text="Ngươi đã vượt qua thử thách Cửu Lai. Khi đạt cấp 121 có thể tiếp tục Thăng Tiên Nhập Ma."
 elseif op==9 then text="Qua cổng tại Diêu Trì (200,196), đến Bất Chu Thiên Quan gặp Huyền Đô Đại Pháp Sư (205,242), rồi quay về phục mệnh."
 elseif op==10 then text="Sau khi tìm hiểu Tiên Ma Giới, hãy trở lại vị tiếp dẫn sư của mình để phục mệnh."
 elseif op==11 then text="Hoàn thành Thăng Tiên Nhập Ma, nhận cấp Tiên Ma 1 và một <color=yellow>vũ khí Tiên Ma<color> theo hệ phái."
 elseif op==12 then text="Đến Bất Chu Thiên Quan?" end
 MsgBox(text,"ptrq_confirm","ptrq_close")
end
function ptrq_confirm()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok~=1 or op==0 then CloseDialog();return end
 local result=PtRealmQuest(op)
 if result~=1 then Talk(1,"main","Chưa thể thực hiện. Kiểm tra điều kiện nhiệm vụ và chỗ trống trong F4.");return end
 if op==12 then CloseDialog();return end
 main()
end
function ptrq_close() CloseDialog() end
function ptrq_rep()
 local ok,state,runs,stage=PtRealmReputation(0)
 if ok~=1 or stage<10 then CloseDialog();return end
 if state==0 and runs<4 then
  local text="Danh vọng Nhân Giới: đã nhận <color=red>"..runs.."/4 lượt<color> hôm nay.<enter>Nhận lượt <color=red>"..(runs+1).."<color>. "
  if runs==0 then text=text.."Lượt đầu không tốn tiền đồng." else text=text.."Tự trừ <color=yellow>"..runs.." tiền đồng<color> trong túi F4." end
  MsgBox(text,"ptrq_rep_start","ptrq_close")
 elseif state==15 or state==25 then MsgBox("Đã dẫn hồn về Đại Phu. Phục mệnh nhiệm vụ?","ptrq_rep_finish","ptrq_close")
 else Talk(1,"main","Đến Khổn Tiên cung tầng 1, ngoài khu an toàn gần Đại Phu phía đông bắc. Dùng pháp khí gọi hồn để hiện trận; dụ quái đến gần trận và tiêu diệt đủ 5 hồn. Dùng vật phẩm dẫn hồn đưa hồn về Đại Phu, rồi trở về phục mệnh. Mỗi giai đoạn có thời hạn 5 phút.") end
end
function ptrq_rep_start() if PtRealmReputation(1)~=1 then Talk(1,"main","Chưa thể nhận. Kiểm tra điều kiện, số Xu và chỗ trống trong F4.");return end main() end
function ptrq_rep_finish() if PtRealmReputation(2)~=1 then Talk(1,"main","Chưa thể phục mệnh. Hãy kiểm tra tiến độ nhiệm vụ.");return end main() end
function ptrq_reissue() if PtRealmReputation(3)~=1 then Talk(1,"main","Chưa thể bổ sung pháp khí. Kiểm tra chỗ trống trong F4.");return end main() end
function ptrq_world()
 local ok,phase,days,reputation,final,registered,mask=PtRealmWorld(0)
 if ok~=1 then Talk(1,"main","Chưa đọc được tiến độ mở cổng. Hãy thử lại sau.");return end
 local text=""
 if phase==0 then text="Giải cứu Linh Quang đạo nhân: mỗi ngày tiêu diệt Giao Long ở Đông Doanh, Di Long ở Phương Trượng và Ly Long ở Bồng Lai, liên tiếp trong 3 ngày. Đã hoàn thành "..days.."/3 ngày liên tiếp."
 elseif phase==1 then text="Linh Quang đạo nhân đã được giải thoát, tại Diêu Trì (196,203). Cùng hoàn thành nhiệm vụ Danh vọng Nhân Giới: "..reputation.."/1500 lượt. Nhân vật cấp 121 trở lên có thể ghi danh mở cổng."
 elseif phase==2 then text="Đã đủ 1500 nhiệm vụ Danh vọng Nhân Giới. Các kỳ sĩ đã ghi danh cần tiêu diệt Giao Long, Di Long và Ly Long trong cùng một ngày để mở cổng."
 else text="Cổng Bất Chu Thiên Quan đã mở tại Diêu Trì (200,196). Hãy hoàn thành chuỗi nhiệm vụ cá nhân Thăng Tiên Nhập Ma để bước vào Tiên Ma Giới." end
 if phase==0 or phase==2 then
  local m=mask;if phase==2 then m=final end
  local ly,di,giao="chưa","chưa","chưa"
  if m>=4 then ly="đã hạ";m=m-4 end
  if m>=2 then di="đã hạ";m=m-2 end
  if m>=1 then giao="đã hạ" end
  text=text.."<enter>Hôm nay: Giao Long "..giao..", Di Long "..di..", Ly Long "..ly.."."
 end
 Talk(1,"main",text)
end
function ptrq_register() MsgBox("Ghi danh tham gia mở cổng Bất Chu Thiên Quan?","ptrq_register_yes","ptrq_close") end
function ptrq_register_yes() if PtRealmWorld(1)~=1 then Talk(1,"main","Chưa thể ghi danh. Kiểm tra cấp độ và tiến độ mở cổng.");return end main() end
function ptrq_lure_start() MsgBox("Mang 10 <color=yellow>Chấn Thiên Tiễn<color> đến Thuyền Phu tại Đông Doanh (223,197) để dụ Giao Long. Tiêu diệt nó rồi nhận <color=yellow>Râu Thần Long<color>.","ptrq_lure_start_yes","ptrq_close") end
function ptrq_lure_start_yes() if PtRealmIsland(1)~=1 then Talk(1,"main","Chưa thể nhận nhiệm vụ. Kiểm tra điều kiện.");return end main() end
function ptrq_lure() MsgBox("Dùng 10 <color=yellow>Chấn Thiên Tiễn<color> để dụ Giao Long?","ptrq_lure_yes","ptrq_close") end
function ptrq_lure_yes() if PtRealmIsland(2)~=1 then Talk(1,"main","Cần đủ 10 Chấn Thiên Tiễn và đã nhận lời Vân Tiêu.");return end main() end
function ptrq_whisker() if PtRealmIsland(3)~=1 then Talk(1,"main","Chưa thể nhận Râu Thần Long. Kiểm tra chỗ trống trong F4.");return end main() end
function ptrq_boat_east() MsgBox("Đến Đông Doanh đảo, phí <color=yellow>200 vạn lượng<color>. Đồng ý lên thuyền?","ptrq_boat_east_yes","ptrq_close") end
function ptrq_boat_east_yes() if PtRealmIsland(4)~=1 then Talk(1,"main","Chưa thể lên thuyền. Cần đủ 200 vạn lượng.");return end CloseDialog() end
function ptrq_boat_phuong() MsgBox("Đến Phương Trượng (Vân Long đảo) cần cấp <color=yellow>120<color> và có <color=yellow>Râu Thần Long<color>. Đồng ý lên thuyền?","ptrq_boat_phuong_yes","ptrq_close") end
function ptrq_boat_phuong_yes() if PtRealmIsland(5)~=1 then Talk(1,"main","Cần cấp 120 trở lên và có Râu Thần Long trong hành trang.");return end CloseDialog() end
function ptrq_boat_back() if PtRealmIsland(6)~=1 then Talk(1,"main","Chưa thể rời đảo, hãy thử lại sau.");return end CloseDialog() end
function GetPlayerTaskState()
 local ok,s,r,c=PtRealmQuest(0)
 local op,label=ptrq_op(s,r,c)
 if ok==1 and op>0 then
  if op==4 or op==8 or op==11 then return 3,0 end
  return 1,0
 end
 if r==3 or r==4 then
  local ready,state,runs,stage=PtRealmReputation(0)
  if ready==1 and stage>=10 then
   if state==(r-2)*10+5 then return 3,110 end
   if state==0 and runs<4 then return 1,110 end
  end
 elseif r==7 then
  local ready,phase,days,reputation,mask,registered=PtRealmWorld(0)
  if ready==1 and (phase==1 or phase==2) and registered==0 and GetLevel()>=121 then return 1,121 end
 elseif r==8 or r==10 then
  local ready,lure,whisker=PtRealmIsland(0)
  if ready==1 and lure==3 then return 3,120 end
  if r==8 and ready==1 and (lure==0 or lure==4) and whisker==0 then return 1,120 end
 end
 if PT_REALM_OLD_MARK then return PT_REALM_OLD_MARK() end
 return 0,0
end
'''
paths=['script/phongthan/giapsi/npc_khuong_young.lua','script/phongthan/giapsi/npc_dac_young.lua',
 'script/'+bytes.fromhex('d3f1d0e9b9ac5cb3e0beabd7d32e6c7561').decode('latin1').replace('\\','/'),
 'script/phongthan/npc_restore/1004_07.lua','script/phongthan/npc_restore/1073_01.lua','script/phongthan/npc_restore/1052_04.lua',
 'script/'+bytes.fromhex('d1feb3d85cd4c6cff6c4efc4ef2e6c7561').decode('latin1').replace('\\','/')]
for path in paths:
 data=(R/'Server'/path).read_bytes();data=data.split(b'-- PT_REALM_PERSONAL_BEGIN')[0]
 p=S/'Server'/path;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data+b'\n'+enc(lua))
for file in ['linh_quang','portal','boat_tran_duong','boat_dong_doanh']:
 p=S/'Server/script/phongthan/realm'/(file+'.lua');p.parent.mkdir(parents=True,exist_ok=True)
 base='PT_REALM_HAS_OLD=0\nfunction main() CloseDialog() end\n'
 if file.startswith('boat_'):base+='PT_REALM_PROMPT=11125\n'
 p.write_bytes(enc(base+lua))
p=S/'Client/Ui/ui4/taskinfo.ini';p.parent.mkdir(parents=True,exist_ok=True);data=(R/'Client/Ui/ui4/taskinfo.ini').read_bytes()
for n,title in [(3083,'Dụ Giao Long'),(3084,'Tu Tiên Đạo'),(3085,'Nhập Ma Pháp Môn'),(3086,'Phong Hồi Lộ Chuyển'),(3087,'Cửu Lai'),(3088,'Thăng Tiên Nhập Ma')]:
 assert ('[Task_%d]'%n).encode() not in data
 data+=enc('\r\n[Task_%d]\r\nTitle=$ %s\r\nIntro=$ Hoàn thành từng bước nhiệm vụ rồi trở về phục mệnh.\r\n'%(n,title))
p.write_bytes(data)
print('Staged seven scoped NPC appendices, four new hubs and six quest titles')
