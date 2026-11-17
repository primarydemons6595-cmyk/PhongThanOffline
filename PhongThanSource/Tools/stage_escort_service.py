"""Stage linked escort NPCs and the Event GUI catalog. Never writes live runtime."""
import json,re,struct,hashlib
from restore_original_world import ROOT,Original
from prepare_npc_restoration import npc_records,standalone_npcs,split_region,merge_region
from build_authored_npcs import place_in_tile
from extract_client_coordinate_dictionary import decode_tcvn3,encode_tcvn3
from boss_announcement_names import boss_announcement_name

OUT=ROOT/'analysis/event_service_20261019';STAGE=OUT/'staged';server=ROOT/'PhongThanRuntime-Loose/Server';original=Original();changes={};placements=[]
def put(relative,data):changes[relative]=data
def text(relative,s):put(relative,s.encode('ascii'))
def sha(b):return hashlib.sha256(b).hexdigest()
def tail(kind,giver,old=False):
    title='Van Luong' if kind==1 else 'Van Tieu'
    destination='Truong Que Phuong - Tuyet Long Linh (164/196)' if kind==1 else 'Tong tieu dau - Trieu Ca (233/198)'
    description=('Cap 55, gia nhap lanh dia. 20 chuyen/ngay: chuyen dau mien phi, cac chuyen sau can Khao Quan Lenh. Giao xe roi ve Phong Lam nhan thuong.' if kind==1 else 'Cap 30, it nhat 10 uy danh. Ho tong xe tu Tuyet Long Linh den Trieu Ca. Thuong goc: 50 van luong, 10 uy danh va 1 Ngoc ty.')
    options=['"Nhan chuyen/pt_escort_take"','"Tinh trang chuyen/pt_escort_info"','"Huong dan/pt_escort_help"'] if giver else ['"Giao xe/pt_escort_deliver"','"Tinh trang chuyen/pt_escort_info"','"Huong dan/pt_escort_help"']
    if giver and kind==1:options.insert(1,'"Nhan phan thuong chuyen da giao/pt_escort_claim"')
    options+=['"Huy chuyen/pt_escort_cancel"']
    if old:options+=['"Nhiem vu chinh tuyen/pt_escort_old"']
    options+=['"Ket thuc doi thoai/pt_escort_close"']
    return f'''
-- Connected escort adapter. Original payload above remains byte-for-byte.
function main()
    Say("{title} - {destination}", {len(options)}, {','.join(options)})
end
function pt_escort_close() CloseDialog() end
function no() CloseDialog() end
function pt_escort_help()
    Say("{description} Xe di theo khi ban o gan. Khong bo xe qua xa; roi xe 2 phut, xe bi pha hoac het han thi chuyen that bai. Thoi han va thuong do Studio cai dat.",2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_info()
    local phase,left,cart,runs,limit=EventEscortInfo({kind})
    local status="Chua nhan chuyen"
    if phase==1 then status="Dang ho tong xe; con "..floor(left/60).." phut "..mod(left,60).." giay"
    elseif phase==2 then status="Da giao xe. Ve Phong Lam o Manh Tan (191/212) nhan thuong" end
    Say(status..". Hom nay: "..runs.."/"..limit.." chuyen.",2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_result(result)
    local message="Chua the thuc hien. Hay de trong hanh trang F4, kiem tra tien va trang thai giao dich. Quyen nhan thuong van duoc giu."
    if result==1 then message="Thanh cong. Hay xem tinh trang chuyen de tiep tuc."
    elseif result==-1 then message="Thieu ve nhan them chuyen (Khao Quan Lenh) trong F4. Chua tru vat pham."
    elseif result==-2 then message="Can doi thoai dung NPC va dung buoc nhiem vu."
    elseif result==-3 then message="Event dang tat nhan chuyen moi hoac app Event Phong Than chua chay. Chuyen da nhan van duoc tra va nhan thuong."
    elseif result==-4 then message="Chua du cap/lanh dia/uy danh, het luot trong ngay, hoac dang co chuyen chua hoan tat."
    elseif result==-5 then message="Xe khong con hoac chuyen da het han. Hay kiem tra lai tinh trang chuyen."
    elseif result==-6 then message="Can dua xe den sat NPC tra nhiem vu. Chi chu xe moi duoc giao xe." end
    Say(message,2,"Quay lai/main","Ket thuc doi thoai/pt_escort_close")
end
function pt_escort_take() pt_escort_result(EventEscortAction({kind},1)) end
function pt_escort_deliver() pt_escort_result(EventEscortAction({kind},2)) end
function pt_escort_claim() pt_escort_result(EventEscortAction({kind},3)) end
function pt_escort_cancel()
    Say("Huy chuyen se mat xe va luot da nhan. Ban co chac chan?",2,"Dong y huy/pt_escort_cancel_yes","Quay lai/main")
end
function pt_escort_cancel_yes() pt_escort_result(EventEscortAction({kind},4)) end
'''.encode('ascii')

