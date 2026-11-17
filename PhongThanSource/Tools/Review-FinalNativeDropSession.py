"""Analyze frozen real Main gameplay, committed companions and cold-copy evidence."""
from pathlib import Path
from collections import Counter,defaultdict
import hashlib,json,struct
root=Path(__file__).resolve().parents[2];out=root/'analysis/native_drop_final_20261018/gameplay_review'
def load(path):
    if not path.exists():return []
    result=[]
    for line in path.read_text('utf8',errors='replace').splitlines():
        try:result.append(json.loads(line))
        except ValueError:continue
    return result
events=[]
for path in (out/'logs/Server').glob('legacy_drop_debug.jsonl*'):
    events += [e for e in load(path) if e.get('pid')=='57432']
events.sort(key=lambda e:int(e.get('log_seq','0')))
deaths={e['death_id']:e for e in events if e['event']=='NPC_DEATH'}
bydeath=defaultdict(list)
for e in events:
    if e.get('death_id'):bydeath[e['death_id']].append(e)
created=[e for e in events if e['event']=='ITEM_CREATE' and e.get('result')=='CREATED']
trace=defaultdict(list)
for side in ['Server','Client']:
    for e in load(out/'logs'/side/'p1_blue_magic.jsonl'):trace[str(e['serial'])].append(e)
items=[]
for e in created:
    serial=str(e['serial']);rows=bydeath[e['death_id']];magic=[[int(v) for v in e.get('magic_'+str(n),'0:0,0,0').replace(':',',').split(',')] for n in range(20)]
    related=trace[serial];mismatches=[dict(event=t['event'],pid=t['pid']) for t in related if t.get('magic')!=magic or int(t['quality'])!=int(e['quality']) or int(t['level'])!=int(e['actual_native_level'])]
    items.append(dict(serial=serial,death_id=e['death_id'],npc=deaths[e['death_id']]['template'],genre=e['genre'],detail=e['detail'],particular=e['particular'],level=e['actual_native_level'],required_level=e['required_player_level'],quality=e['quality'],magic=magic,
        spawned=any(r['event']=='GROUND_SPAWN' and str(r.get('serial'))==serial and r.get('result')=='SPAWNED' for r in rows),
        picked=any(r['event']=='ITEM_PICKUP' and str(r.get('serial'))==serial and r.get('result')=='PICKED_UP' for r in rows),
        traces=dict(Counter(t['event'] for t in related)),client_pids=sorted({t['pid'] for t in related if t['event']=='CLIENT_PROPERTIES_APPLIED'}),attribute_mismatches=mismatches))
committed=[];cold=Path((out/'cold_copy_root.txt').read_text())
for file in (out/'backup/CharacterStore.saved').glob('role_*.pthc'):
    raw=file.read_bytes();magic,version,header_size,state_size,checksum=struct.unpack_from('<IHHII',raw);state=raw[header_size:]
    digest=2166136261
    for value in state:digest=((digest^value)*16777619)&0xffffffff
    assert magic==0x43544850 and version==1 and state_size==len(state) and digest==checksum
    state_key=hashlib.sha256(state).hexdigest();folder=out/'backup/NativeDropPersistenceV1.saved';companion=(folder/(state_key+'.magic-v1')).read_bytes();required=(folder/(state_key+'.required')).read_bytes()
    assert required.decode()==hashlib.sha256(companion).hexdigest()
    m,v,size,count,sh,ph=struct.unpack_from('<IIII64s64s',companion);payload=companion[144:]
    assert m==0x474d3050 and v==1 and size==748 and len(payload)==size*count and sh.decode()==state_key and ph.decode()==hashlib.sha256(payload).hexdigest()
    fight,states,tasks,item_count=struct.unpack_from('<4I',state,392);assert item_count==count
    item_offset=408+12*(fight+states)+20*tasks;role=bytes.fromhex(file.stem[5:]).decode('ascii')
    golden=(cold/(role+'.golden')).read_bytes();assert golden==payload,'Cold native restore differed from committed snapshot'
    entries=[]
    for n in range(count):
        record=payload[n*size:(n+1)*size];ordinal,serial,quality=struct.unpack_from('<IIi',record);assert ordinal==n
        vals=struct.unpack_from('<80i',record,12);attrs=[list(vals[j:j+4]) for j in range(0,80,4)]
        canonical=state[item_offset+n*268:item_offset+(n+1)*268];genre,detail,particular,level=struct.unpack_from('<i',canonical,8)[0],*struct.unpack_from('<3i',canonical,24)
        entries.append(dict(serial=str(serial),quality=quality,genre=genre,detail=detail,particular=particular,level=level,magic=attrs,canonical_item_sha256=hashlib.sha256(canonical).hexdigest()))
    committed.append(dict(role=role,revision=struct.unpack_from('<I',state,8)[0],items=count,state_sha256=state_key,cold_native_payload_byte_exact=True,entries=entries))
index={entry['serial']:entry for role in committed for entry in role['entries']}
for item in items:
    entry=index.get(item['serial']);item['present_in_committed_store']=entry is not None
    item['committed_magic_matches']=None if entry is None else entry['magic']==item['magic'] and entry['quality']==int(item['quality']) and entry['level']==int(item['level'])
counts=Counter(e['event'] for e in events);skips=Counter(e.get('reason') for e in events if e['event']=='BLUE_SKIP')
result=dict(scope='FROZEN_REAL_MAIN_GAMEPLAY_THROUGH_NATIVE_CLEAN_STOP',pid=57432,events=dict(counts),npc_deaths=dict(Counter(e['template'] for e in deaths.values())),
    profiles=dict(Counter(e.get('resolved_profile') for e in events if e['event']=='DROP_PROFILE')),items=items,committed_roles=committed,
    blue_roll_outcomes=dict(Counter(e.get('outcome') for e in events if e['event']=='BLUE_ROLL')),blue_skips=dict(skips),
    duplicate_created_serials=[s for s,n in Counter(e['serial'] for e in created).items() if n>1],
    native_then_fallback=[death for death,rows in bydeath.items() if any(e['event']=='DROP_END' for e in rows) and any(e['event']=='NORMAL_ENGINE_DISPATCH' for e in rows)],
    bad_death_completion=[death for death in deaths if sum(e['event']=='DROP_END' for e in bydeath[death])!=1],
    errors=[e for e in events if e['event'] in ['DROP_ERROR','QUARANTINE_DETAIL']],
    note='Cold-copy native restore is not proof of a post-restart GameServer/Client login; F4 visual acceptance remains separate.')
(out/'session_review.json').write_text(json.dumps(result,ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(dict(deaths=len(deaths),created=len(items),spawned=sum(i['spawned'] for i in items),picked=sum(i['picked'] for i in items),blue=[{k:i[k] for k in ['serial','npc','picked','committed_magic_matches','client_pids']} for i in items if i['quality']=='1'],blue_skips=result['blue_skips'],committed_items=sum(r['items'] for r in committed),attribute_mismatches=sum(len(i['attribute_mismatches']) for i in items),errors=len(result['errors']),duplicate_serials=result['duplicate_created_serials'],bad_completion=result['bad_death_completion'],native_then_fallback=result['native_then_fallback'])))
