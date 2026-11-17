"""Audit available original Dao Si/Di Nhan Lua before editing quest chains."""
from pathlib import Path
import json,re,hashlib,sys,struct
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from restore_original_world import Original,path_bytes
from extract_client_coordinate_dictionary import decode_tcvn3
out=ROOT/'analysis/two_faction_quests_20261027';out.mkdir(exist_ok=True)
library=json.loads((ROOT/'PhongThanSource/Docs/NPC_LUA_LIBRARY.json').read_text(encoding='utf-8'))
original=Original();rows=[];seen=set();candidates=[]
for e in library['entries']:
 obj=ROOT/'PhongThanSource/SourceMigration/NpcLuaLibrary'/e['object']
 if obj.exists():candidates.append((path_bytes(e['path']),obj.read_bytes()))
index=(ROOT/'PhongThanRuntime-Loose/Server/loose_index.bin').read_bytes();at=12
for _ in range(struct.unpack_from('<I',index,8)[0]):
 key,size=struct.unpack_from('<IH',index,at);at+=6;raw=index[at:at+size];at+=size
 if not raw.lower().endswith(b'.lua') or raw.lower() in seen:continue
 seen.add(raw.lower())
 live=ROOT/'PhongThanRuntime-Loose/Server'/raw.lstrip(b'\\').replace(b'\\',b'/').decode('cp1252')
 if live.is_file():candidates.append((raw,live.read_bytes()))
seen=set()
for raw,old in candidates:
 faction_hub=any(x.encode('gbk') in raw for x in ['玉虚宫','蚩尤墓'])
 if not faction_hub and not re.search(rb'(?:GetTask|SetTask)\s*\(\s*[12]\s*[,)]',old):continue
 if raw.lower() in seen:continue
 seen.add(raw.lower());result=original.get(raw)
 if not result:continue
 data,proof=result
 if not faction_hub and not re.search(rb'(?:GetTask|SetTask)\s*\(\s*[12]\s*[,)]',data):continue
 n=len(rows);folder=out/'originals';folder.mkdir(exist_ok=True)
 (folder/f'{n:03}.lua').write_bytes(data);(folder/f'{n:03}.txt').write_text(decode_tcvn3(data),encoding='utf-8')
 rel=raw.lstrip(b'\\').replace(b'\\',b'/').decode('cp1252');live=ROOT/'PhongThanRuntime-Loose/Server'/rel
 markers=[]
 for match in re.finditer(rb'(?:GetTask|SetTask)\s*\(\s*([12])\s*(?:,\s*(\d+))?',data):
  markers.append([int(match[1]),int(match[2]) if match[2] else None])
 rows.append(dict(number=n,path=proof['path'],raw_hex=raw.hex(),proof=proof,
  runtime_path=rel,runtime_exists=live.exists(),runtime_exact=live.exists() and live.read_bytes()==data,
  runtime_original_prefix=live.exists() and live.read_bytes().startswith(data),tasks=markers,
  calls=sorted(set(x.decode('ascii') for x in re.findall(rb'\b([A-Za-z_]\w*)\s*\(',data))),
  lines=[x for x in decode_tcvn3(data).splitlines() if re.search(r'GetTask\([12]\)|SetTask\([12],|SayTask\(|Add.*Item|AddOwnExp|AddExp|AddRepute|Del.*Item',x)]))
(out/'audit.json').write_text(json.dumps(dict(package_order=[p.name for p in original.listed],scripts=rows),ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(dict(scripts=len(rows),exact=sum(r['runtime_exact'] for r in rows),original_prefix=sum(r['runtime_original_prefix'] for r in rows),missing=sum(not r['runtime_exists'] for r in rows),task1_set_values=sorted({t[1] for r in rows for t in r['tasks'] if t[0]==1 and t[1] is not None}),task2_set_values=sorted({t[1] for r in rows for t in r['tasks'] if t[0]==2 and t[1] is not None}))))
