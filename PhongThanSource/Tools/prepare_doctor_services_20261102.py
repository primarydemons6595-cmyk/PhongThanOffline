"""Stage doctor menus preserving existing quest callbacks and shop identities."""
import json,re,hashlib
from pathlib import Path
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];server=ROOT/'PhongThanRuntime-Loose/Server'
out=ROOT/'analysis/doctors_thuongchu_20261102';stage=out/'staged/Server'
audit=json.loads((out/'full-audit.json').read_text('utf8'));files=[]
for row in audit['doctors']:
 world=row['world']
 if world in (1001,1002,1003,1004,1020,1021,1052,1067,1068,1069,1070,1072):continue
 relative=row['script_relative'];path=server/relative
 if not path.is_file():
  # Loader filenames on disk preserve original bytes in Latin-1 spelling.
  relative=bytes.fromhex(row['script_hex']).decode('latin1').lstrip('\\').replace('\\','/');path=server/relative
 if not path.is_file():raise ValueError('Missing actual doctor script '+str(path))
 data=path.read_bytes();marker=b'-- PT_DOCTOR_SERVICES_20261102'
 if marker in data:data=data.split(marker)[0].rstrip()+b'\n'
 shops=re.findall(rb'\bSale\s*\(\s*(\d+)\s*\)',data)
 # Shop23 is the recovered outdoor medicine shop. Preserve a doctor's own
 # existing shop where one exists; quest-only doctors gain the same service.
 shop=int(shops[0]) if shops else 23
 text='''
-- PT_DOCTOR_SERVICES_20261102
pt_doctor_quest_main=main
function main()
 Say("Đại Phu",4,"Cửa hàng thuốc/pt_doctor_shop","Trị thương/pt_doctor_heal","Nhiệm vụ/pt_doctor_quests","Kết thúc/pt_doctor_close")
end
function pt_doctor_shop() CloseDialog();Sale(SHOP) end
function pt_doctor_heal()
 local elapsed=SystemTime()-GetTask(763)
 if elapsed<1800 then
  Talk(1,"pt_doctor_close","Ta chưa hồi phục nguyên khí, chưa thể trị thương tiếp. Hãy quay lại sau.");return
 end
 RestoreLife();RestoreMana();SetTask(763,SystemTime());CloseDialog()
 Msg2Player("Sinh lực và nội lực của bạn đã hoàn toàn hồi phục.")
end
function pt_doctor_quests() return pt_doctor_quest_main() end
function pt_doctor_close() CloseDialog() end
'''.replace('SHOP',str(shop))
 after=data+encode_tcvn3(text)
 target=stage/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(after)
 files.append(dict(world=world,relative=relative,shop=shop,before_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),sha256=hashlib.sha256(after).hexdigest()))
(out/'doctor-services-plan.json').write_text(json.dumps(dict(files=files),ensure_ascii=False,indent=2),'utf8')
syntax=out/'syntax';syntax.mkdir(exist_ok=True);paths=[]
for n,row in enumerate(files):
 p=syntax/(str(n)+'.lua');p.write_bytes((stage/row['relative']).read_bytes());paths.append(str(p).replace('\\','/'))
(out/'doctor-lua-list.txt').write_text('\n'.join(paths)+'\n','ascii')
(out/'doctor-callback-list.txt').write_text('\n'.join(path+'\t'+str(row['shop']) for path,row in zip(paths,files))+'\n','ascii')
print('Staged outdoor doctor menus',len(files),'worlds',sorted({r['world'] for r in files}))
