"""One-time byte-preserving native/Studio integration."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];r=ROOT/'PhongThanSource/Sources/Core/Src'
def edit(path,a,b):
 d=path.read_bytes();a=a.encode('utf8');b=b.encode('utf8')
 if b in d:return
 assert d.count(a)==1,(path,a,d.count(a));path.write_bytes(d.replace(a,b))
edit(r/'CoreServerShell.cpp','#include "PhongThanThuongChu.inl"','#include "PhongThanThuongChu.inl"\n#include "PhongThanCustomEvents.inl"')
edit(r/'CoreServerShell.cpp','PhongThanThuongChuTick();','PhongThanThuongChuTick();\n    PhongThanCustomEventsTick();')
edit(r/'ScriptFuns.cpp','#include "PhongThanQuestExchange.inl"','#include "PhongThanQuestExchange.inl"\n#ifdef _SERVER\n#include "PhongThanCustomEventActions.inl"\n#endif')
edit(r/'ScriptFuns.cpp','{"ThuongChu", LuaThuongChu},','{"ThuongChu", LuaThuongChu},\n    {"CustomEvent", LuaCustomEvent},\n    {"CustomEventConsume", LuaCustomEventConsume},')
p=r/'KItemList.cpp';d=p.read_bytes();a=b'int realmUse=PhongThanRealmUseItem(m_PlayerIdx,nIdx);'
if b'int customUse=PhongThanCustomUseItem' not in d:
 at=d.index(a,d.index(b'BOOL KItemList::NowEatItem'));d=d[:at]+b'extern int PhongThanCustomUseItem(int,int);int customUse=PhongThanCustomUseItem(m_PlayerIdx,nIdx);if(customUse>=0)return customUse!=0;\n    '+d[at:];p.write_bytes(d)
app=ROOT/'PhongThanRuntime-Loose/ManagerStudio/Wpf/PhongThanStudio.App'
edit(app/'EventManagerView.cs','evidence.Text = selected.SourceEvidence;','evidence.Text = "";')
edit(app/'EventManagerView.cs','Theo luật Tam Giới: báo danh','Báo danh')
print('Native custom-event runtime wired; website reference details removed from event forms')
