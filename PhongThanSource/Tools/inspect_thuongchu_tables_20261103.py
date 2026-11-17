from pathlib import Path
from extract_client_coordinate_dictionary import decode_tcvn3
from restore_original_world import Original
ROOT=Path(__file__).resolve().parents[2];s=ROOT/'PhongThanRuntime-Loose/Server'
for name in ['armor','meleeweapon','rangeweapon','helm','ibitem']:
 rows=(s/f'settings/item/001/{name}.txt').read_bytes().splitlines();h=rows[0].split(b'\t')
 print(name,'HEADER',[(i,x.decode('latin1')) for i,x in enumerate(h)])
 if name=='ibitem':print('SEAL',dict(zip([x.decode('latin1') for x in h],[decode_tcvn3(x) for x in rows[487].split(b'\t')])));continue
 n=0
 for row in rows[1:]:
  c=row.split(b'\t');d=dict(zip(h,c));level=d.get(b'RequireLevel',d.get(b'Level',b''))
  if c and (b'80'==level or b'100'==level):
   print('CANDIDATE',[(h[i].decode('latin1'),decode_tcvn3(x)) for i,x in enumerate(c) if x and x!=b'0'][:27]);n+=1
   if n>=3:break
o=Original();rows=o.get(b'\\settings\\Npcs.txt')[0].splitlines();h=rows[0].split(b'\t')
for i,row in enumerate(rows[1:]):
 d=dict(zip(h,row.split(b'\t')));name=d.get(b'Name',b'').decode('gbk','replace')
 if any(w in name for w in ['旗','图腾','统帅','元帅','斥候','将军']): print('NPC',i,name,d.get(b'Kind'),d.get(b'Camp'))
