"""Stage the merchant handoff without changing character task state or shop logic."""
from pathlib import Path
import hashlib,json,sys
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
RUNTIME=ROOT/'PhongThanRuntime-Loose'
CONTENT=ROOT/'PhongThanSource/Deploy/ProjectContent/LifeGiaoHuan20261030'
MARK=b'\n-- PT_GIAO_HUAN_MERCHANT_20261030\n'
def sha(b):return hashlib.sha256(b).hexdigest()
def build():
    files={};server=RUNTIME/'Server'
    # Legacy journal functions are copied into each NPC scope. Correct every
    # exact shared home line so talking to another quest NPC cannot restore it.
    old=encode_tcvn3('local home=GetPlayerType()==1 and "Ngọc Hư Cung (217,202)" or "Xi Vưu Mộ (200,200)"')
    new=encode_tcvn3('local home=GetPlayerType()==1 and "Ngọc Hư Cung (216,201)" or "Xi Vưu Mộ (202,201)"')
    for p in (server/'script').rglob('*.lua'):
        raw=p.read_bytes()
        if old in raw:files['Server/'+p.relative_to(server).as_posix()]=raw.replace(old,new)
    # These are the paths that the native startup/service binder actually uses.
    merchants=[(1003,1,'script/phongthan/restoration/maolu_giao_dao.lua','Ngọc Hư Cung (216,201)'),
               (1003,1,'script/phongthan/npc_recovered_batch/1003_7006c6a9.lua','Ngọc Hư Cung (216,201)'),
               (1004,2,'script/phongthan/npc_recovered_batch/1004_d599fb23.lua','Xi Vưu Mộ (202,201)')]
    module=(CONTENT/'giao_huan.lua').read_text('utf8')
    names='{[1]="Tuyết Quái",[4]="Băng Lang",[2]="Hỏa Diện",[5]="Lục Quái",[6]="Cuồng Điểu",[9]="Thảo Tiên",[18]="Hắc Phong"}'
    for mid,faction,path,home in merchants:
        rel='Server/'+path;raw=files.get(rel,(server/path).read_bytes()).split(MARK)[0]
        config=f'PT_GH_MAP={mid}\nPT_GH_FACTION={faction}\nPT_GH_HOME="{home}"\nPT_GH_NAMES={names}\n'
        files[rel]=raw+MARK+encode_tcvn3(config+module)
    # The teacher has its own functional profession script in the next stage.
    # Remove its old quest block, including the old quest-marker overrides.
    for mid in [1003,1004]:
        rel=f'Server/script/phongthan/npc_restore/{mid}_15.lua'
        raw=(RUNTIME/rel).read_bytes().split(b'-- PT_TWO_FACTION_BEGIN')[0]
        files[rel]=raw.rstrip()+b'\n'
    manifest=[]
    for rel,data in files.items():
        target=CONTENT/'compiled'/rel;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
        manifest.append(dict(path=rel,sha256=sha(data),before_sha256=sha((RUNTIME/rel).read_bytes())))
    result=dict(scope='LOOSE_ONLY',files=manifest,merchants=merchants,
                preserved_tasks=[4554,4555,4556,4557,4558,4559])
    (CONTENT/'manifest.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf8')
    return result
if __name__=='__main__':
    sys.stdout.reconfigure(encoding='utf8');m=build();print('STAGED',len(m['files']),'files; existing task progress preserved')
