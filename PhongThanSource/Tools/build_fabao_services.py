from pathlib import Path
import sys,re,json
sys.path.insert(0,'PhongThanSource/Tools');from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3
out=Path('analysis/fabao_audit_20261027');stage=out/'compiled/Server';root=Path('PhongThanRuntime-Loose/Server');tables={}
for name in ['amulet','material','magicscript','ibitem','questkey']:
 tables[name]=[r.split(b'\t') for r in (root/'settings/item/001'/(name+'.txt')).read_bytes().splitlines() if r]
def clean(b):return re.sub(rb'<[^>]*>',b'',b).lstrip(b'#$')
def name(token):
 m=re.match(rb'(\d+)\.(\d+)\.(\d+)\.(\d+)',token);assert m;g,d,p,q=map(int,m.groups());table={0:'amulet',3:'material',4:'questkey',6:'magicscript',8:'ibitem'}[g]
 if g==6 and 890<=p<=892:return 'Đồ Phổ Thất Bảo Kim Liên ('+['Giáp Sĩ','Đạo Sĩ','Dị Nhân'][p-890]+')'
 for r in tables[table][1:]:
  if g==0 and int(r[3])==p and int(r[11])==(10 if b'[10]' in token else 1):return decode_tcvn3(clean(r[0]))
  if g in (3,4) and int(r[2])==d:return decode_tcvn3(clean(r[0]))
  if g==6:
   anchor=next((i for i,v in enumerate(r) if b'.spr' in v.lower()),-1)
   if anchor>1 and r[anchor-1].isdigit() and int(r[anchor-1])==p:
    label=clean(r[0]);label=label[:-1] if anchor==3 and label.endswith(b'6') else label
    return decode_tcvn3(label)
  if g==8 and r[3].isdigit() and int(r[2])==d and int(r[3])==p:return decode_tcvn3(clean(r[0]))
 raise ValueError(token)
def quote(s):return '"'+s.replace('\\','\\\\').replace('"','\\"').replace('\n','\\n')+'"'
rows=[r.split(b'\t') for r in (root/'settings/item/001/zhuang_bei_he_cheng_gui_ze_biao.txt').read_bytes().splitlines() if r]
fees={r[0]:r for r in [l.split(b'\t') for l in (root/'settings/item/001/zhuang_bei_he_cheng_jin_qian_xu_qiu.txt').read_bytes().splitlines() if l]}
def book(realm):
 realm_parts=set()
 for a in tables['amulet'][1:]:
  if a[11]!=b'1':continue
  if any(a[c]==b'219' and int(a[c+1] or b'0')>0 for c in range(44,56,2)):realm_parts.add(int(a[3]))
 data=['PtFbBook={}']
 for r in rows[1:]:
  if len(r)!=19 or r[15]!=b'6' or not r[13].startswith(b'0.4.'):continue
  if int(r[14]) not in realm and not (100 in realm and r[14]==b'1' and int(r[13].split(b'.')[2]) in realm_parts):continue
  title=name(r[13]);inputs=[]
  for token in r[3:3+int(r[2])]:
   q=int(token.split(b'.')[3].split(b'(')[0].split(b'[')[0]);label=name(token)
   suffix=' #9' if b'(9)' in token else ''
   inputs.append(str(q)+' '+label+suffix)
  pages=[]
  for at in range(0,len(inputs),3):pages.append('Nguyên liệu hợp thành <color=green>'+title+'<color>:\n'+'\n'.join('<color=yellow>'+x+'<color>' for x in inputs[at:at+3]))
  fee=int(fees[r[0]][1] or b'0');rate=int(r[16]);pages.append('Phí hợp thành: <color=yellow>'+str(fee//10000)+' vạn<color>.\nTỷ lệ thành công: <color=yellow>'+str(rate)+'%<color>.'+('\nThất bại mất nguyên liệu và phí.' if rate<100 else ''))
  data.append('tinsert(PtFbBook,{'+quote(title)+',{'+','.join(quote(p) for p in pages)+'}})')
 return '\n'.join(data)+'\n'+'''
function pt_fabao_book(page)
 local options={}
 local first=(page-1)*6+1
 local last=min(getn(PtFbBook),first+5)
 for i=first,last do tinsert(options,PtFbBook[i][1].."/pt_fabao_formula("..i..","..page..")") end
 if page>1 then tinsert(options,"Trang trước/pt_fabao_book("..(page-1)..")") end
 if last<getn(PtFbBook) then tinsert(options,"Trang sau/pt_fabao_book("..(page+1)..")") end
 tinsert(options,"Quay lại/main")
 Say("Chọn công thức Pháp Bảo muốn xem.",getn(options),options)
end
function pt_fabao_formula(i,page)
 if not PtFbBook[i] then return end
 local pages=PtFbBook[i][2]
 local callback="pt_fabao_book("..page..")"
 if getn(pages)==4 then Talk(4,callback,pages[1],pages[2],pages[3],pages[4])
 elseif getn(pages)==3 then Talk(3,callback,pages[1],pages[2],pages[3])
 elseif getn(pages)==2 then Talk(2,callback,pages[1],pages[2])
 else Talk(1,callback,pages[1]) end
end
'''
xpath=Path('script/phongthan/npc_services/xich_tung_tu.lua');base=(root/xpath).read_bytes();mark=base.find(b'-- PT_FABAO_RESTORE_BEGIN');base=base[:mark] if mark>=0 else base;m=re.search(rb'function pt_xich_menu\(\).*?\nend\n',base,re.S);assert m
menu=decode_tcvn3(m[0]).replace('dụng.",5,','dụng.",6,').replace('"Đổi Hồn/pt_xich_soul")','"Đổi Hồn/pt_xich_soul", "Pháp Bảo/pt_fabao_menu")');assert "/pt_fabao_menu" in menu;base=base[:m.start()]+encode_tcvn3(menu)+base[m.end():]
extra='''
-- PT_FABAO_RESTORE_BEGIN
function pt_fabao_menu()
 SayButtons("Hợp thành hoặc thăng cấp Pháp Bảo Nhân Giới. Đặt đúng vật phẩm vào cửa sổ ghép để kiểm tra phí trước khi xác định.",3,
 "Hợp thành/dz","Thăng cấp/dz","Công thức/pt_fabao_book(1)")
end
'''
p=stage/xpath;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(base+encode_tcvn3(extra+book({1})))
tao='''-- Scoped restored service: Tao Bao, Bat Chu Thien Quan 244/236.
function main()
 if GetWorldPos()~=1073 then CloseDialog() return end
 SayButtons("Ta phụ trách hợp thành và thăng cấp Pháp Bảo Tiên Ma. Ngươi muốn làm gì?",3,
 "Hợp thành/pt_tao_fabao","Thăng cấp/pt_tao_fabao","Công thức/pt_fabao_book(1)")
end
function pt_tao_fabao()
 if GetWorldPos()~=1073 then CloseDialog() return end
 CloseDialog()
 EnchaseItem(7)
end
function no() CloseDialog() end
'''
p=stage/'script/phongthan/npc_restore/1073_06.lua';p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(encode_tcvn3(tao+book({100,101})))
print('Staged scoped NPC scripts; human/Tien Ma recipe books')
