"""Stage explicit Dao Si/Di Nhan quest branches; publication is separate."""
from pathlib import Path
import json,subprocess,sys,re,struct
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
B=ROOT/'analysis/two_faction_quests_20261027';S=B/'compiled/Server';R=ROOT/'PhongThanRuntime-Loose/Server'
ENC_CACHE={}
def enc(text):
 if '\ufffd' in text:
  at=text.index('\ufffd');raise ValueError('Invalid source character near '+ascii(text[max(0,at-60):at+60]))
 out=bytearray()
 for c in text:
  if c in ENC_CACHE:out.extend(ENC_CACHE[c]);continue
  try:encoded=encode_tcvn3(c)
  except KeyError:
   try:encoded=encode_tcvn3(c.lower())
   except KeyError:raise ValueError('Unsupported TCVN3 source character '+ascii(c))
  ENC_CACHE[c]=encoded;out.extend(encoded)
 return bytes(out)
def lua(value):
 if value is None:return 'nil'
 if isinstance(value,bool):return '1' if value else 'nil'
 if isinstance(value,(float,int)):return str(value)
 if isinstance(value,str):return json.dumps(value,ensure_ascii=False)
 if isinstance(value,list):return '{'+','.join(lua(x) for x in value)+'}'
 return '{'+','.join(('['+str(k)+']' if isinstance(k,int) else k)+'='+lua(v) for k,v in value.items())+'}'
payload=subprocess.check_output(['node','-e',"process.stdout.write(JSON.stringify(require('./PhongThanSource/Tools/two_faction_quests.cjs')))"],cwd=ROOT)
steps=json.loads(payload)['steps'];audit=json.loads((B/'audit.json').read_text(encoding='utf-8'))
colors={}
for color,words in {
 'yellow':['Thư tiến cử','thư tiến cử','Mật Thư','mật thư','thiếp mời','thuốc tỉnh rượu','Băng Cơ','Ngọc Cốt','Mặt Quỷ','Kim Quán','Ma Huyết','Thần Mộc','An Cư Đồ','An Cư đồ','Thần Dụ Kính','Huỳnh Đế Nội Kinh','Dã Thiết Thiên','Luyện Kim Thuật','Huyền Nữ Binh Pháp','Trúc Giản','Trúc Giản','trúc giản','Bảng Phong Thần','bảng Phong Thần','Bàn Cổ Thần Khí','Bàn Cổ thần khí','Hàn Băng Nguyên Khí','Hạp Đằng','Nguyên Sâm','Sơn Xuyên Liễu','Khứ Lai Phù','Mật Đồ Da Dê'],
 'green':['Khương Tử Nha','Nam Cực Tiên Ông','Hoàng Long Chân Nhân','Hoàng Long chân nhân','Lôi Chấn Tử','Hoàng Phi Hổ','Lý Tịnh','Đặng Cửu Công','Hoàng Thiên Hóa','Thổ Hành Tôn','Hồ Hỷ Mị','Hình Thiên','Phong Lâm','Ngô Long','Thường Hạo','Văn Thái Sư','Đa Bảo đạo nhân','Tây Vương Mẫu','Hiên Viên','Xi Vưu','Đắc Kỷ','Nguyên Thủy Thiên Tôn','Võ Vương','Cao Giác','Cao Minh','Hậu Thổ','Chúc Dung','Cộng Công','Phong Bá','Khoa Phụ','Vân Trung Tử','Linh Bảo Đại Pháp Sư','Nhiên Đăng Đạo Nhân','Thủ Lĩnh Tộc Nhân','Đại Phu'],
 'red':['Thần Oanh','Bất Tử Thần Mộc','Hiệu Úy Thương','Hỏa Thú','Cự Dã','Trạnh Nanh','Bộ Đầu','Bộ đầu','Bàn Cổ','Yếm Hỏa','Thảo Tiên Bà Bà','Độc Lam Quái','Tuyết Quái','Hỏa Diện','Băng Lang Vương','Băng Lang','Hoàn Cẩu','Giáp Cốt','Nguyên Tham Tinh','Ngưu Sát'],
}.items():
 for word in words:colors[word]=color
