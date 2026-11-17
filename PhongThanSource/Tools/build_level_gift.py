"""Build a persistent IB gift card and replace the starter Xu reward, Loose only."""
from pathlib import Path
import sys,json,hashlib,re,struct
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
ROOT=Path(__file__).resolve().parents[2];LIVE=ROOT/'PhongThanRuntime-Loose'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/LevelGift20261030'
REPORT=ROOT/'analysis/level_gift_20261030'
def enc(text):
 out=bytearray()
 for c in text:
  try:out.extend(encode_tcvn3(c))
  except KeyError:out.extend(encode_tcvn3(c.lower()))
 return bytes(out)
def lua(v):
 if isinstance(v,str):return json.dumps(v,ensure_ascii=False)
 if isinstance(v,int):return str(v)
 return '{'+','.join(lua(x) for x in v)+'}'
def ib(d,p,n,name):return [8,d,p,0,0,0,n,0,name]
def mat(d,n,name):return [3,d,0,0,0,0,n,0,name]
EXTRAS=[
 [ib(227,2,1,'Đồng Nhân'),ib(162,3,5,'Thanh Lộ'),ib(163,4,5,'Chân Khí'),ib(164,0,1,'Nhật Nguyệt Đơn')],
 [ib(227,2,5,'Đồng Nhân'),mat(41,10,'Lam Bảo Thạch'),ib(164,0,1,'Nhật Nguyệt Đơn'),ib(141,2,1,'Hồng Không Thư'),ib(159,2,1,'Di Ngoại Phù (Siêu Cấp)')],
 [ib(233,0,1,'La Hán Hiệu Giác'),ib(193,5,2,'Mảnh Sách Chư Hầu'),ib(164,0,1,'Nhật Nguyệt Đơn'),ib(141,2,1,'Hồng Không Thư')],
 [ib(193,5,4,'Mảnh Sách Chư Hầu'),ib(165,2,1,'Thái Cực Đơn'),ib(142,2,1,'Hoàng Không Thư'),mat(41,10,'Lam Bảo Thạch')],
 [ib(193,5,4,'Mảnh Sách Chư Hầu'),ib(269,2,1,'Mảnh Vải'),ib(142,2,1,'Hoàng Không Thư'),mat(79,10,'Hồng Bảo Thạch')],
 [ib(193,5,4,'Mảnh Sách Chư Hầu'),ib(165,2,1,'Thái Cực Đơn'),mat(66,1,'Thanh Long Lệnh'),ib(269,2,2,'Mảnh Vải')],
 []]
