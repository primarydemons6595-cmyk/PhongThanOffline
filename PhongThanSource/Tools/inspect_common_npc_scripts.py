from restore_original_world import Original,OUT
import json,re
o=Original();paths=['script/彩票/'+n+'.lua' for n in ('卦','应签商','二十八星宿','太岁','阴阳师','selling')]
paths += ['script/运镖/'+n+'.lua' for n in ('西岐镖师','朝歌总镖头','总镖头','镖师')]
paths += ['script/龙套/'+n+'.lua' for n in ('符印师','酒保','古代勇士之魂')]
paths += ['script/狱法山/'+n+'.lua' for n in ('蜉蝣子','青云客')]
paths += ['script/阪泉圣地/'+n+'.lua' for n in ('蜉蝣子','青云客','冰龙之眼','火龙之眼')]
found=[]
for path in paths:
    match=o.get(path.encode('gbk'))
    if match:
        data,proof=match;found.append(proof)
        print(path,proof['id'],repr(data[:1100].decode('gbk','replace')))
    else:print(path,'MISSING')
(OUT/'common_npc_script_candidates.json').write_text(json.dumps(found,ensure_ascii=False,indent=2),'utf8')