pattern=re.compile('|'.join(re.escape(x) for x in sorted(colors,key=len,reverse=True)))
for a in steps:
 if '<color=' not in a['note']:a['note']=pattern.sub(lambda m:'<color='+colors[m[0]]+'>'+m[0]+'<color>',a['note'])
def oldpath(suffix):return next(x['runtime_path'] for x in audit['scripts'] if x['path'].endswith(suffix))
roles={
 'hoanglong':(1003,oldpath('玉虚宫\\黄龙真人.lua')),
 'lei_tay':(1020,oldpath('西岐\\雷震子.lua')),
 'khuong':(1020,oldpath('西岐\\姜子牙.lua')),
 'hoang':(1021,'script/phongthan/npc_restore/1021_00.lua'),
 'ly':(1065,'script/phongthan/npc_restore/1065_00.lua'),
 'dang':(1016,'script/phongthan/giapsi/npc_dang.lua'),
 'hoangthien':(1021,'script/phongthan/npc_restore/1021_16.lua'),
 'thohanh':(1021,'script/phongthan/npc_restore/1021_15.lua'),
 'ho':(1021,'script/phongthan/npc_restore/1021_02.lua'),
 'hinh':(1004,'script/phongthan/two_faction/hinh_thien.lua'),
 'phonglam':(1015,'script/phongthan/npc_restore/1015_01.lua'),
 'ngolong':(1015,'script/phongthan/two_faction/ngo_long.lua'),
 'thuonghao':(1015,'script/phongthan/npc_restore/1015_03.lua'),
 'wine':(1021,'script/phongthan/npc_restore/1021_07.lua'),
 'van':(1014,oldpath('潼关\\闻太师.lua')),
 'dabao':(1044,'script/phongthan/two_faction/da_bao.lua'),
 'dac':(1021,'script/phongthan/npc_restore/1021_01.lua'),
 'tay':(1052,'script/phongthan/giapsi/npc_tay.lua'),
 'hienvien':(1064,'script/phongthan/two_faction/hien_vien.lua'),
 'xivuu':(1064,'script/phongthan/two_faction/xi_vuu.lua'),
 'vo':(1020,'script/phongthan/npc_restore/1020_02.lua'),
 'dac_future':(1063,'script/phongthan/giapsi/npc_dac_future.lua'),
 'nguyen':(1062,'script/phongthan/giapsi/npc_nguyen.lua'),
 'dac_young':(1061,'script/phongthan/giapsi/npc_dac_young.lua'),
 'khuong_young':(1061,'script/phongthan/giapsi/npc_khuong_young.lua'),
 'merchant':(1034,'script/phongthan/giapsi/npc_merchant.lua'),
 'cuudi':(1016,'script/phongthan/giapsi/npc_cuudi.lua'),
 'thong':(1070,'script/phongthan/giapsi/npc_thong.lua'),
 'doctor_manh':(1015,'script/'+ '龙套/野外医生-孟津.lua'.encode('gbk').decode('cp1252')),
 'doctor_tam':(1016,'script/phongthan/giapsi/npc_doctor_tam.lua'),
 'doctor_dong':(1014,'script/phongthan/giapsi/npc_doctor_dong.lua'),
 'tuhang':(1003,oldpath('玉虚宫\\慈航道人.lua')),
 'xich':(1003,oldpath('玉虚宫\\赤精子.lua')),
 'linhbao':(1003,'script/phongthan/npc_restore/1003_02.lua'),
 'namcuc':(1003,'script/phongthan/npc_restore/1003_03.lua'),
 'nhiendang':(1003,oldpath('玉虚宫\\燃灯道人.lua')),
 'vantrung':(1003,oldpath('玉虚宫\\云中子.lua')),
 'thieuhao':(1004,'script/phongthan/npc_restore/1004_00.lua'),
 'phongba':(1004,'script/phongthan/npc_restore/1004_02.lua'),
 'chucdung':(1004,'script/phongthan/npc_restore/1004_03.lua'),
 'khoaphu':(1004,'script/phongthan/two_faction/khoa_phu.lua'),
 'hautho':(1004,'script/phongthan/npc_restore/1004_01.lua'),
 'caogiac':(1004,'script/phongthan/shared/caogiac.lua'),
 'caogiac_original':(1004,oldpath('蚩尤墓\\高觉.lua')),
 'cao':(1004,'script/phongthan/npc_restore/1004_07.lua'),
 'congcong':(1004,'script/phongthan/npc_restore/1004_04.lua'),
 'mao_giao_dao':(1003,'script/phongthan/restoration/maolu_giao_dao.lua'),
 'mao_store_dao':(1003,'script/phongthan/restoration/maolu_store_dao.lua'),
 'tocnhan':(1013,'script/phongthan/two_faction/toc_nhan.lua'),
 'giao_dao':(1003,'script/phongthan/npc_restore/1003_15.lua'),
 'giao_di':(1004,'script/phongthan/npc_restore/1004_15.lua'),
}
new_originals={'hinh':'蚩尤墓\\刑天.lua','ngolong':'孟津\\吴龙.lua','dabao':'碧游宫\\多宝道人.lua','hienvien':'远古\\轩辕.lua','xivuu':'远古\\蚩尤.lua','khoaphu':'蚩尤墓\\夸父图腾.lua','tocnhan':'蚩尤墓\\失踪的族人.lua'}
notes={1:{},2:{}};harm={}
for a in steps:
 a['label']=a.get('label',({1:'Nhận giày',2:'Nhận yêu đái',3:'Nhận mão'}[a['rewardChoice']] if a.get('rewardChoice') else a['title'].split(':')[0]))
 if len(enc(a['label']))>17:raise ValueError('Use an explicit short quest label: '+a['label'])
 if a['to']!=a['from']:
  if a['task']==(1 if a['f']==1 else 2):notes[a['f']].setdefault(a['to'],a['note'])
  if a['task']==4550:harm.setdefault(a['to'],a['note'])
