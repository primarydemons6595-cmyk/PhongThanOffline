"""Build the test handbook from authoritative item rows, never display names.

Run from the workspace root. Runtime files are staged, not published here.
"""
from pathlib import Path
import sys, json, re

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'PhongThanSource/Tools'))
from extract_client_coordinate_dictionary import decode_tcvn3, encode_tcvn3

OUT = ROOT / 'analysis/event_handbook_20261026'
STAGE = OUT / 'compiled'

def skill_manifest():
    root = ROOT / 'PhongThanRuntime-Loose/Server/settings'
    skills = {}
    for r in (root / 'skills.txt').read_bytes().splitlines()[1:]:
        c = r.split(b'\t')
        if len(c) > 49 and c[2].isdigit():
            skills[int(c[2])] = c
    result = []
    for raw in (root / 'item/001/skillbook.txt').read_bytes().splitlines()[1:]:
        c = raw.split(b'\t')
        if len(c) < 9 or not c[3].isdigit():
            continue
        skill = int(c[3]); description = decode_tcvn3(c[8]).casefold()
        profession = next((p for s, p in [('giáp sĩ',0),('đạo sĩ',1),('dị nhân',2),('vũ sĩ',3)] if s in description), -1)
        if profession < 0 or skill not in skills:
            continue
        cap = int(skills[skill][49] or 0)
        if cap > 0:
            result.append((profession, skill, cap))
    assert len(result) >= 80 and len({x[1] for x in result}) == len(result)
    # These player-cast summons use a separate loader, not Skills.txt/books.
    # Match PhongThanIsSummon: later table entries are not supported by runtime.
    summon_ids = [int(r.split(b'\t')[0]) for r in
                  (root / 'summonskill.txt').read_bytes().splitlines()[1:]
                  if r.split(b'\t')[0].isdigit()]
    for skill in range(450, 459):
        assert summon_ids.count(skill) == 1, ('Missing/duplicate summon', skill)
        assert not any(row[1] == skill for row in result)
        result.append((2, skill, 10))
    header = ('#ifndef PT_HANDBOOK_ORIGINAL_SKILLS\n#define PT_HANDBOOK_ORIGINAL_SKILLS\n'
              '// Generated from player skill books plus supported summons in summonskill.txt.\n'
              '// Summons 450..458 are learned by Di Nhan; child/NPC attack skills are excluded.\n'
              'struct PtHandbookSkill { int profession,id,maximum; };\n'
              'static const PtHandbookSkill kPtHandbookSkills[] = {\n')
    header += ''.join('    {%d,%d,%d},\n' % x for x in result) + '};\n#endif\n'
    target = ROOT / 'PhongThanSource/Sources/Core/Src/PhongThanEventHandbookSkills.inl'
    target.write_text(header, encoding='ascii')
    (OUT / 'skills.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
    return result

def item_manifest():
    data = json.loads((OUT / 'catalog.json').read_text(encoding='utf-8'))
    # Index zero is deliberately invalid: callbacks carry one trusted entry ID.
    header = ('struct PtHandbookItem { int genre,detail,part,level,row,recipe; };\n'
              'static const PtHandbookItem kPtHandbookItems[] = {\n    {0,0,0,0,-1,0},\n')
    for e in data['entries']:
        header += '    {%d,%d,%d,%d,%d,%d},\n' % (e['genre'],e['detail'],e['part'],e['level'],e['row']-1,e['recipe'])
    header += '};\n'
    (ROOT/'PhongThanSource/Sources/Core/Src/PhongThanEventHandbookItems.inl').write_text(header,encoding='ascii')
    return data

def lua_menu(data):
    class_names={-1:'Dùng chung',0:'Giáp Sĩ',1:'Đạo Sĩ',2:'Dị Nhân',3:'Vũ Sĩ'}
    slot_names={7:'Mũ',2:'Áo',6:'Đai',9:'Phi Phong',5:'Giầy'}
    paths={
      'white':['Trang Bị','Trang Bị Trắng'], 'green':['Trang Bị','Trang Bị Lục'],
      'orange':['Trang Bị','Trang Bị Cam'], 'realm':['Trang Bị','Trang Bị Tiên Ma'],
      'weapon_white':['Vũ Khí','Trắng'], 'weapon_gold':['Vũ Khí','Hoàng Kim'],
      'upgrade_material':['Nguyên Liệu Thăng Cấp - Khảm Nạm - Đục Lỗ'],
      'orange_material':['Nguyên liệu ép đồ cam'], 'fabao':['Pháp Bảo'], 'phapkhi':['Pháp Khí'], 'ngocboi':['Ngọc Bội'],
      'jade_material':['Ngọc Bội','Nguyên liệu tẩy tủy và mài ngọc'],
      'tinh_quan':['Trang Bị','Trang Bị Tinh Quân'],
      'tinh_quan_weapon':['Vũ Khí','Tinh Quân'],
      'pet_material':['Nguyên liệu Linh thú'], 'mount':['Thú Cưỡi']}
    tree={'name':'Cẩm Nang Sự Kiện','children':{},'items':[]}
    for e in data['entries']:
        if e.get('disabled_reason'):continue
        path=list(paths[e['category']])
        if e.get('polish_state'):path.append(e['polish_state'])
        if e['genre']==0:
            path.append(class_names.get(e['profession'],'Dùng chung'))
            if e['category'] in ('white','green','orange','realm','tinh_quan'):path.append(slot_names[e['slot']])
            path.append(('Tiên Ma ' if e['realm'] else 'Cấp ') + str(e['tier']) + (' (%dx)'%(e['tier']//10) if e['tier']>0 and e['tier']%10==0 else ''))
        node=tree
        for label in path:node=node['children'].setdefault(label,{'name':label,'children':{},'items':[]})
        node['items'].append(e)
    nodes=[]
    def visit(node,parent):
        node['id']=len(nodes)+1;node['parent']=parent;nodes.append(node)
        for child in node['children'].values():visit(child,node['id'])
    visit(tree,0)
    def quote(text):
        if isinstance(text,bytes):raw=text
        else:
            raw=b''
            for char in text:
                try:raw+=encode_tcvn3(char)
                except KeyError:raw+=encode_tcvn3(char.lower())
        return '"'+''.join('\\%03d'%b if b<32 or b>=127 else '\\'+chr(b) if b in (34,92) else chr(b) for b in raw)+'"'
    lua='-- Generated trusted catalogue; no arbitrary item tuple supplied by the client.\nEBNodes={}\n'
    for node in nodes:
        options=['{'+quote(c['name'])+',"EBMenu('+str(c['id'])+',1)",""}' for c in node['children'].values()]
        for e in node['items']:
            label=bytes.fromhex(e['name_hex'])+encode_tcvn3(' [bậc '+str(e['level'])+'; '+'.'.join(str(e[k]) for k in ('genre','detail','part'))+']')
            options.append('{'+quote(label)+',"EBItem('+str(e['id'])+','+str(node['id'])+')",'+quote(e['image'])+'}')
        if node['id']==1:
            options=['{'+quote(label)+',"'+cb+'",""}' for label,cb in [
              ('Nhận cấp độ 200','EBAction(2)'),('Học tối đa kỹ năng theo hệ phái','EBAction(1)'),
              ('Nhận Xu x100','EBAction(47)'),('Nhận Nguyên Bảo x1','EBAction(46)'),
              ('Nhận Tiền Vạn (nhập số lượng)','EBMoney')]]+options
        lua+='EBNodes[%d]={'%node['id']+quote(node['name'])+','+str(node['parent'])+',{}}\n'
        for index,option in enumerate(options,1):lua+='EBNodes[%d][3][%d]='%(node['id'],index)+option+'\n'
    lua+='''
function main()
    EBMenu(1,1)
end
function EBShow(title,options)
    Say("LINK:\\\\spr\\\\pt_test\\\\book.spr",0,title,getn(options),options)
end
function EBMenu(id,page)
    local node=EBNodes[id]
    if not node then return end
    local options={}
    local first=(page-1)*8+1
    local last=min(first+7,getn(node[3]))
    for i=first,last do
        local row=node[3][i]
        local icon=""
        if row[3]~="" then icon="LINK:"..row[3] end
        tinsert(options,row[1]..icon.."/"..row[2])
    end
    if id==1 and GetPlayerType()==0 then tinsert(options,EBLabels[11].."/EBConvert") end
    if page>1 then tinsert(options,EBLabels[1].."/EBMenu("..id..","..(page-1)..")") end
    if last<getn(node[3]) then tinsert(options,EBLabels[2].."/EBMenu("..id..","..(page+1)..")") end
    if node[2]>0 then tinsert(options,EBLabels[3].."/EBMenu("..node[2]..",1)") end
    EBShow(node[1].."  ["..page.."/"..max(1,ceil(getn(node[3])/8)).."]",options)
end
function EBConvert()
    EBShow(EBLabels[12],{EBLabels[11].."/EBAction(5)",EBLabels[3].."/EBMenu(1,1)"})
end
function EBAction(op)
    local result=EventHandbookAction(op)
    local text=EBLabels[5]
    if result>0 then text=EBLabels[4] end
    EBShow(text,{EBLabels[3].."/EBMenu(1,1)"})
end
function EBItem(id,parent)
    local options={}
    tinsert(options,EBLabels[6].."/EBGive("..id..",1,"..parent..")")
    if EBStackable[id] then
        tinsert(options,EBLabels[7].."/EBGive("..id..",10,"..parent..")")
        tinsert(options,EBLabels[8].."/EBGive("..id..",100,"..parent..")")
    end
    tinsert(options,EBLabels[3].."/EBMenu("..parent..",1)")
    EBShow(EBLabels[9],options)
end
function EBResultText(result)
    if result>0 then return EBLabels[4] end
    if result<=-1 and result>=-7 then return EBLabels[12-result] end
    return EBLabels[5]
end
function EBGive(id,qty,parent)
    local result=EventHandbookAction(3,id,qty)
    local text=EBResultText(result)
    EBShow(text,{EBLabels[3].."/EBMenu("..parent..",1)"})
end
function EBMoney()
    InputDialog(EBLabels[10],2,"OK/EBMoneyDone", "Cancel/main")
end
function EBMoneyDone(value)
    local quantity=tonumber(value)
    local result=0
    if quantity then result=EventHandbookAction(4,quantity) end
    local text=EBLabels[5]
    if result>0 then text=EBLabels[4] end
    EBShow(text,{EBLabels[3].."/EBMenu(1,1)"})
end
'''
    labels=['Trang trước','Trang sau','Quay lại','Đã nhận thành công.',
            'Chưa thực hiện được. Kiểm tra chỗ trống F4, khóa túi hoặc dữ liệu kỹ năng.',
            'Nhận 1','Nhận 10','Nhận 100','Chọn số lượng cần nhận.', 'Số lượng tiền vạn (1 - 1000)',
            'Đổi Giáp Sĩ thành Vũ Sĩ','Tháo toàn bộ trang bị trước khi đổi. Giữ cấp độ và chỉ số; xóa kỹ năng Giáp Sĩ. Sau đó chọn học tối đa kỹ năng Vũ Sĩ.']
    lua+='EBLabels={'+','.join(map(quote,labels))+'}\n'
    errors=['Cẩm Nang không còn trong túi hoặc đang khóa/giao dịch.',
            'Lựa chọn hoặc số lượng vật phẩm chưa hợp lệ.',
            'Mẫu vật phẩm này chưa được nạp. Vui lòng báo tên món.',
            'Chưa tạo được vật phẩm này. Vui lòng báo tên món.',
            'Mẫu trang bị chưa phù hợp với công thức. Vui lòng báo tên món.',
            'Không còn ô trống phù hợp với kích thước vật phẩm trong túi.',
            'Chưa nhận được số lượng đã chọn. Hãy thử nhận 1 món; vật phẩm và tiền của bạn vẫn được giữ nguyên.']
    for index,message in enumerate(errors,13):lua+='EBLabels[%d]=%s\n'%(index,quote(encode_tcvn3(message)))
    lua+='EBStackable={'+','.join('[%d]=1'%e['id'] for e in data['entries'] if e['genre']!=0)+'}\n'
    target=STAGE/'Server/script/phongthan/test/event_handbook.lua';target.parent.mkdir(parents=True,exist_ok=True)
    realm_source=(ROOT/'PhongThanSource/Tools/event_handbook_realm.lua').read_text('utf8').replace('–','-')
    target.write_bytes(lua.encode('ascii')+b'\r\n-- PT_EVENT_HANDBOOK_REALM_20261106\r\n'+encode_tcvn3(realm_source))
    return len(nodes)

if __name__ == '__main__':
    print('Authoritative player skill books:', len(skill_manifest()))
    data=item_manifest()
    print('Trusted handbook items:',len(data['entries']))
    print('Menu folders:',lua_menu(data))
