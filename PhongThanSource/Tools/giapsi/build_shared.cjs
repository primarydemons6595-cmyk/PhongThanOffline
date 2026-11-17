module.exports=function(ctx){
 const {fs,path,R,S,assets,put,original,lua,enc,dec,live,addNpc,npcTable,deathCol,worldNames}=ctx;
 const {actions,roles}=require('./shared_quests.cjs');
 const mark=Buffer.from('\r\n-- PT_SHARED_QUESTS_20261024\r\n');
 const trim=b=>{const i=b.indexOf(mark);return i<0?b:b.subarray(0,i);};
 const source=rel=>trim(assets.get(rel)||(fs.existsSync(path.join(S,rel))?original(rel):Buffer.from('function main() end\r\n')));
 // Repair the malformed original transformation-item row; retain the actual
 // original spell callbacks, with short buttons and a separate dialog footer.
 const magicRel='settings/item/001/magicscript.txt';
 const magic=(assets.get(magicRel)||fs.readFileSync(path.join(S,magicRel))).toString('latin1').split(/\r?\n/).filter(Boolean).map(x=>x.split('\t'));
 const bad=magic.findIndex(x=>x[1]==='1'&&x[2]==='11');
 if(bad>=0){const r=magic[bad];r[0]=enc('#Biến Thân Phù').toString('latin1');r.splice(1,0,'6');r[8]=enc('#Biến thân để giúp Nhậm Đại Ca và Nhậm Đại Tẩu.').toString('latin1');magic[bad]=r;}
 const morphRow=magic.find(x=>x[1]==='6'&&x[2]==='1'&&x[3]==='11');if(!morphRow)throw Error('Missing transformation item');
 morphRow[14]='\\script\\phongthan\\shared\\morph.lua';
 put(magicRel,Buffer.from(magic.map(x=>x.join('\t')).join('\r\n')+'\r\n','latin1'));
 let morph=original('script/item/±ä¶¯Îï.lua').toString('latin1');
 morph=morph.replace(/Say\(10626,[^\r\n]+/,enc('SayButtons(10626,8,"Khôi phục/esc","Cát Tường/zhuzhu","Tuyết Quái/fs","Hỏa ngư/mms","Ngọc Nữ/xh","Phi Thố/bz","Tình yêu/pty","Love/qe")').toString('latin1'));
 put('script/phongthan/shared/morph.lua',Buffer.from(morph,'latin1'));
 const towers=[[1006,190,206,0,'Bắc Hải'],[1007,213,195,1,'Yến Sơn'],[1012,211,207,2,'Miêu Cương'],[1013,225,188,3,'Cự Lộc'],[1010,151,175,4,'Thủ Dương Sơn'],[1009,200,200,5,'Tây Côn Lôn']];
 // Tower locations are authored inside the original map geometry. Coordinates
 // for the guide's ordinary monsters below remain exact.
 for(const tower of towers){
  const rel='script/phongthan/shared/tower_'+tower[3]+'.lua';
  let placed=false,lastError;
  for(const [dx,dy] of [[0,0],[1,0],[-1,0],[0,1],[0,-1],[2,0],[-2,0]]){
   try{addNpc(tower[0],399,tower[1]+dx,tower[2]+dy,'Thiên Cương Phong Ấn Tháp',rel);tower[1]+=dx;tower[2]+=dy;placed=true;break;}catch(e){lastError=e;}
  }
  if(!placed)throw lastError;
 }
 const titles={51:'Thiên Ngoại Phi Tiên',50:'Họa Địa Vi Lao',92:'Nhân Chi Tương Tử',91:'Yên Phúc',41:'Minh Chu Ám Đầu'};
 const headers={51:'3020',50:'3021',92:'3022',91:'3023',41:'3024'};
 const itemRows={};
 for(const [genre,file] of [[3,'material.txt'],[4,'questkey.txt'],[6,'magicscript.txt']]){
  for(const line of dec(fs.readFileSync(path.join(S,'settings/item/001',file))).split(/\r?\n/)){
   const c=line.split('\t');if(c.length>2)itemRows[genre+':'+c[2]]=(c[0]||'Vật phẩm').replace(/^#/,'');
  }
 }
 const journals=Object.keys(titles).map(task=>({task:+task,header:headers[task],title:titles[task],phases:{}}));
 function row(note,needs=[]){return {note,needs,names:needs.map(r=>itemRows[r[0]+':'+(r[0]===6?r[2]:r[1])]||'Vật phẩm')};}
 for(const q of journals){
  const aa=actions.filter(a=>a.task===q.task);
  for(const a of aa){if(a.finish)continue;const next=aa.find(b=>b.from===a.to);q.phases[a.to]=row(a.note,next?next.needs:[]);}
 }
 const byTask=Object.fromEntries(journals.map(q=>[q.task,q]));
 byTask[51].phases[3]=row(actions.find(a=>a.task===51&&a.to===3).note,[[4,37,0,0,0,0,1]]);
 byTask[51].phases[4]=row('Mang Băng Tinh về Người Tây Vực, Tây Kỳ (185,192), mở hợp thành thú cưỡi.',[[4,37,0,0,0,0,1]]);
 byTask[50].phases[2]=row(actions.find(a=>a.task===50&&a.to===2).note,[[4,38,0,0,0,0,1]]);
 byTask[50].phases[3]=row('Mang Huyên Thảo đến Võ Cát, Tây Kỳ (181,195).',[[4,38,0,0,0,0,1]]);
 const fishNeeds=actions.find(a=>a.task===41&&a.from===27).needs;
 for(let phase=20;phase<=27;phase++)byTask[41].phases[phase]=row('Thu ngư huyết ở Đông Hải: Chúc Ngư (225,207), Thố Ngư (222,179), Thể Ngư (204,177). Trả tại Cù Lưu Tôn, Triều Ca (206,197), kèm 3 Sơn Thạch và 10 Đồng Thau.',fishNeeds);
 byTask[41].phases[10]=row('Có thể đến Chủ Tiệm Cầm Đồ, Triều Ca (209,198), nhận Bảo Châu mới.');
 const mounts=[[0,1,29],[0,2,30],[0,3,31],[0,4,32],[0,5,33],[0,6,42],[0,7,43],[0,8,44],[0,9,45],[0,10,46],[3,10,47],[4,10,48],[5,10,49],[6,10,50],[7,10,51],[8,10,52]];
 const blueMounts=require('./blue_mounts.cjs')({fs,path,S,put,enc,dec,mounts});
 for(const q of journals)for(const r of Object.values(q.phases)){
  const worst='<color=yellow>'+q.title+'<color><enter><color=cyan>'+r.note+'<color><enter><color=green>'+r.names.map((n,i)=>n+' 999/'+r.needs[i][7]+'; ').join('')+'<color>';
  if(enc(worst).length>510)throw Error('Shared journal overflow '+q.task+' '+enc(worst).length);
 }
 const common='PTS_ACTIONS='+lua(actions)+'\r\nPTS_JOURNALS='+lua(journals)+'\r\nPTS_MOUNTS='+lua(mounts)+'\r\nPTS_TOWERS='+lua(towers)+'\r\nPTS_MATERIALS={[22]="Địa Tâm",[23]="Phong Lệ",[24]="Thủy Hồn",[25]="Hỏa Linh"}\r\nPTS_FISH={[110]=34,[111]=35,[112]=36}\r\nPTS_FISH_BITS={[110]=1,[111]=4,[112]=2}\r\n'+fs.readFileSync(path.join(__dirname,'shared_runtime.lua'),'utf8')+'\r\n'+fs.readFileSync(path.join(__dirname,'shared_journal.lua'),'utf8');
 const oldTitles=['Phi Tiên','Trùng Mộc','Vi Lao','Phu Thê','Yên Phúc','Minh Châu','Quy Tinh','Tứ Tượng'];
 for(const [role,[map,p]] of Object.entries(roles)){
  let rel=p.startsWith('script/')?p:'script/phongthan/'+p;
  if(role==='caogiac'){
   rel='script/phongthan/shared/caogiac.lua';roles[role]=[map,rel];
   if(!live.some(r=>r.map===1004&&r.tid===177&&Math.floor(r.x/256)===205&&Math.floor(r.y/512)===197))addNpc(1004,177,205,197,'Cao Giác',rel);
  }
  let b=source(rel),text=b.toString('latin1');
  text=text.replaceAll('local w = pt_runtime_world_pos()','local w = GetContentWorldPos()+1000');
  // A single recovered diviner script is also bound to Sung Thanh. Preserve
  // that unrelated instance while enabling the shared quest only in Trieu Ca.
  let append=common+`\r\nPTS_MAP=${map}\r\nPTS_ROLE=${lua(role)}\r\nPTS_OLD_MAIN=main\r\nPTS_HAS_SERVICE=${['west','culuu','caogiac'].includes(role)?0:1}\r\n`;
  append+='PTS_OLD_SAYTASK=SayTask\r\nPTS_OLD_TITLES={'+oldTitles.map(t=>'['+lua(t)+']=1').join(',')+'}\r\nfunction SayTask(text,choices)\r\n for n=1,getn(choices) do if PTS_OLD_TITLES[choices[n][1]] then choices[n].show=0 end end\r\n return PTS_OLD_SAYTASK(text,choices)\r\nend\r\n';
  if(role==='duong')append+='function main() if GetPlayerType()==0 then return PTGMenu() end;return PTSMenu() end\r\n';
  else if(role==='east_boi')append+='function main() if PTSMap()==PTS_MAP then return PTSMenu() end;if PTS_OLD_MAIN then return PTS_OLD_MAIN() end end\r\n';
  else append+='function main() return PTSMenu() end\r\n';
  append+="\nPTS_PREVIOUS_MARK=GetPlayerTaskState\nfunction GetPlayerTaskState()\n    local state,level=PTSMarker()\n    if PTS_PREVIOUS_MARK then local old,required=PTS_PREVIOUS_MARK();if old and old>state then return old,required end end\n    return state,level\nend\n";
  for(let i=0;i<actions.length;i++)append+=`function pts_preview${i+1}() PTSPreview(${i+1}) end\r\nfunction pts_apply${i+1}() PTSApply(${i+1}) end\r\n`;
  put(rel,Buffer.concat([Buffer.from(text,'latin1'),mark,enc(append)]));
 }
 for(const t of towers)put('script/phongthan/shared/tower_'+t[3]+'.lua',common+`\r\nPTS_TOWER_MAP=${t[0]}\r\nPTS_TOWER_X=${t[1]}\r\nPTS_TOWER_Y=${t[2]}\r\nPTS_TOWER_GROUP=${t[3]}\r\nfunction main() SayButtons("<color=yellow>Thiên Cương Phong Ấn Tháp<color><enter>Dùng Bách Linh Phướn để thả Thiên Cương Tinh.",1,"Mở phong ấn/pts_tower") end\r\n`);
 for(let count=0;count<36;count++)put('script/phongthan/shared/star_'+count+'.lua',common+`\r\nfunction OnDeath(index)\r\n if not PlayerIndex or PlayerIndex<=0 or not PTSClass() then return end\r\n local value=GetTask(53)\r\n if GetByte(value,2)~=${count} or GetByte(value,1)~=2 or GetNpcWorldPos(index)~=PTSMap() or PTSMap()~=${towers[Math.floor(count/6)][0]} then return end\r\n if PTSExchange(53,value,SetByte(value,1,3),{{4,43,0,0,0,0,1}},{{4,44,0,0,0,0,1}},nil,nil)==1 then SetTask(${250+count},1);PTSJournal() end\r\nend\r\n`);
 for(const tid of [104,108,110,111,112,114,7,8,9,10]){
  const logical=npcTable[tid+1][deathCol].replaceAll('\\','/').replace(/^\//,'');
  const rel='script/phongthan/shared/death_'+tid+'.lua';
  // Preserve current Giap Si hooks and all unrelated native death behavior.
  const physical=new TextDecoder('windows-1252').decode(Buffer.from(logical,'latin1'));
  let b=source(physical);
  if(logical.startsWith('script/phongthan/shared/'))b=source('script/phongthan/npc_quests/recovered_'+tid+'.lua');
  put(rel,Buffer.concat([b,mark,enc(common+`\r\nPTS_OLD_DEATH=OnDeath\r\nfunction OnDeath(index)\r\n local killer=PlayerIndex\r\n if not killer or killer<=0 or GetNpcWorldPos(index)~=PTSMap() then return end\r\n if PTS_OLD_DEATH then PTS_OLD_DEATH(index) end\r\n PlayerIndex=killer\r\n PTSDeath(${tid})\r\nend\r\n`)]));
  npcTable[tid+1][deathCol]='\\'+rel.replaceAll('/','\\');
 }
 // Exact guide positions, validated against the actual obstacle payloads.
 const mobs=[[1022,108,187,186,'Trư Tinh',40],[1022,108,183,192,'Trư Tinh',40],[1022,108,183,188,'Trư Tinh',40],[1022,108,187,189,'Trư Tinh',40],[1010,104,151,175,'Cự Thạch',57],[1010,104,134,185,'Cự Thạch',57],[1010,104,134,204,'Cự Thạch',57],[1043,114,212,211,'Thi Thú',57],[1043,114,196,213,'Thi Thú',57],[1043,114,190,219,'Thi Thú',57],[1037,110,225,207,'Chúc Ngư',43],[1037,111,222,179,'Thố Ngư',43],[1037,112,204,177,'Thể Ngư',43]];
 for(let n=0;n<4;n++)for(let j=0;j<3;j++)mobs.push([1018,7+n,185+n,190+j,['Địa Tâm yêu','Phong Lệ yêu','Thủy Hồn yêu','Hỏa Linh yêu'][n],31]);
 for(const [map,tid,x,y,name,level] of mobs)if(!live.some(r=>r.map===map&&r.tid===tid&&Math.floor(r.x/256)===x&&Math.floor(r.y/512)===y&&r.b.readInt16LE(54)===0))addNpc(map,tid,x,y,name,'',0,level);
 const login='script/player/playerlogin.lua';put(login,Buffer.concat([source(login),mark,enc(common+'\r\nPTS_OLD_LOGIN=main\r\nfunction main() if PTS_OLD_LOGIN then PTS_OLD_LOGIN() end;PTSJournal() end\r\n')]));
 put('settings/phongthan/Npcs.txt',Buffer.from(npcTable.map(x=>x.join('\t')).join('\r\n'),'latin1'));
 for(const scheme of ['ui3','ui4']){
  const p=path.join(R,'PhongThanSource/Deploy/ProjectContent/Ui',scheme,'taskinfo.ini');let text=fs.readFileSync(p);
  const marker=Buffer.from('\r\n; PT_SHARED_JOURNAL\r\n'),i=text.indexOf(marker);if(i>=0)text=text.subarray(0,i);
  let ini=journals.map(q=>`[Task_${q.header}]\r\nTitle=$ ${q.title}\r\nIntro=$ Nhiệm vụ chung cho Giáp Sĩ, Đạo Sĩ và Dị Nhân. Xem mục tiêu, tọa độ và tiến độ ở khung dưới.\r\n`).join('\r\n');
  ini+='\r\n[Task_3025]\r\nTitle=$ Thiên Cương Tinh Quy Vị\r\nIntro=$ Cấp 59. Thu đủ 36 loại. Thưởng cuối: 40000 kinh nghiệm, 108 danh vọng và Bá Lạc Nhãn cấp 9.\r\n\r\n[Task_3026]\r\nTitle=$ Tứ Tượng Đồng Quy\r\nIntro=$ Cấp 31. Thu 10 nguyên liệu được chỉ định. Thưởng 1200 lượng, 5 danh vọng và cơ hội nhận trang bị lục cấp 40 hoặc pháp bảo cấp 30. Có thể lặp lại.\r\n';
  fs.writeFileSync(p,Buffer.concat([text,marker,enc(ini)]));
 }
 return {quests:8,actions:actions.length,roles,towers,mobs,blueMounts};
};
