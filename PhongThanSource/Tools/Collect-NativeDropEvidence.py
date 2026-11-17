from pathlib import Path
import json,datetime,collections
root=Path(__file__).resolve().parents[2]
report=root/'analysis/native_drop_completion_20261018'
manifest=json.loads((report/'deployment.json').read_text(encoding='utf-8-sig'))
server=Path(manifest['Server']);client=Path(manifest['Client'])
def read(p):
 if not p.exists():return []
 lines=p.read_text(encoding='utf-8').splitlines();rows=[]
 for i,line in enumerate(lines):
  if not line.strip():continue
  try:rows.append(json.loads(line))
  except json.JSONDecodeError:
   if i!=len(lines)-1:raise
 return rows
legacy=[]
for p in reversed([server/'legacy_drop_debug.jsonl']+list(server.glob('legacy_drop_debug.jsonl.*'))):legacy+=read(p)
legacy=[v for v in legacy if v.get('utc','')>=manifest['StartedUtc']]
legacy.sort(key=lambda v:(v.get('utc',''),int(v.get('log_seq',0))))
sm=read(server/'p1_blue_magic.jsonl');cm=read(client/'p1_blue_magic.jsonl')
deaths={v['death_id']:v for v in legacy if v.get('event')=='NPC_DEATH'}
chains=[]
for item in [v for v in legacy if v.get('event')=='ITEM_CREATE' and v.get('result')=='CREATED']:
 serial=item['serial'];death=item['death_id']
 chains.append({'death_id':death,'serial':serial,'npc':deaths.get(death),'drop_events':[v for v in legacy if v.get('death_id')==death and (v.get('serial')==serial or v.get('event') in ['DROP_PROFILE','DROP_BIND','ITEM_LEVEL','MAGIC_ROLL'])],'server_magic_by_serial':[v for v in sm if str(v.get('serial'))==serial],'client_magic_by_serial':[v for v in cm if str(v.get('serial'))==serial]})
result={'status':'PLAYER_EVIDENCE_REQUIRES_REVIEW','automaticPass':False,'event_counts':dict(collections.Counter(v.get('event') for v in legacy)),'deaths':len(deaths),'items':len(chains),'errors':[v for v in legacy if v.get('event') in ['DROP_ERROR','QUARANTINE_DETAIL','BLUE_SKIP']],'chains':chains,'currency':[v for v in legacy if v.get('kind')=='MONEY'],'note':'Review serial, character, process and tooltip evidence. Native fixture/build success never establishes live acceptance.'}
out=report/('player_logs_'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%d_%H%M%S'));out.mkdir()
(out/'chains.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
(out/'legacy.jsonl').write_text('\n'.join(json.dumps(v) for v in legacy)+'\n',encoding='utf-8')
print(json.dumps({'folder':str(out),'deaths':len(deaths),'items':len(chains),'status':result['status']}))
