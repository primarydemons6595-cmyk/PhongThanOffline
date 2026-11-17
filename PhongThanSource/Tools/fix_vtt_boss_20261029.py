"""Stage the VTT resource, skill and boss chest repair without editing original PAKs."""
from pathlib import Path
import hashlib, json, shutil
from extract_client_coordinate_dictionary import encode_tcvn3

ROOT = Path(__file__).resolve().parents[2]
LOOSE = ROOT / 'PhongThanRuntime-Loose'
PACKAGE = ROOT / 'PhongThanSource/Deploy/ProjectContent/VttBoss20261029'
OUT = PACKAGE / 'compiled'
files = []

def baseline(side, relative):
    snapshot=PACKAGE/'source'/side/relative
    if not snapshot.exists():
        snapshot.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(LOOSE/side/relative,snapshot)
    return snapshot.read_bytes()

def stage(side, relative, data):
    dest = OUT / side / relative
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_bytes(data)
    files.append(dict(role=side, relative=relative, sha256=hashlib.sha256(data).hexdigest()))

skill_rows = [r.split(b'\t') for r in (LOOSE/'Server/settings/Skills.txt').read_bytes().splitlines()]
sh = skill_rows[0]
skills = {}
for r in skill_rows[1:]:
    # Original name lookup takes the first matching row (e.g. Bang Phong Bao).
    skills.setdefault(r[sh.index(b'SkillName')], r[sh.index(b'SkillId')])
