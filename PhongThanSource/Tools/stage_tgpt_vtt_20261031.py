"""TGPT VTT content derivative. Stage only; original PAK and Release stay read-only."""
from pathlib import Path
import hashlib,json,re,struct,math,configparser
from extract_client_coordinate_dictionary import encode_tcvn3 as _encode_tcvn3
from prepare_npc_restoration import split_region,npc_records,merge_region,standalone_npcs
from build_authored_npcs import barrier_at,blocked
from restore_original_world import Original
ROOT=Path(__file__).resolve().parents[2];LIVE=ROOT/'PhongThanRuntime-Loose'
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/TgptVtt20261031'
files={}
def encode_tcvn3(text):return _encode_tcvn3(text.replace('→','->').replace('–','-').replace('Ứ','ứ'))
def baseline(role,rel):
 p=PACKAGE/'source'/role/rel
 if not p.exists():
  live=LIVE/role/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(live.read_bytes() if live.exists() else b'')
 return p.read_bytes()
def stage(role,rel,data):
 before=baseline(role,rel);p=PACKAGE/'compiled'/role/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
 files[role+'/'+rel]=dict(path=role+'/'+rel,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(before).hexdigest() if (LIVE/role/rel).exists() else None)
def lua(rel,text):stage('Server',rel,encode_tcvn3(text))
def chest_positions():
 layouts=json.loads((ROOT/'analysis/vtt_collision_20261024/layouts.json').read_text('utf8'))
 worlds=dict((int(k),v.decode('latin1')) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(LIVE/'Server/settings/WorldSet.ini').read_bytes()))
 header=(ROOT/'PhongThanSource/Headers/PhongThanVttLayout.h').read_text('ascii');result=[];proof=[]
 for q in layouts:
  free=set()
  for p in (LIVE/'Client/maps'/worlds[q['Map']]).rglob('*_region_c.dat'):
   rx=int(p.name.split('_')[0]);ry=int(p.parent.name.split('_')[1]);raw=p.read_bytes();head,sec,_=split_region(raw);off,size=sec[0]
   for x in range(16):
    for y in range(32):
     if not size or not struct.unpack_from('<I',raw,head+off+(x*32+y)*4)[0]&255:free.add((rx*16+x,ry*32+y))
  text=re.search(r'PT_DOOR_%d\[\]=\{([^;]+)'%q['Id'],header)[1];doors={(int(x)//32,int(y)//32) for x,y in re.findall(r'\{(\d+),(\d+)\}',text)}
  boss=tuple(v//32 for v in q['Boss']);room={boss};pending=[boss]
  for x,y in pending:
   for n in [(x-1,y),(x+1,y),(x,y-1),(x,y+1)]:
    if n in free and n not in doors and n not in room:room.add(n);pending.append(n)
  assert tuple(q['Entry']) not in room and all(tuple(v//32 for v in p) not in room for p in q['Guards'])
  available=[p for p in room if 8**2<=(p[0]-boss[0])**2+(p[1]-boss[1])**2<=24**2 and all((p[0]+dx,p[1]+dy) in room for dx in [-1,0,1] for dy in [-1,0,1])]
  slots=[];used=[]
  for i in range(5):
   angle=2*math.pi*i/5;target=(boss[0]+16*math.cos(angle),boss[1]+16*math.sin(angle))
   center=min((p for p in available if all((p[0]-v[0])**2+(p[1]-v[1])**2>=7**2 for v in used)),key=lambda p:(p[0]-target[0])**2+(p[1]-target[1])**2);used.append(center)
   options=sorted((p for p in room if abs(p[0]-center[0])<=2 and abs(p[1]-center[1])<=2),key=lambda p:(p[0]-center[0])**2+(p[1]-center[1])**2)[:9]
   assert len(options)==9;slots.append([[x*32+16,y*32+16] for x,y in options])
  mission=baseline('Server','script/phongthan/van_tien/mission%02d.lua'%q['Id'])
  mobs=re.findall(rb'\{(\d+),(\d+),(\d+)\}',re.search(rb'PT_MOBS=([^\r\n]+)',mission)[1])
  assert all((int(x)//32,int(y)//32) not in room for tid,x,y in mobs)
  result.append(slots);proof.append(dict(map=q['Map'],castle_cells=len(room),positions=[slot[0] for slot in slots],all_inside_closed_gate_boss_component=True,legacy_cache_candidates_outside_castle=True))
 (PACKAGE/'chest-position-proof.json').write_text(json.dumps(proof,indent=2),'utf8');return result
def npc_base(world,rx,ry):
 relative='settings/phongthan/npc_regions/%d/v_%03d/%03d_region_s.dat'%(world,ry,rx)
 staged=PACKAGE/'compiled/Server'/relative
 if relative in npc_changed:return relative,staged.read_bytes()
 if (LIVE/'Server'/relative).exists():return relative,baseline('Server',relative)
 worlds=dict((int(k),v.decode('latin1')) for k,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(LIVE/'Server/settings/WorldSet.ini').read_bytes()))
 for suffix in ['_S','']:
  p=LIVE/'Server/maps'/(worlds[world]+suffix)/('v_%03d/%03d_region_s.dat'%(ry,rx))
  if p.exists():
   base=p.read_bytes();standalone=relative.replace('_region_s.dat','_npc_s.dat')
   if (LIVE/'Server'/standalone).exists() and not npc_records(split_region(base)[2]):
    base=merge_region(base,npc_records(baseline('Server',standalone)))
   return relative,base
 raise ValueError(('Missing original server region',world,rx,ry))
npc_changed=set()
def edit_records(world,rx,ry,change):
 rel,raw=npc_base(world,rx,ry);head,sections,payload=split_region(raw);records=change(npc_records(payload))
 # Replace only the NPC pointer; retain geometry, traps and all unrelated data.
 out=bytearray(raw);struct.pack_into('<II',out,20,len(raw)-head,12+sum(map(len,records)));out.extend(standalone_npcs(records))
 stage('Server',rel,bytes(out));npc_changed.add(rel)
def place(world,dx,dy,record):
 rx,ry=dx//2,dy//2;rel,raw=npc_base(world,rx,ry)
 points=[(dx*256+x*32+16,dy*512+y*32+16) for x in range(8) for y in range(16)]
 points.sort(key=lambda p:(p[0]-(dx*256+128))**2+(p[1]-(dy*512+256))**2)
 existing=npc_records(split_region(raw)[2]);occupied=[struct.unpack_from('<iii',r)[1:] for r in existing]
 x,y=next(p for p in points if not blocked(barrier_at(raw,*p),*p) and all((p[0]-a)**2+(p[1]-b)**2>=64**2 for a,b in occupied))
 value=bytearray(record);struct.pack_into('<ii',value,4,x,y);edit_records(world,rx,ry,lambda rows:rows+[bytes(value)])
 return x,y
def record(tid,name,script):
 b=('\\'+script.replace('/','\\')).encode('ascii');return struct.pack('<iiiB3x32shhhhBBH',tid,0,0,0,encode_tcvn3(name),1,0,0,3,0,0,len(b))+b
def stage_thien():
 source=baseline('Server','script/phongthan/npc_recovered_batch/1021_b5deef29.lua')
 source=source.replace(b'if w ~= 1021 then CloseDialog(); return end',b'if w ~= 1021 and (w<1067 or w>1070) then CloseDialog(); return end')
 pattern=rb'(\{"[^"\r\n]+","chaxun";show=0\})'
 extra=encode_tcvn3(',\n {"Đổi vũ khí Thông Thiên Châu","pt_chau";show=1},\n {"Nhận đồ phổ còn chờ","pt_extra";show=1}')
 source,n=re.subn(pattern,lambda m:m[0]+extra,source,count=1);assert n==1
 source=source.replace(b'function reward_normal()',b'function pt_tgpt_previous_reward_normal()',1)
 source=source.replace(b'wxz_reward[wxz_level][1]*GetLevel()*2',b'wxz_reward[wxz_level][1]*GetLevel()*2*EventVttExpMultiplier()')
 source=source.replace(b'AddOwnExp(5000)',b'AddOwnExp(5000*EventVttExpMultiplier())').replace(b'(5000)',b'(5000*EventVttExpMultiplier())')
 source=source.replace(b'floor(SystemTime()/86400)',b'floor((SystemTime()+7*3600)/86400)')
 source=source.replace(encode_tcvn3('Ô Vân Quy'),encode_tcvn3('Quỷ Vương'))
 source+=encode_tcvn3('''
function reward_normal()
 EventVttExtra(0)
 local r=pt_tgpt_previous_reward_normal()
 EventVttExtra(1)
 return r
end
function pt_extra()
 if EventVttExtra(1)==1 then Msg2Player("Đã nhận đồ phổ Đồ Cam cấp 3.")
 else Msg2Player("Chưa có đồ phổ đang chờ hoặc túi không đủ chỗ.") end
end
function pt_green(slot)
 if EventVttGreen(slot)==1 then reward_normal();CloseDialog()
 else Talk(1,"no","Chưa nhận được trang bị. Hãy mở khóa hành trang và để đủ chỗ trong F4.") end
end
function item_1() pt_green(1) end
function item_2() pt_green(2) end
function item_3() pt_green(3) end
function item_4() pt_green(4) end
function item_5() pt_green(5) end
function pt_chau()
 Say("Đổi Thông Thiên Châu lấy vũ khí Hoàng Kim không khóa. Chọn loại Châu:",5,
 "Thổ: 2000 Châu, vũ khí 5x/pt_chau_class(1)","Thủy: 3000 Châu, vũ khí 7x/pt_chau_class(2)",
 "Hỏa: 3000 Châu, vũ khí 9x/pt_chau_class(3)","Phong: 4000 Châu, vũ khí 10x/pt_chau_class(4)","Quay lại/main")
end
function pt_chau_class(id)
 Say("Chọn hệ của vũ khí muốn nhận:",4,"Giáp Sĩ/pt_chau_confirm("..id..",0)","Đạo Sĩ/pt_chau_confirm("..id..",1)","Dị Nhân/pt_chau_confirm("..id..",2)","Quay lại/pt_chau")
end
function pt_chau_confirm(id,profession)
 local costs={2000,3000,3000,4000}
 local tiers={50,70,90,100}
 if id<1 or id>4 or profession<0 or profession>2 then return end
 Say("Dùng "..costs[id].." Thông Thiên Châu tương ứng đổi 1 vũ khí Hoàng Kim cấp "..tiers[id].." không khóa?",2,
 "Xác định/pt_chau_do("..id..","..profession..")","Quay lại/pt_chau")
end
function pt_chau_do(id,profession)
 if EventVttExchange(id,profession)==1 then Msg2Player("Đã đổi vũ khí Hoàng Kim. Vật phẩm đã vào F4.");CloseDialog()
 else Talk(1,"no","Chưa đổi được. Cần đủ Châu đúng loại trong túi, mở khóa hành trang và đủ chỗ nhận vũ khí.") end
end
''')
 stage('Server','script/phongthan/npc_recovered_batch/1021_b5deef29.lua',source)
 stage('Server','script/phongthan/van_tien/thien_hung.lua',source)
 doctor=Original().get('script/万仙阵/医生.lua'.encode('gbk'));assert doctor
 stage('Server','script/phongthan/van_tien/doctor.lua',doctor[0]+b'\nGetWorldPos=GetContentWorldPos\n')
def stage_xich():
 rel='script/phongthan/npc_services/xich_tung_tu.lua';raw=baseline('Server',rel)
 start=raw.index(b'function pt_xich_menu()');end=raw.index(b'\nend',start);part=raw[start:end]
 part=part.replace(b',6,',b',7,',1).rstrip();assert part.endswith(b')');part=part[:-1]+b', '+encode_tcvn3('"Ghép Linh Phù/pt_vtt_merge_menu"')+b')'
 raw=raw[:start]+part+raw[end:]
 for old,new in [('3 Thổ linh phù=Hỏa linh phù','2 Thổ Linh Phù = 1 Thủy Linh Phù'),('3 Thủy linh phù=Hỏa linh','2 Thủy Linh Phù = 1 Hỏa Linh'),('3 Hỏa linh phù=Phong linh phù','2 Hỏa Linh Phù = 1 Phong Linh Phù')]:raw=raw.replace(encode_tcvn3(old),encode_tcvn3(new))
 raw+=encode_tcvn3('''
function pt_vtt_merge_menu()
 SayButtons("Ghép 2 Linh Phù cùng loại thành 1 Linh Phù bậc kế tiếp.",4,
 "2 Thổ → 1 Thủy/pt_vtt_merge(1)","2 Thủy → 1 Hỏa/pt_vtt_merge(2)","2 Hỏa → 1 Phong/pt_vtt_merge(3)","Quay lại/main")
end
function pt_vtt_merge(tier)
 if EventVttMerge(tier)==1 then Msg2Player("Ghép Linh Phù thành công.")
 else Msg2Player("Cần 2 Linh Phù đúng loại trong túi và chỗ trống F4. Chưa trừ nguyên liệu.") end
end
''')
 stage('Server',rel,raw)
def stage_lottery():
 original=Original().get('script/彩票/应签商.lua'.encode('gbk'));assert original
 raw=original[0]+encode_tcvn3('''
function main()
 Say("Ứng Tiêm Thương: mua Thăm May Mắn, 1000 lượng/lá. Thăm có thể nhận tiền, Thổ Linh Phù hoặc Thủy Linh Phù.",3,
 "Mua 1 lá: 1000 lượng/yes_1","Mua 5 lá: 5000 lượng/yes_5","Kết thúc/no")
end
function pt_buy(count)
 if EventVttBuyLottery(count)==1 then Msg2Player("Đã mua Thăm May Mắn. Nhấn chuột phải vào thăm trong F4 để mở.")
 else Msg2Player("Chưa mua được. Cần cấp 5, đủ tiền, đủ chỗ trong F4 và hành trang không khóa.") end
end
function yes_1() pt_buy(1) end
function yes_5() pt_buy(5) end
''')
 stage('Server','script/phongthan/van_tien/ung_tiem_thuong.lua',raw)
 for role in ['Server','Client']:
  rel='settings/item/001/magicscript.txt';rows=[r.split(b'\t') for r in baseline(role,rel).splitlines()]
  found=[r for r in rows[1:] if r[1:4]==[b'6',b'1',b'12']];assert len(found)==1
  found[0][0]=encode_tcvn3('#Thăm May Mắn');found[0][8]=encode_tcvn3('#Mở nhận tiền hoặc Linh Phù. 10% Thổ Linh Phù, 5% Thủy Linh Phù.')
  stage(role,rel,b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')
def stage_npcs():
 # Keep the original four doctors and Thien Hung identities, move the earth
 # pair to the documented doctor's tile. No unrelated NPC is discarded.
 old='settings/phongthan/npc_regions/1067/v_102/082_region_s.dat';raw=baseline('Server',old)
 rs=npc_records(split_region(raw)[2]);moved=[r for r in rs if struct.unpack_from('<i',r)[0] in [155,766]];assert len(moved)==2
 edit_records(1067,82,102,lambda rows:[r for r in rows if struct.unpack_from('<i',r)[0] not in [155,766]])
 place(1067,185,205,record(766,'Đại Phu','script/phongthan/van_tien/doctor.lua'))
 place(1067,186,205,record(155,'Thiên Hùng','script/phongthan/van_tien/thien_hung.lua'))
 # Align the two portal tiles which differed from the supplied guide.
 for world,fn,target in [(1004,'1004_281c1583.lua',(194,201)),(1003,'1003_479d2a84.lua',(213,193))]:
  expected=('\\script\\phongthan\\npc_recovered_batch\\'+fn).encode();hits=[]
  for p in (LIVE/'Server'/('settings/phongthan/npc_regions/%d'%world)).rglob('*.dat'):
   payload=split_region(p.read_bytes())[2] if '_region_s' in p.name else p.read_bytes()
   for r in npc_records(payload):
    if r[60:].rstrip(b'\0').lower()==expected.lower():hits.append((p,r))
  assert len(hits)==1;path,r=hits[0];tid,x,y=struct.unpack_from('<iii',r)
  if '_npc_s' in path.name:
   rel=path.relative_to(LIVE/'Server').as_posix();stage('Server',rel,standalone_npcs([a for a in npc_records(baseline('Server',rel)) if a!=r]))
  else:edit_records(world,x//512,y//1024,lambda rows:[a for a in rows if a!=r])
  place(world,*target,r)
 # Original Xich Tung Tu exists one display tile south. Preserve his exact
 # identity and service script; relocate him to the referenced 195:195 tile.
 rel,raw=npc_base(1052,97,98);rs=npc_records(split_region(raw)[2]);matches=[r for r in rs if struct.unpack_from('<i',r)[0]==206];assert len(matches)==1
 old=matches[0];edit_records(1052,97,98,lambda rows:[r for r in rows if r!=old])
 place(1052,195,195,record(206,'Xích Tùng Tử','script/phongthan/npc_services/xich_tung_tu.lua'))
 # The lottery merchant has an original script, but no recovered placement.
 # Use an existing merchant appearance and bind a dedicated guarded service.
 place(1021,207,191,record(778,'Ứng Tiêm Thương','script/phongthan/van_tien/ung_tiem_thuong.lua'))
def build():
 if (PACKAGE/'publication.json').exists():raise ValueError('Already published; preserve the source snapshots and use a new follow-up package')
 global pearls,weapons
 material=[]
 for role in ['Server','Client']:
  rel='settings/item/001/material.txt';raw=baseline(role,rel);rows=[r.split(b'\t') for r in raw.splitlines()]
  material.append((role,rel,rows))
 first=max(int(r[2]) for _,_,rs in material for r in rs[1:] if len(r)>2 and r[1]==b'3' and r[2].isdigit())+1
 pearls=list(range(first,first+4))
 for role,rel,rows in material:
  template=next(r for r in rows[1:] if r[1:3]==[b'3',b'41'])
  for ident,label in enumerate(['Thổ','Thủy','Hỏa','Phong']):
   r=template.copy();r[0]=encode_tcvn3('#Thông Thiên Châu '+label);r[2]=str(pearls[ident]).encode();r[7]=encode_tcvn3('#Đổi vũ khí Hoàng Kim tại Thiên Hùng. Có thể giao dịch, vứt bỏ và bán tiệm.');r[9]=b'10000';r[14]=r[15]=r[17]=r[20]=b'1';r[18]=b'0';r[19]=b''
   rows.append(r)
  stage(role,rel,b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')
 catalog=json.loads((ROOT/'analysis/event_handbook_20261026/catalog.json').read_text('utf8'))['entries']
 weapons=[]
 for tier in [50,70,90,100]:
  selected=[next(e for e in catalog if e['category']=='weapon_gold' and e['tier']==tier and e['profession']==prof and e['realm']==0 and not e.get('disabled_reason')) for prof in range(3)]
  # The handbook JSON records data rows starting at one; the native catalog
  # and Gen_EquipmentByTemplateRow use zero-based data-record indices.
  weapons.append([dict(e,source_row=e['row'],row=e['row']-1) for e in selected])
 h='#ifndef PT_TGPT_VTT_DATA_H\n#define PT_TGPT_VTT_DATA_H\nstatic const int PT_TGPT_PEARLS[4]={'+','.join(map(str,pearls))+'};\nstatic const int PT_TGPT_COSTS[4]={2000,3000,3000,4000};\nstruct PTTgptWeapon{int row,part,level;};\nstatic const PTTgptWeapon PT_TGPT_WEAPONS[4][3]={\n'
 for es in weapons:h+='{'+','.join('{%d,%d,%d}'%(e['row'],e['part'],e['level']) for e in es)+'},\n'
 h+='};\nstatic const int PT_TGPT_BLUEPRINTS[3]={278,279,280};\n'
 for key,value in {'CHEST_NAME':'Rương Thông Thiên','LOTTERY_MONEY':'Mở thăm may mắn nhận được tiền lượng.','LOTTERY_ITEM':'Mở thăm may mắn nhận được vật phẩm.','LOTTERY_NONE':'Thăm này chưa trúng thưởng.'}.items():h+='static const char PT_TGPT_'+key+'[]="'+''.join('\\%03o'%v for v in encode_tcvn3(value))+'";\n'
 h+='static const int PT_TGPT_CHEST_POS[4][5][9][2]='+json.dumps(chest_positions()).replace('[','{').replace(']','}')+';\n#endif\n'
 (ROOT/'PhongThanSource/Headers/PhongThanTgptVttData.h').write_text(h,'ascii')
 rel='EventService/events.json';config=json.loads(baseline('Server',rel).decode('utf8'));config['Revision']+=1
 for r in config['Events']:
  if 1<=r['Id']<=4:r['DailyTimes']=['%02d:%02d'%(hour,5*(r['Id']-1)) for hour in [10,13,18,21,23]];r['RegistrationSeconds']=300;r['DurationSeconds']=1800;r['MinimumLevel']=[30,51,71,91][r['Id']-1]
 stage('Server',rel,json.dumps(config,indent=2).encode())
 rel='EventService/catalog.json';definitions=json.loads(baseline('Server',rel).decode('utf8'))
 for definition in definitions:
  if 1<=definition['Id']<=4:
   definition['Defaults']['DailyTimes']=['%02d:%02d'%(hour,5*(definition['Id']-1)) for hour in [10,13,18,21,23]]
   definition['Defaults']['MinimumLevel']=[30,51,71,91][definition['Id']-1]
   definition['SourceEvidence']='TGPT VTT reference; five pearl chests and lottery odds overridden by user on 2026-10-31.'
 stage('Server',rel,json.dumps(definitions,ensure_ascii=False,indent=2).encode('utf8'))
 # Scope repaired tables to these eight templates. Zero-probability stale
 # rows cannot produce loot but previously blocked native catalog preflight.
 for ident in range(1,5):
  for kind in ['tongtian','baoxiang']:
   rel='settings/item/droprate/%s%d.ini'%(kind,ident);c=configparser.RawConfigParser(strict=False);c.optionxform=str;c.read_string(baseline('Server',rel).decode('latin1'))
   positive=[dict(c[s]) for s in c.sections() if s!='Main' and int(c[s]['RandRate'])>0]
   if kind=='tongtian':
    cap=[30,50,70,70][ident-1]
    tiers={e['part']:e['tier'] for e in catalog if e['category']=='fabao' and e['realm']==0}
    positive=[r for r in positive if not (r['Genre']=='0' and r['Detail']=='4' and tiers.get(int(r['Particular']),999)>cap)]
   main=dict(c['Main']);main['Count']=str(len(positive))
   if kind=='tongtian' and ident in [2,3]:main['MinItemLevel']=str(5 if ident==2 else 6)
   text='[Main]\r\n'+''.join(k+'='+v+'\r\n' for k,v in main.items())
   for n,row in enumerate(positive,1):text+='\r\n[%d]\r\n'%n+''.join(k+'='+v+'\r\n' for k,v in row.items())
   stage('Server','settings/item/droprate/pt_tgpt_%s%d.ini'%(kind,ident),text.encode('latin1'))
 for ident in [2,3]:
  stale=PACKAGE/'compiled/Server/settings/item/droprate'/('tongtian%d.ini'%ident)
  if stale.exists():stale.unlink()
 rel='settings/phongthan/Npcs.txt';rows=[r.split(b'\t') for r in baseline('Server',rel).splitlines()];headers=rows[0]
 for ident in range(1,5):
  for tid,kind in [(128+ident*5,'tongtian'),([410,413,414,415][ident-1],'baoxiang')]:
   for slot in [1,3]:rows[tid+1][headers.index(('DropRateFile%d'%slot).encode())]=('\\settings\\item\\droprate\\pt_tgpt_%s%d.ini'%(kind,ident)).encode()
 stage('Server',rel,b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')
 # This sole NPC hold came from the malformed old Fire cache table. Its
 # dedicated replacement is validated with the production plan before deploy.
 rel='settings/phongthan/NativeDropQuarantine.txt';raw=baseline('Server',rel);lines=raw.splitlines();assert b'N 414' in lines
 stage('Server',rel,b'\r\n'.join(r for r in lines if r!=b'N 414')+b'\r\n')
 rel='settings/systemtimetask.txt';raw=baseline('Server',rel);prefix='\\script\\万仙阵\\'.encode('gbk')
 obsolete={prefix+(label+suffix).encode('gbk') for label in ['土','水','火','风'] for suffix in ['on.lua','review.lua']}
 kept=[];removed=0
 for line in raw.splitlines():
  cols=line.split(b'\t')
  if len(cols)>2 and cols[2].lower() in obsolete:removed+=1
  else:kept.append(line)
 assert removed==48,removed
 stage('Server',rel,b'\r\n'.join(kept)+b'\r\n')
 # Keep the managed and standalone heartbeat on the same explicit UTC+7 schedule.
 rel='script/servertimer.lua';raw=baseline('Server',rel);raw=re.sub(rb'PT_NEWS_VTT=\{[^\r\n]+',b'PT_NEWS_VTT={}',raw)
 append='''
-- TGPT staggered schedule; managed service owns its own openings.
pt_tgpt_previous_heartbeat=main
pt_tgpt_last_minute=-1
function main()
 pt_tgpt_previous_heartbeat()
 local minute=floor(SystemTime()/60)
 if pt_tgpt_last_minute==minute then return end
 pt_tgpt_last_minute=minute
 local localminute=mod(minute+420,1440)
 local hours={10,13,18,21,23}
 for id=1,4 do
  if EventServiceOwns(id)==0 then
   for i=1,getn(hours) do
    local start=hours[i]*60+(id-1)*5
    local diff=mod(start-localminute+1440,1440)
    if diff==5 or diff==1 then AddNews("[Vạn Tiên Trận] Trận "..id.." còn "..diff.." phút mở cổng.",1) end
    if diff==0 then
     local old=SubWorld;SubWorld=SubWorldID2Idx(66+id)
     if SubWorld>=0 and IsMission(id)==0 then OpenMission(id) end
     SubWorld=old
    end
   end
  end
 end
end
'''
 stage('Server',rel,raw+encode_tcvn3(append))
 # Boss credit and original four treasure caches remain. Five collectible
 # pearl chests are authoritative native NPCs with timed interaction.
 for ident in range(1,5):
  rel='script/phongthan/van_tien/boss%d.lua'%ident;raw=baseline('Server',rel)
  needle=b'  pt_original_boss_death()';assert raw.count(needle)==1
  raw=raw.replace(needle,b'  PT_TGPT_CACHE_USED={}\n  EventVttBossLoot(npc)\n'+needle)
  raw=re.sub(rb'  -- A full ten-minute[^\n]+\n  StopMissionTimer[^\n]+\n  StartMissionTimer[^\n]+\n',b'',raw)
  start=raw.index(b'function AddNpc(tid,level,world,x,y,barrier)');end=raw.rindex(b'\nfunction OnDeath(npc)')
  wrapper='''function AddNpc(tid,level,world,x,y,barrier)
 if tid==410 or tid==413 or tid==414 or tid==415 then
  local spot=random(1,getn(PT_MOBS))
  for i=1,getn(PT_MOBS) do if PT_TGPT_CACHE_USED[spot]==nil then break end;spot=mod(spot,getn(PT_MOBS))+1 end
  PT_TGPT_CACHE_USED[spot]=1
  x=PT_MOBS[spot][2];y=PT_MOBS[spot][3]
 end
 local n=pt_boss_native_add(tid,level,world,x,y,1)
 if n>0 and (tid==410 or tid==413 or tid==414 or tid==415) then
  if AddMSNpc(PT_ID,n,0)==0 then DelNpc(n);return 0 end
  SetNpcKind(n,0);SetNpcDeathScript(n,"\\\\script\\\\phongthan\\\\van_tien\\\\chest.lua")
  Msg2MSAll(PT_ID,"Bảo rương xuất hiện tại "..floor(x/256)..":"..floor(y/512).." trong Vạn Tiên Trận.")
 end
 return n
end
'''
  raw=raw[:start]+encode_tcvn3(wrapper)+raw[end:]
  stage('Server',rel,raw)
  rel='script/phongthan/van_tien/guard%d.lua'%ident;raw=baseline('Server',rel)
  old=b'if EventVttKill(npc)==1 then pt_progress() end'
  new=b'''if EventVttKill(npc)==1 then
  local old=PlayerIndex
  local world=GetContentWorldPos()
  local kind=GetNpcTemplateID(npc)-(129+(PT_ID-1)*5)+4
  if GetTeam()~=0 then
   for i=1,GetTeamSize() do PlayerIndex=GetTeamMember(i);pt_tgpt_guard_credit(world,kind) end
  else pt_tgpt_guard_credit(world,kind) end
  PlayerIndex=old;pt_progress()
 end'''
  assert old in raw;raw=raw.replace(old,new)
  raw+=b'''\nfunction pt_tgpt_guard_credit(world,kind)
 if PlayerIndex==nil or PlayerIndex<=0 then return end
 if GetContentWorldPos()~=world or floor(SystemTime()/86400)~=GetTask(409) then return end
 if GetTask(408)~=kind or GetTask(407)<=0 then return end
 local left=GetTask(407)-1;SetTask(407,left)
 if left==0 then SetTask(408,100000) end
end
'''
  stage('Server',rel,raw)
 lua('script/phongthan/van_tien/pearl_chest.lua','''function main()
 local r=EventVttLoot(0)
 if r==1 then CloseDialog();return end
 local text="Chưa thể nhặt rương. Hãy đứng gần rương, bật PK chiến đấu và mở khóa hành trang."
 if r==-2 then text="Chờ đủ 1 phút từ lần nhặt rương trước."
 elseif r==-3 then text="Rương đang có người nhặt." end
 Talk(1,"no",text)
end
function pt_vtt_loot_finish()
 local r=EventVttLoot(1)
 if r==1 then Msg2Player("Nhận 1 Thông Thiên Châu.")
 elseif r==-4 then Msg2Player("Túi không đủ chỗ. Rương vẫn còn, hãy thử lại.") end
end
function no() CloseDialog() end
''')
 stage_thien();stage_xich();stage_lottery();stage_npcs()
 for ident in range(1,5):
  # The original NPCs use the same UTC+7 quest-day boundary as the guide.
  for name in ['boss%d.lua'%ident,'guard%d.lua'%ident,'mob%d.lua'%(51+ident*2),'mob%d.lua'%(52+ident*2)]:
   rel='script/phongthan/van_tien/'+name;staged=PACKAGE/'compiled/Server'/rel
   raw=staged.read_bytes() if ('Server/'+rel) in files else baseline('Server',rel)
   raw=raw.replace(b'floor(SystemTime()/86400)',b'floor((SystemTime()+7*3600)/86400)');stage('Server',rel,raw)
  rel='script/phongthan/npc_recovered_batch/'+['1002_f57647b5.lua','1004_281c1583.lua','1003_479d2a84.lua','1001_41e3ecb2.lua'][ident-1]
  label=['Thổ','Thủy','Hỏa','Phong'][ident-1];minute=5*(ident-1)
  text='\nfunction main()\n Say("Vạn Tiên Trận '+label+': 10h%02d, 13h%02d, 18h%02d, 21h%02d, 23h%02d. Mỗi lượt 35 phút. Dùng '%((minute,)*5)+label+' Linh Phù hoặc Linh Thạch tương ứng.",2,"Vào trận/pt_enter","Kết thúc/no")\nend\n'
  stage('Server',rel,baseline('Server',rel)+encode_tcvn3(text))
  for name in ['mission%02d.lua'%ident,'guard%d.lua'%ident]:
   rel='script/phongthan/van_tien/'+name;path=PACKAGE/'compiled/Server'/rel
   raw=path.read_bytes() if ('Server/'+rel) in files else baseline('Server',rel)
   names=encode_tcvn3('PT_GUARD_NAMES={"Quỷ Vương","Đao Ngưu","Đại Thạch Thần","Thiết Kinh Thần"}\n')
   raw=raw.replace(b'function pt_progress()',names+b'function pt_progress()',1)
   raw=raw.replace(b'if npc>0 then SetMissionV(20+n,npc) end',b'if npc>0 then SetNpcName(npc,PT_GUARD_NAMES[n]);SetMissionV(20+n,npc) end')
   stage('Server',rel,raw)
 manifest=dict(scope='LOOSE_ONLY',reference='https://tamgioipt.vn/van-tien-tran-tgpt',files=list(files.values()),pearls=pearls,weapons=weapons,policy=dict(chest_count=5,channel_seconds=20,cooldown_seconds=60,pearls_per_chest=[1,5],drop_rates='preserve weights of guide-supported original outcomes in dedicated tables; remove zero-probability/above-tier rows; fix Water/Fire lower bounds',lottery_per_1000=dict(money100000=1,money20000=5,another_token=100,earth_ticket=100,water_ticket=50,empty=744),phong_ticket=[3,65,0]))
 (PACKAGE/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),'utf8')
 ticket=(ROOT/'PhongThanSource/Sources/Core/Src/PhongThanEventTicket.inl').read_bytes()
 admission=re.search(rb'int level=Npc\[npc\].m_Level,minLevel=[^;]+;',ticket)[0]
 (ROOT/'PhongThanSource/Output/Tools/TgptAdmissionActual.inl').write_bytes(admission+b'\n')
 print('STAGED TGPT base content; pearls='+str(pearls))
if __name__=='__main__':build()
