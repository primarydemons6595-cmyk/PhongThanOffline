from pathlib import Path
import sys,re,json,struct,hashlib,collections
sys.path.insert(0,'PhongThanSource/Tools')
from restore_original_world import Original
from build_offline_loose import pak_id,sprite_blob_valid
root=Path.cwd();runtime=root/'PhongThanRuntime-Loose/Client';out=root/'analysis/ui_gold_channels_20261027';stage=out/'compiled/Client';out.mkdir(exist_ok=True);original=Original()
def sha(b):return hashlib.sha256(b).hexdigest()
def swap_truecolor(blob):
 assert sprite_blob_valid(blob,False) and not sprite_blob_valid(blob,True)
 data=bytearray(blob);frames=struct.unpack_from('<H',data,12)[0];start=32+8*frames;pixels=0
 for n in range(frames):
  off,size=struct.unpack_from('<II',data,32+n*8);at=start+off;w,h=struct.unpack_from('<HH',data,at);at+=8;done=0
  while done<w*h:
   run,alpha=data[at:at+2];at+=2;done+=run
   if alpha:
    for j in range(run):data[at],data[at+2]=data[at+2],data[at];at+=3;pixels+=1
 assert sprite_blob_valid(data,False) and len(data)==len(blob)
 return bytes(data),pixels
index=(runtime/'loose_index.bin').read_bytes();at=12;records={}
for n in range(struct.unpack_from('<I',index,8)[0]):
 key,length=struct.unpack_from('<IH',index,at);at+=6;records[key]=index[at:at+length];at+=length
refs=[]
for ini in (runtime/'ui').rglob('*.ini'):
 status=ini.name.lower()=='uistatus.ini';npc=ini.name.lower().startswith('npc') or ini.name.lower().startswith('uiinformation')
 if not(status or npc):continue
 section=''
 for line in ini.read_bytes().splitlines():
  if line.startswith(b'['):section=line.strip(b'[]').decode('ascii','replace')
  if not line.startswith(b'Image='):continue
  if status and section not in ['MainProperty','b_AttackProperty','b_DefenceProperty','b_BaseProperty','img_background1','img_background2','img_background3']:continue
  raw=b'\\'+line[6:].replace(b'/ ',b'\\').lstrip(b'\\');key=pak_id(raw);logical=records.get(key)
  if logical:refs.append(dict(ini=ini.relative_to(runtime).as_posix(),section=section,raw_hex=raw.hex(),id=key,physical=logical.lstrip(b'\\').decode('latin1').replace('\\','/')))
seen={};audit=[]
for ref in refs:
 rel=ref['physical'];live=runtime/rel
 if rel in seen:continue
 row=dict(physical=rel,refs=[x for x in refs if x['physical']==rel]);seen[rel]=row;audit.append(row)
 if not live.exists():row['state']='MISSING';continue
 blob=live.read_bytes();row['before']=sha(blob)
 if not sprite_blob_valid(blob,False):row['state']='INDEXED_OR_OTHER_UNCHANGED';continue
 if sprite_blob_valid(blob,True):row['state']='AMBIGUOUS_UNCHANGED';continue
 proof=None
 for r in row['refs']:
  found=original.get(bytes.fromhex(r['raw_hex']))
  if found and found[0]==blob:proof=found[1];break
 if not proof:
  logical=('\\'+rel.replace('/','\\')).encode('latin1')
  for key,value in records.items():
   if value.lower()!=logical.lower() or key not in original.winners:continue
   e=original.winners[key]
   from build_offline_loose import entry_payload
   try:candidate,_=entry_payload(e,{},None)
   except ValueError:continue
   if candidate==blob:proof=dict(id=f'{key:08X}',pak=e['pak'].name,entry=e['entry'],sha256=sha(blob));break
 if not proof:row['state']='NOT_EXACT_ORIGINAL_UNCHANGED';continue
 corrected,pixels=swap_truecolor(blob);assert swap_truecolor(corrected)[0]==blob
 target=stage/rel;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(corrected)
 row.update(state='BGR_TO_RENDERER_RGB',sha256=sha(corrected),source=proof,opaquePixels=pixels,frames=struct.unpack_from('<H',blob,12)[0])
(out/'audit.json').write_text(json.dumps(audit,ensure_ascii=False,indent=2),encoding='utf-8');print('UI resources',len(audit),'states',dict(collections.Counter(x['state'] for x in audit)))
(out/'sprite-list.txt').write_bytes(b'\r\n'.join(('\\'+r['physical'].replace('/','\\')).encode('latin1') for r in audit if r['state']=='BGR_TO_RENDERER_RGB')+b'\r\n')
