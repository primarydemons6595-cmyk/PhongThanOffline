"""Prepare explicit, reversible client-only visual substitutions; no item IDs/stats."""
from pathlib import Path
from collections import Counter
import json,re,struct
import build_offline_loose as b

root=Path(__file__).resolve().parents[2];client=root/'PhongThanRuntime-Loose/Client'
out=root/'analysis/spr_visual_compat_20261018';out.mkdir(exist_ok=True)
npc=client/'settings/npcres';human=client/'spr/npcres/human'
kindpath=npc/bytes.fromhex('c8cbceefc0e0d0cd2e747874').decode('latin1')
kind=[r.split(b'\t') for r in kindpath.read_bytes().splitlines()]
actors=[kind[i] for i in [1,2,3,5,6,7]]
changes=[];prepared={};headers={}

def read(path):
    return [r.split(b'\t') for r in path.read_bytes().splitlines()] if path.is_file() else []

def info(raw):
    if not raw.lower().endswith(b'.spr'):return None
    if raw not in headers:
        p=human/raw.decode('latin1')
        if not p.is_file():headers[raw]=None
        else:
            data=p.read_bytes();h=struct.unpack_from('<8H',data,4) if len(data)>=32 and data[:3]==b'SPR' else None
            headers[raw]=h if h and h[4]>=8 and h[4]%8==0 else None
    return headers[raw]

def get(row,col):return row[col] if col<len(row) else b''
def put(row,col,value):
    while len(row)<=col:row.append(b'')
    row[col]=value

def change(table,row,col,value,rule,donor=None):
    old=get(table[row],col)
    if old==value:return
    changes.append(dict(table=str(current.relative_to(client)),row=row+1,column=col+1,
                        before_hex=old.hex(),after_hex=value.hex(),rule=rule,donor=donor))
    put(table[row],col,value)

def stage(path,table):
    payload=b'\r\n'.join(b'\t'.join(r) for r in table)+b'\r\n'
    if path.is_file() and payload==path.read_bytes():return
    prepared[path]=payload

def group(cell):
    m=re.search(rb'_([a-z]+)\d*\.spr$',cell,re.I)
    return m[1].lower() if m else b''

def choose_same(row,cell):
    if b'_' not in cell:return None
    stem=cell.rsplit(b'_',1)[0]
    family=group(cell)
    options=[v for v in row[1:] if v.startswith(stem+b'_') and group(v)==family and info(v)]
    return min(options,key=lambda x:(x!=cell,x)) if options else None

