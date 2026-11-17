"""Scoped Lei Zhen Zi / needle dialogs. Native code owns all rewards and costs."""
from pathlib import Path
import sys,json
sys.path.insert(0,str(Path(__file__).parent))
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
OUT=ROOT/'analysis/jade_restore_20261026'
STAGE=OUT/'compiled'

common='''
function jade_result(result)
 if result==1 then Talk(1,"main","Đã hoàn thành.")
 elseif result==-2 then Talk(1,"main","Ngọc Bội đã đủ số hệ thuộc tính hoặc dữ liệu chưa phù hợp. Có thể tẩy lại hệ đã có bằng Tẩy Tủy Châu.")
 else Talk(1,"main","Chưa thực hiện được. Kiểm tra vật phẩm đã chọn, khóa túi, chỗ trống và nguyên liệu hoặc tiền cần dùng.") end
end
function jade_list(mode,page)
 local items=JadeList(mode)
 local options={}
 local first=(page-1)*6+1
 local last=min(getn(items),first+5)
 for i=first,last do
  tinsert(options,items[i][2].."/jade_confirm("..mode..","..items[i][1]..")")
 end
 if page>1 then tinsert(options,"Trang trước/jade_list("..mode..","..(page-1)..")") end
 if last<getn(items) then tinsert(options,"Trang sau/jade_list("..mode..","..(page+1)..")") end
 tinsert(options,"Quay lại/main")
 local text="Chọn Ngọc Bội trong túi để thủ tủy. Tháo Ngọc Bội khỏi người trước. Sau khi xác nhận, Ngọc Bội mất và nhận 1 Ngọc Tủy ngẫu nhiên."
 if mode==1 then text="Chọn ngọc hoặc Quái Phù chưa mài trong túi. Phí mài sẽ hiện trước khi xác định; mỗi lần mài 1 vật phẩm." end
 if mode==2 then text="Chọn ngọc đã mài trong túi để hoàn nguyên. Thuộc tính hiện tại sẽ mất, nhận lại đúng loại ngọc chưa mài." end
 Say(text,getn(options),options)
end
function jade_extract() jade_list(0,1) end
function jade_polish() jade_list(1,1) end
function jade_revert() jade_list(2,1) end
function jade_confirm(mode,id)
 if mode==2 then jade_revert_confirm(id) return end
 local text="Thủ tủy sẽ mất Ngọc Bội đã chọn để nhận 1 Ngọc Tủy ngẫu nhiên. Đồng ý?"
 local operation=2
 if mode==1 then
  local info=JadeService(5,id)
  if not info[1] then jade_list(1,1) return end
  text="Mài sáng 1 <color=green>"..info[1].."<color>, cần <color=green>"..(info[2]/10000).." vạn<color> bạc, đồng ý khai quang chứ?"
  operation=3
 end
 SayButtons(text,2,"Xác định/jade_do("..operation..","..id..")","Hủy/jade_list("..mode..",1)")
end
function jade_do(operation,id)
 local result,output=JadeService(operation,id)
 if operation==3 and result==1 then
  local info=JadeService(5,output)
  if not info[1] then jade_result(result) return end
  local star=""
  if info[5]==1 then star="<pic=428>" end
  local text="Mài sáng thành công <color=green>"..info[1].."<color>! Thuộc tính của ngọc là:\\n\\n<color=99,101,255>"..star..info[3].."<color>\\n\\nCó phải muốn tiến hành hoàn nguyên cho nó?"
  SayButtons(text,2,"Xác định/jade_revert_confirm("..output..")","Hủy/main")
 else jade_result(result) end
end
function jade_revert_confirm(id)
 local info=JadeService(5,id)
 if not info[1] or info[4]~=1 then jade_list(2,1) return end
 SayButtons("Hoàn nguyên <color=green>"..info[1].."<color> cần <color=green>1 Di Quang Kính<color> hoặc <color=green>18 Xu<color>. Xác định muốn hoàn nguyên chứ?",2,
 "Xác định/jade_revert_do("..id..")","Hủy/main")
end
function jade_revert_do(id) jade_result(JadeService(4,id)) end
'''
needle='''-- Native transaction validates the equipped serial and every ingredient.
function main()
 local id=JadeService(0)
 if id==0 then Talk(1,"no","Bạn chưa trang bị Ngọc Bội.") return end
 SayButtons("Chọn hệ Ngọc Tủy muốn chú nhập. Mỗi lần dùng 1 Dẫn Tủy Châm và 5 Ngọc Tủy cùng hệ. Tẩy lại hệ đã có cần thêm 1 Tẩy Tủy Châu.",5,
 "Hỏa Ngọc Tủy/jade_choose(1,"..id..")","Băng Ngọc Tủy/jade_choose(2,"..id..")","Thổ Ngọc Tủy/jade_choose(3,"..id..")","Lôi Ngọc Tủy/jade_choose(4,"..id..")","Kim Ngọc Tủy/jade_choose(5,"..id..")")
end
function jade_choose(group,id)
 SayButtons("Tẩy tủy Ngọc Bội đang đeo với 5 Ngọc Tủy cùng hệ và 1 Dẫn Tủy Châm. Nếu hệ này đã có dòng thì dùng thêm 1 Tẩy Tủy Châu. Đồng ý?",2,
 "Xác định/jade_refine("..group..","..id..")","Quay lại/main")
end
function jade_refine(group,id) jade_result(JadeService(1,group,id)) end
function no() CloseDialog() end
'''

