import ast,csv,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import build_offline_loose as b
sys.stdout.reconfigure(encoding='utf-8')
root=Path(__file__).parents[2]
t=ast.parse((root/'PhongThanRuntime-Loose/ManagerStudio/main.py').read_text('utf-8'))
ns={};exec(compile(ast.Module(body=[n for n in t.body if isinstance(n,(ast.FunctionDef,ast.Assign))],type_ignores=[]),'x','exec'),ns)
for f in [root/'PhongThanRuntime-Loose/Client/settings/item/001/armor.txt',root/'PhongThanRuntime-Loose/Client/settings/item/001/helm.txt',root/'PhongThanRuntime-Loose/Client/settings/item/001/boot.txt',root/'PhongThanRuntime-Loose/Client/settings/item/001/belt.txt',root/'PhongThanRuntime-Loose/Client/settings/item/001/cuff.txt']:
 for i,line in enumerate(f.read_bytes().splitlines()):
  c=line.split(b'\t'); name=ns['decode_tcvn3'](c[0]).lstrip('#')
  if 'Chấn Lôi' in name or 'Hỏa Lôi' in name or 'Phong Lôi' in name:
   path=c[4].decode('mbcs','replace') if len(c)>4 else ''
   print(f.name,i+1,name,'|',repr(path))
   if 'Chấn Lôi Khải' in name:
    eid=b.pak_id(c[4]); print('ID',hex(eid))
