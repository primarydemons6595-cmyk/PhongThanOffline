"""Read original event Lua and record source evidence, without runtime writes."""
import json,re
from restore_original_world import Original,OUT,ROOT,path_bytes
import build_offline_loose as b
from prepare_npc_lua_library import dependencies,TOKEN

def run():
    original=Original(); folder=OUT/'priority_events';folder.mkdir(exist_ok=True)
    paths=['跑商/'+n for n in ('货商李崇城','货商张玉虚宫','货商王蚩尤墓','货商胡西岐','货商周朝歌')]
    paths+=['孟津/风林','孟津/风林影子','运镖/总兵张桂芳','朝歌/值年太岁杨任','西岐/值年太岁杨任','封神台/武王','瑶池/安安仙子','彩票/太岁','彩票/卦','彩票/二十八星宿']
    paths+=['万仙阵/'+n for n in ('万仙阵土','万仙阵水','万仙阵火','万仙阵风','万仙阵幻','传送门幻','土on','土off','土start','土review','医生','通天教主')]
    # Discover event paths from all recorded package names, not just NPC stems.
    words=('运粮','张桂芳','安安','武王','太岁','跑商','货商','万仙阵','Roulette')
    for row in b.csv_read(ROOT/'PakRecoveryWork/02_PathDictionary/all_paths.csv'):
        try:path=path_bytes(row['CanonicalPath']).decode('gbk').replace('\\','/').lstrip('/')
        except UnicodeError:continue
        if path.startswith('script/') and path.endswith('.lua') and any(w in path for w in words):paths.append(path[7:-4])
    found=[];missing=[]
    api_source=(ROOT/'PhongThanSource/Sources/Core/Src/ScriptFuns.cpp').read_text('latin1')
    registered=set(re.findall(r'\{\s*"([A-Za-z_]\w*)"\s*,',api_source))
    builtins=set('floor ceil random mod abs min max sqrt sin cos tan strfind strsub strlen format tonumber tostring type next nextvar foreach foreachi getn tinsert tremove sort assert error print call setglobal getglobal dostring dofile unpack date time clock'.split())
    for path in sorted(set(paths)):
        got=original.get(('script/'+path+'.lua').encode('gbk'))
        if not got:missing.append(path);continue
        data,proof=got;(folder/(proof['id']+'.lua')).write_bytes(data)
        deps,dynamic=dependencies(data);text=data.decode('gbk','replace')
        tokens=[t for t in TOKEN.findall(text) if not t.startswith('--')]
        defined={tokens[i+1] for i,t in enumerate(tokens[:-1]) if t=='function'}
        calls=sorted({t for i,t in enumerate(tokens[:-1]) if tokens[i+1]=='(' and re.fullmatch(r'[A-Za-z_]\w*',t) and t not in defined and t not in ('if','elseif','while','for','and','or','not')})
        absent=sorted(set(calls)-registered-builtins)
        found.append(proof|dict(dependencies=deps,dynamic=dynamic,calls=calls,apis_not_registered_in_server=absent))
        print(path,proof['id'],len(data),'CALLS',calls)
    result=dict(found=found,missing=missing)
    (folder/'original_scripts.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),'utf8')
    print('MISSING',missing)

if __name__=='__main__':run()
