"""Read real Main gameplay logs only. Never spawn, create, pick up or mutate items."""
from pathlib import Path
from collections import Counter,defaultdict
import json
root=Path(__file__).resolve().parents[2];out=root/'analysis/native_drop_final_20261018'
deployment=json.loads((out/'deployment.json').read_text('utf-8-sig'));since=deployment['Utc'][:19]
server=root/'PhongThanRuntime-Loose/Server';client=root/'PhongThanRuntime-Loose/Client'
def read(path):
    if not path.exists():return []
    result=[]
    for line in path.read_text('utf8',errors='replace').splitlines():
        try:result.append(json.loads(line))
        except (ValueError,TypeError):continue
    return result
events=[]
for name in ['legacy_drop_debug.jsonl.4','legacy_drop_debug.jsonl.3','legacy_drop_debug.jsonl.2','legacy_drop_debug.jsonl.1','legacy_drop_debug.jsonl']:
    events += [e for e in read(server/name) if e.get('utc','')[:19]>=since]
deaths=defaultdict(list)
for e in events:
    if e.get('death_id'):deaths[e['death_id']].append(e)
trace=defaultdict(list)
for folder in [server,client]:
    for e in read(folder/'p1_blue_magic.jsonl'):
        if e.get('serial'):trace[str(e['serial'])].append(e)
items=[]
for death,rows in deaths.items():
    for made in [e for e in rows if e.get('event')=='ITEM_CREATE' and e.get('result')=='CREATED']:
        serial=str(made['serial']);related=trace[serial];names=Counter(e.get('event') for e in related)
        original=[]
        for n in range(20):
            fields=made.get('magic_'+str(n),'0:0,0,0').replace(':',',').split(',')
            original.append([int(v) for v in fields])
        mismatches=[dict(event=e.get('event'),pid=e.get('pid')) for e in related if e.get('magic') is not None and
                    (e['magic']!=original or int(e.get('quality',made['quality']))!=int(made['quality']) or int(e.get('level',made['actual_native_level']))!=int(made['actual_native_level']))]
        spawned=any(e.get('event')=='GROUND_SPAWN' and str(e.get('serial'))==serial and e.get('result')=='SPAWNED' for e in rows)
        picked=any(e.get('event')=='ITEM_PICKUP' and str(e.get('serial'))==serial and e.get('result')=='PICKED_UP' for e in rows)
        items.append(dict(death_id=death,serial=serial,quality=made['quality'],native_level=made.get('actual_native_level'),required_level=made.get('required_player_level'),
                          ground_spawned=spawned,picked_up=picked,trace_events=dict(names),attribute_mismatches=mismatches,
                          created_pid=str(made.get('pid')),server_load_pids=sorted({str(e.get('pid')) for e in related if e.get('event')=='SERVER_LOAD_RESTORED'}),
                          loaded_by_restarted_gameserver=any(e.get('event')=='SERVER_LOAD_RESTORED' and str(e.get('pid'))!=str(made.get('pid')) for e in related),
                          status='ATTRIBUTE_MISMATCH' if mismatches else 'REAL_LOG_PRESENT_REVIEW_F4_RELOG_RESTART' if picked else 'AWAITING_REAL_PICKUP'))
result=dict(status='AWAITING_PLAYER_GAMEPLAY' if not deaths else 'REAL_GAMEPLAY_LOGS_AVAILABLE_NOT_AUTOMATIC_FULL_PASS',since_utc=deployment['Utc'],
    npc_deaths=sum(e.get('event')=='NPC_DEATH' for e in events),events=dict(Counter(e.get('event') for e in events)),items=items,
    errors=[e for e in events if e.get('event') in ['DROP_ERROR','QUARANTINE_DETAIL']],
    blue_policy_blocks=[e for e in events if e.get('event')=='BLUE_SKIP' and e.get('reason') not in ['OFF','NON_EQUIPMENT_UNCHANGED','CATALOG_MAGIC_PRESERVED']],
    spawn_or_item_creation_by_collector=False)
(out/'gameplay_evidence.json').write_text(json.dumps(result,ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(dict(status=result['status'],npc_deaths=result['npc_deaths'],items=len(items),errors=len(result['errors']))))
