from pathlib import Path
import hashlib,json,re,sys,shutil

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'PhongThanSource/Tools'))
from extract_client_coordinate_dictionary import encode_tcvn3 as enc,decode_tcvn3 as dec
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/ItemUiCorrections20261031'
assert not (PACKAGE/'publication.json').exists(),'Do not overwrite a published snapshot'
files=[]
def stage(role,relative,data):
    live=ROOT/'PhongThanRuntime-Loose'/role/relative
    original=PACKAGE/'source'/role/relative
    original.parent.mkdir(parents=True,exist_ok=True)
    if not original.exists():original.write_bytes(live.read_bytes())
    target=PACKAGE/'compiled'/role/relative
    target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
    files.append(dict(path=role+'/'+relative,before=hashlib.sha256(original.read_bytes()).hexdigest(),after=hashlib.sha256(data).hexdigest()))

# Preserve the recovered map dictionary. Change only this item's presentation.
raw=(ROOT/'PhongThanRuntime-Loose/Server/script/phongthan/items/compass.lua').read_bytes()
dictionary=raw.split(b'\n',1)[0]
lua='''
function no() CloseDialog() end
function main(item)
 if TravelItem(0,item)~=12 then return end
 SayTask("Chỉ Nam Châu có thể đưa ngươi đến nơi đã ghi nhớ bất kỳ! Hãy chọn chức năng!",{{"Ghi nhớ","pt_compass_save"},{"Chuyển tiếp","pt_compass_go"},{"Xóa","pt_compass_clear"}})
end
function pt_compass_menu(action)
 local choices={}
 for i=1,5 do
  local ok,map,x,y=CompassItem(0,i)
  local label="Địa điểm "..i
  if ok==1 then label=label..": <color=green>"..(PT_COMPASS_MAPS[map] or tostring(map)).."<color> ("..x..":"..y..")" end
  tinsert(choices,label.."/pt_compass_"..action..i)
 end
 tinsert(choices,"Kết thúc/no")
 local prompt="Hãy chọn địa điểm cần ghi nhớ:"
 if action=="go" then prompt="Hãy chọn địa điểm cần chuyển đến:" end
 if action=="clear" then prompt="Hãy chọn địa điểm cần xóa:" end
 SayButtons(prompt,getn(choices),choices)
end
function pt_compass_save() pt_compass_menu("save") end
function pt_compass_go() pt_compass_menu("go") end
function pt_compass_clear() pt_compass_menu("clear") end
function pt_compass_apply(op,slot)
 local ok=CompassItem(op,slot)
 if ok~=1 then Msg2Player("Không thể thực hiện tại đây. Vị trí và lượt sử dụng được giữ nguyên.");return end
 if op==1 then Msg2Player("Đã ghi nhớ vị trí. Không tiêu hao lượt sử dụng.") end
 if op==3 then Msg2Player("Đã xóa vị trí. Không tiêu hao lượt sử dụng.") end
 CloseDialog()
end
'''
for i in range(1,6):
    lua+=f'function pt_compass_save{i}() local ok=CompassItem(0,{i});if ok==1 then MsgBox("Ghi đè vị trí đã lưu?","pt_compass_confirm{i}","no") else pt_compass_apply(1,{i}) end end\n'
    lua+=f'function pt_compass_confirm{i}() pt_compass_apply(1,{i}) end\nfunction pt_compass_go{i}() pt_compass_apply(2,{i}) end\nfunction pt_compass_clear{i}() pt_compass_apply(3,{i}) end\n'
stage('Server','script/phongthan/items/compass.lua',dictionary+b'\n'+enc(lua))

# 25 map options exceeded MAX_ANSWERNUM=20. Page inside this script; stable
# destination/callback indexes continue to match the native destination table.
p=ROOT/'PhongThanRuntime-Loose/Server/script/phongthan/travel_items.lua'
raw=p.read_bytes().replace(b'\r\n',b'\n')
raw=raw.replace(b' local options={}\n',b' PT_TRAVEL_PROFILES=PT_TRAVEL_PROFILES or {}\n PT_TRAVEL_PROFILES[PlayerIndex]=profile\n pt_travel_page(1)\nend\nfunction pt_travel_page(page)\n local profile=PT_TRAVEL_PROFILES and PT_TRAVEL_PROFILES[PlayerIndex]\n if not profile then return end\n local all={}\n')
raw=raw.replace(b'tinsert(options,d[3].."/pt_travel_"..i)',b'tinsert(all,d[3].."/pt_travel_"..i)')
a=b' if getn(options)>0 then Say(10286,getn(options),options) end'
b=enc(''' local options={}
 local first=(page-1)*16+1
 for i=first,min(first+15,getn(all)) do tinsert(options,all[i]) end
 if page>1 then tinsert(options,"Trang trước/pt_travel_page1") end
 if first+15<getn(all) then tinsert(options,"Trang sau/pt_travel_page2") end
 tinsert(options,"Kết thúc/no")
 if getn(all)>0 then Say(10286,getn(options),options) end''')
