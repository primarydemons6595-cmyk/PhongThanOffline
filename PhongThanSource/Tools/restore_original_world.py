"""Recover original world evidence and stage exact original Lua; publication is separate.

PAKs are read only. Every available package is scanned by payload structure, not
only guessed names. Unknown placement ownership is never assigned to a map.
"""
from __future__ import annotations
import argparse, collections, csv, json, re, struct
from pathlib import Path
import build_offline_loose as b
from prepare_npc_restoration import split_region, npc_records
from prepare_npc_lua_library import dependencies

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'analysis/restoration_20261016'


def path_bytes(value):
    # Some saved deployment paths were serialized from UTF-8 viewed as CP1252.
    try:
        mixed=b''.join(bytes([ord(c)]) if ord(c)<256 else c.encode('cp1252') for c in value)
        repaired=mixed.decode('utf8')
        if any('\u3400' <= c <= '\u9fff' for c in repaired):
            return repaired.encode('gbk')
    except UnicodeError:
        pass
    for encoding in ('cp1252', 'latin1'):
        try:
            repaired=value.encode(encoding).decode('utf8')
            if any('\u3400' <= c <= '\u9fff' for c in repaired):
                return repaired.encode('gbk')
        except UnicodeError:
            pass
    try:
        return value.encode('gbk')
    except UnicodeEncodeError:
        return value.encode('latin1')


class Original:
    def __init__(self):
        root = ROOT / 'Client_Original'
        self.listed = [root / 'data' / m.group(1).strip() for m in
                       re.finditer(r'^\d+=(.+)$', (root/'package.ini').read_text('ascii'), re.M)]
        self.missing = [str(p) for p in self.listed if not p.exists()]
        self.winners, self.entries = b.pak_index([p for p in self.listed if p.exists()], strict=False)
        self.cache = {}

    def get(self, raw):
        raw = b'\\' + raw.replace(b'/', b'\\').lstrip(b'\\')
        eid = b.pak_id(raw)
        if eid not in self.winners:
            return None
        if eid not in self.cache:
            e = self.winners[eid]
            data, _ = b.entry_payload(e, {}, None)
            self.cache[eid] = data, dict(id=f'{eid:08X}', pak=e['pak'].name,
                entry=e['entry'], sha256=b.sha(data), bytes=len(data),
                path=raw.decode('gbk', 'replace'), raw_hex=raw.hex())
        return self.cache[eid]


def valid_records(payload, template_count=2703):
    records = npc_records(payload)
    if not records:
        return []
    result = []
    for r in records:
        tid, x, y = struct.unpack_from('<iii', r)
        level, kind = struct.unpack_from('<h', r, 48)[0], struct.unpack_from('<h', r, 54)[0]
        if not (0 <= tid < template_count and 0 <= x < 10000000 and 0 <= y < 10000000
                and 0 < level <= 1000 and 0 <= kind <= 3):
            raise ValueError('NPC fields outside native contract')
        result.append(dict(template=tid, x=x, y=y, level=level, kind=kind,
            special=r[12], name=r[16:48].split(b'\0')[0].decode('gbk','replace'),
            script=r[60:].rstrip(b'\0').decode('gbk','replace'), raw_hex=r.hex()))
    return result


