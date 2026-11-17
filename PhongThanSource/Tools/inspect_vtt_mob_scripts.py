from restore_original_world import Original
o=Original();lines=o.get(b'settings/Npcs.txt')[0].splitlines();header=lines[0].split(b'\t')
for tid in range(53,61):
 row=dict(zip(header,lines[tid+1].split(b'\t')))
 print(tid,{k.decode('latin1'):v.decode('latin1') for k,v in row.items() if b'Script' in k or k in (b'Name',b'Kind',b'Level')})
 for key,value in row.items():
  if b'Death' in key and value:
   got=o.get(value)
   if got:print('DEATH',got[1]['id'],got[0].decode('latin1')[:2200])