def build():
 files=[];REPORT.mkdir(parents=True,exist_ok=True)
 def put(role,rel,data):
  p=PACKAGE/'compiled'/role/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  before=LIVE/role/rel
  files.append(dict(role=role,relative=rel,sha256=hashlib.sha256(data).hexdigest(),before=hashlib.sha256(before.read_bytes()).hexdigest() if before.exists() else None))
 payload=enc('PTLG_EXTRAS='+lua(EXTRAS)+'\n'+(ROOT/'PhongThanSource/Tools/level_gift_runtime.lua').read_text('utf8'))
 for role in ('Server','Client'):
  rel='settings/item/001/ibitem.txt';raw=(LIVE/role/rel).read_bytes();rows=[l.split(b'\t') for l in raw.splitlines()]
  matches=[r for r in rows if len(r)>3 and r[2]==b'9700']
  if matches:
   assert len(matches)==1 and matches[0][12]==b'\\script\\phongthan\\rewards\\level_gift.lua'
   raw=b'\r\n'.join(b'\t'.join(r) for r in rows if not (len(r)>3 and r[2]==b'9700'))+b'\r\n'
  row=[b'0']*len(rows[0]);row[0]=enc('#Thẻ Quà: Thăng Cấp Nhận Thưởng')
  for i,v in {1:b'8',2:b'9700',3:b'2',4:b'\\spr\\item\\questkey\\taskobj025.spr',5:b'39',6:b'1',7:b'1',8:enc('#Nhấn chuột phải để nhận quà cấp 30/40/50/60/70/80/90. Thẻ không mất khi mở. Giữ đến khi nhận đủ quà; sau đó có thể bán shop.'),9:b'1',10:b'0',11:b'0',12:b'\\script\\phongthan\\rewards\\level_gift.lua',26:b'1',69:b'NONE',70:b'1'}.items():row[i]=v
  put(role,rel,raw.rstrip(b'\r\n')+b'\r\n'+b'\t'.join(row)+b'\r\n')
  put(role,'script/phongthan/rewards/level_gift.lua',payload)
 # Preserve initial gear/potions and partial delivery receipts. Never call the old 100-Xu API.
 for rel in ('script/phongthan/escort/lequan.lua','script/item/camnangtanthu.lua'):
  text=decode_tcvn3((LIVE/'Server'/rel).read_bytes())
  text=text.split('\nPTLG_LEQUAN_MAIN=main')[0]
  text=text.replace('-- 4702 delivered equipment/potion units; 4703 atomic 100-Xu grant.','-- 4702 delivered gear/potion units; legacy 4703 Xu marker retained, never paid here.')
  text=text.replace('và 100 Xu','và Thẻ Quà: Thăng Cấp Nhận Thưởng')
  if 'if GetTask(PT_STARTER_TASK)==2 then PTLGCard();return end' not in text:
   text=text.replace('function PhongThanClaimStarter()','function PhongThanClaimStarter()\n    if GetTask(PT_STARTER_TASK)==2 then PTLGCard();return end')
  text=text.replace('if LeQuanStarterXu() ~= 1 then','if PTLGCard() ~= 1 then')
  text=text.replace('if GetTask(4703) ~= 1 and IsServerBot','if GetTask(4704) ~= 1 and IsServerBot')
  text=text.replace('CanCarryNormalItem(4, 47, 0, 0, 0, 0, 100, 100)','CanCarryNormalItem(8, 9700, 2, 0, 0, 0, 1, 1)')
  text=text.replace('Chưa nhận được 100 Xu.','Chưa nhận được thẻ quà thăng cấp.')
  assert 'LeQuanStarterXu()' not in text
  # Include defines card main/GetDesc; restore this NPC's main afterwards.
  text+='\nPTLG_LEQUAN_MAIN=main\nInclude("\\\\script\\\\phongthan\\\\rewards\\\\level_gift.lua")\nmain=PTLG_LEQUAN_MAIN\n'
  put('Server',rel,enc(text))
 # Publish the exact original teacher at its actual PAK path as well as the
 # recovered placement path; keep all original quests and the original shop.
 from restore_original_world import Original
 original=Original();raw=bytes.fromhex('5c7363726970745cd3f1d0e9b9ac5cbdcccaa62e6c7561')
 data,proof=original.get(raw)
 (REPORT/'vo_su.original.lua').write_bytes(data)
 canonical=raw.decode('cp1252').lstrip('\\').replace('\\','/')
 compat=b'\nPTVS_RuntimePos=GetWorldPos\nGetWorldPos=GetContentWorldPos\nSubWorldIdx2ID=SubWorldIdx2ContentID\n'
 teacher=data+compat+enc((ROOT/'PhongThanSource/Tools/vo_su_ngoc_hu_guard.lua').read_text('utf8'))
 # Keep original source text intact; advertise that the skill menu sells books.
 for role in ('Server','Client'):put(role,canonical,teacher)
 put('Server','script/phongthan/npc_recovered_batch/1003_176efdac.lua',teacher)
 (REPORT/'vo_su_provenance.json').write_text(json.dumps(dict(proof=proof,canonical=canonical,recovered='script/phongthan/npc_recovered_batch/1003_176efdac.lua',original_preserved_prefix=True,shop=9),ensure_ascii=False,indent=2),'utf8')
 # Insert only our named paths into the current index, retaining all other IDs.
 from prepare_npc_restoration import pak_id
 for role in ('Server','Client'):
  raw=(LIVE/role/'loose_index.bin').read_bytes();assert raw[:8]==b'PTLOOSE1'
  count=struct.unpack_from('<I',raw,8)[0];at=12;entries={}
  for i in range(count):
   key,size=struct.unpack_from('<IH',raw,at);at+=6;entries[key]=raw[at:at+size];at+=size
  assert at==len(raw)
  for f in files:
   if f['role']!=role or not f['relative'].startswith('script/'):continue
   path=('\\'+f['relative'].replace('/','\\')).encode('cp1252');entries[pak_id(path)]=path
  data=b'PTLOOSE1'+struct.pack('<I',len(entries))+b''.join(struct.pack('<IH',k,len(v))+v for k,v in sorted(entries.items()))
  put(role,'loose_index.bin',data)
 (PACKAGE/'manifest.json').write_text(json.dumps(dict(schema=1,files=files),indent=2),'utf8')
 (PACKAGE/'rewards.json').write_text(json.dumps(dict(extras=EXTRAS,card=[8,9700,2],tasks=[4704,4705,4706,*range(4710,4717)],note='70 and 80 independently grant the same green equipment; all seven milestones retain the card.'),ensure_ascii=False,indent=2),'utf8')
 print(json.dumps(dict(files=len(files),package=str(PACKAGE))))
if __name__=='__main__':build()