for f in (1,2):
 for phase in (90,):notes[f].pop(phase,None)
 for phase in (22,23,32,33,42,43,51,52,63,73,74,85,87,88,89):
  # A kill transition keeps the last objective until the next NPC hand-in.
  if phase not in notes[f] and phase-1 in notes[f]:notes[f][phase]=notes[f][phase-1]
units=[]
for f,task,id,title,finished in [(1,10,3043,'Bách Lý',16),(1,11,3044,'Ngũ Thất',5),(1,15,3045,'Khảo Nghiệm',7),(1,14,3046,'Linh Lực',2),(2,30,3047,'Khai Trí',16),(2,31,3048,'Tân Thức',6),(2,35,3049,'Thần Khí',8),(2,34,3050,'Cứu Tế',7)]:
 rows={};needs={}
 for a in steps:
  if a['f']==f and a['task']==task:
   if a['to']!=finished:rows.setdefault(a['to'],a['note'])
   if a['needs']:needs[a['from']]=a['needs']
 if task==15:rows[2]='Trở về <color=green>Vân Trung Tử<color>, Ngọc Hư Cung (215,194) phục mệnh.';rows[6]=rows[5]
 if task==35:
  for phase in (4,5,6):rows[phase]=rows[3]
 if task==34:rows[4]=rows[3]
 units.append(dict(f=f,task=task,id=str(id),title=title,rows=rows,needs=needs))
source=(ROOT/'PhongThanSource/Tools/two_faction_runtime.lua').read_text(encoding='utf-8')
pool=json.loads((B/'transform_pool.json').read_text(encoding='utf-8'))
transforms=[[6,1,x['key'],1,0,0,1] for x in pool if 42<=x['key']<=68];assert len(transforms)==27
common='PT2_STEPS='+lua(steps)+'\nPT2_NOTES='+lua(notes)+'\nPT2_UNITS='+lua(units)+'\nPT2_HARM_NOTES='+lua(harm)+'\nPT2_KILLS={}\nPT2_TRANSFORMS='+lua(transforms)+'\nPT2_LOCAL_NAMES='+lua({1:'Tuyết Quái',4:'Băng Lang',2:'Hỏa Diện',5:'Lục Quái',6:'Cuồng Điểu',9:'Thảo Tiên',18:'Hắc Phong'})+'\n'+source
# Keep the verified random skill-book pool already used for Binh An.
text=decode_tcvn3((R/'script/phongthan/giapsi/npc_tay.lua').read_bytes())
books=re.search(r'^PTG_BOOKS=(.+)$',text,re.M);assert books
common='PTG_BOOKS='+books[1]+'\n'+common
common+='\n'+(ROOT/'PhongThanSource/Tools/starter_story_generated.lua').read_text(encoding='utf-8')
for i in range(1,len(steps)+1):common+=f'\nfunction pt2_view{i}() PT2Preview({i}) end\nfunction pt2_yes{i}() PT2Apply({i}) end\n'
def put(path,data):
 p=S/path;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