assert raw.count(a)==1;raw=raw.replace(a,b)
raw+=b'\nfunction pt_travel_page1() pt_travel_page(1) end\nfunction pt_travel_page2() pt_travel_page(2) end\n'
stage('Server','script/phongthan/travel_items.lua',raw)

attrs=[(198,5),(146,-5),(86,10),(111,10),(115,15),(116,15)]
description='Tấn công tập trung phép thuật: +5%<enter>Tấn công tập trung: -5%<enter>Sinh lực (HP): +10%<enter>Di chuyển (Tốc độ chạy): +10%<enter>Xuất chiêu Vũ Khí: +15%<enter>Xuất chiêu Bùa Pháp: +15%'
for role in ['Server','Client']:
    rel='settings/item/001/ibitem.txt';rows=(ROOT/'PhongThanRuntime-Loose'/role/rel).read_bytes().splitlines()
    for i,line in enumerate(rows[1:],1):
        c=line.split(b'\t')
        if len(c)<13 or c[1]!=b'8':continue
        if c[2]==b'9444':c[0]=enc('#Hóa Thân Tỏa');c[8]=enc('#Sau khi sử dụng, trong vòng 7 ngày không thể nhận thêm kinh nghiệm nhân vật.')
        if c[2]==b'9445':c[0]=enc('#Buff Hóa Thân Tỏa');c[8]=enc('#Không thể nhận thêm kinh nghiệm nhân vật trong 7 ngày.')
        if c[2] in [b'7446',b'7447']:
            c[8]=enc('#'+description)
            # Legacy catalog has five pairs; native buff handler applies all six.
            assert len(c)>40
            c[31:41]=[str(v).encode() for a in attrs[:5] for v in a]
        rows[i]=b'\t'.join(c)
    stage(role,rel,b'\r\n'.join(rows)+b'\r\n')
    rel='settings/ibshop/ibshopgoods.txt';rows=(ROOT/'PhongThanRuntime-Loose'/role/rel).read_bytes().splitlines()
    for i,line in enumerate(rows[1:],1):
        c=line.split(b'\t')
        if len(c)>3 and c[1:4]==[b'8',b'9444',b'2']:c[0]=enc('Hóa Thân Tỏa')
        if len(c)>3 and c[1:4]==[b'8',b'1408',b'2']:c[0]=enc('Túi Pháp Bảo đặc biệt')
        rows[i]=b'\t'.join(c)
    stage(role,rel,b'\r\n'.join(rows)+b'\r\n')

rel='Ui/ui3/PtConsumableBuff.ini'
raw=(ROOT/'PhongThanRuntime-Loose/Client'/rel).read_bytes().replace(b'\r\n',b'\n').replace(b'\r',b'')
for detail,title,desc in [(7446,'Buff Thanh Long Lệnh',description),(7447,'Buff Thanh Long Lệnh',description),(9445,'Hóa Thân Tỏa','Không thể nhận thêm kinh nghiệm nhân vật trong 7 ngày.')]:
    pattern=rb'(\[BuffInfo_'+str(detail).encode()+rb'\]\n)([^\[]*)'
    match=re.search(pattern,raw);assert match
    body=re.sub(rb'^Title=[^\n]*',lambda _:b'Title=$'+enc(title),match[2],flags=re.M)
    body=re.sub(rb'^Desc=[^\n]*',lambda _:b'Desc=$'+enc(desc),body,flags=re.M)
    raw=raw[:match.start(2)]+body+raw[match.end(2):]
stage('Client',rel,raw.replace(b'\n',b'\r\n'))
(PACKAGE/'manifest.json').write_text(json.dumps(dict(scope='LOOSE_ONLY',files=files),indent=2),'utf8')
print('Staged',len(files),'item-specific content corrections')
