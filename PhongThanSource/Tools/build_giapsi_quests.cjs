// Build explicit loose overrides; PAKs and character stores are never edited.
const fs=require('fs'),path=require('path'),crypto=require('crypto');
const R=path.resolve(__dirname,'../..'),S=path.join(R,'PhongThanRuntime-Loose/Server');
const A=path.join(R,'analysis/giapsi_20261024');
const O=path.join(R,'PhongThanSource/Deploy/ProjectContent/GiapSiQuests');
const {steps,roles,added,kills}=require('./giapsi/quests.cjs');
const cs=fs.readFileSync(path.join(R,'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.Core/Workspace.cs'),'utf8');
const high=JSON.parse('"'+cs.match(/const string TcvnHigh = "([^"]+)"/)[1]+'"');
const chars=Array.from({length:256},(_,x)=>x<128?String.fromCharCode(x):high[x-128]);
const enc=s=>Buffer.from(Array.from(s,c=>{let n=chars.indexOf(c);if(n<0)n=chars.indexOf(c.toLowerCase());if(n<0)throw Error('TCVN3 character '+c);return n;}));
const dec=b=>{let s='';for(const x of b)s+=chars[x];return s;};
const hash=b=>crypto.createHash('sha256').update(b).digest('hex');
const walk=d=>fs.existsSync(d)?fs.readdirSync(d,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(path.join(d,e.name)):[path.join(d,e.name)]):[];
const cp=b=>new TextDecoder('windows-1252').decode(b);
const mark=Buffer.from('\r\n-- PT_GIAPSI_20261024_BEGIN\r\n');
const assets=new Map(),edits=[],placements=[],bounds=[];
function put(rel,b){assets.set(rel.replaceAll('\\','/'),Buffer.isBuffer(b)?b:enc(b));}
// Retain authored region payloads on rebuild after deployment. Runtime copies
// are the merge base so unrelated subsequent world edits are preserved.
const previousManifest=path.join(O,'manifest.json');
if(fs.existsSync(previousManifest)){
 const previous=JSON.parse(fs.readFileSync(previousManifest,'utf8'));
 placements.push(...(previous.placements||[]).map(p=>p.script==='script/phongthan/giapsi/npc_thong.lua'?{...p,tid:202}:p));
 bounds.push(...(previous.geometry||[]));
 for(const e of previous.files||[])if(/^settings\/phongthan\/npc_regions\/.*\.dat$/.test(e.relative)){
  const current=path.join(S,e.relative),compiled=path.join(O,'compiled',e.relative);
  if(fs.existsSync(current))put(e.relative,fs.readFileSync(current));else if(fs.existsSync(compiled))put(e.relative,fs.readFileSync(compiled));
 }
}
function original(rel){let b=fs.readFileSync(path.join(S,rel));let i=b.indexOf(mark);return i<0?b:b.subarray(0,i);}
function bundle(rel,append){put(rel,Buffer.concat([original(rel),mark,enc(append)]));}
function lua(v){if(v===null)return 'nil';if(typeof v==='string')return JSON.stringify(v);if(typeof v==='number')return String(v);if(Array.isArray(v))return '{'+v.map(lua).join(',')+'}';return '{'+Object.entries(v).map(([k,x])=>(/^\d+$/.test(k)?'['+k+']':k)+'='+lua(x)).join(',')+'}';}
// Quests fit the existing three-column buttons: full names are in the prompt;
// button labels are intentional short actions, never truncated strings.
for(const a of steps){
 a.label=a.rewardChoice?['','Nhận giày','Nhận yêu đái','Nhận mão'][a.rewardChoice]:a.title.split(':')[0];
 if(enc(a.label).length>13)throw Error('Button label exceeds 95px at font14: '+a.label);
}
const goalNotes={
 '3:83':'Tham Lam: Thương Nhân, Đại Phong (200,200). Vô Tri: Cửu Di, Tam Sơn (200,200). Tàn Nhẫn: dùng Mật Đồ tại Bắc Hải (190,206).',
 '25:0':'Diệt Hoàn Cẩu Vương tại Yến Sơn (235,208), lấy đủ ba bộ phận đao.',
 '24:1':'Diệt Hoàn Cẩu Tinh tại Yến Sơn (213,187), (202,193), (213,194); về Sùng ứng Loan.',
 '3:24':'Diệt Trấn Tướng Quân tại Mục Dã (187,189), lấy ấn tín Võ Thành Vương.',
 '3:30':'Diệt Thần Long tại Long Cung tầng 3 (206,175), lấy Râu Thần Long.',
 '3:41':'Diệt Cự Dã tại Bích Du Cung tầng 4: (181,199), (196,204), (185,217); lấy An Cư Đồ.',
 '3:50':'Diệt Trạnh Nanh tại Khổn Tiên Cung tầng 1 (197,212), lấy Thần Dụ Kính.',
 '3:62':'Lấy Huỳnh Đế Nội Kinh ở tầng 2 (196,189); Luyện Kim Thuật ở tầng 3 (206,204); Huyền Nữ Binh Pháp ở tầng 4 (191,210), Khổn Tiên Cung.',
 '3:72':'Diệt Bộ Đầu quân Chu tại Ngọc Hư Cung mười năm sau (206,204), lấy Trúc Giản.',
 '3:82':'Diệt Bàn Cổ tại Khổn Tiên Cung tầng 5 (199,212), lấy Bàn Cổ Thần Khí.',
 '4550:0':'Diệt Hoàn Cẩu tại Mạnh Tân (188,210) đến (192,210), rồi trong 10 giây diệt Giáp Cốt ngay phía nam để lấy đủ 5 Hạp Đằng.',
 '4550:2':'Hái 10 Nguyên Sâm tại Tam Sơn, từ (196,203) đến (205,203). Cẩn thận Nguyên Tham Tinh ở gần đó.',
 '4550:4':'Diệt Ngưu Sát tại Đồng Quan (195,208) đến (199,208), dùng Tịnh Hóa Phù, rồi tịnh hóa 5 Sơn Xuyên Liễu ở hàng (195,211) đến (199,211).'
};
const rewardText={
 '25:3':'Pháp bảo cấp 10; 10 điểm danh vọng.',
 '24:3':'5 Hồi Thành Phù; sách Bàn Cổ Khai Thiên.',
 '3:11':'Mật tịch Ban Môn Lộng Phủ.',
 '3:27':'Vũ khí hoàng kim cấp 40.',
 '3:33':'Bá Lạc Nhãn cấp 10.',
 '3:43':'Sách kỹ năng ngẫu nhiên của ba hệ phái.',
 '3:52':'Vũ khí hoàng kim cấp 70.',
 '3:63':'Một bộ bốn pháp bảo cấp 70.',
 '3:74':'Lam Bảo Thạch; Khứ Lai Phù.',
 '3:85':'Một trang bị lục cấp 80.',
 '4550:5':'Một trang bị lục.'
};
for(const a of steps){const k=a.task+':'+a.from;if(goalNotes[k])a.note=goalNotes[k];if(rewardText[k])a.rewardText=rewardText[k];if(a.rewardChoice)a.rewardText=['','Giày lục cấp 90.','Yêu đái lục cấp 90.','Mão lục cấp 90.'][a.rewardChoice];}
// The fifth equipment tuple field is the VNG profession/series marker, not
// a boolean for green quality. Resolve it from the exact reward template.
const equipmentFiles={0:'meleeweapon.txt',2:'armor.txt',4:'amulet.txt',5:'boot.txt',6:'belt.txt',7:'helm.txt',10:'horse.txt'};
for(const a of steps)for(const gift of a.gifts)if(gift[0]===0){
 const rows=fs.readFileSync(path.join(S,'settings/item/001',equipmentFiles[gift[1]]),'latin1').split(/\r?\n/).map(x=>x.split('\t'));
 const row=rows.find(r=>+r[1]===0&&+r[2]===gift[1]&&+r[3]===gift[2]&&+r[11]===gift[3]);
 if(!row)throw Error('Missing reward equipment template '+gift);
 gift[4]=+row[9]===1000?0:+row[9];
}
const books=dec(fs.readFileSync(path.join(S,'settings/item/001/skillbook.txt'))).split(/\r?\n/).map(x=>x.split('\t')).filter(x=>{
 const m=(x[8]||'').match(/(đạo sĩ|dị nhân|giáp sĩ) cấp (\d+)/i);if(!m)return false;
 const lvl=+m[2];return m[1].toLowerCase()==='giáp sĩ'?lvl>=54&&lvl<=72:lvl>=60&&lvl<=74;
}).map(x=>[7,+x[2],+x[3],1,0,0,1]);
if(books.length<5)throw Error('Missing verified skill-book reward pool');
let common;
const npcTable=fs.readFileSync(path.join(S,'settings/phongthan/Npcs.txt'),'latin1').split(/\r?\n/).map(x=>x.split('\t'));
const headers=npcTable[0],deathCol=headers.indexOf('DeathScript');
while(npcTable.length>1&&npcTable[npcTable.length-1].every(x=>!x))npcTable.pop();
if(npcTable.length<2704||npcTable.length>2705)throw Error('Unexpected NPC template registry size');
// A story boss must not inherit VTT's hard-coded template148 spawn, event
// credit or overflow-prone 30,000,000 * 100 life calculation.
const story=[...npcTable[149]];while(story.length<headers.length)story.push('');
story[0]=enc('Thông Thiên Giáo Chủ').toString('latin1');story[headers.indexOf('LifeParam1')]='1000000';
for(const column of ['DropRateFile','DropRateFile1','DropRateFile2','DropRateFile3'])story[headers.indexOf(column)]='';
story[deathCol]='\\script\\phongthan\\giapsi\\death_2703.lua';npcTable[2704]=story;
npcTable[149][deathCol]='\\script\\phongthan\\van_tien\\boss4.lua';
const clientRows=fs.readFileSync(path.join(R,'PhongThanRuntime-Loose/Client/settings/phongthan/Npcs.txt'),'latin1').split(/\r?\n/).map(x=>x.split('\t'));
while(clientRows.length>1&&clientRows[clientRows.length-1].every(x=>!x))clientRows.pop();
if(clientRows.length<2704||clientRows.length>2705)throw Error('Unexpected client NPC registry size');
clientRows[2704]=[...story];put('client/settings/phongthan/Npcs.txt',Buffer.from(clientRows.map(x=>x.join('\t')).join('\r\n')+'\r\n','latin1'));
const worldRows=fs.readFileSync(path.join(S,'settings/WorldSet.ini'),'latin1');
const worldNames=Object.fromEntries([...worldRows.matchAll(/^(\d+)=([^\r\n]+)/gm)].map(m=>[+m[1],cp(Buffer.from(m[2],'latin1'))]));
function npcPayload(b,combined){if(!combined)return b;const n=b.readUInt32LE(0),base=4+n*8;return b.subarray(base+b.readUInt32LE(20),base+b.readUInt32LE(20)+b.readUInt32LE(24));}
function records(b){if(!b.length)return [];let off=12,out=[];for(let i=0;i<b.readUInt32LE(0);i++){const len=60+b.readUInt16LE(off+58);out.push(b.subarray(off,off+len));off+=len;}if(off!==b.length)throw Error('NPC payload trailing bytes');return out;}
const live=[];
for(const p of walk(path.join(S,'settings/phongthan/npc_regions'))){
 if(!/_(npc|region)_s\.dat$/.test(p))continue;
 const rel=path.relative(S,p).replaceAll('\\','/'),map=+rel.split('/')[3];
 const combined=p.endsWith('_region_s.dat');
 if(!combined&&fs.existsSync(p.replace('_npc_s.dat','_region_s.dat')))continue;
 for(const b of records(npcPayload(fs.readFileSync(p),combined)))live.push({map,b,rel,combined,tid:b.readInt32LE(0),x:b.readInt32LE(4),y:b.readInt32LE(8),script:b.subarray(60).toString('latin1').replaceAll('\\','/').replace(/^\//,'')});
}
// Include the base NPC sections as well as authored overrides when checking
// duplicate actors and objective positions.
for(const map of [...new Set([...Object.values(added).map(x=>x[0]),...kills.map(x=>x[1]),1007])]){
 for(const p of walk(path.join(S,'maps',worldNames[map]+'_S'))){
  const m=p.replaceAll('\\','/').match(/v_(\d{3})\/(\d{3})_region_s\.dat$/);if(!m)continue;
  const rel=`settings/phongthan/npc_regions/${map}/v_${m[1]}/${m[2]}_region_s.dat`;
  if(fs.existsSync(path.join(S,rel)))continue;
  for(const b of records(npcPayload(fs.readFileSync(p),true)))live.push({map,b,rel,combined:true,tid:b.readInt32LE(0),x:b.readInt32LE(4),y:b.readInt32LE(8),script:b.subarray(60).toString('latin1').replaceAll('\\','/').replace(/^\//,'')});
 }
}
function replaceRecord(row,b){
 let old=assets.get(row.rel)||fs.readFileSync(path.join(S,row.rel));
 let payload=npcPayload(old,row.combined),list=records(payload),found=false;
 list=list.map(r=>{if(r.equals(row.b)){found=true;return b;}return r;});
 if(!found)throw Error('NPC binding changed '+row.rel);
 const next=Buffer.concat([payload.subarray(0,12),...list]);
 if(!row.combined)put(row.rel,next);
 else {
  let n=old.readUInt32LE(0),head=4+n*8,parts=[];const h=Buffer.from(old.subarray(0,head));let offset=0;
  for(let i=0;i<n;i++){let part=i===2?next:old.subarray(head+old.readUInt32LE(4+i*8),head+old.readUInt32LE(4+i*8)+old.readUInt32LE(8+i*8));h.writeUInt32LE(offset,4+i*8);h.writeUInt32LE(part.length,8+i*8);offset+=part.length;parts.push(part);}put(row.rel,Buffer.concat([h,...parts]));
 }
}
// Remove only the twelve remote-template actors authored by the previous Giao Huan revision.
for(const rel of [...assets.keys()])if(rel.startsWith('settings/phongthan/npc_regions/1007/')){
 const combined=rel.endsWith('_region_s.dat'),base=assets.get(rel),payload=npcPayload(base,combined),list=records(payload);
 const kept=list.filter(r=>!(r.readInt16LE(54)===0&&((r.readInt32LE(0)===13&&Math.floor(r.readInt32LE(8)/512)===190&&Math.floor(r.readInt32LE(4)/256)>=200&&Math.floor(r.readInt32LE(4)/256)<=205)||(r.readInt32LE(0)===16&&Math.floor(r.readInt32LE(8)/512)===196&&Math.floor(r.readInt32LE(4)/256)>=201&&Math.floor(r.readInt32LE(4)/256)<=206))));
 if(kept.length!==list.length){const h=Buffer.from(payload.subarray(0,12));h.writeUInt32LE(kept.length,0);const next=Buffer.concat([h,...kept]);
  if(!combined)put(rel,next);else{const n=base.readUInt32LE(0),head=4+n*8,hdr=Buffer.from(base.subarray(0,head));let offset=0;const parts=[];for(let i=0;i<n;i++){let part=i===2?next:base.subarray(head+base.readUInt32LE(4+i*8),head+base.readUInt32LE(4+i*8)+base.readUInt32LE(8+i*8));hdr.writeUInt32LE(offset,4+i*8);hdr.writeUInt32LE(part.length,8+i*8);offset+=part.length;parts.push(part);}put(rel,Buffer.concat([hdr,...parts]));}}
}
for(let i=placements.length-1;i>=0;i--)if(placements[i].map===1007&&placements[i].kind===0&&[13,16].includes(placements[i].tid))placements.splice(i,1);

// Revision of our own authored dialogue actor only. Preserve the coordinates,
// action script and every other NPC in that region.
for(const row of live)if(row.map===1070&&row.tid===148&&row.b.readInt16LE(54)===3&&row.script==='script/phongthan/giapsi/npc_thong.lua'){
 const next=Buffer.from(row.b);next.writeInt32LE(202,0);replaceRecord(row,next);row.b=next;row.tid=202;
}
function point(map,x,y){
 const wx=x*256+128,wy=y*512+256,rx=Math.floor(wx/512),ry=Math.floor(wy/1024);
 const wor=path.join(S,'maps',worldNames[map]+'.wor');
 if(!fs.existsSync(wor))throw Error('Missing map '+map);
 const m=fs.readFileSync(wor,'latin1').match(/rect\s*=\s*(\d+),(\d+),(\d+),(\d+)/i);
 if(!m||rx<+m[1]||ry<+m[2]||rx>+m[3]||ry>+m[4])throw Error('Outside map '+[map,x,y]);
 let base=path.join(S,'maps',worldNames[map]+'_S',`v_${String(ry).padStart(3,'0')}`,`${String(rx).padStart(3,'0')}_region_s.dat`);
 let geometryKind='SERVER_OBSTACLE';
 if(!fs.existsSync(base)){base=path.join(R,'PhongThanRuntime-Loose/Client/maps',worldNames[map],`v_${String(ry).padStart(3,'0')}`,`${String(rx).padStart(3,'0')}_region_c.dat`);geometryKind='CLIENT_OBSTACLE_ONLY';}
 if(!fs.existsSync(base))throw Error('Missing geometry '+base);
 const b=fs.readFileSync(base),head=4+b.readUInt32LE(0)*8,len=b.readUInt32LE(8),off=head+b.readUInt32LE(4);
 if(len!==0&&len!==2048)throw Error('Unknown obstacle format');
 let chosen;
 for(const dx of [128,144,112,160,96,176,80,192,64,208,48,224,32,240,16]){
  for(const dy of [256,272,240,288,224,304,208,320,192,336,176,352,160,368,144,384,128,400,112,416,96,432,80,448,64,464,48,480,32,496,16]){
   const xx=x*256+dx,yy=y*512+dy,gx=Math.floor((xx%512)/32),gy=Math.floor((yy%1024)/32),v=len?b.readUInt32LE(off+4*(gx*32+gy)):0;
   if((v&15)===0){chosen=[xx,yy];break;}
  }if(chosen)break;
 }
 if(!chosen)throw Error('No free point in guide tile '+[map,x,y]);
 bounds.push({map,x,y,mps:chosen,geometryKind,geometry:path.relative(S,base).replaceAll('\\','/'),sha256:hash(b)});
 return chosen;
}
function addNpc(map,tid,x,y,name,script,kind=3,level=1){
 const [wx,wy]=point(map,x,y),rx=Math.floor(wx/512),ry=Math.floor(wy/1024);
 const logical=script?'\\'+script.replaceAll('/','\\'):'',nb=enc(name),sb=Buffer.from(logical,'latin1');
 if(nb.length>31||sb.length>127)throw Error('NPC record field overflow');
 let b=Buffer.alloc(60+sb.length);b.writeInt32LE(tid,0);b.writeInt32LE(wx,4);b.writeInt32LE(wy,8);nb.copy(b,16);b.writeInt16LE(level,48);b.writeInt16LE(kind,54);b[56]=+npcTable[tid+1][2]||0;b[57]=+npcTable[tid+1][3]||0;b.writeUInt16LE(sb.length,58);sb.copy(b,60);
 const dir=`settings/phongthan/npc_regions/${map}/v_${String(ry).padStart(3,'0')}`;
 let rel=dir+`/${String(rx).padStart(3,'0')}_npc_s.dat`,combined=false;
 if(assets.has(rel.replace('_npc_s.dat','_region_s.dat'))||fs.existsSync(path.join(S,rel.replace('_npc_s.dat','_region_s.dat')))){rel=rel.replace('_npc_s.dat','_region_s.dat');combined=true;}
 let old=assets.get(rel)||(fs.existsSync(path.join(S,rel))?fs.readFileSync(path.join(S,rel)):null);
 if(!old){
  const base=path.join(S,'maps',worldNames[map]+'_S',`v_${String(ry).padStart(3,'0')}`,`${String(rx).padStart(3,'0')}_region_s.dat`);
  if(fs.existsSync(base)&&records(npcPayload(fs.readFileSync(base),true)).length){rel=rel.replace('_npc_s.dat','_region_s.dat');old=assets.get(rel)||fs.readFileSync(base);combined=true;}
 }
 if(old){let payload=npcPayload(old,combined),list=records(payload);const duplicate=list.some(r=>r.readInt32LE(0)===tid&&r.readInt32LE(4)===wx&&r.readInt32LE(8)===wy);if(duplicate)return;const header=Buffer.from(payload.subarray(0,12));header.writeUInt32LE(list.length+1,0);const next=Buffer.concat([header,...list,b]);
  if(combined){const n=old.readUInt32LE(0),head=4+n*8,h=Buffer.from(old.subarray(0,head)),parts=[];let offset=0;for(let i=0;i<n;i++){let p=i===2?next:old.subarray(head+old.readUInt32LE(4+i*8),head+old.readUInt32LE(4+i*8)+old.readUInt32LE(8+i*8));h.writeUInt32LE(offset,4+i*8);h.writeUInt32LE(p.length,8+i*8);offset+=p.length;parts.push(p);}put(rel,Buffer.concat([h,...parts]));}else put(rel,next);
 }else{let h=Buffer.alloc(12);h.writeUInt32LE(1,0);put(rel,Buffer.concat([h,b]));}
 if(!placements.some(p=>p.map===map&&p.tid===tid&&p.x===x&&p.y===y&&p.kind===kind))placements.push({map,tid,x,y,name,script,kind,level,relative:rel});
}
for(const [role,[map,tid,x,y,name]] of Object.entries(added)){
 const matches=live.filter(r=>r.map===map&&r.tid===tid&&Math.abs(r.x/256-x)<3&&Math.abs(r.y/512-y)<3&&r.b.readInt16LE(54)===3);
 if(matches.length===1&&matches[0].script&&fs.existsSync(path.join(S,cp(Buffer.from(matches[0].script,'latin1'))))){roles[role]=[map,cp(Buffer.from(matches[0].script,'latin1'))];}
 else {
  const rel='script/phongthan/giapsi/npc_'+role+'.lua';roles[role]=[map,rel];
  // Boss/dialogue actors share an appearance but distinct kinds and positions.
  addNpc(map,tid,x,y,name,rel);
 }
}
const journal=require('./giapsi/journal.cjs')({steps,roles,added,kills,live,placements,fs,path,S,dec,enc});
// Include literal journal templates in the client override, independently of
// the original numeric registry. The records themselves come from the server.
for(const scheme of ['ui3','ui4']){
 const target=path.join(R,'PhongThanSource/Deploy/ProjectContent/Ui',scheme,'taskinfo.ini');
 const marker=Buffer.from('\r\n; PT_GIAPSI_JOURNAL\r\n');
 let source=fs.readFileSync(fs.existsSync(target)?target:path.join(path.dirname(target),'../ui3/taskinfo.ini')),cut=source.indexOf(marker);if(cut>=0)source=source.subarray(0,cut);
 fs.writeFileSync(target,Buffer.concat([source,marker,enc(journal.templates)]));
}
const journalLua='PTG_JOURNAL='+lua(journal.quests)+'\r\nPTG_PROGRESS='+lua([3,4,6,7,8,10,11,12,13,21,22,23,25,174,175,176,177,178,191,192,201])+'\r\n'+fs.readFileSync(path.join(__dirname,'giapsi/journal.lua'),'utf8');
common='PTG_STEPS='+lua(steps)+'\r\nPTG_KILLS='+lua(kills)+'\r\nPTG_BOOKS='+lua(books)+'\r\nPTG_TARGET_NAMES={[3]="Kiếm Nhân hoặc Xạ Nhân",[7]="Hoàn Cẩu"}\r\nPTG_TARGET_MAPS={[3]="Yến Sơn (204,224), (227,224), quái sẵn có ngoài Sùng Thành",[7]="Yến Sơn (235,226), quái sẵn có ngoài Sùng Thành"}\r\n'+journalLua+'\r\n'+fs.readFileSync(path.join(__dirname,'giapsi/runtime.lua'),'utf8');
common+='\r\n'+fs.readFileSync(path.join(__dirname,'starter_story_generated.lua'),'utf8');
// Login replay also repairs characters that accepted a quest before this fix.
bundle('script/player/playerlogin.lua',common+'\r\nPTG_OLD_LOGIN=main\r\nfunction main()\r\n if PTG_OLD_LOGIN then PTG_OLD_LOGIN() end\r\n PTGJournal()\r\nend\r\n');
// The recovered instructor used a runtime map ID against a content map ID.
const instructor='script/phongthan/npc_recovered_batch/1002_95ca852d.lua';
let instructorSource=original(instructor).toString('latin1');
instructorSource=instructorSource.replace('local w = pt_runtime_world_pos()','local w = GetContentWorldPos()+1000').replaceAll('IsMaster()','IsMantleMaster()');
put(instructor,Buffer.from(instructorSource,'latin1'));
for(const [role,[map,p]] of Object.entries(roles)){
 const rel=p.startsWith('script/')?p:'script/phongthan/'+p;
 let append=common+'\r\nPTG_MAP='+map+'\r\nPTG_ROLE='+lua(role)+'\r\n';
 append+='PTG_OLD_MAIN=main\r\nPTG_OLD_MARK=GetPlayerTaskState\r\nfunction main() return PTGMenu() end\r\nfunction GetPlayerTaskState() return PTGMarker() end\r\n';
 // Hide legacy branches for quests replaced by this graph, preserving every
 // non-GiapSi branch and unrelated service through the existing NPC menu.
 append+='PTG_OLD_SAYTASK=SayTask\r\nfunction SayTask(text,choices)\r\n if GetPlayerType()==0 then for n=1,getn(choices) do local t=choices[n][1];if PTG_OLD_TITLES[t] then choices[n].show=0 end end end\r\n return PTG_OLD_SAYTASK(text,choices)\r\nend\r\n';
 append+='PTG_OLD_TITLES='+lua(Object.fromEntries([...new Set(steps.map(a=>a.title.split(':')[0]).concat(['Khuyên hàng','Tam sách']))].map(x=>[x,1]))).replace(/([^,{]+)=/g,'$1=')+'\r\n';
 // Lua object keys containing spaces need bracket notation.
 append=append.replace(/PTG_OLD_TITLES=.*\r\n/, 'PTG_OLD_TITLES={'+[...new Set(steps.map(a=>a.title.split(':')[0]).concat(['Khuyên hàng','Tam sách']))].map(x=>'['+lua(x)+']=1').join(',')+'}\r\n');
 for(let i=0;i<steps.length;i++)append+=`function ptg_a${i+1}() PTGPreview(${i+1}) end\r\nfunction ptg_yes${i+1}() PTGApply(${i+1}) end\r\n`;
 if(!rel.startsWith('script/phongthan/giapsi/')&&fs.existsSync(path.join(S,rel)))bundle(rel,append);else put(rel,append);
}
// Override only exact monster rows. Each wrapper retains unrelated common
// quest/death behavior; original mainline branches are skipped for GiapSi.
const deathIds=[0,3,7,13,18,16,575,...kills.map(k=>k[0]),541,542,543,2703];
for(const tid of [...new Set(deathIds)]){
 const logical=npcTable[tid+1][deathCol],physical=cp(Buffer.from(logical.replaceAll('\\','/').replace(/^\//,''),'latin1'));
 const old=tid!==2703&&fs.existsSync(path.join(S,physical))?original(physical):Buffer.from('function OnDeath(index) end\r\n');
 const rel=`script/phongthan/giapsi/death_${tid}.lua`;
 let append=common+`\r\nPTG_OLD_DEATH=OnDeath\r\nfunction OnDeath(index)\r\n local killer=PlayerIndex\r\n if not killer or killer<=0 then return end\r\n local world=GetNpcWorldPos(index)\r\n if world~=PTGMap() then return end\r\n`;
 const originalMain=kills.some(k=>k[0]===tid)||tid===575;
 if(originalMain)append+=' if GetPlayerType()==0 then\r\n  PTGDeath('+tid+')\r\n  if pt_common_death then pt_common_death(index) end\r\n else PTG_OLD_DEATH(index) end\r\n';
 else append+=` PTG_OLD_DEATH(index)\r\n PlayerIndex=killer\r\n PTGDeath(${tid})\r\n`;
 append+=' PlayerIndex=killer\r\nend\r\n';put(rel,Buffer.concat([old,mark,enc(append)]));
 npcTable[tid+1][deathCol]='\\'+rel.replaceAll('/','\\');
}
put('settings/phongthan/Npcs.txt',Buffer.from(npcTable.map(x=>x.join('\t')).join('\r\n'),'latin1'));
// Exact guide tiles; unspecified positions below are authored within the
// named area. Each tile is checked against actual server obstacle data.
const mobs=[
[1007,575,235,208,'Hoàn Cẩu Vương',15,'VNG'],[1007,576,213,187,'Hoàn Cẩu Tinh',18,'VNG'],[1007,576,202,193,'Hoàn Cẩu Tinh',18,'VNG'],[1007,576,213,194,'Hoàn Cẩu Tinh',18,'VNG'],
[1018,122,187,189,'Trấn Tướng Quân',45,'VNG'],[1039,113,206,175,'Thần Long',55,'VNG'],
[1045,121,181,199,'Cự Dã',65,'VNG'],[1045,121,196,204,'Cự Dã',65,'VNG'],[1045,121,185,217,'Cự Dã',65,'VNG'],
[1047,120,197,212,'Trạnh Nanh',70,'VNG'],[1048,117,196,189,'Sương Ma Nữ',75,'VNG'],[1048,117,193,211,'Sương Ma Nữ',75,'VNG'],[1049,118,206,204,'Hỏa Ma Nữ',75,'VNG'],[1049,118,210,188,'Hỏa Ma Nữ',75,'VNG'],[1050,119,191,210,'Mộc Ma Nữ',75,'VNG'],[1050,119,179,197,'Mộc Ma Nữ',75,'VNG'],[1062,116,206,204,'Bộ Đầu quân Chu',80,'VNG'],[1051,82,199,212,'Bàn Cổ',85,'VNG'],
[1016,608,198,204,'Nguyên Tham Tinh',22,'AUTHORED_AREA'],[1016,608,203,204,'Nguyên Tham Tinh',22,'AUTHORED_AREA'],
];
for(let i=0;i<5;i++){mobs.push([1015,7,188+i,210,'Hoàn Cẩu',18,'AUTHORED_AREA'],[1015,13,188+i,211,'Giáp Cốt',20,'AUTHORED_AREA'],[1014,18,195+i,208,'Ngưu Sát',25,'AUTHORED_AREA'],[1014,16,195+i,209,'Thiên Hạo',29,'AUTHORED_AREA']);}
// Beginner Giao Huan stays in Yen Son beside the Giap Si starting settlement.
// Giao Huan uses existing local monsters; do not author extra populations.
for(const [map,tid,x,y,name,lvl,basis] of mobs){
 const existing=live.some(r=>r.map===map&&r.tid===tid&&Math.floor(r.x/256)===x&&Math.floor(r.y/512)===y&&r.b.readInt16LE(54)===0);
 if(!existing)addNpc(map,tid,x,y,name,'',0,lvl);
}
for(let i=0;i<10;i++){
 const role='herb_'+i,rel='script/phongthan/giapsi/'+role+'.lua';
 put(rel,common+`\r\nfunction main() SayButtons("<color=cyan>Nguyên Sâm<color><enter>Thu thập dược liệu cho Đại Phu Tam Sơn.",2,"Hái dược liệu/ptg_gather","Đóng/ptg_close") end\r\nfunction ptg_gather() PTGGather(1,${2**i});CloseDialog() end\r\n`);
 addNpc(1016,399,196+i,203,'Nguyên Sâm',rel);
}
for(let i=0;i<5;i++){
 const rel='script/phongthan/giapsi/willow_'+i+'.lua';
 put(rel,common+`\r\nfunction main() SayButtons("<color=cyan>Sơn Xuyên Liễu<color><enter>Dùng Tịnh Hóa Phù trước khi tịnh hóa.",2,"Tịnh hóa/ptg_gather","Đóng/ptg_close") end\r\nfunction ptg_gather() PTGGather(2,${2**i});CloseDialog() end\r\n`);
 addNpc(1014,1815,195+i,211,'Sơn Xuyên Liễu',rel);
}
// Restore well-formed table rows for four quest-use items. Some current VNG
// rows contain missing tab separators inside their translated names.
const magic=fs.readFileSync(path.join(S,'settings/item/001/magicscript.txt'),'latin1').split(/\r?\n/).filter(Boolean).map(x=>x.split('\t'));
const template=magic.find(x=>x[1]==='6'&&x[2]==='1'&&x[3]==='141');
for(const [id,name] of [[34,'Khứ Lai Phù'],[142,'Mật Đồ Da Dê'],[181,'Sách Chỉ Dẫn'],[9201,'Tịnh Hóa Phù']]){
 const rel='script/phongthan/giapsi/item_'+id+'.lua';
 const row=[...template];row[0]=enc('#'+name).toString('latin1');row[3]=String(id);row[8]=enc('#Vật phẩm nhiệm vụ Giáp Sĩ. Nhấn chuột phải để sử dụng.').toString('latin1');row[10]='0';row[11]='1';row[14]='\\'+rel.replaceAll('/','\\');row[23]='0';row[24]='0';row[25]='0';row[27]='0';
 let idx=magic.findIndex(x=>(x[1]==='6'&&x[2]==='1'&&+x[3]===id)||(x[1]==='1'&&+x[2]===id));
 if(idx<0)magic.push(row);else magic[idx]=row;
 const legacyPath={34:'script/item/Ê±¿Õ·û.lua',142:'script/item/³Â¾ÉµÄÑòÆ¤¾íÖá.lua',181:'script/item/ÆôÊ¾Ö®Êé.lua'}[id];
 const legacy=legacyPath?original(legacyPath):Buffer.alloc(0);
 const append=common+'\r\nPTG_ITEM_OLD=main\r\nGetWorldPos=GetContentWorldPos\r\n'+fs.readFileSync(path.join(__dirname,'giapsi/items.lua'),'utf8')+'\r\nPTG_ITEM='+id+'\r\nfunction main() return ptg_item_main() end\r\n';
 put(rel,Buffer.concat([legacy,mark,enc(append)]));
}
put('settings/item/001/magicscript.txt',Buffer.from(magic.map(x=>x.join('\t')).join('\r\n')+'\r\n','latin1'));
const shared=require('./giapsi/build_shared.cjs')({fs,path,R,S,assets,put,original,lua,enc,dec,live,addNpc,npcTable,deathCol,worldNames});
// Client journal/dialog assets must ship with the Lua records that use them.
for(const scheme of ['ui3','ui4'])for(const name of ['taskinfo.ini','UiTaskNote.ini','UiMsgSel.ini','UiHelper2.ini','DetailHelper.ini','PtHelpTopics.ini','gong_ju_kong_zhi_tiao.ini']){
 const rel=`Ui/${scheme}/${name}`,base=path.join(R,'PhongThanSource/Deploy/ProjectContent',rel);
 put('client/'+rel,fs.readFileSync(fs.existsSync(base)?base:path.join(path.dirname(base),'../ui3',name)));
}
fs.writeFileSync(path.join(A,'npc_bindings.json'),JSON.stringify(live.map(r=>({map:r.map,tid:r.tid,x:Math.floor(r.x/256),y:Math.floor(r.y/512),script:r.script})),null,2));
for(const [rel,b] of assets){const p=path.join(O,'compiled',rel);fs.mkdirSync(path.dirname(p),{recursive:true});fs.writeFileSync(p,b);edits.push({relative:rel,sha256:hash(b),bytes:b.length,before:fs.existsSync(path.join(S,rel))?hash(fs.readFileSync(path.join(S,rel))):null});}
fs.mkdirSync(A,{recursive:true});
const manifest={schema:1,source:'https://pt.vnggames.com/cam-nang/gioi-thieu/tam-gioi-giap-si.html',date:'2026-10-24',classification:'AUTHORED_GUIDE_RECONSTRUCTION_WITH_VNG_PHASE_COMPATIBILITY',steps:steps.length,books,roles,shared,placements,objectiveTiles:mobs.map(x=>({map:x[0],tid:x[1],x:x[2],y:x[3],name:x[4],basis:x[6]})),geometry:bounds,files:edits};
fs.writeFileSync(path.join(O,'manifest.json'),JSON.stringify(manifest,null,2));
const syntaxDir=path.join(A,'syntax');fs.mkdirSync(syntaxDir,{recursive:true});
fs.writeFileSync(path.join(A,'syntax_inputs.txt'),edits.filter(e=>e.relative.endsWith('.lua')).map((e,i)=>{const p=path.join(syntaxDir,i+'.lua');fs.copyFileSync(path.join(O,'compiled',e.relative),p);return p;}).join('\r\n')+'\r\n');
console.log(JSON.stringify({files:edits.length,steps:steps.length,placements:placements.length,books:books.length}));