put('script/phongthan/two_faction/journal.lua',enc(common))
for key in (34,142,181,9201):
 path=f'script/phongthan/giapsi/item_{key}.lua';base=(R/path).read_bytes().split(b'-- PT_TWO_FACTION_ITEM')[0]
 items=(ROOT/'PhongThanSource/Tools/giapsi/items.lua').read_text(encoding='utf-8')
 items=items.replace('GetPlayerType()~=0','not PT2Class()').replace('GetTask(3)','GetTask(PT2MainTask())').replace('SetTask(3,','SetTask(PT2MainTask(),')
 for old,new in [('PTG_ITEM_OLD','PT2_ITEM_OLD'),('PTG_ITEM','PT2_ITEM'),('PTGEvent','PT2Event'),('PTGJournal','PT2Journal'),('PTGNotice','PT2Notice'),('PTGMap()','(GetContentWorldPos()+1000)'),('ptg_','pt2_')]:items=items.replace(old,new)
 put(path,base+enc('\n-- PT_TWO_FACTION_ITEM\n'+common+'\nPT2_ITEM_OLD=main\n'+items+f'\nPT2_ITEM={key}\nfunction main() return pt2_item_main() end\n'))
# Original native template identities: VNG Npcs Name plus retained Giap Si
# objective placements for the common latter half of the three story lines.
kills=[
 [1,100,1006,0,22,23,1,1],[1,109,1024,0,32,33,2,1],
 [2,116,1015,0,21,22,-1,0],[2,107,1016,0,31,32,18,1],
 [1,8,1010,15,5,6,-1,0],[2,106,1013,34,3,4,30,1],
 [0,121,1045,0,42,43,3,1],[0,120,1047,0,51,52,4,1],
 [0,117,1048,0,63,63,7,1],[0,118,1049,0,63,63,8,1],[0,119,1050,0,63,63,6,1],
 [0,116,1062,0,73,74,10,1],[0,82,1051,0,85,85,175,1],
]
services=[(1003,778,216,201,'mao_giao_dao','Tạp Hóa Thương'),(1003,768,217,200,'mao_store_dao','Thủ Khố'),(1015,156,209,195,'ngolong','Ngô Long'),(1044,205,238,193,'dabao','Đa Bảo Đạo Nhân'),
 (1064,198,194,191,'hienvien','Hiên Viên'),(1064,176,226,202,'xivuu','Xi Vưu'),
 (1013,220,239,192,'tocnhan','Đầu Lĩnh')]
spawns=[
 [1,1,22,1006,100,213,204,45],[1,1,32,1024,109,212,181,55],
 [2,2,21,1015,116,209,195,45],[2,2,31,1016,107,180,211,55],
 [2,35,3,1012,105,199,206,15],[2,35,4,1012,105,187,197,15],[2,35,5,1012,105,219,209,15],
 [2,34,3,1013,106,236,191,18],
 [0,4550,5,1014,12,196,208,20],
]
def cstr(text):return '"'+''.join('\\x%02x'%v for v in enc(text))+'"'
header=['#ifndef PHONGTHAN_TWO_FACTION_DATA_H','#define PHONGTHAN_TWO_FACTION_DATA_H',
 'struct PtTwoService {int map,tid,x,y;const char* script;const char* name;};',
 'static const PtTwoService PT_TWO_SERVICES[]={']
for map,tid,x,y,role,name in services:
 path='\\'+roles[role][1].replace('/','\\')
 header.append('{%d,%d,%d,%d,%s,%s},'%(map,tid,x,y,cstr(path),cstr(name)))
