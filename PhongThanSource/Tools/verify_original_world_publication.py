"""Verify deployed bytes, original placement preservation and moved NPC count."""
import json,struct
from restore_original_world import ROOT,OUT
from publish_original_world import parse_index
from prepare_npc_restoration import npc_records,split_region
import build_offline_loose as b


server=ROOT/'PhongThanRuntime-Loose/Server'
published=json.loads((OUT/'publication.json').read_text('utf8'))
index=parse_index((server/'loose_index.bin').read_bytes())
for c in published['changes']:
    p=server/c['relative']
    assert b.sha(p.read_bytes())==c['sha256'],c['relative']
    eid=b.pak_id(('\\'+c['relative'].replace('/','\\')).encode('ascii'))
    assert index[eid].decode('ascii').lstrip('\\').replace('\\','/')==c['relative']
originals=json.loads((OUT/'placement_payloads.json').read_text('utf8'))['populated']
manifest={int(r['id'],16):r for r in b.csv_read(ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv') if r.get('id')}
preserved=[]
size_candidates={r['size'] for r in originals if r['selected_by_available_chain']}
disk_hashes={}
for p in (server/'maps').rglob('*region_s.dat'):
    if p.stat().st_size in size_candidates:disk_hashes.setdefault(b.sha(p.read_bytes()),p)
for row in originals:
    if not row['selected_by_available_chain']:continue
    raw=index[int(row['id'],16)]
    # Manifest stores the actual Unicode disk name; legacy index names retain
    # ANSI bytes and depend on the engine's filename conversion on this host.
    p=server/manifest[int(row['id'],16)]['path'].lstrip('\\').replace('\\','/')
    if not p.exists():p=disk_hashes[row['sha256']]
    assert b.sha(p.read_bytes())==row['sha256'],str(p)
    preserved.append(row)
needle=b'\\script\\phongthan\\npc_restore\\1002_03.lua'
matches=[]
for p in (server/'settings/phongthan/npc_regions/1002').rglob('*.dat'):
    data=p.read_bytes();payload=data if p.name.endswith('_npc_s.dat') else split_region(data)[2]
    for r in npc_records(payload):
        if r[60:].rstrip(b'\0')==needle:
            wx,wy=struct.unpack_from('<ii',r,4);matches.append(dict(file=str(p.relative_to(server)),mps=[wx,wy],display=[wx//256,wy//512]))
assert len(matches)==1 and matches[0]['display']==[206,193],matches
assert b'Enabled=0' in (server/'settings/phongthan/OriginalWorldSchedule.ini').read_bytes()
assert b.sha((server/'CoreServer.dll').read_bytes())==published['core_sha256']
result=dict(status='PASS',published_files=len(published['changes']),direct_original_npc_scripts=published['npc_direct_original'],
    exact_original_placement_files_preserved=len(preserved),original_records_preserved=sum(r['count'] for r in preserved),
    coordinate_actor=matches[0],unverified_legacy_schedule_enabled=False,full_restoration=False)
restart=json.loads((OUT/'restart.json').read_text('utf-8-sig'))
ready=(server/'native_world_ready.log').read_bytes()[restart['ReadyOffset']:].decode('ascii')
assert 'WORLD_READY bishop_pid=90444 service=1001 maps=102' in ready
result.update(game_server_pid=restart['Pid'],world_ready_maps=102,world_ready_marker=ready.strip())
(OUT/'verification.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
print(json.dumps(result,ensure_ascii=True))
