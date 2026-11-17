from pathlib import Path
import hashlib,json,struct

ROOT=Path(__file__).resolve().parents[2]
P=ROOT/'PhongThanSource/Deploy/ProjectContent/VttBoss20261029'
manifest=json.loads((P/'manifest.json').read_text('utf8'))
checks=0
def check(ok,label):
    global checks
    assert ok,label
    checks+=1
def table(data):
    rows=[r.split(b'\t') for r in data.splitlines()]
    return rows[0],[dict(zip(rows[0],r)) for r in rows[1:]]

for e in manifest['files']:
    check(hashlib.sha256((P/'compiled'/e['role']/e['relative']).read_bytes()).hexdigest()==e['sha256'],e['relative'])
for side in ['Server','Client']:
    rel='settings/phongthan/Npcs.txt'
    _,old=table((P/'source'/side/rel).read_bytes())
    _,new=table((P/'compiled'/side/rel).read_bytes())
    changed={133,138,143,148,410,413,414,415}
    check(len(old)==len(new),'NPC count')
    for n,(a,b) in enumerate(zip(old,new)):
        if n not in changed:check(a==b,'Unrelated NPC %d'%n)
    for tid in [133,138,143,148]:
        check(new[tid][b'AIMode']==b'1','Active boss AI')
        for slot in range(1,5):
            check(int(new[tid][('Skill%d'%slot).encode()])>0,'Numeric boss skill')
    for tid in [410,413,414,415]:
        check(new[tid][b'Kind']==b'4' and new[tid][b'AIMode']==b'0','Static chest spawn')
        for key in old[tid]:
            if key not in [b'Kind',b'AIMode']:check(old[tid][key]==new[tid][key],'Preserve chest loot/stats')
    for e in manifest['files']:
        if e['role']!=side or not e['relative'].startswith('settings/npcres/'):continue
        h,rows=table((P/'compiled'/side/e['relative']).read_bytes())
        for row in rows:
            if not row[h[0]].startswith(b'pt_vtt_'):continue
            if b'ResFilePath' in row:
                check(row[b'ResFilePath']==row[h[2]]==b'\\spr\\phongthan\\vtt','Effective resource path')
            else:
                for key in h[1:]:check(bool(row[key]),'Visible action '+repr(key))
for p in (ROOT/'PhongThanRuntime-Loose/Client/spr/phongthan/vtt').glob('*.spr'):
    raw=p.read_bytes();check(raw[:4]==b'SPR\0' and struct.unpack_from('<H',raw,12)[0]==1,'Single original frame')
    check(len(raw)>1000,'Nonempty pixel payload')
report=dict(result='PASS',checks=checks,files=len(manifest['files']),bossSkills=manifest['skills'],chests=4,lootSeconds=600)
out=ROOT/'analysis/vtt_boss_20261029/content_verification.json';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(report,indent=2),'utf8')
print('VTT_CONTENT_PASS',checks,'checks')