for actor in actors:
    paths={i:npc/actor[i].decode('latin1') for i in range(7,16)}
    tables={i:read(p) for i,p in paths.items()}
    styles={i:read(npc/actor[i].decode('latin1')) for i in [4,5]}
    right=tables[12];left=tables[11]
    if not right:continue
    action_header=right[0]
    # Availability is evaluated for the actions the style table really selects.
    def required(wi,table=right):
        cols=set()
        for ride in [0,1]:
            style=styles[5 if ride else 4]
            if wi>=len(style):continue
            # Neutral stand can intentionally refer to an empty-hand layer;
            # do not replace an otherwise valid equipped model for that cell.
            for sc in [3,4,5,6]:
                label=get(style[wi],sc)
                if label in action_header:cols.add(action_header.index(label))
        return cols
    originals={i:[list(r) for r in t] for i,t in tables.items()}
    original_styles={i:[list(r) for r in t] for i,t in styles.items()}
    donors=[]
    for wi in range(2,len(right)):
        cols=required(wi)
        # Neutral stand may intentionally have no weapon; fight/run/attack
        # must have a nonempty real layer. No unarmed row becomes a weapon.
        if cols and all(info(get(right[wi],col)) or choose_same(right[wi],get(right[wi],col)) for col in cols):donors.append(wi)
    for wi in range(1,len(right)):
        current=paths[12];row=right[wi]
        if wi==1:
            # Unarmed parts are optional, not a missing equipped item.
            for col,cell in enumerate(list(row)[1:],1):
                if cell.endswith(b'.spr') and not info(cell):change(right,wi,col,b'','UNARMED_OPTIONAL_LAYER')
            continue
        cols=required(wi)
        bad=[col for col in cols if not info(get(row,col)) and not choose_same(row,get(row,col))]
        if bad and donors:
            # Preserve one/two-hand style family when a complete donor exists.
            def same_style(d):
                return all(get(original_styles[i][wi],5)==get(original_styles[i][d],5)
                           for i in [4,5] if wi<len(original_styles[i]) and d<len(original_styles[i]))
            pool=[d for d in donors if same_style(d)] or donors
            donor=min(pool,key=lambda d:(abs(d-wi),d))
            for col in range(1,len(action_header)):
                value=get(originals[12][donor],col)
                if value and not info(value):value=choose_same(originals[12][donor],value) or b''
                change(right,wi,col,value,'SUBSTITUTE_COMPLETE_WEAPON_SAME_ACTOR',donor-1)
            for si in [4,5]:
                style=styles[si]
                if wi<len(style) and donor<len(original_styles[si]):
                    current=npc/actor[si].decode('latin1')
                    for col in range(1,len(style[0])):change(style,wi,col,get(original_styles[si][donor],col),'MATCH_DONOR_WEAPON_ACTION_STYLE',donor-1)
            if left and wi<len(left):
                current=paths[11]
                for col in range(1,max(len(left[0]),len(action_header))):
                    value=get(originals[11][donor],col) if donor<len(originals[11]) else b''
                    if value and not info(value):value=choose_same(originals[11][donor],value) or b''
                    change(left,wi,col,value,'MATCH_DONOR_LEFT_WEAPON_LAYER',donor-1)
        else:
            for col,cell in enumerate(list(row)[1:],1):
                if cell.endswith(b'.spr') and not info(cell):
                    alternative=choose_same(row,cell)
                    if alternative:change(right,wi,col,alternative,'SAME_WEAPON_SAME_ACTION_FAMILY')
                    elif col not in cols:change(right,wi,col,b'','UNUSED_OR_OPTIONAL_WEAPON_ACTION')
    if left:
        current=paths[11]
        for ri,row in enumerate(left[1:],1):
            for col,cell in enumerate(list(row)[1:],1):
                if cell.endswith(b'.spr') and not info(cell):
                    alt=choose_same(row,cell)
                    change(left,ri,col,alt or b'','SAME_LEFT_LAYER_OR_ABSENT_OPTIONAL')
    # Mounts/wings are bundles: never duplicate a whole bird into head/tail.
    mount=[tables[i] for i in [13,14,15]]
    if all(mount):
        count=min(map(len,mount));source=[[list(r) for r in t] for t in mount]
        def complete(ri):
            return all(any(info(get(t[ri],col)) for t in source) for col in range(17,29))
        valid=[ri for ri in range(1,count) if complete(ri)]
        for ri in range(1,count):
            donor=ri if complete(ri) else (min(valid,key=lambda d:(abs(d-ri),d)) if valid else None)
            if donor is None:continue
            for part,t in enumerate(mount):
                current=paths[13+part]
                for col in range(17,29):
                    value=get(source[part][donor],col)
                    if not info(value):value=b''
                    change(t,ri,col,value,'MOUNT_BUNDLE_KEEP_REAL_LAYERS' if donor==ri else 'SUBSTITUTE_COMPLETE_MOUNT_OR_WINGS_SAME_ACTOR',donor-1)
    # Hair layers are optional; a missing hair SPR must not invalidate body/head.
    hair=tables[8]
    if hair:
        current=paths[8]
        for ri,row in enumerate(hair[1:],1):
            for col,cell in enumerate(list(row)[1:],1):
                if cell.endswith(b'.spr') and not info(cell):change(hair,ri,col,choose_same(row,cell) or b'','OPTIONAL_HAIR_SAME_STYLE_OR_ABSENT')
    # Cloaks: leave base/naked row empty; explicitly substitute an existing
    # same-sex cape for an equipped row that has no usable original art.
    cape=tables[10]
    if cape and actor not in (actors[0], actors[3]):
        current=paths[10]
        for ri,row in enumerate(cape[1:],1):
            for col,cell in enumerate(list(row)[1:],1):
                if cell.endswith(b".spr"):change(cape,ri,col,b"","NO_WARRIOR_CAPE_ON_OTHER_PROFESSIONS")
    if cape and actor in (actors[0], actors[3]):
        sex_source=actors[0] if actor in actors[:3] else actors[3]
        donor_table=read(npc/sex_source[10].decode('latin1'))
        donor_header=donor_table[0] if donor_table else []
        current=paths[10]
        for ri,row in enumerate(cape[1:],1):
            donor_row=donor_table[min(max(ri,2),len(donor_table)-1)] if len(donor_table)>2 else []
            for col,cell in enumerate(list(row)[1:],1):
                if not cell.endswith(b'.spr') or info(cell):continue
                alternative=choose_same(row,cell)
                if not alternative and ri>1 and donor_row:
                    label=get(cape[0],col)
                    if label in donor_header:alternative=get(donor_row,donor_header.index(label))
                    if not info(alternative or b''):alternative=choose_same(donor_row,cell)
                change(cape,ri,col,alternative if info(alternative or b'') else b'',
                       'CUSTOM_SAME_SEX_CAPE_SUBSTITUTION' if ri>1 else 'NAKED_NO_CAPE')
    for i,table in tables.items():
        if table and any(ch['table']==str(paths[i].relative_to(client)) for ch in changes):stage(paths[i],table)
    for si,table in styles.items():
        p=npc/actor[si].decode('latin1')
        if table and any(ch['table']==str(p.relative_to(client)) for ch in changes):stage(p,table)

# Emit corrected metadata from selected SPRs; avoid stale old-frame sidecars.
for path,data in list(prepared.items()):
    if path.parent!=npc or path.name not in [a[i].decode('latin1') for a in actors for i in range(7,16)]:continue
    table=[r.split(b'\t') for r in data.splitlines()];meta=[list(table[0])]
    for row in table[1:]:
        line=[row[0]]
        for cell in row[1:]:
            h=info(cell)
            line.append(('%d,8,%d'%(h[4],h[7])).encode('ascii') if h else b'')
        meta.append(line)
    sidecar=path.with_name(path.stem+bytes.fromhex('d7cad4b4').decode('latin1')+'.txt')
    stage(sidecar,meta)
files=[]
for path,data in prepared.items():
    rel=path.relative_to(client);dest=out/'prepared'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
    files.append(dict(path=str(rel),before_exists=path.exists(),before_sha256=b.sha(path.read_bytes()) if path.exists() else None,after_sha256=b.sha(data)))
summary=dict(status='PREPARED_NOT_DEPLOYED',files=len(files),cells=len(changes),rules=dict(Counter(x['rule'] for x in changes)),
             parity='CUSTOM_VISUAL_COMPATIBILITY_NOT_ORIGINAL_PARITY',server_modified=False)
(out/'plan.json').write_text(json.dumps(dict(summary=summary,files=files,changes=changes),ensure_ascii=True,indent=2),encoding='utf8')
print(json.dumps(summary),flush=True)
