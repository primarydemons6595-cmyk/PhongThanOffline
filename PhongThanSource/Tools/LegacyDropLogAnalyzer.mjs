import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
export function analyze(events,parseErrors=[]) {
 const deaths=new Map(),runs=[],issues=[];
 let activeRun='';
 const get=id=>{if(!deaths.has(id))deaths.set(id,{id,events:[],sections:new Map(),items:new Map(),spawns:new Map(),attempts:new Map(),pickups:new Set(),removes:new Map(),rolledBack:new Set(),deathCount:0,rollHits:0,rollMisses:0});return deaths.get(id)};
 for(const e of events){if(!e||typeof e.event!=='string'){parseErrors.push({error:'INVALID_EVENT_SCHEMA'});continue;}if(e.event.startsWith('SELF_TEST_'))runs.push(e);if(e.event==='SELF_TEST_BEGIN')activeRun=e.nonce??'';if(e.event==='SELF_TEST_END')activeRun='';if(!e.death_id)continue;const d=get(e.death_id);d.events.push(e);
  if(e.event==='NPC_DEATH'){d.deathCount++;d.context=e;d.run=activeRun;}
  if(e.event==='DROP_PROFILE')d.profile=e;
  if(e.event==='TABLE_SECTION')d.sections.set(`${e.slot}/${e.section}`,e);
  if(e.event==='DROP_ROLL'){if(e.outcome==='HIT')d.rollHits++;else if(e.outcome==='MISS')d.rollMisses++;
   const range=+e.range,rate=+e.threshold,roll=+e.rng_draw;if(!Number.isInteger(range)||range<=0||!Number.isInteger(rate)||rate<0||rate>range||!Number.isInteger(roll)||roll<0||roll>=range||(roll<rate)!==(e.outcome==='HIT'))issues.push({death_id:d.id,kind:'INVALID_ROLL_DECISION',slot:e.slot,section:e.section});}
  if(e.event==='ITEM_CREATE'&&e.result==='CREATED'){if(d.items.has(e.serial))issues.push({death_id:d.id,kind:'DUPLICATE_ITEM_SERIAL',serial:e.serial});d.items.set(e.serial,e);}
  if(e.event==='GROUND_SPAWN'&&e.result==='SPAWNED')d.spawns.set(e.serial,e);
  if(e.event==='ITEM_PICKUP'){d.attempts.set(e.serial,e);if(e.result==='PICKED_UP')d.pickups.add(e.serial);}
  if(e.event==='GROUND_REMOVE')d.removes.set(e.serial,e);
  if(e.event==='ITEM_ROLLBACK')d.rolledBack.add(e.serial);
  if(e.event==='DROP_END')d.end=e;
 }
 const groups=new Map(),itemRates=new Map(),levels={},magicLines={},errors={},zero=[],pickup=[],deathRows=[],equipment=[];
 for(const d of deaths.values()){
  if(!d.context)continue;const c=d.context,pid=c.pid??d.id.split('-')[0],key=[pid,c.test_scope??'UNKNOWN',d.run??'',c.template,c.npc_level,c.player_level,c.npc_special,c.mode,d.profile?.profile??'?',d.profile?.level_rule??'?',d.profile?.treasure_rule??'?'].join('/');
  if(!groups.has(key))groups.set(key,{key,pid,test_scope:c.test_scope,run_nonce:d.run,template:c.template,npc_level:c.npc_level,player_level:c.player_level,npc_special:c.npc_special,mode:c.mode,profile:d.profile?.profile??'?',level_rule:d.profile?.level_rule??'?',treasure_rule:d.profile?.treasure_rule??'?',deaths:0,hits:0,misses:0,created:0,ground:0,rolled_back:0,picked:0});
  const g=groups.get(key);g.deaths++;g.hits+=d.rollHits;g.misses+=d.rollMisses;g.created+=d.items.size;g.ground+=d.spawns.size;g.rolled_back+=d.rolledBack.size;g.picked+=d.pickups.size;
  if(d.deathCount>1)issues.push({death_id:d.id,kind:'DUPLICATE_DEATH_EVENT',count:d.deathCount});
  if(d.events.some(e=>e.event==='NORMAL_ENGINE_DISPATCH')&&d.spawns.size)issues.push({death_id:d.id,kind:'TWO_NORMAL_ENGINES_DISPATCHED_WITH_LEGACY_GROUND'});
  const errorEvents=d.events.filter(e=>e.event==='DROP_ERROR');for(const e of errorEvents)errors[e.reason]=(errors[e.reason]??0)+1;
  if(!d.end&&!errorEvents.length)issues.push({death_id:d.id,kind:'INCOMPLETE_DEATH_LOG'});
  const seenTuples=new Set();
  for(const e of d.events.filter(e=>e.event==='DROP_ROLL')){
   const s=d.sections.get(`${e.slot}/${e.section}`),tuple=s?[s.Genre,s.Detail,s.Particular].join('/'):'UNKNOWN';const rk=[key,e.slot,e.section,tuple].join('/');
   if(!itemRates.has(rk))itemRates.set(rk,{group:key,slot:e.slot,section:e.section,tuple,trials:0,hits:0,deaths_with_hit:0,created:0,ground:0,picked:0,deaths_with_ground:0});
   const r=itemRates.get(rk);r.trials++;if(e.outcome==='HIT'){r.hits++;if(!seenTuples.has(rk)){r.deaths_with_hit++;seenTuples.add(rk);}}
  }
  const seenGround=new Set();for(const [serial,e] of d.items){
   const rk=[key,e.slot,e.section,[e.genre,e.detail,e.particular].join('/')].join('/'),r=itemRates.get(rk);if(r){r.created++;if(d.spawns.has(serial)){r.ground++;if(!seenGround.has(rk)){r.deaths_with_ground++;seenGround.add(rk);}}if(d.pickups.has(serial))r.picked++;}
   if(!d.spawns.has(serial)&&!d.rolledBack.has(serial))issues.push({death_id:d.id,kind:'CREATED_WITHOUT_GROUND',serial});
   if(+e.genre===0){levels[e.level]=(levels[e.level]??0)+1;const mk=`quality=${e.quality??'UNRECORDED'}/lines=${e.magic_lines??'UNRECORDED'}`;magicLines[mk]=(magicLines[mk]??0)+1;
    equipment.push({death_id:d.id,pid,template:c.template,npc_level:c.npc_level,player_level:c.player_level,item_level:e.level,actual_native_level:e.actual_native_level,required_player_level:e.required_player_level,serial,quality:e.quality,magic_lines:e.magic_lines,level_rule:d.profile?.level_rule,mode:c.mode});
    if(d.profile?.level_rule==='MONSTER_EQUIPMENT_CATALOG'&&(!Number.isInteger(+e.required_player_level)||+e.required_player_level>+c.npc_level||+e.actual_native_level!==+e.level))issues.push({death_id:d.id,kind:'MONSTER_EQUIPMENT_NATIVE_LEVEL_OR_REQUIREMENT_INVALID',serial});
    if(+e.level>Math.max(1,Math.ceil(+c.npc_level/10)))issues.push({death_id:d.id,kind:'REVIEW_LOW_NPC_HIGH_ITEM_TIER',serial,npc_level:c.npc_level,item_level:e.level,confidence:'HEURISTIC_NOT_KINGSOFT_RULE'});
   }
  }
  for(const [serial,s] of d.spawns){const a=d.attempts.get(serial),removed=d.removes.get(serial);let status=d.pickups.has(serial)?'PICKED_UP':removed?.reason==='SELF_TEST_CLEANUP'?'SELF_TEST_CLEANUP_NOT_PICKUP_TEST':a?'PICKUP_ATTEMPT_WITHOUT_SUCCESS':removed?'EXPIRED_OR_REMOVED_NO_PICKUP':'PENDING_NO_ATTEMPT';
   pickup.push({death_id:d.id,serial,object:s.object,world_object_id:s.world_object_id,status});if(status==='PICKUP_ATTEMPT_WITHOUT_SUCCESS')issues.push({death_id:d.id,kind:status,serial});
  }
  const reason=errorEvents.length?`ERROR:${errorEvents.map(e=>e.reason).join('|')}`:d.end?.reason??'INCOMPLETE';
  const row={death_id:d.id,template:c.template,npc_level:c.npc_level,player_level:c.player_level,special:c.npc_special,mode:c.mode,profile:d.profile?.profile??'?',hits:d.rollHits,misses:d.rollMisses,created:d.items.size,ground:d.spawns.size,rolled_back:d.rolledBack.size,picked:d.pickups.size,reason,test_scope:c.test_scope};deathRows.push(row);if(!d.spawns.size)zero.push(row);
 }
 for(const r of itemRates.values()){r.hit_rate=r.trials?r.hits/r.trials:0;r.death_hit_rate=r.deaths_with_hit/groups.get(r.group).deaths;r.ground_per_death=r.ground/groups.get(r.group).deaths;r.ground_death_rate=r.deaths_with_ground/groups.get(r.group).deaths;}
 return {schema:1,scope:'FUNCTIONAL_TEST',parity:'KINGSOFT_PARITY_UNVERIFIED',events:events.length,deaths:deathRows.length,parseErrors,runs,groups:[...groups.values()],itemRates:[...itemRates.values()],levels,equipment,magicLines,errors,deathRows,zero,pickup,issues,pickupEvidence:pickup.some(p=>p.status==='PICKED_UP')?'OBSERVED':'NOT_OBSERVED',notes:['PENDING_NO_ATTEMPT is not proof that pickup is broken.','AUTOMATED_UNOWNED runs suppress callbacks only on newly spawned test entities; they do not prove player inventory or quest runtime correctness.','High tier review uses ceil(npc_level/10) as an explicitly unverified diagnostic heuristic, not an execution gate.','Item rates are separated by process/build, template, level, special, mode, profile, level/Treasure policy, slot and section.']};
}
function csv(rows){if(!rows.length)return '';const keys=[...new Set(rows.flatMap(Object.keys))],quote=x=>'"'+String(x??'').replaceAll('"','""')+'"';return [keys.map(quote).join(','),...rows.map(r=>keys.map(k=>quote(r[k])).join(','))].join('\n')+'\n'}
export function writeReport(result,out,previous){fs.mkdirSync(out,{recursive:true});
 for(const [name,rows] of Object.entries({deaths:result.deathRows,groups:result.groups,item_rates:result.itemRates,equipment_levels:result.equipment,zero_items:result.zero,pickup:result.pickup,issues:result.issues}))fs.writeFileSync(path.join(out,`${name}.csv`),csv(rows));
 fs.writeFileSync(path.join(out,'summary.json'),JSON.stringify(result,null,2)+'\n');
 const critical=result.issues.filter(i=>!i.kind.startsWith('REVIEW_'));
 let comparison='';if(previous){const before=JSON.parse(fs.readFileSync(previous,'utf8')),by=new Map();
  const key=(r,s)=>{const g=s.groups.find(g=>g.key===r.group);return [g.test_scope,g.template,g.npc_level,g.player_level,g.npc_special,g.mode,g.profile,r.slot,r.section,r.tuple].join('/')};
  for(const r of before.itemRates){const k=key(r,before);if(!by.has(k))by.set(k,[]);by.get(k).push(r);}
  const rows=result.itemRates.map(r=>{const matches=by.get(key(r,result))??[],old=matches.length===1?matches[0]:null;return {...r,before_group:old?.group??'',before_hit_rate:old?.hit_rate??'',hit_rate_delta:old?r.hit_rate-old.hit_rate:matches.length?'AMBIGUOUS_PREVIOUS_VARIANT':'NEW_GROUP'};});fs.writeFileSync(path.join(out,'comparison.csv'),csv(rows));comparison='\nĐã xuất comparison.csv; delta là mô tả mẫu, không tự chứng minh thay đổi xác suất hay parity.\n';}
 const text=`# Legacy Drop runtime log analysis\n\nScope: FUNCTIONAL_TEST. KINGSOFT_PARITY_UNVERIFIED.\n\nĐọc ${result.events} event, ${result.deaths} death, ${result.parseErrors.length} lỗi JSON và ${critical.length} diagnostic cần điều tra. Pickup: ${result.pickupEvidence}.\n\n| Template/level/special/mode/profile | Death | HIT | MISS | Created | Ground | Picked |\n|---|---:|---:|---:|---:|---:|---:|\n${result.groups.map(g=>`| ${g.key} | ${g.deaths} | ${g.hits} | ${g.misses} | ${g.created} | ${g.ground} | ${g.picked} |`).join('\n')}\n\nChọn bảng thực tế xem TABLE_SECTION/DROP_BIND trong log và item_rates.csv (slot/section/tuple). Phân bố cấp: ${JSON.stringify(result.levels)}. Magic theo quality native và số dòng: ${JSON.stringify(result.magicLines)}. Không gọi mọi trang bị có dòng magic là đồ xanh.\n\nLỗi: ${JSON.stringify(result.errors)}. death_id và serial cụ thể nằm trong issues.csv, deaths.csv, pickup.csv. Không có ITEM_CREATE là lỗi khi có DROP_ERROR, hoặc có lý do MISS/SHADOW/level gate trong zero_items.csv; không gộp chúng thành thành công rơi 0 item.\n\nPENDING_NO_ATTEMPT chưa đủ kết luận không nhặt được. Pickup có ATTEMPT nhưng chưa SUCCESS được đánh dấu để điều tra. SELF_TEST_CLEANUP không phải test pickup. CREATED_WITHOUT_GROUND bỏ qua ITEM_ROLLBACK có log; log bị cắt có thể gây diagnostic thiếu.\n\nNPC thấp/item tier cao chỉ là REVIEW với heuristic ceil(npc_level/10), chưa phải quy tắc Kingsoft. Quest callback BEGIN/END chỉ chứng minh dispatch; LUA_REWARD có api/item/serial mới là quan sát native item reward.\n${comparison}`;
 fs.writeFileSync(path.join(out,'REPORT.md'),text);return critical.length;
}
if(process.argv[1]&&path.resolve(process.argv[1])===fileURLToPath(import.meta.url)){
 const [input,out,...args]=process.argv.slice(2);if(!input||!out)throw Error('Usage: node LegacyDropLogAnalyzer.mjs log.jsonl outputDir [--compare previous/summary.json]');
 const paths=[4,3,2,1].map(n=>input+'.'+n).filter(f=>fs.existsSync(f));paths.push(input);const events=[],parseErrors=[];
 for(const file of paths){const lines=fs.readFileSync(file,'utf8').split(/\r?\n/);for(let i=0;i<lines.length;i++){if(!lines[i])continue;try{events.push(JSON.parse(lines[i]))}catch(e){parseErrors.push({file,line:i+1,error:e.message})}}}
 const result=analyze(events,parseErrors),compare=args[0]==='--compare'?args[1]:undefined;writeReport(result,out,compare);console.log(JSON.stringify({deaths:result.deaths,events:events.length,parseErrors:parseErrors.length,issues:result.issues.length,pickup:result.pickupEvidence}));
 if(parseErrors.length)process.exitCode=2;
}
