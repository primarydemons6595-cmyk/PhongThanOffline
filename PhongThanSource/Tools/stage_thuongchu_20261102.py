"""Build the documented Thuong Chu entry/NPC menus and additive Event catalog."""
import json,hashlib
from pathlib import Path
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2];server=ROOT/'PhongThanRuntime-Loose/Server';out=ROOT/'analysis/doctors_thuongchu_20261102';stage=out/'staged/Server';files=[]
def put(relative,text,binary=False):
 data=text if binary else encode_tcvn3(text);p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data);files.append(relative)
for side,suffix in [(1,'shang'),(2,'zhou')]:
 relative='script/phongthan/npc_restore/1001_01.lua' if side==1 else 'script/phongthan/thuong_chu/stele_zhou.lua'
 put(relative,'''-- Documented VNG Thuong Chu entry; native session owns admission and PK.
function main()
 Say("Chiến trường Thương Chu",4,"Báo danh phe SIDE/pt_tc_join","Vào chiến trường/pt_tc_enter","Hướng dẫn/pt_tc_help","Kết thúc/pt_tc_close")
end
function pt_tc_join()
 if ThuongChu(1,SIDE)==1 then Talk(1,"main","Đã báo danh. Khi hết thời gian chuẩn bị, quay lại đây để vào trận.")
 else Talk(1,"main","Chưa báo danh được: cần cấp 120, đang mở báo danh, phe chưa đủ 100 người và không hơn phe kia quá 10 người. Hãy nhận thưởng trận trước nếu còn.") end
end
function pt_tc_enter()
 if ThuongChu(2)==0 then Talk(1,"main","Chưa vào được. Cần báo danh đúng phe và chờ trận chiến bắt đầu.") end
end
function pt_tc_help() Talk(1,"main","Mỗi phe tối đa 100 người, cần ít nhất 15 người để bắt đầu. Bảo vệ tướng phe mình, hạ Nguyên Soái hoặc tướng phe địch để chiếm ba mặt trận. Nhận nhiệm vụ tại Văn Trọng hoặc Khương Thượng. Kết thúc trận, nhận thưởng tại Trụ Vương hoặc Võ Vương.") end
function pt_tc_close() CloseDialog() end
'''.replace('SIDE',str(side)))
mission='''function main()
 if ThuongChu(10)==0 then Talk(1,"pt_tc_close","Ngươi chưa tham gia trận Thương Chu này.");return end
 Say("Vị Quốc Lập Công",7,"Hạ quân Tiên Phong/pt_tc_killquest","Tiêu diệt Nguyên Soái/pt_tc_marshalquest","Trả nhiệm vụ/pt_tc_finish","Tình hình chiến trường/pt_tc_status","Tích lũy cá nhân/pt_tc_points","Rời chiến trường/pt_tc_leave","Kết thúc/pt_tc_close")
end
function pt_tc_killquest() if ThuongChu(4,1)==1 then Talk(1,"main","Hãy tiêu diệt 10 nhân vật phe đối phương rồi quay lại trả nhiệm vụ.") else Talk(1,"main","Chưa nhận được: đang có nhiệm vụ hoặc đã hoàn thành mục này.") end end
function pt_tc_marshalquest() if ThuongChu(4,2)==1 then Talk(1,"main","Tham gia tiêu diệt một Nguyên Soái rồi quay lại trả nhiệm vụ.") else Talk(1,"main","Chưa nhận được: đang có nhiệm vụ hoặc đã hoàn thành mục này.") end end
function pt_tc_finish() if ThuongChu(5)==1 then Talk(1,"main","Đã xác nhận chiến công. Hoàn thành cả hai mục sẽ nhận Giấy Chứng Nhận Thương Chu.") else Talk(1,"main","Chưa đủ điều kiện trả nhiệm vụ hoặc túi không còn chỗ nhận thưởng.") end end
function pt_tc_status() Talk(1,"main","Điểm phe Thương: "..ThuongChu(7,1)..". Điểm phe Chu: "..ThuongChu(7,2)..".") end
function pt_tc_points() Talk(1,"main","Tích lũy trận này: "..ThuongChu(6)..". Tối đa 1.000 điểm mỗi trận.") end
function pt_tc_leave() ThuongChu(3) end
function pt_tc_close() CloseDialog() end
'''
put('script/phongthan/thuong_chu/mission_npc.lua',mission)
for suffix,index in [('shang','03'),('zhou','04')]:
 put('script/phongthan/npc_restore/1071_'+index+'.lua','''function main() Say("Tiếp dẫn chiến trường - mỗi lượt 1 vạn lượng",4,"Mặt trận thứ nhất/pt_tc_port1","Mặt trận thứ hai/pt_tc_port2","Mặt trận thứ ba/pt_tc_port3","Kết thúc/pt_tc_close") end
function pt_tc_port1() pt_tc_port(1) end
function pt_tc_port2() pt_tc_port(2) end
function pt_tc_port3() pt_tc_port(3) end
function pt_tc_port(n) if ThuongChu(9,n)==0 then Talk(1,"main","Chưa di chuyển được. Cần đang tham chiến đúng phe và có 1 vạn lượng trong người.") end end
function pt_tc_close() CloseDialog() end
''')
for relative in ['script/phongthan/npc_restore/1020_02.lua','script/phongthan/npc_restore/1021_02.lua']:
 # Find the actual Tru Vuong binding by the authored catalog rather than guess
 # which same-map NPC happens to have index02.
 if relative.endswith('1021_02.lua'):
  audit=json.loads((out/'audit.json').read_text('utf8'))
  candidates=[r for r in audit['npcs'] if r['world']==1021 and (bytes.fromhex(r['name_hex']) in [encode_tcvn3('Trụ Vương'), '纣王'.encode('gbk')])]
  if not candidates:
   # Authored coordinates table is authoritative for the original NPC ordinal.
   catalog=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/coordinates.json').read_text('utf8'))
   idx=next(i for i,r in enumerate(catalog['maps']['1021']) if r[0]=='Trụ Vương')
   relative=f'script/phongthan/npc_restore/1021_{idx:02d}.lua'
  else:relative=bytes.fromhex(candidates[0]['script_hex']).decode('ascii').lstrip('\\').replace('\\','/')
 before=(server/relative).read_bytes();wrapper='''
-- PT_THUONG_CHU_REWARD_20261102
pt_tc_previous_main=main
function main() Say("Chiến công Thương Chu",5,"Nhận thưởng trận trước/pt_tc_reward","Đổi 50 điểm lấy Giấy Chứng Nhận/pt_tc_exchange","Xem điểm đã tích lũy/pt_tc_saved","Nhiệm vụ khác/pt_tc_old","Kết thúc/pt_tc_close") end
function pt_tc_reward() local xp=ThuongChu(8);if xp>0 then Talk(1,"main","Đã nhận "..xp.." điểm kinh nghiệm.") else Talk(1,"main","Không còn thưởng trận trước tại đây.") end end
function pt_tc_exchange() if ThuongChu(13)==1 then Talk(1,"main","Đã đổi 50 điểm lấy một Giấy Chứng Nhận Thương Chu.") else Talk(1,"main","Cần 50 điểm tích lũy và chỗ trống trong túi.") end end
function pt_tc_saved() Talk(1,"main","Điểm tích lũy còn lại: "..ThuongChu(12)..".") end
function pt_tc_old() return pt_tc_previous_main() end
function pt_tc_close() CloseDialog() end
'''
 put(relative,before+encode_tcvn3(wrapper),True)