header.extend(['{1001,198,206,201,"\\\\script\\\\phongthan\\\\territory\\\\founder.lua","\\x54\\x68\\xad\\xac\\x6e\\x67\\x20\\x54\\x68\\x61\\x6e\\x67"},', '{1001,768,207,201,"\\x5c\\x73\\x63\\x72\\x69\\x70\\x74\\x5c\\x70\\x68\\x6f\\x6e\\x67\\x74\\x68\\x61\\x6e\\x5c\\x74\\x65\\x72\\x72\\x69\\x74\\x6f\\x72\\x79\\x5c\\x77\\x61\\x72\\x65\\x68\\x6f\\x75\\x73\\x65\\x2e\\x6c\\x75\\x61","\\x54\\x68\\xf1\\x20\\x4b\\x68\\xe8\\x20\\x4c\\xb7\\x6e\\x68\\x20\\xa7\\xde\\x61"},', '{1001,190,208,201,"\\\\script\\\\phongthan\\\\territory\\\\gate.lua","\\x53\\xf8\\x20\\x47\\x69\\xb6\\x20\\x4c\\xb7\\x6e\\x68\\x20\\xa7\\xde\\x61"},'])
header+=['};','enum {PT_TWO_SERVICE_COUNT=sizeof(PT_TWO_SERVICES)/sizeof(PT_TWO_SERVICES[0])};','static const int PT_TWO_KILLS[][8]={']
header+=['{'+','.join(str(v) for v in row)+'},' for row in kills]
header+=['};','enum {PT_TWO_KILL_COUNT=sizeof(PT_TWO_KILLS)/sizeof(PT_TWO_KILLS[0])};','static const int PT_TWO_SPAWNS[][8]={']
header+=['{'+','.join(str(v) for v in row)+'},' for row in spawns]
header+=['};','enum {PT_TWO_SPAWN_COUNT=sizeof(PT_TWO_SPAWNS)/sizeof(PT_TWO_SPAWNS[0])};','static const char* PT_TWO_SPAWN_NAMES[]={'+','.join(cstr(x) for x in ['Thần Oanh','Bất Tử Thần Mộc','Hiệu Úy Thương','Hỏa Thú','Thảo Tiên Bà Bà','Thảo Tiên Bà Bà','Thảo Tiên Bà Bà','Độc Lam Quái','Ngưu Sát'])+'};','static const char PT_TWO_WOLF_KING[]='+cstr('Băng Lang Vương')+';','#endif']
(ROOT/'PhongThanSource/Headers/PhongThanTwoFactionData.h').write_text('\n'.join(header)+'\n',encoding='ascii')
client=B/'compiled/Client'
titles={3040:'Chủ tuyến Đạo Sĩ',3041:'Chủ tuyến Dị Nhân',3042:'Trừ Họa',3052:'Băng Lang',3053:'Giáo Huấn'}
titles.update({int(x['id']):x['title'] for x in units})
for scheme in ('ui3','ui4'):
 rel=f'Ui/{scheme}/taskinfo.ini';data=(ROOT/'PhongThanRuntime-Loose/Client'/rel).read_bytes()
 for key,title in titles.items():
  assert f'[Task_{key}]'.encode() not in data
  data+=enc(f'\r\n[Task_{key}]\r\nTitle=$ {title}\r\nIntro=$ Theo dõi từng bước nhiệm vụ để phục mệnh.\r\n')
 p=client/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
magic=(R/'settings/item/001/magicscript.txt').read_bytes().splitlines()
changed=0
for i,line in enumerate(magic[1:],1):
 cells=line.split(b'\t')
 if len(cells)>14 and cells[1:3]==[b'6',b'1'] and cells[3] in (b'34',b'142',b'181',b'9201'):
  cells[8]=enc('#Vật phẩm nhiệm vụ. Nhấn chuột phải để sử dụng.');magic[i]=b'\t'.join(cells);changed+=1
