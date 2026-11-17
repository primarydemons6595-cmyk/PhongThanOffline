// Build journal text from the same accepted phases and verified NPC bindings.
module.exports=function({steps,roles,added,kills,live,placements,fs,path,S,dec,enc}){
 const names={lo:'Lỗ Hùng',au:'Âu Thiên Hóa',buu:'Sùng Ứng Bưu',loan:'Sùng Ứng Loan',dien:'Triều Điền',hau:'Sùng Hầu Hổ',hac:'Sùng Hắc Hổ',trinh:'Trịnh Luân',hoang:'Hoàng Phi Hổ',dac:'Đắc Kỷ',ho:'Hồ Hỷ Mị',khuong:'Khương Tử Nha',duong:'Dương Tiễn',vo:'Võ Vương',ly:'Lý Tịnh'};
 const maps={1002:'Sùng Thành Doanh',1021:'Triều Ca',1020:'Tây Kỳ',1065:'Trần Đường',1016:'Tam Sơn',1052:'Diêu Trì',1064:'Hỏa Vân Động',1063:'Triều Ca mười năm sau',1062:'Ngọc Hư Cung mười năm sau',1061:'Ngọc Hư Cung mười năm trước',1034:'Đại Phong',1070:'Viễn Cổ Hỏa Diệm',1015:'Mạnh Tân',1014:'Đồng Quan'};
 const locations={};
 for(const [role,[map,script]] of Object.entries(roles)){
  const rel=script.startsWith('script/')?script:'script/phongthan/'+script;
  const row=live.find(r=>r.map===map&&r.script===rel);
  const p=placements.find(r=>r.map===map&&r.script===rel);
  const a=added[role];
  if(!row&&!p&&!a)throw Error('Missing journal NPC binding '+role);
  const x=row?Math.floor(row.x/256):p?p.x:a[2],y=row?Math.floor(row.y/512):p?p.y:a[3];
  locations[role]=`${names[role]||(a&&a[4])}, ${maps[map]||('bản đồ '+map)} (${x},${y})`;
 }
 const titles={21:'Tân Thức',25:'Dũng Đao',24:'Kiếm Ái',4550:'Trừ Họa',3:'Chủ tuyến Giáp Sĩ'};
 const headers={21:'3000',25:'3001',24:'3002',4550:'3003',3:'3004'};
 const terminal={21:9,25:4,24:4,4550:6,3:90};
 const itemRows=dec(fs.readFileSync(path.join(S,'settings/item/001/questkey.txt'))).split(/\r?\n/).map(x=>x.split('\t'));
 const itemName=r=>r[0]===4?(itemRows.find(x=>+x[2]===r[1])||[])[0]?.replace(/^#/,'')||'Vật phẩm nhiệm vụ':r[0]===3&&r[1]===10?'Đoản Kiếm':'Vật phẩm nhiệm vụ';
 const quests=Object.keys(headers).map(task=>({task:+task,header:headers[task],phases:{}}));
 for(const q of quests){
  const actions=steps.filter(a=>a.task===q.task);
  for(const a of actions){
   if(a.to===terminal[q.task])continue;
   const next=actions.find(b=>b.from===a.to);
   let note=a.note;
   if(next){
    const returnText=locations[next.role];
    if(note.startsWith('Hoàn thành'))note+=` Đạt cấp ${next.level}, gặp ${returnText} nhận ${next.title.split(':')[0]}.`;
    else if(note.includes(names[next.role]||'\u0000')){
     if(!note.includes(returnText))note=note.replace(names[next.role],returnText);
    }else note+=` Sau đó gặp ${returnText}.`;
   }
   q.phases[a.to]={title:a.title.split(':')[0],note,needs:next?next.needs:[],needNames:next?next.needs.map(itemName):[]};
  }
  // Death hooks advance phases independently of NPC dialogue.
  for(const k of kills.filter(k=>k[2]===q.task&&k[3]!==k[4])){
   const next=actions.find(a=>a.from===k[4]);
   if(!next)throw Error('Missing return step '+k);
   q.phases[k[4]]={title:next.title.split(':')[0],note:`Đã lấy được vật phẩm. Mang về ${locations[next.role]}.`,needs:next.needs,needNames:next.needs.map(itemName)};
  }
 }
 const main=quests.find(q=>q.task===3);
 main.phases[87]={title:'Thông Thiên',note:'Diệt Thiên Hồn, Địa Hồn, Mệnh Hồn tại Viễn Cổ Hỏa Diệm (202,209), (203,209), (204,209). Đủ ba hồn sẽ bắt đầu giới hạn 3 phút.',needs:[],needNames:[]};
 main.phases[88]={title:'Thông Thiên',note:'Trong 3 phút kể từ lúc hạ đủ ba hồn, bấm Khiêu chiến ở Thông Thiên Giáo Chủ, Viễn Cổ Hỏa Diệm (204,210), rồi hạ bản thể chiến đấu để lấy Ngọc Thiên Cơ.',needs:[],needNames:[]};
 const last=steps.find(a=>a.task===3&&a.from===89);
 main.phases[89]={title:'Thông Thiên',note:`Mang Ngọc Thiên Cơ về ${locations.khuong_young}; chọn một phần thưởng.`,needs:last.needs,needNames:last.needs.map(itemName)};
 // Reuse the literal goal coordinates for kill and gathering objectives.
 const d=quests.find(q=>q.task===25);d.phases[1].note=steps.find(a=>a.task===25&&a.from===0).note+` Trả tại ${locations.dien}.`;
 const t=quests.find(q=>q.task===21);t.phases[3].note=`Mua đủ 10 Đoản Kiếm ở ${locations.au}. Sau đó giao cho Âu Thiên Hóa.`;
 for(const q of quests)for(const [phase,r] of Object.entries(q.phases)){
  const worst=`<color=yellow>${r.title}<color><enter><color=cyan>${r.note}<color>`+r.needNames.map((name,i)=>`<enter><color=green>${name}: 999/${r.needs[i][7]}<color>`).join('');
  if(enc(worst).length>510)throw Error('Journal record too long '+q.task+':'+phase+' '+enc(worst).length);
 }
 const templates=quests.map(q=>`[Task_${q.header}]\r\nTitle=$ ${titles[q.task]}\r\nIntro=$ <color=yellow>${titles[q.task]}<color><enter>Giáp Sĩ. Xem mục tiêu, tọa độ và tiến độ ở khung dưới. Nút Theo dõi đưa chỉ dẫn ra màn hình.<enter><color=cyan>Xanh lam: mục tiêu.<color><enter><color=green>Xanh lá: tiến độ.<color>\r\n`).join('\r\n')+'\r\n[Task_3005]\r\nTitle=$ Giáo Huấn\r\nIntro=$ Giáp Sĩ từ cấp 20. Diệt đủ 40 quái được giao rồi quay về Tạp Hóa Thương Nhân nhận thưởng.\r\n';
 return {quests,templates,locations};
};
