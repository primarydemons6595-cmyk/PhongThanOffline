"""Stage original starter dialogue with the explicitly requested Loose rewards."""
from pathlib import Path
import hashlib,json,re,subprocess
from restore_original_world import Original
from extract_client_coordinate_dictionary import decode_tcvn3,encode_tcvn3

ROOT=Path(__file__).resolve().parents[2]
SRC=ROOT/'PhongThanSource'
LIVE=ROOT/'PhongThanRuntime-Loose/Server'
OUT=SRC/'Deploy/ProjectContent/StarterStory20261029'
REPORT=ROOT/'analysis/starter_story_20261029'
REPORT.mkdir(parents=True,exist_ok=True)
MARK=b'\r\n-- PT_STARTER_STORY_20261029_BEGIN\r\n'
sha=lambda b:hashlib.sha256(b).hexdigest()
def lua(v):
    if isinstance(v,str):return json.dumps(v,ensure_ascii=False)
    if isinstance(v,int):return str(v)
    if isinstance(v,list):return '{'+','.join(lua(x) for x in v)+'}'
    return '{'+','.join(('['+lua(k)+']')+'='+lua(x) for k,x in v.items())+'}'

original=Original()
data,proof=original.get(b'\\settings\\StringResource.txt')
strings={int(c[0]):decode_tcvn3(c[1]) for line in data.splitlines()[1:]
         if len(c:=line.split(b'\t'))>=2 and c[0].isdigit()}
# The scripts and resource table have historical naming/count inconsistencies.
# Keep authored story; align obsolete aliases with the verified script graph.
aliases={
 10536:[('2 Băng Cơ','10 Băng Cơ'),('Thợ đồng','Hoàng Long Chân Nhân'),('Kim Hà Đồng Tử','Nhiên Đăng Đạo Nhân')],
 10537:[('Thợ đồng','Hoàng Long Chân Nhân'),('Kim Hà Đồng Tử','Nhiên Đăng Đạo Nhân')],
 10535:[('Kim Hà Đồng Tử','Nhiên Đăng Đạo Nhân')],
 10527:[('Xích Tinh Tử','Nam Cực Tiên Ông')],
 10157:[('Thợ đồng','Cao Giác')],
 10155:[('Thợ đồng:',''),('mau về báo tin cho <c=r>Hậu Thổ<c>!','ta sẽ dùng chúng để tăng năng lực trang bị!')],
 10148:[('Miêu Cương','Xi Vưu Mộ')],
}
ids={21:{0:[10269,10270],1:[10279],2:[10250],3:[10278],4:[10247],5:[10268],6:[10240],7:[10277],8:[10267]},
     11:{0:[10536,10537],1:[10557],2:[10535],3:[10527],4:[10534]},
     31:{0:[10156,10157],1:[10143],2:[10149],3:[10179],4:[10148],5:[10155]}}
story={};dialogue=[]
def display(s):
    s=s.lstrip('#$')
    colors={'r':'red','g':'green','y':'yellow','b':'blue','p':'pink','w':'white'}
    s=re.sub(r'<c=([^>]+)>',lambda m:'<color='+colors.get(m[1].lower(),m[1].lower())+'>',s)
    return s.replace('<c>','<color>')
for task,phases in ids.items():
    story[task]={}
    for phase,keys in phases.items():
        texts=[]
        for key in keys:
            raw=strings[key];text=raw
            for old,new in aliases.get(key,[]):
                assert old in text,(key,old)
                text=text.replace(old,new)
            texts.append(display(text))
            dialogue.append(dict(task=task,phase=phase,id=key,original=raw,display=display(text),adaptations=aliases.get(key,[])))
        story[task][phase]=dict(ids=keys,text='<enter>'.join(texts))