audit = []
for side in ['Server', 'Client']:
    rows = [r.split(b'\t') for r in baseline(side,'settings/phongthan/Npcs.txt').splitlines()]
    h = rows[0]
    for tid in [133,138,143,148]:
        row = rows[tid+1]
        if tid in [133,138,143,148]: row[h.index(b'AIMode')] = b'1'
        for slot in range(1,5):
            col = h.index(('Skill%d'%slot).encode())
            if row[col] and not row[col].isdigit():
                original = row[col]
                row[col] = skills[original]
                if side == 'Server': audit.append(dict(template=tid,slot=slot,skill=int(row[col]),name=original.decode('latin1')))
    for tid in [410,413,414,415]:
        # Static on AddNpc prevents the monster spawn-domain corrector from
        # moving treasure out of the boss room. The death script changes each
        # live chest to kind_normal immediately after mission ownership binds.
        row=rows[tid+1]
        row[h.index(b'Kind')]=b'4'
        row[h.index(b'AIMode')]=b'0'
    stage(side,'settings/phongthan/Npcs.txt',b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')
    for p in (LOOSE/side/'settings/npcres').glob('*.txt'):
        raw=p.read_bytes()
        if b'pt_vtt_' not in raw: continue
        raw=baseline(side,p.relative_to(LOOSE/side))
        rows=[r.split(b'\t') for r in raw.splitlines()];h=rows[0]
        for row in rows[1:]:
            if not row[0].startswith(b'pt_vtt_'): continue
            if b'ResFilePath' in h:
                row[h.index(b'ResFilePath')]=b'\\spr\\phongthan\\vtt'
                row[2]=b'\\spr\\phongthan\\vtt'
            else:
                value=(b'1,1,1' if b'1,1,1' in row else
                       b'gate_closed.spr' if row[0]==b'pt_vtt_gate_closed' else
                       b'fence_down.spr' if row[0]==b'pt_vtt_fence_down' else b'fence_up.spr')
                # Every action remains visible, including hit/death transitions.
                row[1:]=[value]*(len(h)-1)
        stage(side,p.relative_to(LOOSE/side).as_posix(),b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')

for arena in range(1,5):
    rel='script/phongthan/van_tien/boss%d.lua'%arena
    raw=baseline('Server',rel).replace(b'\r\n',b'\n')
    # Retain original completion logic, original chest templates/loot and four
    # AddNpc calls. Only replace their positions at the native AddNpc boundary.
    raw=raw.replace(b'function AddNpc(tid,level,world,x,y,barrier)\n local n=', b'''PT_CHEST_ORIGIN_X=0
PT_CHEST_ORIGIN_Y=0
PT_CHEST_SLOT=0
function AddNpc(tid,level,world,x,y,barrier)
 local chest=(tid==410 or tid==413 or tid==414 or tid==415)
 if chest then
  PT_CHEST_SLOT=PT_CHEST_SLOT+1
  local offsets={{-3,0},{0,-3},{3,0},{0,3}}
  local p=offsets[PT_CHEST_SLOT]
  if p==nil then return 0 end
  x=(PT_CHEST_ORIGIN_X+p[1])*32+16
  y=(PT_CHEST_ORIGIN_Y+p[2])*32+16
 end
 local n=''')
    raw=raw.replace(b' local n=pt_boss_native_add(tid,level,world,x,y,barrier)',b''' local n=0
 if chest then
  local directions={{-1,0},{0,-1},{1,0},{0,1}}
  local d=directions[PT_CHEST_SLOT]
  local radii={3,2,1,4,5,6}
  for attempt=1,getn(radii) do
   local radius=radii[attempt]
   n=pt_boss_native_add(tid,level,world,(PT_CHEST_ORIGIN_X+d[1]*radius)*32+16,(PT_CHEST_ORIGIN_Y+d[2]*radius)*32+16,1)
   if n>0 then break end
  end
 else
  n=pt_boss_native_add(tid,level,world,x,y,barrier)
 end''')
    raw=raw.replace(b'  SetNpcDeathScript(n,',b'  SetNpcKind(n,0)\n  SetNpcDeathScript(n,')
    raw=raw.replace(b'  pt_original_boss_death()',b'''  local map,x,y=GetNpcPos(npc)
  if map~=1066+PT_ID then return end
  PT_CHEST_ORIGIN_X=x;PT_CHEST_ORIGIN_Y=y;PT_CHEST_SLOT=0
  pt_original_boss_death()
  -- A full ten-minute loot phase even when the combat timer was nearly over.
  StopMissionTimer(PT_ID,3000+PT_ID*2)
  StartMissionTimer(PT_ID,3000+PT_ID*2,18*600)''')
    assert b'GetNpcPos(npc)' in raw and b'local chest=' in raw
    stage('Server',rel,raw)

gates=['1002_f57647b5','1004_281c1583','1003_479d2a84','1001_41e3ecb2']
for arena,gate in enumerate(gates,1):
    rel='script/phongthan/npc_recovered_batch/'+gate+'.lua'
    raw=baseline('Server',rel).replace(b'ShowMissionCountdown(PT_ID,3001)',b'ShowMissionCountdown(PT_ID,2999+PT_ID*2)').replace(b'ShowMissionCountdown(PT_ID,3002)',b'ShowMissionCountdown(PT_ID,3000+PT_ID*2)')
    prompt=encode_tcvn3('Vào Vạn Tiên Trận? Lượt mới dùng một Linh Phù hoặc Linh Thạch còn hạn 30 ngày. Linh Thạch không tiêu hao. Vào lại cùng lượt không tốn thêm vé.')
    raw=raw.replace(b'MsgBox(11393,"pt_enter","no")',b'if GetTask(4789+PT_ID)==GetMissionV(1) then pt_enter();return end\n MsgBox("'+prompt+b'","pt_enter","no")')
    # The task button still invokes renwu2, whose legacy item/task checks ran
    # before the restored native admission function. Route that button too.
    raw+=b'\nrenwu1=renwu3\nrenwu2=renwu3\n'
    stage('Server',rel,raw)

PACKAGE.mkdir(parents=True,exist_ok=True)
(PACKAGE/'manifest.json').write_text(json.dumps(dict(schema=1,files=files,skills=audit,chests=4,lootSeconds=600),ensure_ascii=True,indent=2),'utf8')
print('STAGED',len(files),'files;',len(audit),'resolved NPC skills; four local chests; 600 seconds')
