"""Record target-server evidence and stage one reviewed coordinate correction.

Coordinates from guides are display tiles, never exact sub-tile MPS positions.
No new NPC is spawned without a proven template and original script identity.
"""
import csv,json,re,struct,unicodedata
from collections import Counter
from restore_original_world import ROOT,OUT,Original,path_bytes
from recover_npc_coordinate_tables import Page,fetch
from prepare_npc_restoration import npc_records,split_region,standalone_npcs
from build_authored_npcs import place_in_tile
from publish_original_world import parse_index
import build_offline_loose as b


def norm(value):
    return re.sub('[^a-z0-9]+',' ',''.join(c for c in unicodedata.normalize('NFD',value.lower().replace('đ','d')) if not unicodedata.combining(c))).strip()


def run():
    server=ROOT/'PhongThanRuntime-Loose/Server'
    project=ROOT/'PhongThanSource'
    deployment=json.loads((project/'Docs/NPC_RESTORATION_DEPLOYMENT.json').read_text('utf8'))
    catalog=json.loads((project/'Deploy/ProjectContent/NpcRestoration/coordinates.json').read_text('utf8'))
    target='https://phongthan.zing.vn/su-kien/may-chu-moi-khai-minh-dien/the-tan-thu-1581.html'
    pages=[target,
        'https://phongthan.zing.vn/tin-tuc/can-biet/qua-den-bu-sau-bao-tri-ngay-14-08-tai-khai-minh-dien.html',
        'https://phongthan.zing.vn/su-kien/nu-than-phong-than/nu-than-phong-than.html',
        'https://phongthan.zing.vn/su-kien/hoa-dang-hoi/su-kien-mien-phi-1585.html',
        'https://phongthan.zing.vn/su-kien/hoa-dang-hoi/su-kien-tich-luy-1586.html']
    texts={url:' '.join(Page(fetch(url).decode('utf8','replace')).parts) for url in pages}
    facts=[]
    for world,label,x,y in ((1003,'Ngọc Hư Cung',207,197),(1002,'Sùng Thành Doanh',205,198),(1004,'Xi Vưu Mộ',198,205)):
        assert re.search(str(x)+r'\s*/\s*'+str(y),texts[target])
        facts.append(dict(world=world,map=label,name='Tân Thủ Phong Thần',x=x,y=y,source=target,
            coordinate_precision='DISPLAY_TILE_ONLY',applicability='TARGET_SERVER_EXPLICIT',
            lifecycle='Page start 2026-08-14; event banner ends 2026-10-01; continued availability requires confirmation',
            spawn_status='BLOCKED_ORIGINAL_TEMPLATE_AND_SCRIPT_IDENTITY_NOT_PROVEN'))
    for url,text in texts.items():
        if url==target:continue
        excluded='không áp dụng' in text.lower() and 'khai minh điện' in text.lower()
        facts.append(dict(source=url,classification='TARGET_EXCLUDED' if excluded else 'TARGET_EVENT_EXPIRED_2026_09_30',
            auto_enable=False,context=text[text.lower().find('không áp dụng')-80:text.lower().find('không áp dụng')+160] if excluded else 'Historical compensation; not a permanent NPC proof'))
    (OUT/'khai_minh_dien_2026.json').write_text(json.dumps(dict(target_server='2026 Khai Minh Điện',as_of='2026-10-16',facts=facts),ensure_ascii=False,indent=2),'utf8')

    # Review the existing named actor only: its identity/script is unchanged.
    # Repeated VNG quest instructions put Au Thien Hoa in display tile 206/193.
    guide='https://phongthan.zing.vn/cam-nang/gioi-thieu/tam-gioi-giap-si.html'
    guide_text=norm(' '.join(Page(fetch(guide).decode('utf8','replace')).parts))
    assert re.search(r'au thien hoa.{0,90}206 193',guide_text)
    actor=next(n for n in deployment['rows'] if n['world']==1002 and n['name']=='Au Thien Hoa')
    assert (actor['x'],actor['y'])==(206,196)
    before_rel=actor['override'];before=(server/before_rel).read_bytes()
    assert before_rel.endswith('_npc_s.dat'), 'Only reviewed standalone override supported'
    records=npc_records(before)
    matching=[r for r in records if r[60:].rstrip(b'\0')==actor['lua']['logical'].encode('ascii')]
    assert len(matching)==1
    old=matching[0];wx,wy=206*256+128,193*512+256;rx,ry=wx//512,wy//1024
    after_rel=f'settings/phongthan/npc_regions/1002/v_{ry:03d}/{rx:03d}_npc_s.dat'
    after=(server/after_rel).read_bytes() if (server/after_rel).exists() else standalone_npcs([])
    peers=npc_records(after)
    original=Original()
    world=dict(re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))[b'1002']
    geometry=None
    index=parse_index((server/'loose_index.bin').read_bytes())
    for suffix in (b'_S',b''):
        raw=b'\\maps\\'+world+suffix+f'\\v_{ry:03d}\\{rx:03d}_region_s.dat'.encode()
        binding=index.get(b.pak_id(raw))
        if binding:
            p=server/binding.decode('mbcs').lstrip('\\').replace('\\','/')
            if p.exists():geometry=p.read_bytes();break
    if geometry is None:
        raw=b'\\maps\\'+world+f'\\v_{ry:03d}\\{rx:03d}_region_c.dat'.encode()
        found=original.get(raw)
        assert found, 'Destination geometry not found'
        geometry=found[0]
    wx,wy=place_in_tile(geometry,wx,wy,[struct.unpack_from('<ii',r,4) for r in peers])
    assert (wx//256,wy//512)==(206,193)
    moved=bytearray(old);struct.pack_into('<ii',moved,4,wx,wy);moved=bytes(moved)
    assert moved[:4]==old[:4] and moved[12:]==old[12:]
    old_other=[r for r in records if r!=old]
    updates={before_rel:standalone_npcs(old_other),after_rel:standalone_npcs(peers+[moved])}
    assert Counter(records+peers)==Counter(old_other+peers+[old])
    plan=[]
    for relative,data in updates.items():
        stage=OUT/'staged/Server'/relative;stage.parent.mkdir(parents=True,exist_ok=True);stage.write_bytes(data)
        plan.append(dict(relative=relative,before_sha256=b.sha((server/relative).read_bytes()) if (server/relative).exists() else None,sha256=b.sha(data),
            actor='Au Thien Hoa',world=1002,from_display=[206,196],to_display=[206,193],source=guide,
            precision='DISPLAY_TILE; walkable subcell selected; template and script retained',monsters_changed=0))
    (OUT/'coordinate_changes.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
    actor.update(x=206,y=193,world_x=wx,world_y=wy,region_x=rx,region_y=ry,local_x=wx%512,local_y=wy%1024,override=after_rel,
        coordinate_evidence=dict(source=guide,precision='DISPLAY_TILE_ONLY',review='2026-10-16'),note='VNG guide 206/193; old 206/196 corrected')
    for n in catalog['maps']['1002']:
        if n[0]=='Au Thien Hoa':n[2:5]=[206,193,'VNG guide 206/193; reviewed 2026-10-16']
    prepared=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
    for c in prepared['npc_changes']:
        n=next(n for n in deployment['rows'] if n['world']==c['world'] and n['name']==c['name'])
        n['lua']['sha256']=c['sha256'];n['lua_status']='ORIGINAL_MAIN_DIRECT_DISPATCH_NOT_FULLY_LIVE_ACCEPTED'
    for row in deployment['overrides']:
        if row['relative'] in updates:row['sha256']=b.sha(updates[row['relative']])
    if not any(r['relative']==after_rel for r in deployment['overrides']):
        deployment['overrides'].append(dict(relative=after_rel,sha256=b.sha(updates[after_rel]),added=1,base=None,base_sha256=None))
    for row in deployment.get('scripts',[]):
        found=next((c for c in prepared['npc_changes'] if c['relative']==row['relative']),None)
        if found:row['sha256']=found['sha256'];row['bytes']=len((OUT/'staged/Server'/row['relative']).read_bytes())
    for rel,data in (('Deploy/ProjectContent/NpcRestoration/coordinates.json',catalog),('Docs/NPC_RESTORATION_DEPLOYMENT.json',deployment)):
        dest=OUT/'staged/Project'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n','utf8')
    # Export all prior catalog entries with corroboration and explicit limits.
    vng=json.loads((OUT/'vng_coordinate_evidence.json').read_text('utf8'))['facts']
    audited=[]
    for n in deployment['rows']:
        name=norm(n['name'])
        evidence=[f for f in vng if name in norm(f['context']) and (f['x'],f['y'])==(n['x'],n['y'])]
        audited.append(dict(world=n['world'],name=n['name'],x=n['x'],y=n['y'],
            state='DOCUMENT_CONTEXT_MATCH_REQUIRES_MAP_REVIEW' if evidence else 'PRIOR_CATALOG_NOT_TARGET_VERIFIED',
            template_identity=n.get('identity_basis','ORIGINAL_OR_PRIOR'),script_state=n.get('lua_status'),
            sources=sorted({f['source'] for f in evidence})))
    (OUT/'npc_coordinate_review.json').write_text(json.dumps(dict(target='Khai Minh Dien 2026',rows=audited,new_target_facts=facts,
        all_target_coordinates_verified=False,exact_mps_recoverable_only_from_region_records=True),ensure_ascii=False,indent=2),'utf8')
    with (OUT/'npc_coordinate_review.csv').open('w',newline='',encoding='utf-8-sig') as stream:
        writer=csv.DictWriter(stream,fieldnames=['world','name','x','y','state','template_identity','script_state','sources']);writer.writeheader()
        for row in audited:writer.writerow(row|dict(sources=' | '.join(row['sources'])))
    print(json.dumps(dict(catalog_rows=len(audited),target_new_coordinates=3,reviewed_corrections=1,coordinate_files=len(plan),monsters_changed=0)))


if __name__=='__main__':run()
