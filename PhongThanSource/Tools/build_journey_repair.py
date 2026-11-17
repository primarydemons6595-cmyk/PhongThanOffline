"""Stage journey fixes from current Loose files, preserving original callbacks."""
from pathlib import Path
import hashlib,json,re
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
LIVE=ROOT/'PhongThanRuntime-Loose/Server'
CONTENT=ROOT/'PhongThanSource/Deploy/ProjectContent/JourneyRepair20261031'
def sha(b):return hashlib.sha256(b).hexdigest()
def build():
 files=[]
 def put(rel,data):
  before=(LIVE/rel).read_bytes();p=CONTENT/'compiled/Server'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  files.append(dict(path='Server/'+rel,before_sha256=sha(before),sha256=sha(data)))
 for name in ['wildsuper','bigwildsuper']:
  rel=f'script/item/ibitem/{name}.lua';raw=(LIVE/rel).read_bytes()
  match=re.search(rb'Say\(10286,25,([^\r\n]+)\)',raw);assert match
  choices=re.findall(rb'"([^"\r\n]+)"',match[1]);assert len(choices)==25
  raw=raw[:match.start()]+b'pt_wild_page1()'+raw[match.end():]
  for page,items in [(1,choices[:13]),(2,choices[13:])]:
   options=[b'"'+x+b'"' for x in items]
   options.append(b'"'+encode_tcvn3('Trang sau' if page==1 else 'Trang truoc')+b'/pt_wild_page'+str(3-page).encode()+b'"')
   options.append(b'"'+encode_tcvn3('Dong')+b'/pt_wild_close"')
   raw+=b'\nfunction pt_wild_page'+str(page).encode()+b'()\n local w=GetWorldPos()\n if w==64 or w==66 or w==71 or w==72 then return main() end\n Say(10286,'+str(len(options)).encode()+b','+b','.join(options)+b')\nend\n'
  raw+=b'\nfunction pt_wild_close() CloseDialog() end\n'
  put(rel,raw)
 rel='script/phongthan/commerce/runtime.lua';raw=(LIVE/rel).read_bytes()
 assert raw.count(b'GetTask(332)<=5')==1
 raw=raw.replace(b'and HaveIBBuff(160)==1',b'and (GetMorphType()==364 or HaveIBBuff(160)==1)')
 put(rel,raw)
 rel='script/phongthan/giapsi/npc_doctor_dong.lua';raw=(LIVE/rel).read_bytes()
 fallback='''-- Visible fallback when no class/phase-specific objective is available.
function main()
 if GetContentWorldPos()~=14 then CloseDialog();return end
 SayButtons("Đại Phu Đồng Quan<enter>Ta giúp người hoàn thành nhiệm vụ Trừ Họa. Nếu chưa có nhiệm vụ tại đây, hãy xem chỉ dẫn F11 và hoàn thành bước trước.",1,"Kết thúc/pt_dong_doctor_close")
end
function pt_dong_doctor_close() CloseDialog() end
'''
 put(rel,encode_tcvn3(fallback)+raw)
 (CONTENT/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_FIRST',files=files),indent=2),'utf8')
 print('STAGED journey repairs:',len(files),'scripts')
if __name__=='__main__':build()
