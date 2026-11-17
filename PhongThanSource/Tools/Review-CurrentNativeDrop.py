"""Verify real Main drops and stable committed v1 snapshots; no gameplay mutations."""
from pathlib import Path
from collections import Counter, defaultdict
import datetime, hashlib, json, struct

root = Path(__file__).resolve().parents[2]
out = root / 'analysis/native_drop_final_20261018'
start = json.loads((out / 'latest_start.json').read_text('utf-8-sig'))
pid = str(start['GameServerPid'])
server = root / 'PhongThanRuntime-Loose/Server'
client = root / 'PhongThanRuntime-Loose/Client'
review = out / ('session_' + datetime.datetime.now().strftime('%Y%m%d_%H%M%S'))
review.mkdir()

def read(path):
    if not path.exists():
        return []
    rows = []
    for line in path.read_text('utf-8', errors='replace').splitlines():
        try:
            rows.append(json.loads(line))
        except ValueError:
            continue  # A concurrent writer may have an unfinished last line.
    return rows

events = []
for path in server.glob('legacy_drop_debug.jsonl*'):
    events += [e for e in read(path) if str(e.get('pid')) == pid]
events.sort(key=lambda e: int(e.get('log_seq', 0)))
(review / 'native.jsonl').write_text(''.join(json.dumps(e) + '\n' for e in events))
deaths = {e['death_id']: e for e in events if e['event'] == 'NPC_DEATH'}
bydeath = defaultdict(list)
for event in events:
    bydeath[event.get('death_id', '')].append(event)
trace = defaultdict(list)
packet_errors = []
for side, folder in [('Server', server), ('Client', client)]:
    rows = read(folder / 'p1_blue_magic.jsonl')
    previous = {}
    for event in rows:
        key = str(event.get('pid'))
        if event['event'] == 'SERVER_PROPERTIES_SEND':
            old = previous.get(key, {})
            if old.get('event') != 'SERVER_SNAPSHOT_SEND' or old.get('serial') != event.get('serial'):
                packet_errors.append(dict(pid=key, serial=event.get('serial'), seq=event.get('seq')))
        previous[key] = event
        trace[str(event.get('serial'))].append(event)
    (review / (side + '_trace.jsonl')).write_text(''.join(json.dumps(e) + '\n' for e in rows))

# Read a consistent canonical/companion pair without stopping the live world.
# A concurrent save retries instead of treating the moving state as corruption.
committed = {}
snapshots = []
snapshot_errors = []
for file in (server / 'CharacterStore').glob('role_*.pthc'):
    for attempt in range(3):
        raw = file.read_bytes()
        try:
            magic, version, hsize, size, checksum = struct.unpack_from('<IHHII', raw)
            state = raw[hsize:]
            fnv = 2166136261
            for value in state:
                fnv = ((fnv ^ value) * 16777619) & 0xffffffff
            assert (magic, version, hsize, size, checksum) == (0x43544850, 1, 16, len(state), fnv)
            key = hashlib.sha256(state).hexdigest()
            folder = server / 'NativeDropPersistenceV1'
            companion = (folder / (key + '.magic-v1')).read_bytes()
            required = (folder / (key + '.required')).read_text('ascii')
            assert required == hashlib.sha256(companion).hexdigest()
            m, v, entry_size, count, sh, ph = struct.unpack_from('<IIII64s64s', companion)
            payload = companion[144:]
            assert (m, v, entry_size) == (0x474d3050, 1, 748)
            assert sh.decode() == key and ph.decode() == hashlib.sha256(payload).hexdigest()
            assert len(payload) == count * entry_size
            fight, states, tasks, item_count = struct.unpack_from('<4I', state, 392)
            assert count == item_count
            offset = 408 + 12 * (fight + states) + 20 * tasks
            assert offset + count * 268 <= len(state)
            if file.read_bytes() != raw:
                continue
            (review / file.name).write_bytes(raw)
            (review / (key + '.magic-v1')).write_bytes(companion)
            for n in range(count):
                record = payload[n * entry_size:(n + 1) * entry_size]
                ordinal, serial, quality = struct.unpack_from('<IIi', record)
                assert ordinal == n and str(serial) not in committed
                attrs = [list(struct.unpack_from('<4i', record, 12 + j * 16)) for j in range(20)]
                reqs = [list(struct.unpack_from('<4i', record, 652 + j * 16)) for j in range(6)]
                canonical = state[offset + n * 268:offset + (n + 1) * 268]
                genre = struct.unpack_from('<i', canonical, 8)[0]
                detail, particular, level = struct.unpack_from('<3i', canonical, 24)
                committed[str(serial)] = dict(quality=quality, magic=attrs, requirements=reqs,
                    level=level, genre=genre, detail=detail, particular=particular,
                    seed=struct.unpack_from('<I', canonical, 44)[0],
                    canonical_sha256=hashlib.sha256(canonical).hexdigest(),
                    companion_entry_sha256=hashlib.sha256(record).hexdigest())
            snapshots.append(dict(state_sha256=key, item_count=count, retries=attempt))
            break
        except (AssertionError, OSError, struct.error) as error:
            if file.read_bytes() != raw:
                continue
            snapshot_errors.append(dict(file=file.name, reason=type(error).__name__))
            break
    else:
        snapshot_errors.append(dict(file=file.name, reason='CONCURRENT_SAVE_RETRY_EXHAUSTED'))