put('settings/phongthan/ThuongChu.ini','[Battle]\nMinimumPerSide=15\n')
catalog=json.loads((server/'EventService/catalog.json').read_text('utf-8-sig'));assert not any(r['Id']==2101 for r in catalog)
event=dict(Id=2101,Key='thuong_chu',Name='Chiến trường Thương Chu',Group='Chiến trường',Adapter='thuong_chu',Map=1071,Mission=0,Template=0,BossFlag=-1,StartScriptHex='',SourceEvidence='Reconstructed native session from https://phongthan.zing.vn/cam-nang/tinh-nang/chien-truong-thuong-chu.html ; original PAK battle program not recovered. Original item tuple3.5093 verified from live registry.',Defaults=dict(Id=2101,Enabled=True,DailyTimes=['14:00','20:00'],RegistrationSeconds=900,DurationSeconds=2700,MinimumLevel=120,TicketCount=0))
catalog.append(event);put('EventService/catalog.json',json.dumps(catalog,ensure_ascii=False,indent=2).encode('utf8'),True)
native=(server/'EventService/catalog.tsv').read_bytes();native+=('\t'.join(['2101','4','1071','71','0','0','0','-1','',''+encode_tcvn3('Chiến trường Thương Chu').hex()])+'\n').encode('ascii');put('EventService/catalog.tsv',native,True)
(out/'battle-content-plan.json').write_text(json.dumps(dict(files=files,event=event),ensure_ascii=False,indent=2),'utf8')
print('Staged battle content files',len(files))