def scan(original):
    found, failures = [], collections.Counter()
    for number, e in enumerate(original.entries):
        if not 12 <= e['size'] <= 16*1024*1024 or e['flags'] >> 24 not in (0, 1):
            continue
        try:
            data, _ = b.entry_payload(e, {}, None)
        except (ValueError, MemoryError) as exc:
            failures[str(exc)] += 1
            continue
        records, kind = [], ''
        try:
            if len(data) >= 52 and 6 <= struct.unpack_from('<I',data)[0] <= 64:
                _, _, payload = split_region(data)
                records, kind = valid_records(payload), 'REGION_NPC_SECTION'
            elif len(data) >= 72 and 0 < struct.unpack_from('<I',data)[0] <= (len(data)-12)//60:
                records, kind = valid_records(data), 'STANDALONE_NPC_SECTION'
        except (ValueError, struct.error):
            pass
        if records:
            winner = original.winners.get(e['id'])
            found.append(dict(id=f"{e['id']:08X}", pak=e['pak'].name, entry=e['entry'],
                sha256=b.sha(data), size=len(data), kind=kind, count=len(records),
                selected_by_available_chain=(winner == e), records=records))
        if number % 10000 == 0:
            print(f'scanned={number}/{len(original.entries)} populated_candidates={len(found)}',flush=True)
    (OUT/'placement_payloads.json').write_text(json.dumps(dict(missing_packages=original.missing,
        entries=len(original.entries), populated=found, decode_failures=dict(failures)),ensure_ascii=False,indent=2),'utf8')
    print('PLACEMENT',len(found),sum(x['count'] for x in found),flush=True)