items = []
for made in (e for e in events if e['event'] == 'ITEM_CREATE' and e.get('result') == 'CREATED'):
    serial = str(made['serial'])
    related = trace[serial]
    magic = [[int(v) for v in made.get('magic_' + str(n), '0:0,0,0').replace(':', ',').split(',')] for n in range(20)]
    reqs = [[int(v) for v in made['require_' + str(n)].split(':')] for n in range(6)]
    mismatches = []
    for event in related:
        if (event.get('magic') != magic or int(event['quality']) != int(made['quality'])
                or int(event['level']) != int(made['actual_native_level'])
                or [x[:2] for x in event['requirements']] != reqs):
            mismatches.append(dict(event=event['event'], pid=event['pid'], seq=event['seq']))
    saved = committed.get(serial)
    saved_matches = None if saved is None else (
        saved['magic'] == magic and saved['quality'] == int(made['quality'])
        and saved['level'] == int(made['actual_native_level'])
        and [x[:2] for x in saved['requirements']] == reqs
        and saved['genre'] == int(made['genre']) and saved['detail'] == int(made['native_detail'])
        and saved['particular'] == int(made['native_particular']))
    rows = bydeath[made['death_id']]
    items.append(dict(serial=serial, death_id=made['death_id'], npc=deaths[made['death_id']]['template'],
        genre=made['genre'], detail=made['detail'], level=made['actual_native_level'], quality=made['quality'],
        spawned=any(e['event'] == 'GROUND_SPAWN' and str(e.get('serial')) == serial and e.get('result') == 'SPAWNED' for e in rows),
        picked=any(e['event'] == 'ITEM_PICKUP' and str(e.get('serial')) == serial and e.get('result') == 'PICKED_UP' for e in rows),
        magic=magic, requirements=reqs, trace_events=dict(Counter(e['event'] for e in related)),
        mismatches=mismatches, committed_matches=saved_matches,
        committed_generator_seed=None if saved is None else saved['seed'],
        seed_note='ITEM_CREATE native_seed is global RNG before generation, not the stored item seed; Gen_SkillBook explicitly zeros generator parameters.'))

result = dict(pid=pid, last_utc=events[-1].get('utc') if events else None,
    death_count=len(deaths), npc_counts=dict(Counter(e['template'] for e in deaths.values())),
    profiles=dict(Counter(e.get('resolved_profile') for e in events if e['event'] == 'DROP_PROFILE')),
    created=len(items), spawned=sum(i['spawned'] for i in items), picked=sum(i['picked'] for i in items),
    items=items, snapshots=snapshots, snapshot_errors=snapshot_errors,
    duplicate_serials=[s for s, n in Counter(i['serial'] for i in items).items() if n != 1],
    bad_death_completion=[d for d in deaths if sum(e['event'] == 'DROP_END' for e in bydeath[d]) != 1],
    native_then_fallback=[d for d in deaths if any(e['event'] == 'DROP_END' for e in bydeath[d]) and any(e['event'] == 'NORMAL_ENGINE_DISPATCH' for e in bydeath[d])],
    errors=[e for e in events if e['event'] in ('DROP_ERROR', 'QUARANTINE_DETAIL')],
    ground_failures=[e for e in events if e['event'] == 'GROUND_SPAWN' and e.get('result') != 'SPAWNED'],
    blue_skips=dict(Counter(e.get('reason') for e in events if e['event'] == 'BLUE_SKIP')),
    packet_order_errors=packet_errors, kingsoft_parity='UNVERIFIED', world_modified=False,
    note='No proof of F4 visual inspection/equip actions; absent snapshot item may have been consumed or removed.')
(review / 'review.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
(out / 'latest_session_review.json').write_text(json.dumps({'path': str(review / 'review.json')}, indent=2))
print(json.dumps({k: v for k, v in result.items() if k not in ('items', 'snapshots')}))
print(json.dumps({'blue_items': [i for i in items if i['quality'] == '1']}))