runtime='PT_STARTER_STORY='+lua(story)+'\nPT_STARTER_FINAL={[21]=9,[11]=5,[31]=6}\n'+(SRC/'Tools/starter_story_runtime.lua').read_text('utf-8')
(SRC/'Tools/starter_story_generated.lua').write_text(runtime,'utf-8')
giap=json.loads(subprocess.check_output(['node','-e',"process.stdout.write(JSON.stringify(require('./PhongThanSource/Tools/giapsi/quests.cjs')))"],cwd=ROOT))
audit=json.loads((ROOT/'analysis/two_faction_quests_20261027/audit.json').read_text('utf-8'))
def oldpath(suffix):return next(x['runtime_path'] for x in audit['scripts'] if x['path'].endswith(suffix))
paths={('script/phongthan/'+giap['roles'][role][1]) for role in ['lo','au','buu','hau','hac']}
paths.update(['script/phongthan/npc_restore/1003_03.lua','script/phongthan/npc_restore/1004_01.lua',
              'script/phongthan/npc_restore/1004_07.lua','script/phongthan/two_faction/hinh_thien.lua',
              oldpath('玉虚宫\\黄龙真人.lua'),oldpath('玉虚宫\\燃灯道人.lua'),oldpath('蚩尤墓\\高觉.lua')])
files=[]
for rel in sorted(paths):
    p=LIVE/rel;before=p.read_bytes();base=before.split(MARK)[0]
    assert b'PTGApply' in base or b'PT2Apply' in base,rel
    result=base+MARK+encode_tcvn3(runtime)
    dst=OUT/'compiled/Server'/rel;dst.parent.mkdir(parents=True,exist_ok=True);dst.write_bytes(result)
    files.append(dict(role='Server',relative=rel,sha256=sha(result),before=sha(before)))
# Verify every original callback still comes from the winning PAK and archive it.
sources=[]
inventory=json.loads((ROOT/'analysis/original_quest_audit_20261025/inventory.json').read_text('utf-8'))
for e in inventory['scripts']:
    if 21 not in e.get('tasks',[]):continue
    raw=bytes.fromhex(e['proof']['raw_hex']);payload,origin=original.get(raw)
    assert sha(payload)==e['proof']['sha256']
    target=REPORT/'originals'/Path(e['fixture']).name;target.parent.mkdir(exist_ok=True);target.write_bytes(payload)
    sources.append(origin)
fixtures=json.loads((SRC/'Tests/fixtures/NpcQuestOriginal/provenance.json').read_text('utf-8'))
for e in fixtures['originals']:
    if e['fixture'].removesuffix('.lua') not in ['nam_cuc','hoang_long','nhien_dang','hau_tho','cao_giac','cao_minh','hinh_thien']:continue
    payload,origin=original.get(e['logical'].encode('gbk'));assert sha(payload)==e['sha256']
    (REPORT/'originals'/e['fixture']).write_bytes(payload);sources.append(origin)
(OUT/'manifest.json').write_text(json.dumps(dict(schema=1,files=files),indent=2)+'\n','utf-8')
(OUT/'provenance.json').write_text(json.dumps(dict(string_table=proof,scripts=sources,dialogue=dialogue,
    reward_policy='USER_REQUEST: 500 EXP each successful intermediate step, 5000 final; 600 final money retained; not original VNG reward policy'),ensure_ascii=False,indent=2)+'\n','utf-8')
# Syntax checks execute full staged scripts with only top-level Include stubbed.
syntax=[]
for i,e in enumerate(files):
    payload=(OUT/'compiled'/e['role']/e['relative']).read_bytes()
    dst=REPORT/f'syntax_{i:02}.lua';dst.write_bytes(b'function Include(path) end\r\n'+payload);syntax.append(str(dst))
(REPORT/'syntax-list.txt').write_text('\n'.join(syntax)+'\n','ascii')
print(json.dumps(dict(staged=len(files),story_steps=sum(len(x) for x in ids.values()),original_scripts=len(sources),dialogue_ids=len(dialogue))))