assert changed==4
table=b'\r\n'.join(magic)+b'\r\n';put('settings/item/001/magicscript.txt',table)
p=client/'settings/item/001/magicscript.txt';p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(table)
manifest=[]
for role,(map,path) in roles.items():
 canonical='caogiac' if role=='caogiac_original' else role
 if role in new_originals:
  e=next(x for x in audit['scripts'] if x['path'].endswith(new_originals[role]));base=(B/'originals'/('%03d.lua'%e['number'])).read_bytes()
 else:
  p=R/path
  if not p.is_file():raise FileNotFoundError(path)
  base=p.read_bytes().split(b'-- PT_TWO_FACTION_BEGIN')[0]
 titles={f:sorted(set(a['title'].split(':')[0] for a in steps if a['f']==f)) for f in (1,2)}
 titles[1]+=['Thăm Dò','Linh lực','Giáo Huấn'];titles[2]+=['Ma huyết','Bất Túy ','Cổ đao','Giáo Huấn']
 appendix='\n-- PT_TWO_FACTION_BEGIN\n'+common+'\nPT2_ROLE='+lua(canonical)+'\nPT2_MAP='+str(map)+'\nPT2_REPLACED_TITLES='+lua(titles)+'''\nPT2_OLD_MAIN=main
PT2_OLD_MARK=GetPlayerTaskState
PT2_OLD_SAY_TASK=SayTask
function SayTask(text,choices)
 if PT2Class() then
  for i=1,getn(choices) do local label=strlower(gsub(choices[i][1],"^%s*(.-)%s*$","%1"))
   for f=1,2 do for j=1,getn(PT2_REPLACED_TITLES[f]) do
    if label==strlower(PT2_REPLACED_TITLES[f][j]) and not (GetPlayerType()==1 and PT2_ROLE=="linhbao" and label==strlower("Thăm Dò") and GetTask(15)==1) then choices[i].show=0 end
   end end
  end
 end
 return PT2_OLD_SAY_TASK(text,choices)
end
function main() return PT2Menu() end
function GetPlayerTaskState() return PT2Marker() end
'''
 put(path,base+enc(appendix))
 manifest.append(dict(role=canonical,map=map,path=path,base_preserved=True,new=role in new_originals))
for tid,role in [(175,'khoaphu'),(176,'hinh')]:
 routes=json.loads((B/'routes.json').read_text(encoding='utf-8'))
 matches=[x for x in routes if x['map']==1004 and x['tid']==tid and x['script']=='script/phongthan/npc_restore/1004_05.lua']
 assert len(matches)==1,(tid,matches)
 row=matches[0];rel=row['source'];path=S/rel;data=path.read_bytes() if path.is_file() else (R/rel).read_bytes()
 from prepare_npc_restoration import split_region,npc_records
 combined=rel.endswith('_region_s.dat');payload=split_region(data)[2] if combined else data
 records=npc_records(payload);replacement=[];count=0
 script=('\\'+roles[role][1].replace('/','\\')).encode('ascii')
 for record in records:
  if struct.unpack_from('<i',record)[0]==tid and struct.unpack_from('<ii',record,4)==(row['x'],row['y']):
   record=record[:58]+struct.pack('<H',len(script))+script;count+=1
  replacement.append(record)
 assert count==1
 payload=payload[:12]+b''.join(replacement)
 if combined:
  head,sections,_=split_region(data);offset=0;parts=[];newhead=bytearray(data[:head])
  for i,(start,size) in enumerate(sections):
   part=payload if i==2 else data[head+start:head+start+size]
   struct.pack_into('<II',newhead,4+i*8,offset,len(part));parts.append(part);offset+=len(part)
  data=bytes(newhead)+b''.join(parts)
 else:data=payload
 put(rel,data)
for tid in (7,13,18):
 rel=f'script/phongthan/giapsi/death_{tid}.lua';base=(R/rel).read_bytes().split(b'-- PT_TWO_FACTION_DEATH')[0]
 put(rel,base+enc('\n-- PT_TWO_FACTION_DEATH\n'+common+'''\nPT2_PREVIOUS_GIAP_DEATH=PTGDeath
function PTGDeath(template)
 if PT2Class() then return PT2Death(template) end
 return PT2_PREVIOUS_GIAP_DEATH(template)
end
'''))
(B/'staged_dialogs.json').write_text(json.dumps(dict(steps=steps,roles=manifest,status='staged; publication receipt is authoritative',manual_validation=['Full play-through in the game client has not been performed']),ensure_ascii=False,indent=2),encoding='utf-8')
print('STAGED ONLY',len(steps),'steps',len(roles),'roles; not published')
