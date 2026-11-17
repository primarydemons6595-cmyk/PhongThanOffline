"""Stage TGPT menus and scene changes without touching a running world."""
import hashlib,json,re,struct
from pathlib import Path
from extract_client_coordinate_dictionary import encode_tcvn3
from prepare_npc_restoration import split_region,npc_records,standalone_npcs
ROOT=Path(__file__).resolve().parents[2];server=ROOT/'PhongThanRuntime-Loose/Server'
out=ROOT/'analysis/thuongchu_tgpt_20261103';package=ROOT/'PhongThanSource/Deploy/ProjectContent/ThuongChuTgpt20261103';stage=package/'compiled/Server';files=[]
def put(path,data):
 if isinstance(data,str):data=font(data)
 target=stage/path;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data);before=server/path
 files.append(dict(relative=path,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(before.read_bytes()).hexdigest() if before.exists() else None,bytes=len(data)))
def font(text):
 # This original client has no uppercase accented vowel glyph slots.
 while True:
  try:return encode_tcvn3(text)
  except KeyError as error:
   ch=error.args[0];assert ch.lower()!=ch; text=text.replace(ch,ch.lower())
common='''
function pt_tc_close() CloseDialog() end
function pt_tc_status()
 local names={"Truyền thống","Viễn Cổ","Viễn Cổ Thống Soái"}
 local mode=ThuongChu(19);local name="Chưa mở";if mode>0 then name=names[mode] end
 Talk(1,"main","Phiên này: "..name..". Điểm Thương/Chu: "..ThuongChu(7,1).."/"..ThuongChu(7,2)..". Cờ Soái Thương/Chu: "..ThuongChu(26,1).."/"..ThuongChu(26,2)..".")
end
function pt_tc_saved() Talk(1,"main","Tích lũy: "..ThuongChu(12).."; công trạng: "..ThuongChu(15).."; danh vọng Thương: "..ThuongChu(14,1).."; danh vọng Chu: "..ThuongChu(14,2)..".") end
function pt_tc_reward() local xp=ThuongChu(8);if xp>0 then Talk(1,"main","Đã nhận "..xp.." kinh nghiệm.") else Talk(1,"main","Không có thưởng chờ nhận tại đây.") end end
function pt_tc_buff() if ThuongChu(25)==1 then Talk(1,"main","Đã dùng 1200 điểm tích lũy: tăng 10% sinh lực, nội lực và xuất chiêu phù hợp hệ phái trong 2 ngày.") else Talk(1,"main","Cần 1200 điểm tích lũy và chưa có buff Thương Chu còn hạn.") end end
function pt_tc_rent() Say("Thuê 3 ngày, trang bị nhiệm vụ khóa. Giáp +6: 20 Tướng Ấn và 15000 công trạng. Vũ khí: 30 Tướng Ấn và 20000 công trạng.",5,"Trang bị lục 8x/pt_tc_armor80","Trang bị lục 10x/pt_tc_armor100","Vũ khí 9x/pt_tc_weapon90","Vũ khí 10x/pt_tc_weapon100","Quay lại/main") end
function pt_tc_rented(ok) if ok==1 then Talk(1,"main","Đã nhận trang bị thuê, hạn dùng 3 ngày kể từ lúc nhận.") else Talk(1,"main","Chưa thuê được. Kiểm tra Tướng Ấn, công trạng, chỗ trống và trang bị đúng hệ phái.") end end
function pt_tc_armor80() pt_tc_rented(ThuongChu(22,80)) end
function pt_tc_armor100() pt_tc_rented(ThuongChu(22,100)) end
function pt_tc_weapon90() pt_tc_rented(ThuongChu(23,90)) end
function pt_tc_weapon100() pt_tc_rented(ThuongChu(23,100)) end
'''
for side,path in [(1,'script/phongthan/npc_restore/1001_01.lua'),(2,'script/phongthan/thuong_chu/stele_zhou.lua')]:
 put(path,'''function main() Say("Chiến trường Thương Chu - chọn một dạng trận chung cho toàn phiên",8,"Báo danh/pt_tc_join","Vào chiến trường/pt_tc_enter","Tình hình phiên/pt_tc_status","Danh vọng và công trạng/pt_tc_saved","Mua buff 2 ngày/pt_tc_buff","Thuê trang bị/pt_tc_rent","Hướng dẫn/pt_tc_help","Kết thúc/pt_tc_close") end
function pt_tc_join() if ThuongChu(1,SIDE)==1 then Talk(1,"main","Đã báo danh. Hết 10 phút báo danh hãy chọn Vào chiến trường.") else Talk(1,"main","Chưa báo danh được. Kiểm tra cấp yêu cầu, giờ báo danh, cân bằng phe và nhận hết thưởng trận trước.") end end
function pt_tc_enter() if ThuongChu(2)==0 then Talk(1,"main","Cần báo danh đúng phe và chờ bắt đầu thi đấu.") end end
function pt_tc_help() Talk(1,"main","Báo danh 12h và 20h trong 10 phút, thi đấu 30 phút. ít nhất 5 người mỗi phe. Một dạng chiến trường được chọn chung cho cả phiên. Nhận Vị Quốc Lập Công ở Văn Trọng hoặc Khương Thượng. Trận Thống Soái: sau 10 phút xuất hiện boss, hạ boss rồi đoạt cờ và mang về nộp; phe nộp đủ 3 cờ Soái đối phương thắng ngay.") end
'''.replace('SIDE',str(side))+common)
put('script/phongthan/thuong_chu/mission_npc.lua','''function main()
 if ThuongChu(10)==0 then Talk(1,"pt_tc_close","Ngươi chưa báo danh phiên này.");return end
 Say("Vị Quốc Lập Công: "..ThuongChu(21).."/10 lượt hôm nay. Mỗi nhiệm vụ 4 vạn; 3 lượt đầu không tốn tiền đồng, lượt 4-10 thêm 3 tiền đồng.",12,"Hạ 10 quân tiên phong/pt_tc_killquest","Cướp 10 cờ đối thủ/pt_tc_flagquest","Hạ 10 thám quân/pt_tc_scoutquest","Phá 3 vật tổ/pt_tc_totemquest","Tiến độ nhiệm vụ/pt_tc_progress","Trả nhiệm vụ/pt_tc_finish","Hoàn thành nhanh - 5 tiền đồng/pt_tc_quick","Đổi nhiệm vụ - 1 tiền đồng/pt_tc_change","Nộp cờ Soái hoặc cờ đối thủ/pt_tc_deliver","Tình hình chiến trường/pt_tc_status","Rời chiến trường/pt_tc_leave","Kết thúc/pt_tc_close")
end
function pt_tc_take(n) if ThuongChu(4,n)==1 then Talk(1,"main","Đã nhận nhiệm vụ. Trả nhiệm vụ được 10 điểm tích lũy và 15 danh vọng của phe mình.") else Talk(1,"main","Cần ở doanh trại phe mình, chưa có nhiệm vụ, còn lượt ngày, đủ bạc và tiền đồng.") end end
function pt_tc_killquest() pt_tc_take(1) end
function pt_tc_flagquest() pt_tc_take(2) end
function pt_tc_scoutquest() pt_tc_take(3) end
function pt_tc_totemquest() pt_tc_take(4) end
function pt_tc_finish() if ThuongChu(5)==1 then Talk(1,"main","Đã trả nhiệm vụ: +10 tích lũy, +15 danh vọng phe.") else Talk(1,"main","Chưa đủ tiến độ hoặc chưa về doanh trại đúng phe.") end end
function pt_tc_quick() if ThuongChu(16)==1 then Talk(1,"main","Đã dùng 5 tiền đồng hoàn thành nhiệm vụ: +10 tích lũy, +15 danh vọng phe.") else Talk(1,"main","Cần nhiệm vụ đang nhận và 5 tiền đồng, đứng tại doanh trại đúng phe.") end end
function pt_tc_change() Say("Đổi nhiệm vụ: 1 tiền đồng, không tăng lượt và không thu thêm bạc.",5,"Hạ quân tiên phong/pt_tc_change1","Cướp cờ/pt_tc_change2","Hạ thám quân/pt_tc_change3","Phá vật tổ/pt_tc_change4","Quay lại/main") end
function pt_tc_changed(n) if ThuongChu(17,n)==1 then Talk(1,"main","Đã đổi nhiệm vụ, tiến độ mới tính lại từ đầu.") else Talk(1,"main","Cần nhiệm vụ đang nhận, chọn nhiệm vụ khác và có 1 tiền đồng.") end end
function pt_tc_change1() pt_tc_changed(1) end
function pt_tc_change2() pt_tc_changed(2) end
function pt_tc_change3() pt_tc_changed(3) end
function pt_tc_change4() pt_tc_changed(4) end
function pt_tc_progress() Talk(1,"main","Loại nhiệm vụ: "..ThuongChu(11).." (1: tiên phong, 2: cờ, 3: thám quân, 4: vật tổ); tiến độ: "..ThuongChu(20)..". Đang mang cờ: "..ThuongChu(27)..".") end
function pt_tc_deliver() if ThuongChu(18)==1 then Talk(1,"main","Đã nộp cờ cho phe mình.") else Talk(1,"main","Cần đang mang cờ đối phương và về đúng doanh trại phe mình.") end end
function pt_tc_leave() ThuongChu(3) end
'''+common)
for path,side in [('script/phongthan/npc_restore/1020_02.lua',2),('script/phongthan/npc_restore/1021_02.lua',1)]:
 # Locate the exact previously published king wrapper rather than trust ordinals.
 if not (server/path).exists() or b'PT_THUONG_CHU_REWARD_20261102' not in (server/path).read_bytes():
  matches=[p for p in (server/'script/phongthan/npc_restore').glob(('1020' if side==2 else '1021')+'_*.lua') if b'PT_THUONG_CHU_REWARD_20261102' in p.read_bytes()];assert len(matches)==1;path=matches[0].relative_to(server).as_posix()
 before=(server/path).read_bytes();at=before.index(b'-- PT_THUONG_CHU_REWARD_20261102');old=before[:at]
 wrapper='''-- PT_THUONG_CHU_TGPT_20261103
pt_tc_previous_main=main
function main() Say("Chiến công Thương Chu",6,"Nhận thưởng trận trước/pt_tc_reward","50 danh vọng đổi Tướng Ấn/pt_tc_exchange","200 Tướng Ấn đổi trang bị/pt_tc_gamble","Danh vọng và công trạng/pt_tc_saved","Nhiệm vụ khác/pt_tc_old","Kết thúc/pt_tc_close") end
function pt_tc_exchange() if ThuongChu(13,SIDE)==1 then Talk(1,"main","Đã đổi 50 danh vọng phe thành 1 Tướng Ấn có thể giao dịch, bày bán và vứt bỏ.") else Talk(1,"main","Cần 50 danh vọng đúng phe và chỗ trống trong túi.") end end
function pt_tc_gamble() local result=ThuongChu(24);if result==1 then Talk(1,"main","Thành công: đã nhận trang bị lục nhiệm vụ 8x-10x đúng hệ phái.") elseif result==2 then Talk(1,"main","Chưa thành công, đã nhận lại 150 Tướng Ấn.") else Talk(1,"main","Cần đủ 200 Tướng Ấn và chỗ trống nhận phần thưởng.") end end
function pt_tc_old() return pt_tc_previous_main() end
'''.replace('SIDE',str(side))
 put(path,old+font(wrapper+common))
