from pathlib import Path
r=Path(__file__).resolve().parents[1]/'Sources/Core/Src'
p=r/'PhongThanThuongChu.inl';d=p.read_bytes();a=b'extern int PhongThanThuongChuEconomy(int,int,int);';assert d.count(a)==1;d=d.replace(a,a+b'\nextern int PhongThanThuongChuQuestPayment(int,int,int);');p.write_bytes(d)
p=r/'ScriptFuns.cpp';d=p.read_bytes();a=b'/* TGPT seal/reward transactions use the existing inventory rollback path. */\n#include "PhongThanThuongChuEconomy.inl"\n';assert d.count(a)==1;d=d.replace(a,b'');a=b'#include "PhongThanEventTicket.inl"';assert d.count(a)==1;d=d.replace(a,a+b'\n#include "PhongThanThuongChuEconomy.inl"');p.write_bytes(d)
p=r.parents[2]/'Tests/Native/ThuongChuRuntimeTests.cpp';d=p.read_bytes();a=b'int PhongThanThuongChuEconomy(int,int,int){return certSpace;}'
assert d.count(a)==1;d=d.replace(a,a+b'\nint PhongThanThuongChuQuestPayment(int p,int silver,int copper){if(Player[p].m_ItemList.money<silver||Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)<copper)return 0;Player[p].m_ItemList.money-=silver;Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_COPPER_CASH,Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_COPPER_CASH)-copper,TRUE);return 1;}');p.write_bytes(d)
