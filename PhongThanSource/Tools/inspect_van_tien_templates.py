import json
from restore_original_world import ROOT, OUT, Original
o=Original(); lines=o.get(b'\\settings\\Npcs.txt')[0].splitlines(); h=lines[0].split(b'\t')
for i in list(range(124,150))+[247]:
 r=dict(zip(h,lines[i+1].split(b'\t')))
 print(i,{k.decode():r.get(k,b'').decode('gbk','replace') for k in [b'Name',b'Kind',b'Camp',b'DeathScript',b'ActionScript']})
worlds=dict((int(a.split(b'=')[0]),a.split(b'=',1)[1]) for a in (ROOT/'PhongThanRuntime-Loose/Server/settings/WorldSet.ini').read_bytes().splitlines() if a[:1].isdigit() and b'=' in a)
print('MAPS',[(i,worlds.get(i,b'').decode('gbk')) for i in range(1067,1071)])