def stage(original):
    project = ROOT/'PhongThanSource'
    deployment = json.loads((project/'Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))
    library = json.loads((project/'Docs/NPC_LUA_LIBRARY.json').read_text('utf8'))
    work = json.loads((project/'Docs/NPC_LUA_IMPLEMENTATION_WORKLIST.json').read_text('utf8'))
    metadata=(ROOT/'PhongThanRuntime-Loose/Server/settings/WorldSet.ini').read_bytes()
    worlds={int(m[0]):m[1] for m in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',metadata)}
    paths=set()
    for e in library['entries']:
        try: paths.add(e['path'].encode('gbk'))
        except UnicodeError: pass
    candidates=[]
    for npc in deployment['rows']:
        proposed=set()
        bound_ids=set()
        for field in ('lua_original','lua'):
            p=(npc.get(field) or {}).get('logical','')
            if p and 'phongthan' not in p:
                proposed.add(path_bytes(p))
                bound_ids.add(b.pak_id(path_bytes(p)))
        name=npc.get('original_name')
        if name:
            encoded=path_bytes(name)
            proposed.update(p for p in paths if p.rsplit(b'\\',1)[-1] == encoded+b'.lua')
            if npc['world'] in worlds:
                proposed.add(b'\\script\\'+worlds[npc['world']]+b'\\'+encoded+b'.lua')
        matches=[]
        for raw in sorted(proposed):
            try: result=original.get(raw)
            except ValueError: continue
            if result and re.search(rb'function\s+main\s*\(',result[0]):
                matches.append(result[1])
        expected_scope=worlds.get(npc['world'],b'')
        accepted=[m for m in matches if int(m['id'],16) in bound_ids or
            bytes.fromhex(m['raw_hex']).split(b'\\')[2:3] == [expected_scope]]
        accepted=list({m['id']:m for m in accepted}.values())
        candidates.append(dict(world=npc['world'],name=npc['name'],status=npc.get('lua_status'),
            current=npc.get('lua',{}).get('logical'),original_name=name,matches=matches,
            accepted=accepted if len(accepted)==1 else [],
            identity_rule='EXISTING_ORIGINAL_BINDING_OR_EXACT_MAP_DIRECTORY; ambiguous remains unresolved'))
    queue=[]
    schedule=original.get(b'\\settings\\systemtimetask.txt')
    schedule_rows=[]
    for line in schedule[0].splitlines()[1:]:
        cells=line.split(b'\t')
        if len(cells)<3: continue
        try: hour,minute=int(cells[0]),int(cells[1])
        except ValueError: continue
        if not (0<=hour<24 and 0<=minute<60): continue
        raw=cells[2].strip()
        schedule_rows.append(dict(hour=hour,minute=minute,path=raw.decode('gbk','replace'),raw_hex=raw.hex()))
        queue.append(raw)
    for c in candidates:
        for e in c['matches']: queue.append(bytes.fromhex(e['raw_hex']))
    templates=original.get(b'\\settings\\Npcs.txt')[0].splitlines()
    headers=templates[0].split(b'\t')
    for line in templates[1:]:
        row=dict(zip(headers,line.split(b'\t')))
        for column in (b'ActionScript',b'DeathScript',b'TimerScript',b'LevelScript'):
            raw=row.get(column,b'').strip()
            if raw: queue.append(raw)
    # The actual original startup/timer are discovery sources, never blindly deployed.
    for raw in (b'\\script\\serverscript.lua',b'\\script\\servertimer.lua'):
        result=original.get(raw)
        if result:
            (OUT/('original_'+raw.rsplit(b'\\',1)[-1].decode('ascii'))).write_bytes(result[0])
    recovered,missing={},set()
    while queue:
        raw=b'\\'+queue.pop().replace(b'/',b'\\').lstrip(b'\\')
        eid=b.pak_id(raw)
        if eid in recovered or raw in missing: continue
        try: result=original.get(raw)
        except ValueError: result=None
        if not result: missing.add(raw);continue
        data,proof=result
        deps,dynamic=dependencies(data)
        proof.update(dependencies=deps,dynamic_dependencies=dynamic)
        recovered[eid]=(data,proof)
        for d in deps: queue.append(d['path'].encode('gbk'))
    for eid,(data,proof) in recovered.items():
        relative=f'script/phongthan/original_recovery/original_{eid:08x}.lua'
        proof['operational_path']=relative
        target=OUT/'staged/Server'/relative
        target.parent.mkdir(parents=True,exist_ok=True)
        target.write_bytes(data)
    report=dict(missing_packages=original.missing,schedule_source=schedule[1],schedule=schedule_rows,
        npc_candidates=candidates,recovered=[proof for data,proof in recovered.values()],
        missing=[raw.decode('gbk','replace') for raw in sorted(missing)],
        scope='EXACT_AVAILABLE_ORIGINAL_BYTES; NOT_FUNCTIONAL_ACCEPTANCE')
    (OUT/'script_recovery.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
    print('SCRIPTS',len(recovered),'MISSING',len(missing),'NPC_MATCHES',sum(bool(c['matches']) for c in candidates),flush=True)


def prepare_changes(original):
    report=json.loads((OUT/'script_recovery.json').read_text('utf8'))
    recovered={e['id']:e for e in report['recovered']}
    staged=OUT/'staged/Server'
    changes=[]
    for npc in report['npc_candidates']:
        if not npc['accepted'] or npc['status']!='AUTHORED_GUIDE_WITH_ORIGINAL_BRANCH': continue
        relative=npc['current'].lstrip('\\').replace('\\','/')
        current=ROOT/'PhongThanRuntime-Loose/Server'/relative
        data=current.read_bytes()
        marker=b'pt_original_main = main'
        if data.count(marker)!=1: continue
        original_data=original.get(bytes.fromhex(npc['accepted'][0]['raw_hex']))[0]
        if original_data not in data: continue
        before=rb'function main\(\)\r?\n    if pt_guard\(\) == 0 then return end\r?\n    Say\([^\r\n]+\)\r?\nend'
        replacement=b'function main()\n    if pt_guard() == 0 then return end\n    return pt_original_main()\nend'
        result,count=re.subn(before,replacement,data)
        if count!=1: continue
        result=result.replace(marker,marker+b'\n-- Original callbacks use content map IDs; guards retain runtime IDs.\n'
            b'pt_runtime_world_pos = GetWorldPos\nGetWorldPos = GetContentWorldPos\n'
            b'SubWorldIdx2ID = SubWorldIdx2ContentID\n',1)
        result=result.replace(b'local w = GetWorldPos()',b'local w = pt_runtime_world_pos()')
        target=staged/relative;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(result)
        changes.append(dict(relative=relative,world=npc['world'],name=npc['name'],
            before_sha256=b.sha(data),sha256=b.sha(result),original=npc['accepted'][0],
            adaptation='direct original main; explicit content-map API; existing quest fixes preserved'))
    templates=original.get(b'\\settings\\Npcs.txt')[0].splitlines()
    headers=templates[0].split(b'\t')
    boss=[]
    for row in report['schedule']:
        eid=f"{b.pak_id(bytes.fromhex(row['raw_hex'])):08X}"
        if 'boss' not in row['path'].lower() or eid not in recovered: continue
        proof=recovered[eid];data=original.get(bytes.fromhex(row['raw_hex']))[0]
        flag=re.search(rb'GetGlobalValue\((\d+)\)\s*==\s*\(?\s*-1',data)
        world=re.search(rb'SubWorldID2Idx\((\d+)\)',data)
        spawn=re.search(rb'AddNpc\((\d+),(\d+),id,(\d+)\*32,(\d+)\*32\)',data)
        if not (flag and world and spawn): continue
        tid,level,x,y=map(int,spawn.groups());slot=int(flag[1]);content=int(world[1])
        template=dict(zip(headers,templates[tid+1].split(b'\t')))
        death=original.get(template[b'DeathScript'])
        if not death or not re.search(rb'SetGlobalValue\(\s*'+str(slot).encode()+rb'\s*,\s*-1\s*\)',death[0]):continue
        boss.append(dict(**row,id=eid,flag=slot,template=tid,level=level,content_map=content,
            x=x*32,y=y*32,death=death[1],source=proof))
    unique_boss={row['id']:row for row in boss}
    folder=staged/'settings/phongthan';folder.mkdir(parents=True,exist_ok=True)
    # Original client schedules prove the old payload, not applicability to the
    # requested Khai Minh Dien 2026 server. Keep review fixtures but do not run
    # them on that server until its schedule and lifecycle are established.
    (folder/'OriginalWorldSchedule.ini').write_bytes(b'[Schedule]\r\nEnabled=0\r\nUtcOffsetMinutes=420\r\n')
    (folder/'OriginalWorldSchedule.txt').write_bytes(b'Hour\tMinute\tScript\r\n'+b''.join(
        f"{row['hour']}\t{row['minute']}\t".encode()+bytes.fromhex(row['raw_hex'])+b'\r\n' for row in boss))
    # Retain the explicitly published project schedule (user authorized working
    # event rules from other versions on 2026-10-17), including restored VTT.
    compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled/settings/phongthan'
    for name in ('OriginalWorldSchedule.ini','OriginalWorldSchedule.txt'):
        if (compiled/name).is_file(): (folder/name).write_bytes((compiled/name).read_bytes())
    (folder/'OriginalBossState.txt').write_bytes(b'Flag\tTemplate\tContentMap\r\n'+b''.join(
        f"{row['flag']}\t{row['template']}\t{row['content_map']}\r\n".encode() for row in unique_boss.values()))
    result=dict(npc_changes=changes,boss_schedule=boss,bosses=list(unique_boss.values()),
        schedule_total=len(report['schedule']),blocked_schedule_rows=len(report['schedule'])-len(boss),
        blocked_reason='original source/dependency/mission lifecycle not available or not verified',
        target_server='PROJECT_FUNCTIONAL_RESTORATION',
        schedule_enabled=b'Enabled=1' in (folder/'OriginalWorldSchedule.ini').read_bytes(),
        schedule_applicability='PUBLISHED_PROJECT_RULES' if (compiled/'OriginalWorldSchedule.ini').is_file() else 'UNVERIFIED_FOR_TARGET_SERVER',
        static_placement_complete=False,full_functional_acceptance=False)
    (OUT/'prepared_changes.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    print('PREPARED npc_direct_original=',len(changes),'bosses=',len(unique_boss),'schedule_rows=',len(boss),flush=True)


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--scan',action='store_true')
    args=parser.parse_args()
    OUT.mkdir(parents=True,exist_ok=True)
    original=Original()
    stage(original)
    prepare_changes(original)
    if args.scan: scan(original)
