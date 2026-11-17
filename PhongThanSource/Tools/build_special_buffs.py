"""Derive the missing original-item buff contract from the current VN table."""
from pathlib import Path
import hashlib,json
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2];LIVE=ROOT/'PhongThanRuntime-Loose/Server'
OUT=ROOT/'PhongThanSource/Deploy/ProjectContent/SpecialBuffs20261031'
def build():
 raw=(LIVE/'settings/item/001/ibitem.txt').read_bytes();rows=[r.split(b'\t') for r in raw.splitlines()[1:]]
 result=[]
 for detail in [7446,7447]:
  row=next(r for r in rows if r[1:4]==[b'8',str(detail).encode(),b'0'])
  assert row[12]==b'NONE' and row[13]==b'3' and row[18]==b'1'
  seconds=int(row[29])*86400;assert seconds==86400
  attrs=[(int(row[i] or b'0'),int(row[i+1] or b'0')) for i in range(30,42,2)]
  assert sum(a!=0 for a,v in attrs)==5
  result.append(dict(id=detail,seconds=seconds,attrs=attrs,source_row_sha256=hashlib.sha256(b'\t'.join(row)).hexdigest()))
 text='#ifndef PT_ORIGINAL_ITEM_BUFF_DATA_H\n#define PT_ORIGINAL_ITEM_BUFF_DATA_H\nstruct PTOriginalItemBuffDef {int id,seconds;int attrs[6][2];};\nstatic const PTOriginalItemBuffDef ptOriginalItemBuffs[]={\n'
 for r in result:text+='{%d,%d,{%s}},\n'%(r['id'],r['seconds'],','.join('{%d,%d}'%tuple(a) for a in r['attrs']))
 text+='};\n'
 messages={'HopeDenied':'Không thể nhận Linh Nguyện: tối đa 30 và chưa từng nhận nuôi linh thú.',
 'HopeAdded':'Đã cộng 1 Linh Nguyện vào ô buff.',
 'DragonActive':'Buff Thanh Long Lệnh vẫn còn hiệu lực; chưa tiêu hao vật phẩm.',
 'DragonAdded':'Đã nhận buff Thanh Long Lệnh trong 1 ngày.',
 'SlotsFull':'Ô buff đã đầy; chưa tiêu hao vật phẩm.'}
 for name,value in messages.items():text+='static const char ptOriginal'+name+'[]="'+''.join('\\%03o'%v for v in encode_tcvn3(value))+'";\n'
 text+='#endif\n'
 (ROOT/'PhongThanSource/Headers/PhongThanOriginalItemBuffData.h').write_text(text,'ascii')
 catalog=json.loads((ROOT/'analysis/event_handbook_20261026/catalog.json').read_text('utf8'))
 hope=next(e['id'] for e in catalog['entries'] if (e['genre'],e['detail'],e['part'])==(8,418,0))
 rel='script/phongthan/test/event_handbook.lua';before=(LIVE/rel).read_bytes()
 marker=b'\n-- Linh Nguyen is the original buff counter, not bag cargo.'
 base=before.split(marker,1)[0]
 append=f'''
-- Linh Nguyen is the original buff counter, not bag cargo.
EBOriginalItemMenu=EBItem
EBOriginalResultText=EBResultText
function EBItem(id,parent)
 if id~={hope} then return EBOriginalItemMenu(id,parent) end
 EBShow("Linh Nguyện được cộng trực tiếp vào ô buff. Tối đa 30, chỉ dành cho nhân vật chưa từng nhận nuôi linh thú.",{{
 "Nhận 1/EBGive("..id..",1,"..parent..")",
 "Nhận 10/EBGive("..id..",10,"..parent..")",
 "Nhận 30/EBGive("..id..",30,"..parent..")",
 EBLabels[3].."/EBMenu("..parent..",1)"}})
end
function EBResultText(result)
 if result==-8 then return "Không thể nhận Linh Nguyện: đã đủ 30, đã từng nhận nuôi linh thú hoặc chưa có ô buff trống." end
 return EBOriginalResultText(result)
end
'''
 payload=base+encode_tcvn3(append)
 script=b'-- Exact IB 8.7445.2 is validated by the native use handler.\nfunction main(item) return UseOriginalBuffItem(item) end\n'
 files=[]
 for rel,payload in [(rel,payload),('script/vnevent/items/thanh_long_lenh.lua',script)]:
  p=OUT/'compiled/Server'/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(payload)
  files.append(dict(path='Server/'+rel,sha256=hashlib.sha256(payload).hexdigest(),before_sha256=hashlib.sha256((LIVE/rel).read_bytes()).hexdigest() if (LIVE/rel).exists() else None))
 (OUT/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_ONLY',files=files,hope_entry=hope,buffs=result,source_sha256=hashlib.sha256(raw).hexdigest(),policy='original VN item rows; missing use callback reconstructed; no Release changes'),indent=2),'utf8')
 print('STAGED Linh Nguyen GM counter and Thanh Long use entry; 2 original attribute sets, VN lifetime 1 day')
if __name__=='__main__':build()
