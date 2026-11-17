"""Compile the approved, editable Loose rebirth configuration to Lua assets.

Does not write to a running game. Install the generated pack in Client and
Server together and restart both so cached skill levels cannot disagree.
"""
from pathlib import Path
import argparse,json,hashlib

def compile_pack(config,skill_table,output,old_control_source):
    data=json.loads(Path(config).read_text('utf8'))
    assert data['status']=='approved_reconstruction_not_recovered_original'
    rows=Path(skill_table).read_bytes().splitlines();head=rows[0].split(b'\t')
    table={int(d.get(b'SkillId',b'0')or 0):d for d in (dict(zip(head,l.split(b'\t'))) for l in rows[1:])}
    outputs=[];root=Path(output).resolve()
    required={1500:{'lifereplenish_v','addexdefense_v'},1502:{'lifemax_v','attackratingenhance_p','addphysicsdamage_p'}}
    for profile in data['skills']:
        id=profile['id'];assert id in required and set(profile['attributes'])==required[id]
        assert profile['maxRank']==10 and profile['durationTicks']==540
        assert profile['cooldownTicks']==int(table[id][b'TimePerCast'])
        effects=dict(profile['attributes']);effects['skill_cost_v']=profile['mana']
        lines=['-- USER-APPROVED LOOSE RECONSTRUCTION. Not recovered VNG numbers.',
               '-- Source: LegacyRebirth20261110/formulas.json; regenerate Client + Server together.',
               'function GetSkillLevelData(name, data, level)',
               '  if type(level) ~= "number" or level < 1 or level > 10 then return "" end']
        for name,formula in effects.items():
            base,step=formula['base'],formula['perRank']
            assert type(base)==int and type(step)==int and 0<=base<=10000 and 0<=step<=1000
            for rank in range(1,11):assert 0<=base+rank*step<=100000
            ticks=0 if name=='skill_cost_v' else profile['durationTicks']
            lines+=['  if name == "'+name+'" then',f'    return ({base} + {step} * level)..",{ticks},0"','  end']
        lines+=['  return ""','end','']
        raw=('\r\n'.join(lines)).encode('ascii')
        relative=table[id][b'LvlSetScript'].decode('latin1').lstrip('\\').replace('\\','/')
        path=root/relative;assert root in path.resolve().parents
        path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(raw)
        outputs.append(dict(skill=id,relative=relative,sha256=hashlib.sha256(raw).hexdigest(),provenance='user_approved_reconstruction'))
    assert set(p['id'] for p in data['skills'])==set(required)
    # Preserve the old learned ID and projectile graph. Reuse ONLY the
    # surviving original same-family controller, not the 1560 root/flag ID.
    source=Path(old_control_source).read_bytes()
    assert b'function get_fastwalkrun_p' in source and b'function get_attackslowhitrecover_v' in source
    for id in [1516,1517]:
        relative=table[id][b'LvlSetScript'].decode('latin1').lstrip('\\').replace('\\','/')
        path=root/relative;assert root in path.resolve().parents
        path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(source)
        outputs.append(dict(skill=id,relative=relative,sha256=hashlib.sha256(source).hexdigest(),provenance='surviving_original_same_family_controller_alias_not_exact_legacy_formula'))
    (root/'manifest.json').write_text(json.dumps(outputs,indent=2),'utf8')
    return outputs

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--config',required=True);p.add_argument('--skills',required=True);p.add_argument('--output',required=True);p.add_argument('--old-control-source',required=True);args=p.parse_args()
    result=compile_pack(args.config,args.skills,args.output,args.old_control_source)
    print('COMPILED_REBIRTH_FORMULAS',len(result),'assets; original IDs retained; no live installation')
