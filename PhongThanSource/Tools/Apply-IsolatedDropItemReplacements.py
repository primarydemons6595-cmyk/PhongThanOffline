from pathlib import Path
import json,csv,hashlib,shutil,os,sys,datetime
workspace=Path(__file__).resolve().parents[2]
report=workspace/'analysis/item_replacements_20261018'
root=workspace/'PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_05d188ae7f02/Server'
assert (root.parent/'p2_player_test.flag').exists()
data=root/'settings/item/droprate';qpath=root/'settings/phongthan/LegacyDropQuarantine.txt'
backup=report/'isolated_before';plan=json.loads((report/'replacement_plan.json').read_text())
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def atomic(p,b):
 temp=p.with_name(p.name+'.itemfix.tmp');temp.write_bytes(b);os.replace(temp,p)
if '--rollback' in sys.argv:
 receipt=json.loads((report/'applied.json').read_text())
 for change in plan['files']:
  target=data/change['file'];assert sha(target)==change['after_sha256'],'A later edit exists; refuse overwrite'
 assert sha(qpath)==receipt['quarantine_after_sha256'],'Quarantine changed after patch'
 for change in plan['files']:atomic(data/change['file'],(backup/change['file']).read_bytes())
 atomic(qpath,(backup/'LegacyDropQuarantine.txt').read_bytes())
 print('ISOLATED_DATA_ROLLBACK_COMPLETE; no character/binary changes');sys.exit(0)
assert not backup.exists(),'Backup already exists; do not apply twice'
valid={r['file'] for r in csv.DictReader((report/'table_validation.csv').open()) if r['valid']=='1'}
plans=list(csv.DictReader((report/'npc_validation.csv').open()))
assert len([r for r in plans if r['npc']=='86' and r['ready']=='1'])==2,'Boss must pass both complete plans'
for change in plan['files']:
 name=change['file'];assert Path(name).name==name and name.endswith('.ini')
 assert sha(data/name)==change['before_sha256'],'Live isolated data changed since planning'
 assert sha(report/'proposed'/name)==change['after_sha256']
backup.mkdir()
for change in plan['files']:shutil.copy2(data/change['file'],backup/change['file'])
shutil.copy2(qpath,backup/'LegacyDropQuarantine.txt')
q=qpath.read_bytes();lines=q.splitlines(keepends=True);removed=[];kept=[]
for line in lines:
 value=line.strip()
 if value.startswith(b'T '):
  name=value[2:].split(b'\\')[-1].decode('ascii')
  if name in valid:removed.append(value.decode('ascii'));continue
 if value==b'N 86':removed.append('N 86');continue
 kept.append(line)
for change in plan['files']:atomic(data/change['file'],(report/'proposed'/change['file']).read_bytes())
# Release data guards last, after all physical table writes have committed.
atomic(qpath,b''.join(kept))
receipt={'scope':'ISOLATED_ONLY','applied_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'files':len(plan['files']),'rows':sum(len(v['sections']) for v in plan['files']),'removed_quarantine':removed,'retained_invalid_changed_tables':sorted(set(c['file'] for c in plan['files'])-valid),'quarantine_before_sha256':hashlib.sha256(q).hexdigest(),'quarantine_after_sha256':sha(qpath),'native_source_changed':False,'binary_changed':False,'client_restart_required':False,'quest_lua_changed':False,'rate_count_level_changes':False}
(report/'applied.json').write_text(json.dumps(receipt,indent=2))
print(json.dumps({k:v for k,v in receipt.items() if k!='removed_quarantine'},indent=2));print('Removed quarantine records:',len(removed))
