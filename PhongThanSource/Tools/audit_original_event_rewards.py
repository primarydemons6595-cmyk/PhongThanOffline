"""Read-only source/binding audit: never infer a VNG probability algorithm."""
import configparser,json,re,csv,hashlib,urllib.request
from restore_original_world import ROOT,OUT,Original
import build_offline_loose as b
folder=OUT/'vng_reward_audit';folder.mkdir(exist_ok=True)
original=Original();server=ROOT/'PhongThanRuntime-Loose/Server'
raw,proof=original.get(b'\\settings\\Npcs.txt');lines=raw.splitlines();headers=lines[0].split(b'\t')
templates=[dict(zip(headers,line.split(b'\t'))) for line in lines[1:]]
runtime=(server/'settings/phongthan/Npcs.txt').read_bytes().splitlines();rh=runtime[0].split(b'\t')
rt=[dict(zip(rh,line.split(b'\t'))) for line in runtime[1:]]
prepared=json.loads((OUT/'prepared_changes.json').read_text('utf8'))
targets=sorted({x['template'] for x in prepared['bosses']}|set(range(129,149))|{410,413,414,1908,1913,1918,1923,1929})
policy=configparser.RawConfigParser(strict=False);policy.read(server/'settings/phongthan/NpcDropPolicy.ini',encoding='ascii')
rows=[];tables={};scripts={}
for tid in targets:
    t=templates[tid];r=rt[tid]
    row=dict(template=tid,name=t[b'Name'].decode('gbk'),kind=t[b'Kind'].decode(),bindings=[],runtime_columns_match=all(t.get(k)==r.get(k) for k in [b'Treasure',b'Treasure1',b'DropRateFile',b'DropRateFile1',b'DropRateFile2',b'DropRateFile3']))
    section='Template_'+str(tid);table=policy.get(section,'Table',fallback=policy.get('Main','DefaultTable',fallback=''))
    row['current_normal_loot_table']=table
    for slot in range(4):
        key=('DropRateFile'+(str(slot) if slot else '')).encode();path=t.get(key,b'').strip()
        if not path:continue
        got=original.get(path)
        bind=dict(column=key.decode(),path=path.decode('gbk'),attempts=t.get(b'Treasure'+(b'1' if slot==1 else b''),b'').decode(),original_found=bool(got))
        if got:
            data,p=got;dest=folder/'original_tables'/path.replace(b'\\',b'/').split(b'/')[-1].decode('gbk');dest.parent.mkdir(exist_ok=True);dest.write_bytes(data)
            ini=configparser.RawConfigParser(strict=False);ini.read_string(data.decode('gbk'))
            count=ini.getint('Main','Count',fallback=0);entries=[]
            for n in range(1,count+1):
                if ini.has_section(str(n)):entries.append(dict(ordinal=n,**dict(ini.items(str(n)))))
            numeric=[int(x.get('randrate','0')) for x in entries]
            record=dict(proof=p,main=dict(ini.items('Main')),entries=entries,weight_sum=sum(numeric),missing_ordinals=[n for n in range(1,count+1) if not ini.has_section(str(n))])
            tables[p['id']]=record;bind['source_id']=p['id'];bind['sha256']=p['sha256'];bind['count']=count;bind['weight_sum']=sum(numeric);bind['rand_range']=ini.getint('Main','RandRange',fallback=0)
        row['bindings'].append(bind)
    for key in [b'ActionScript',b'DeathScript']:
        path=t.get(key,b'').strip()
        if path:
            got=original.get(path)
            if got:
                data,p=got;scripts[p['id']]=p;dest=folder/'original_scripts'/f"{p['id']}.lua";dest.parent.mkdir(exist_ok=True);dest.write_bytes(data)
    rows.append(row)
# Recover the actual quest/reward giver and arena doctor, not old authored ports.
for path in ['script/朝歌/赏金猎人.lua','script/万仙阵/医生.lua','script/万仙阵/城门.lua','script/万仙阵/通天教主.lua','script/万仙阵/宝箱.lua']:
    got=original.get(path.encode('gbk'))
    if got:
        data,p=got;scripts[p['id']]=p;dest=folder/'original_scripts'/f"{p['id']}.lua";dest.parent.mkdir(exist_ok=True);dest.write_bytes(data)
result=dict(npc_source=proof,npcs=rows,tables=list(tables.values()),scripts=list(scripts.values()),
    finding='Native PtDropNpc reads project NpcDropPolicy, not original DropRateFile/Treasure. Original VNG tables exist. Script death recovery alone does not restore normal loot.',
    vng_probability_algorithm_verified=False,changes_applied=False)
(folder/'audit.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
print('NPCS',len(rows),'TABLES',len(tables),'SCRIPTS',len(scripts))
for row in rows:
 if row['template'] in {x['template'] for x in prepared['bosses']}|{133,138,143,148}:
  print(row['template'],row['name'],'CURRENT',row['current_normal_loot_table'],'ORIGINAL',[(v['path'].split('\\')[-1],v['attempts'],v.get('count'),v.get('weight_sum'),v.get('rand_range')) for v in row['bindings']])
print('QUEST_SCRIPTS',[(p['id'],p['path']) for p in scripts.values() if '赏金猎人' in p['path'] or '医生' in p['path']])
