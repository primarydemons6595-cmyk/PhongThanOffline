"""Discover missing NPCs from the original client's map-specific quest links."""
import collections,json,re,struct,unicodedata,hashlib
from restore_original_world import ROOT,OUT,Original,path_bytes
import build_offline_loose as b
from extract_client_coordinate_dictionary import decode_tcvn3


def norm(value):
    value=re.sub(r'<[^>]*>','',value).lstrip('#')
    return re.sub('[^a-z0-9]+',' ',''.join(c for c in unicodedata.normalize('NFD',value.lower().replace('đ','d')) if not unicodedata.combining(c))).strip()


def run():
    original=Original();server=ROOT/'PhongThanRuntime-Loose/Server'
    links=json.loads((OUT/'client_hyperlink_coordinates.json').read_text('utf8'))
    library=json.loads((ROOT/'PhongThanSource/Docs/NPC_LUA_LIBRARY.json').read_text('utf8'))['entries']
    known_paths=set()
    for row in b.csv_read(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv'):
        if row['CanonicalPath'].lower().endswith('.lua'):
            try:known_paths.add(path_bytes(row['CanonicalPath']))
            except UnicodeError:pass
    deployed=json.loads((ROOT/'PhongThanSource/Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))['rows']
    recovered_path=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
    published=json.loads(recovered_path.read_text('utf8'))['ready'] if recovered_path.exists() else []
    localized={}
    for line in (OUT/'localize_vietnamese.expanded.txt').read_bytes().splitlines()[1:]:
        if b'\t' in line:
            key,value=line.split(b'\t',1);localized[key.decode('ascii','replace')]=decode_tcvn3(value)
    templates=original.get(b'\\settings\\Npcs.txt')[0].splitlines()
    header=templates[0].split(b'\t');templates=[dict(zip(header,line.split(b'\t'))) for line in templates[1:]]
    worlds={int(m[0]):m[1] for m in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
    registry={int(a):int(c) for a,c in re.findall(r'\{(\d+),\s*(\d+),\s*PHONGTHAN_MAP_',
        (ROOT/'PhongThanSource/Headers/PhongThanMapIdentity.h').read_text('ascii'))}
    portrait=original.get(b'\\settings\\npclink.txt')
    portraits=collections.defaultdict(list)
    for line in portrait[0].splitlines()[1:]:
        cells=line.split(b'\t')
        if len(cells)<4:continue
        name=norm(decode_tcvn3(cells[1]));res=re.search(rb'(passerby\d+)',cells[3],re.I)
        if res:portraits[name].append(dict(sprite=cells[3].decode('gbk','replace'),resource=res[1].lower().decode(),row=cells[0].decode()))
    source_records=json.loads((OUT/'placement_payloads.json').read_text('utf8'))['populated']
    source_names=collections.defaultdict(list)
    for p in source_records:
        for n in p['records']:
            if n['kind']==3:source_names[n['name']].append(n)
    grouped=collections.defaultdict(list)
    for row in links:
        if row['source']=='cn_dict.txt' and row['name'] and 80<=row['x']<=500 and 80<=row['y']<=500:
            grouped[row['content_map'],row['name']].append(row)
    for row in json.loads((OUT/'client_help_npc_coordinates.json').read_text('utf8')):
        key=row['content_map'],row['name']
        if key not in grouped:grouped[key].append(row)
    result=[]
    for (content,name),facts in sorted(grouped.items()):
        world=registry.get(content,0)
        positions=sorted({(r['x'],r['y']) for r in facts})
        rawname=name.encode('gbk');candidates=[]
        for tid,t in enumerate(templates):
            if t.get(b'Kind')==b'3' and t.get(b'Name')==rawname:candidates.append(tid)
        scopes={worlds[world]} if world in worlds else set()
        # WorldSet is authoritative. Do not infer renamed late-game maps from
        # their position in a different version's map list.
        paths={b'\\script\\'+scope+b'\\'+rawname+b'.lua' for scope in scopes}
        if world==1002:scopes.add('崇城'.encode('gbk'))
        common_alias={'卜卦师':'彩票/卦','西岐卜卦师':'彩票/卦','朝歌卜卦师':'彩票/卦',
            '应签商':'彩票/应签商','朝歌应签商':'彩票/应签商','星官':'彩票/二十八星宿',
            '西岐星官':'彩票/二十八星宿','朝歌星官':'彩票/二十八星宿','太岁师':'彩票/太岁',
            '赌场老板':'彩票/阴阳师'}
        explicit_alias=('\\script\\'+common_alias[name].replace('/','\\')+'.lua').encode('gbk') if content in (20,21) and name in common_alias else None
        if explicit_alias:paths.add(explicit_alias)
        alias_names={"俱留孙":"惧留孙","高级指导":"进阶指导"}
        search_name=alias_names.get(name,name).encode('gbk')
        for raw in known_paths:
            tail=raw.rsplit(b'\\',1)[-1]
            folder=raw.split(b'\\')[2]
            if tail.endswith(search_name+b'.lua') and folder in scopes or tail==rawname+b'.lua' or (rawname in tail and any(scope in tail for scope in scopes)):
                paths.add(raw)
        for e in library:
            try:raw=path_bytes(e['path'])
            except UnicodeError:continue
            if raw.rsplit(b'\\',1)[-1]==rawname+b'.lua':paths.add(raw)
        scripts=[]
        for raw in paths:
            found=original.get(raw)
            if found and re.search(rb'function\s+main\s*\(',found[0]):scripts.append(found[1])
        matching=[e for e in scripts if bytes.fromhex(e['raw_hex']).split(b'\\')[2] in scopes or
            bytes.fromhex(e['raw_hex'])==explicit_alias or
            (rawname in bytes.fromhex(e['raw_hex']).rsplit(b'\\',1)[-1] and any(scope in bytes.fromhex(e['raw_hex']).rsplit(b'\\',1)[-1] for scope in scopes))]
        translated=sorted({r['name'] for r in links if r['source']=='localize_vietnamese.lz' and r['content_map']==content
            and (r['x'],r['y']) in positions})
        direct_translation=localized.get(hashlib.shake_128(rawname).hexdigest(8).upper())
        if direct_translation and direct_translation not in translated:translated.append(direct_translation)
        if not candidates:
            for tid,t in enumerate(templates):
                if t.get(b'Kind')!=b'3':continue
                raw=t.get(b'Name',b'')
                cn=re.sub(r'<[^>]*>','',raw.decode('gbk','replace')).lstrip('#').strip()
                localized_template=localized.get(hashlib.shake_128(raw).hexdigest(8).upper(),decode_tcvn3(raw))
                if cn==name or any(norm(localized_template)==norm(v) for v in translated if v):candidates.append(tid)
        portrait_matches=[p for v in translated for p in portraits.get(norm(v),[])]
        res_types={p['resource'].encode() for p in portrait_matches}
        portrait_templates=[tid for tid,t in enumerate(templates) if t.get(b'Kind')==b'3' and t.get(b'NpcResType',b'').lower() in res_types]
        evidence=source_names.get(name,[])
        if not candidates and evidence:candidates=sorted({e['template'] for e in evidence})
        if not candidates and len(res_types)==1 and portrait_templates:
            # Identical resource aliases are equivalent appearances; retain
            # the original generic dialog template, not a later task alias.
            candidates=[min(portrait_templates)]
        existing=[]
        for n in deployed:
            if n['world']!=world:continue
            try:same=path_bytes(n.get('record_name',n.get('original_name','')))==rawname
            except UnicodeError:same=False
            same=same or any(n.get('lua_original',{}).get('id')==s['id'] for s in matching) if n.get('lua_original') else same
            same=same or any(norm(n['name'])==norm(v) for v in translated)
            if same:existing.append(dict(name=n['name'],position=[n['x'],n['y']],template=n.get('template'),status=n['status']))
        for e in evidence:
            if any(bytes.fromhex(e['raw_hex'])[60:].rstrip(b'\0')==bytes.fromhex(s['raw_hex']) for s in matching):
                existing.append(dict(name=e['name'],position=[e['x']//256,e['y']//512],template=e['template'],status='ORIGINAL_CLIENT_PLACEMENT'))
        existing.extend(dict(name=n['name'],position=n['positions'][0],template=n['template'],status='RECOVERED_PUBLISHED') for n in published if n['world']==world and n['name']==name)
        result.append(dict(content_map=content,world=world,name=name,positions=positions,source_lines=sorted({r['line'] for r in facts}),
            templates=candidates,scripts=scripts,map_scripts=matching,existing=existing,translated_names=translated,
            portrait_evidence=portrait_matches,source_placement_evidence=evidence,
            state='EXISTING' if existing else 'MISSING',world_registered=world in worlds))
    report=dict(source='Client_Original/cn_dict.txt HyperLinkWorldPos',rows=result,
        summary=dict(npc_identities=len(result),missing=sum(not r['existing'] for r in result),
            missing_with_map_script=sum(not r['existing'] and bool(r['map_scripts']) for r in result)))
    (OUT/'missing_npc_batch.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
    print(json.dumps(report['summary']))
    for r in result:
        if not r['existing']:print(r['world'],r['name'],r['positions'],'T',r['templates'],'S',[(e['path'],e['id']) for e in r['map_scripts']])


if __name__=='__main__':run()