put('settings/phongthan/ThuongChu.ini','''[Battle]
; TGPT article: one globally selected variant per session. 0=random,1=traditional,2=ancient,3=commander.
Mode=0
MinimumPerSide=5
; Article gives totem50, quest10/15 and commander3flag victory explicitly.
; The article does not specify these balance values; local defaults remain configurable.
CommanderKillPoints=50
CommanderFlagPoints=100
FlagPoints=10
ScoutKillPoints=2
TotemRespawnSeconds=60
EquipmentSuccessPercent=50
''')
# Remove the previous static camp NPC copies: match-owned service units now
# appear at the correct bases for the selected layout, and disappear at close.
# Move the Zhou stele to the article's212/205 while retaining all other records.
moving=None
for world in (1001,1071):
 for p in (server/f'settings/phongthan/npc_regions/{world}').rglob('*.dat'):
  data=p.read_bytes();region=p.name.endswith('_region_s.dat')
  try:head,sections,payload=split_region(data) if region else (0,[],data);rows=npc_records(payload)
  except ValueError:continue
  kept=[]
  for row in rows:
   script=row[60:].rstrip(b'\0').lower()
   if world==1071 and script==b'\\script\\phongthan\\thuong_chu\\mission_npc.lua':continue
   if world==1001 and script==b'\\script\\phongthan\\thuong_chu\\stele_zhou.lua':assert moving is None;moving=bytearray(row);struct.pack_into('<ii',moving,4,212*256,205*512);continue
   kept.append(row)
  if len(kept)==len(rows):continue
  newpayload=standalone_npcs(kept)
  if region:
   updated=bytearray(data);struct.pack_into('<II',updated,20,len(data)-head,len(newpayload));updated.extend(newpayload);data=bytes(updated)
  else:data=newpayload
  put(p.relative_to(server).as_posix(),data)
