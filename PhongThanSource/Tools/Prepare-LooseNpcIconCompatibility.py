"""Prepare client-only NPC visual fallbacks and same-catalog icon aliases."""
from pathlib import Path
from collections import Counter
import json,re,struct
import build_offline_loose as b

root=Path(__file__).resolve().parents[2];client=root/'PhongThanRuntime-Loose/Client'
out=root/'analysis/spr_visual_compat_20261018';npc=client/'settings/npcres'
normal_path=npc/bytes.fromhex('c6d5cda86e7063d7cad4b42e747874').decode('latin1')
kind_path=npc/bytes.fromhex('c8cbceefc0e0d0cd2e747874').decode('latin1')
normal=[r.split(b'\t') for r in normal_path.read_bytes().splitlines()]
kinds={r.split(b'\t')[0]:r.split(b'\t') for r in kind_path.read_bytes().splitlines()[1:]}
templates=[r.split(b'\t') for r in (client/'settings/Npcs.txt').read_bytes().splitlines()]
head=templates[0];ki=head.index(b'Kind');ri=head.index(b'NpcResType')
active={row[ri] for row in templates[1:] if len(row)>ri and row[ki]==b'0'}
changes=[];cache={}
def rawpath(base,value):return b'\\'+base.strip(b'\\')+b'\\'+value
def real(raw):
    if not raw or not raw.lower().endswith(b'.spr'):return False
    if raw not in cache:
        try:p=client/Path(b.logical(raw.decode('latin1')).replace('\\','/'));data=p.read_bytes()
        except (ValueError,OSError):cache[raw]=False;return False
        cache[raw]=len(data)>=32 and data[:3]==b'SPR' and (b.sprite_blob_valid(data,True) or b.sprite_blob_valid(data,False))
    return cache[raw]
rows_by_name={row[0]:row for row in normal[1:]}
source={name:list(row) for name,row in rows_by_name.items()}
def full(name):
    row=source[name];base=kinds[name][2]
    return all(len(row)>col and real(rawpath(base,row[col])) for col in [1,4,5,6])
valid=[name for name in source if name in active and name in kinds and full(name)]
for name,row in rows_by_name.items():
    if name not in active or name not in kinds:continue
    base=kinds[name][2]
    # Do not substitute decorative NPCs, buildings, UI objects or player rigs.
    if base.lower().strip(b'\\') not in [b'spr\\npcres\\animal',b'spr\\npcres\\battle']:continue
    usable=any(real(rawpath(base,v)) for v in row[1:] if v.endswith(b'.spr'))
    donor=name
    if not usable:
        family=re.sub(rb'\d+.*$',b'',name)
        pool=[n for n in valid if kinds[n][2].lower().strip(b'\\')==base.lower().strip(b'\\') and re.sub(rb'\d+.*$',b'',n)==family]
        if not pool:continue
        number=lambda n:int(re.search(rb'\d+',n)[0]) if re.search(rb'\d+',n) else 0
        donor=min(pool,key=lambda n:(abs(number(n)-number(name)),n))
    for col,value in enumerate(list(row)[1:],1):
        if not value.endswith(b'.spr') or real(rawpath(base,value)):continue
        candidates=[]
        if donor!=name and col<len(source[donor]):candidates.append(source[donor][col])
        # Same-entity variant first. Missing death uses that entity's hurt pose
        # only as explicit compatibility art, never claimed original death art.
        if value.endswith(b'b.spr'):candidates.append(value[:-5]+b'.spr')
        order={1:[1,3],2:[2,1,3],3:[3,1],4:[4,1],5:[5,6,3,1],6:[6,3,1],7:[7,6,2,1]}.get(col,[1])
        candidates += [source[donor][v] for v in order if v<len(source[donor])]
        alternative=next((v for v in candidates if real(rawpath(base,v))),None)
        if alternative:
            row[col]=alternative;changes.append(dict(npc_resource=name.decode('latin1'),column=col+1,
                before_hex=value.hex(),after_hex=alternative.hex(),donor_resource=donor.decode('latin1'),
                rule='SAME_ENTITY_COMPAT_ACTION' if donor==name else 'CUSTOM_COMPLETE_NPC_MODEL_SAME_RESOURCE_FAMILY'))
normal_after=b'\r\n'.join(b'\t'.join(row) for row in normal)+b'\r\n'
prepared=out/'extra_prepared'/normal_path.relative_to(client);prepared.parent.mkdir(parents=True,exist_ok=True);prepared.write_bytes(normal_after)
normal_record=dict(path=str(normal_path.relative_to(client)),before_sha256=b.sha(normal_path.read_bytes()),after_sha256=b.sha(normal_after))

# Each alias preserves the requested filename/ID; only its picture is an
# explicit same-family substitute. Never modify item tuple, stats or rates.
icons={};unresolved=[];tables=0
for folder in ['settings/item/000','settings/item/001']:
    for table in sorted((client/folder).glob('*.txt')):
        rows=[r.split(b'\t') for r in table.read_bytes().splitlines()]
        if len(rows)<2:continue
        # The native item catalogs put the inventory SPR in column 5 and the
        # tuple identity in columns 2..4. Ignore price/palette/magic tables.
        entries=[(n,row) for n,row in enumerate(rows[1:],2) if len(row)>4 and row[4].lower().startswith(b'\\spr\\') and row[4].lower().endswith(b'.spr') and all(re.fullmatch(rb'\d+',x) for x in row[1:4])]
        if not entries:continue
        tables+=1;good=[(n,row) for n,row in entries if real(row[4])]
        for n,row in entries:
            raw=row[4]
            if real(raw) or raw in icons:continue
            try:relative=b.logical(raw.decode('latin1'))
            except ValueError:unresolved.append(dict(table=str(table.relative_to(client)),row=n,reason='UNSAFE_OR_NON_NATIVE_FILENAME'));continue
            # Preserve present-but-invalid files for separate repair evidence.
            if (client/relative.replace('\\','/')).exists():unresolved.append(dict(path=relative,reason='EXISTING_INVALID_SPR_NOT_OVERWRITTEN'));continue
            same=[(i,r) for i,r in good if r[1:3]==row[1:3]]
            pool=same or [(i,r) for i,r in good if r[1]==row[1]]
            if not pool:unresolved.append(dict(table=str(table.relative_to(client)),row=n,path=relative,reason='NO_SAME_CATALOG_GENRE_DONOR'));continue
            dn,donor=min(pool,key=lambda x:(x[1][1:4]!=row[1:4],abs(x[0]-n),x[0]))
            icons[raw]=dict(path=relative,path_hex=raw.hex(),id='%08X'%b.pak_id(raw),source_path=donor[4].decode('latin1'),
                source_sha256=b.sha((client/donor[4].decode('latin1').lstrip('\\').replace('\\','/')).read_bytes()),
                table=str(table.relative_to(client)),row=n,donor_row=dn,tuple=[int(x) for x in row[1:4]],
                rule='CUSTOM_SAME_CATALOG_GENRE_ICON_NOT_ORIGINAL_ART')
summary=dict(npc_cells=len(changes),npc_models_substituted=len({r['npc_resource'] for r in changes if r['donor_resource']!=r['npc_resource']}),
             item_catalogs=tables,icon_aliases=len(icons),unresolved=len(unresolved),status='PREPARED_NOT_APPLIED')
(out/'npc_icon_plan.json').write_text(json.dumps(dict(summary=summary,normal_table=normal_record,npc_changes=changes,
    icon_aliases=list(icons.values()),unresolved=unresolved),ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(summary),flush=True)