phong='script/phongthan/npc_restore/1015_01.lua'
base=(server/phong).read_bytes();assert b'pt_original_main' in base
put('Server/'+phong,base+b'\nfunction pt_escort_old() return pt_original_main() end\n'+tail(1,True,True))
text('Server/script/phongthan/escort/cart.lua','''function main()
Say("Xe dang duoc ho tong. Giu xe o gan khi di chuyen. Chu xe gap NPC giao nhiem vu de xem thoi han.",1,"Ket thuc doi thoai/no")
end
function no() CloseDialog() end
''')
worlds={int(a):scope for a,scope in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes())}
display=(server/'settings/phongthan/NpcDisplayNames.txt').read_bytes();reverse={c:i for i,c in enumerate(decode_tcvn3(bytes(range(256))))}
def vn(s):return bytes(reverse.get(c,ord(c) if ord(c)<128 else 63) for c in s)
new=[(1,False,1019,164*256+128,196*512+256,190,'张桂芳','Trương Quế Phương','van_luong_deliver.lua',None),
     (2,True,1019,1464*32,3239*32,152,'神秘镖头','Tiêu đầu thần bí','van_tieu_giver.lua','AC557A46'),
     (2,False,1021,233*256+128,198*512+256,152,'总镖头','Tổng tiêu đầu','van_tieu_deliver.lua','162B1F5B')]