assert moving is not None
cell=f'v_{205*512//1024:03d}/{212*256//512:03d}'
relative=f'settings/phongthan/npc_regions/1001/{cell}_npc_s.dat';p=server/relative
rows=npc_records(p.read_bytes()) if p.exists() else [];put(relative,standalone_npcs(rows+[bytes(moving)]))
catalog=json.loads((server/'EventService/catalog.json').read_text('utf-8-sig'));event=next(e for e in catalog if e['Id']==2101)
event['SourceEvidence']='TGPT article: https://pc.tamgioipt.vn/he-thong-nhiem-vu-phan-thuong-chien-truong-thuong-chu ; one random shared session mode; native engine plus ordinary NPC dialogs. Unspecified balance values documented in ThuongChu.ini.'
event['Defaults'].update(DailyTimes=['12:00','20:00'],RegistrationSeconds=600,DurationSeconds=1800)
put('EventService/catalog.json',json.dumps(catalog,ensure_ascii=False,indent=2).encode('utf8'))
manifest=dict(schema=1,feature='TGPT_THUONG_CHU_20261103',files=files,event=event,source='https://pc.tamgioipt.vn/he-thong-nhiem-vu-phan-thuong-chien-truong-thuong-chu',notes='One session mode for all registrants; ancient and commander variants use original ancient terrain1064 with distinct objectives. No changes to doctors or GiapSi portal scripts.')
(package/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),'utf8')
syntax=[]
for i,row in enumerate(files):
 if row['relative'].endswith('.lua'):
  p=out/f'lua-{i}.lua';p.write_bytes((stage/row['relative']).read_bytes());syntax.append(p.as_posix())
(out/'lua-list.txt').write_text('\n'.join(syntax)+'\n','ascii')
print('Staged',len(files),'reviewed files,',len(syntax),'complete NPC scripts')