def build():
    info=json.loads((ROOT/'analysis/lei_restore_20261026/content.json').read_text(encoding='utf-8'))
    base=(ROOT/'PhongThanRuntime-Loose/Server'/info['npc_path']).read_bytes()
    # Replace our prior common block; repeated builds must not append copies.
    prior=base.find(b'function jade_result(')
    if prior>=0:base=base[:prior]
    # Keep all existing socket / slot callbacks; replace only root menu.
    end=base.index(b'function socket()')
    root='''-- Lei Zhen Zi, Dieu Tri only. Existing socket callbacks preserved.
function main()
 SayButtons("Ta có thể đục lỗ, khảm nạm, trang trí, thủ tủy Ngọc Bội, mài ngọc và hoàn nguyên ngọc. Ngươi cần giúp điều gì?",6,
 "Đục lỗ - Trang trí/socket","Khảm nạm/inlay","Thủ tủy Ngọc Bội/jade_extract","Mài ngọc/jade_polish","Hoàn nguyên ngọc/jade_revert","Thuyết minh/help")
end
'''
    npc=STAGE/'Server'/info['npc_path'];npc.parent.mkdir(parents=True,exist_ok=True)
    npc.write_bytes(encode_tcvn3(root)+base[end:]+encode_tcvn3(common))
    item=STAGE/'Server/script/phongthan/jade/needle.lua';item.parent.mkdir(parents=True,exist_ok=True)
    item.write_bytes(encode_tcvn3(needle+common))
    for side in ('Client','Server'):
        table=ROOT/'PhongThanRuntime-Loose'/side/'settings/item/001/magicscript.txt'
        rows=[r.split(b'\t') for r in table.read_bytes().splitlines()]
        row=next(r for r in rows[1:] if r[1:4]==[b'6',b'1',b'739'])
        row[14]=br'\script\phongthan\jade\needle.lua'
        target=STAGE/side/'settings/item/001/magicscript.txt';target.parent.mkdir(parents=True,exist_ok=True)
        target.write_bytes(b'\r\n'.join(b'\t'.join(r) for r in rows)+b'\r\n')
        target=STAGE/side/'settings/item/001/jade_magic.txt'
        target.write_bytes((ROOT/'analysis/jade_audit_20261026/jade_magic.txt').read_bytes())
    return npc,item
if __name__=='__main__':
    print(*build(),sep='\n')
