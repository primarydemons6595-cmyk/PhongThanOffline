"""Stage missing client-documented NPCs with original map-specific Lua.

No fabricated reward/menu scripts. Unknown appearances remain explicit pending
items; this batch installs only proven template/resource identities.
"""
import argparse,collections,json,re,struct
from pathlib import Path
from restore_original_world import ROOT,OUT,Original
from prepare_npc_restoration import npc_records,split_region,merge_region,standalone_npcs
from prepare_npc_lua_library import dependencies
from build_authored_npcs import place_in_tile
from extract_client_coordinate_dictionary import decode_tcvn3
import build_offline_loose as b


BATCH=OUT/'missing_npcs';STAGE=BATCH/'staged/Server'


def stage(input_name='missing_npc_batch.json'):
    BATCH.mkdir(exist_ok=True)
    original=Original();server=ROOT/'PhongThanRuntime-Loose/Server'
    report=json.loads((OUT/input_name).read_text('utf8'))
    worlds={int(a):c for a,c in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
    table=original.get(b'\\settings\\Npcs.txt')[0].splitlines();headers=table[0].split(b'\t')
    templates=[dict(zip(headers,row.split(b'\t'))) for row in table[1:]]
    changes={};groups={};ready=[];blocked=[];script_ids=set()
    existing=json.loads((ROOT/'PhongThanSource/Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))['rows']
    ledger_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
    published=json.loads(ledger_path.read_text('utf8'))['ready'] if ledger_path.exists() else []
    occupied=collections.defaultdict(list)
    for n in existing:
        if n.get('status')=='READY':occupied[n['world']].append((n['world_x'],n['world_y']))
    for n in published:occupied[n['world']].append((n['world_x'],n['world_y']))
    for n in report['rows']:
        if n['existing']:continue
        if any(p['world']==n['world'] and p['name']==n['name'] for p in published):continue
        reason=None
        if not n['world_registered']:reason='MAP_NOT_REGISTERED'
        elif len(n['positions'])!=1:reason='MULTIPLE_POSITIONS_REQUIRE_QUEST_LIFECYCLE'
        elif len(n['map_scripts'])!=1:reason='ORIGINAL_MAP_SCRIPT_MISSING_OR_AMBIGUOUS'
        elif len(n['templates'])!=1:reason='ORIGINAL_APPEARANCE_IDENTITY_UNRESOLVED'
        if reason:blocked.append(n|dict(blocker=reason));continue
        world=n['world'];tid=n['templates'][0];t=templates[tid];proof=n['map_scripts'][0]
        if (world,proof['id']) in script_ids:continue
        if t.get(b'TimerScript'):blocked.append(n|dict(blocker='TEMPLATE_HAS_TIMER_REQUIRES_REVIEW'));continue
        lua=original.get(bytes.fromhex(proof['raw_hex']))[0]
        deps,dynamic=dependencies(lua)
        missing=[d for d in deps if original.get(d['path'].encode('gbk')) is None]
        if missing or dynamic:
            blocked.append(n|dict(blocker='SCRIPT_DEPENDENCY_REQUIRES_REVIEW',dependencies=deps,dynamic=dynamic));continue
        x,y=n['positions'][0];wx,wy=x*256+128,y*512+256;rx,ry=wx//512,wy//1024
        scope=worlds[world];stem=f'settings/phongthan/npc_regions/{world}/v_{ry:03d}/{rx:03d}'
        relative=None;base=None;geometry=None
        for suffix in ('_region_s.dat','_npc_s.dat'):
            p=server/(stem+suffix)
            if p.exists():relative=stem+suffix;base=p.read_bytes();break
        original_region=None
        for suffix in (b'_S',b''):
            raw=b'\\maps\\'+scope+suffix+f'\\v_{ry:03d}\\{rx:03d}_region_s.dat'.encode()
            got=original.get(raw)
            if got:original_region=got[0];break
        if original_region is None:
            for encoding in ('latin1','cp1252'):
                try:p=server/'maps'/(scope.decode(encoding)+'_S')/f'v_{ry:03d}/{rx:03d}_region_s.dat'
                except UnicodeError:continue
                if p.exists():original_region=p.read_bytes();break
        if relative is None:
            if original_region is not None:relative=stem+'_region_s.dat';base=original_region
            else:relative=stem+'_npc_s.dat';base=standalone_npcs([])
        if relative.endswith('_region_s.dat'):geometry=base
        elif original_region is not None:geometry=original_region
        if geometry is None:
            raw=b'\\maps\\'+scope+f'\\v_{ry:03d}\\{rx:03d}_region_c.dat'.encode()
            got=original.get(raw)
            if got:geometry=got[0]
        if geometry is None:blocked.append(n|dict(blocker='DESTINATION_GEOMETRY_MISSING'));continue
        wor=original.get(b'\\maps\\'+scope+b'.wor')
        if not wor:blocked.append(n|dict(blocker='ORIGINAL_WOR_MISSING'));continue
        bounds=re.search(rb'(?im)^rect\s*=\s*(\d+),(\d+),(\d+),(\d+)',wor[0])
        left,top,right,bottom=map(int,bounds.groups())
        if not(left<=rx<=right and top<=ry<=bottom):blocked.append(n|dict(blocker='OUTSIDE_ORIGINAL_MAP'));continue
        plan=groups.setdefault(relative,dict(base=base,records=[]))
        current_records=npc_records(split_region(base)[2]) if relative.endswith('_region_s.dat') else npc_records(base)
        occ=occupied[world]+[struct.unpack_from('<ii',r,4) for r in current_records+plan['records']]
        try:wx,wy=place_in_tile(geometry,wx,wy,occ)
        except ValueError:blocked.append(n|dict(blocker='DOCUMENTED_DISPLAY_TILE_BLOCKED'));continue
        logical=f'\\script\\phongthan\\npc_recovered_batch\\{world}_{proof["id"].lower()}.lua'
        wrapper=lua+b'\n-- Exact original payload above. Explicit map-ID compatibility below.\npt_original_main = main\npt_runtime_world_pos = GetWorldPos\nGetWorldPos = GetContentWorldPos\nSubWorldIdx2ID = SubWorldIdx2ContentID\n'+f'''function main()
    local w = pt_runtime_world_pos()
    if w ~= {world} then CloseDialog(); return end
    return pt_original_main()
end
'''.encode('ascii')
        script_relative=logical.lstrip('\\').replace('\\','/');changes[script_relative]=wrapper
        rawname=n['name'].encode('gbk');script=logical.encode('ascii')
        assert len(rawname)<32 and len(script)<128
        record=struct.pack('<iiiB3x32shhhhBBH',tid,wx,wy,0,rawname,1,0,0,3,int(t[b'Camp'] or 0),int(t[b'Series'] or 0),len(script))+script
        plan['records'].append(record);occupied[world].append((wx,wy));script_ids.add((world,proof['id']))
        ready.append(n|dict(template=tid,world_x=wx,world_y=wy,override=relative,lua=logical,
            original_lua=proof,script_relative=script_relative,lua_sha256=b.sha(wrapper),
            appearance_status=n.get('appearance_status','ORIGINAL_NAMED_TEMPLATE_OR_ORIGINAL_PLACEMENT_RESOURCE'),
            business_status='ORIGINAL_SCRIPT_NOT_ALL_BUSINESS_LIVE_ACCEPTED'))
    for relative,plan in groups.items():
        changes[relative]=merge_region(plan['base'],plan['records']) if relative.endswith('_region_s.dat') else standalone_npcs(npc_records(plan['base'])+plan['records'])
    # Match wire names using the same reviewed TCVN3 table as the current build.
    display_rel='settings/phongthan/NpcDisplayNames.txt';display=(server/display_rel).read_bytes()
    sample=bytes(range(256));reverse={c:i for i,c in enumerate(decode_tcvn3(sample))}
    for n in ready:
        choices=[s for s in n['translated_names'] if s and all(ord(c)>=32 and not 127<=ord(c)<160 for c in s)]
        if choices:
            name=min(choices,key=lambda s:(len(s),s))
            translated=bytes(reverse.get(c,ord(c) if ord(c)<128 else 63) for c in name)
            display+=n['name'].encode('gbk')+b'\t'+translated+b'\r\n'
    if ready:changes[display_rel]=display
    files=[]
    for relative,data in changes.items():
        p=STAGE/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
        target=server/relative
        files.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None))
    result=dict(ready=ready,blocked=blocked,files=files,monsters_changed=0,full_restoration=False)
    (BATCH/'plan.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    (BATCH/'dispatch_fixtures.txt').write_text(''.join(str((STAGE/n['script_relative']).relative_to(ROOT)).replace('\\','/')+' '+str(n['world'])+'\n' for n in ready),'ascii')
    print(json.dumps(dict(ready=len(ready),blocked=len(blocked),files=len(files),blockers=dict(collections.Counter(n['blocker'] for n in blocked)))))


if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--input',default='missing_npc_batch.json');parser.add_argument('--batch',default='missing_npcs')
    args=parser.parse_args();BATCH=OUT/args.batch;STAGE=BATCH/'staged/Server';stage(args.input)