for kind,giver,world,x,y,tid,name,translated,file,oid in new:
    logical='\\script\\phongthan\\escort\\'+file;rx,ry=x//512,y//1024
    stem=f'settings/phongthan/npc_regions/{world}/v_{ry:03d}/{rx:03d}';relative=None
    for suffix in ('_region_s.dat','_npc_s.dat'):
        if (server/(stem+suffix)).exists():relative=stem+suffix;break
    geometry=None;scope=worlds[world]
    for suffix in (b'_S',b''):
        got=original.get(b'\\maps\\'+scope+suffix+f'\\v_{ry:03d}\\{rx:03d}_region_s.dat'.encode())
        if got:geometry=got[0];break
    if geometry is None:
        got=original.get(b'\\maps\\'+scope+f'\\v_{ry:03d}\\{rx:03d}_region_c.dat'.encode())
        if got:geometry=got[0]
    if geometry is None:raise ValueError(('No geometry',world,rx,ry))
    if relative is None:relative=stem+'_npc_s.dat';data=standalone_npcs([])
    else:data=changes.get('Server/'+relative,(server/relative).read_bytes())
    old=npc_records(split_region(data)[2]) if relative.endswith('_region_s.dat') else npc_records(data)
    if any(logical.encode() in r for r in old):raise ValueError('Already staged/deployed NPC: '+logical)
    occupied=[struct.unpack_from('<ii',r,4) for r in old];x,y=place_in_tile(geometry,x,y,occupied)
    rawscript=logical.encode('ascii');record=struct.pack('<iiiB3x32shhhhBBH',tid,x,y,0,name.encode('gbk'),1,0,0,3,6,0,len(rawscript))+rawscript
    put('Server/'+relative,merge_region(data,[record]) if relative.endswith('_region_s.dat') else standalone_npcs(old+[record]))
    original_bytes=(OUT/'escort_originals'/(oid+'.lua')).read_bytes() if oid else b'-- Reviewed compatibility implementation; VNG original script unavailable.\n'
    put('Server/script/phongthan/escort/'+file,original_bytes+tail(kind,giver))
    display+=name.encode('gbk')+b'\t'+vn(translated)+b'\r\n'
    placements.append(dict(world=world,name=translated,template=tid,x=x,y=y,display=[x//256,y//512],script=logical,override=relative,
        original_lua_id=oid,appearance='ORIGINAL_SPAWN_TEMPLATE' if giver and kind==2 else 'REUSED_REVIEWED_ARCHETYPE_NOT_PROVEN_ORIGINAL_IDENTITY'))
put('Server/settings/phongthan/NpcDisplayNames.txt',display)

prepared=json.loads((ROOT/'analysis/restoration_20261016/prepared_changes.json').read_text('utf8'))
catalog=[];native=[]
def add(id,key,name,group,adapter,map,mission,tid,flag,path,defaults,evidence):
    defaults=dict(Id=id,**defaults)
    catalog.append(dict(Id=id,Key=key,Name=name,Group=group,Adapter=adapter,Map=map,Mission=mission,Template=tid,BossFlag=flag,StartScriptHex=path.hex(),SourceEvidence=evidence,Defaults=defaults))
    native.append('\t'.join(mapstr(str,id,1 if adapter=='vtt' else 2 if adapter=='boss' else 3,map,map-1000 if map else 0,mission,tid,0,flag,path.hex(),encode_tcvn3(name).hex() if adapter=='boss' else key.encode('ascii').hex())))
def mapstr(fn,*values):return [fn(v) for v in values]
for i,label in enumerate(('Thổ','Thủy','Hỏa','Phong'),1):
    path=f'\\script\\phongthan\\van_tien\\open{i}.lua'.encode('ascii')
    add(i,'vtt_'+('tho','thuy','hoa','phong')[i-1],'Vạn Tiên Trận '+label,'Vạn Tiên Trận','vtt',1066+i,i,0,-1,path,
        dict(DailyTimes=['02:20','10:20','13:20','18:20','21:20','23:20'],RegistrationSeconds=300,DurationSeconds=1800,TicketGenre=3,TicketDetail=61+i,TicketCount=1),
        'Original portal ticket material62..65; original Lua/native drop bindings. Existing deployed schedule retained; editable in Studio.')
for boss in prepared['bosses']:
    flag=boss['flag'];slots=sorted({f"{r['hour']:02d}:{r['minute']:02d}" for r in prepared['boss_schedule'] if r['flag']==flag})
    add(1000+flag,'boss_'+str(flag),boss_announcement_name(boss['template']),'Boss Server','boss',1000+boss['content_map'],0,boss['template'],flag,bytes.fromhex(boss['raw_hex']),dict(DailyTimes=slots),
        'Original client script '+boss['id']+'; recovered OriginalWorldSchedule. Drop engine and global tables unchanged.')
add(2001,'van_luong','Vận Lương','Nhiệm vụ','van_luong',1015,0,556,-1,b'',
    dict(DailyTimes=[],MinimumLevel=55,RequireTong=True,DailyLimit=20,FreeRuns=1,TicketGenre=8,TicketDetail=268,TicketParticular=2,TicketCount=1,RewardGenre=8,RewardDetail=267,RewardCount=1,RewardExperience=100000),
    'VNG 2007 guide: Phong Lam191/212 → Truong Que Phuong164/196 → Phong Lam; 20/day, 1 free; 100000 XP screenshot. Existing IB267 is the compatibility Kim Bai Giap variant, not proof of original random Li Tinh exchange; no invented random rates.')
add(2002,'van_tieu','Vận Tiêu','Nhiệm vụ','van_tieu',1019,0,363,-1,b'',
    dict(DailyTimes=[],MinimumLevel=30,MinimumCredit=10,DailyLimit=20,FreeRuns=20,TicketCount=0,RewardGenre=3,RewardDetail=83,RewardCount=1,RewardMoney=500000,RewardCredit=10),
    'Original AC557A46/F4A59849 spawn at Tuyet Long; 162B1F5B completion: bounty/10 credit/Ngoc ty. Compatibility follow-cart movement; daily20 is an editable project default, not an asserted VNG limit.')
put('Server/EventService/catalog.json',json.dumps(catalog,ensure_ascii=False,indent=2).encode('utf8'))
put('Server/EventService/catalog.tsv',('\n'.join(native)+'\n').encode('ascii'))
put('Server/EventService/managed.enabled',b'EventPhongThan service owns catalog schedules; no automatic fallback.\r\n')
files=[]
for relative,data in changes.items():
    target=ROOT/'PhongThanRuntime-Loose'/relative;p=STAGE/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    files.append(dict(relative=relative,sha256=sha(data),before_sha256=sha(target.read_bytes()) if target.exists() else None))
(OUT/'escort_publication_plan.json').write_text(json.dumps(dict(files=files,npcs=placements,catalog_events=len(catalog),live_modified=False),ensure_ascii=False,indent=2),'utf8')
print(json.dumps(dict(staged_files=len(files),new_npcs=len(placements),events=len(catalog),live_modified=False)))
