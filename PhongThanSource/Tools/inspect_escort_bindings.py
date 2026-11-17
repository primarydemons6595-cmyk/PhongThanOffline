import json,re
from restore_original_world import ROOT
from extract_client_coordinate_dictionary import decode_tcvn3
runtime=ROOT/'PhongThanRuntime-Loose/Server'
for name in ('material','ibitem','questkey'):
    p=runtime/'settings/item/001'/(name+'.txt')
    for i,line in enumerate(p.read_bytes().splitlines()):
        text=decode_tcvn3(line)
        if any(s in text.lower() for s in ('kim bài','khao quân','ngọc tỷ','huy chương','hộ tống')):print('ITEM',name,i,text[:250])
for file in ('coordinates.json','recovered_client_npcs.json'):
    data=json.loads((ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration'/file).read_text('utf8'))
    def walk(d):
        if isinstance(d,dict):
            if any(word in str(d.get('name','')) for word in ('Phong Lam','Lý Tịnh','Truong','镖','风林','张桂芳','李靖')):print('NPC',json.dumps(d,ensure_ascii=False)[:1700])
            for v in d.values():walk(v)
        elif isinstance(d,list):
            for v in d:walk(v)
    walk(data)
uses={}
for p in (runtime/'script').rglob('*.lua'):
    raw=p.read_bytes()
    for value in re.findall(rb'(?:GetTask|SetTask|QuestExchange)\s*\(\s*(47\d\d)',raw):uses.setdefault(int(value),[]).append(str(p.relative_to(runtime)))
print('TASKS4700',json.dumps(uses,ensure_ascii=False))
