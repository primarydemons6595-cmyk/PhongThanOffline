from pathlib import Path
import json,re
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'Output/Tools';OUT.mkdir(exist_ok=True)
raw=(ROOT/'Sources/Core/Src/ScriptFuns.cpp').read_bytes()
def function(signature):
 start=raw.index(signature.encode());brace=raw.index(b'{',start);depth=1;pos=brace+1
 while depth:
  if raw[pos]==123:depth+=1
  elif raw[pos]==125:depth-=1
  pos+=1
 return raw[start:pos]+b'\n'
names=['static BOOL IsValidIBBuffId(','static DWORD PackIBBuffIdentity(','static void UnpackIBBuffIdentity(',
 'static void PersistPlayerIBBuffStore(','static int FindIBBuffSlot(','static int CountIBBuffs(',
 'static int PhongThanPetHopeEligibility(','static int PhongThanPetHopeCount(',
 'static bool PhongThanPetHopeAdjustImpl(','static bool PhongThanPetHopeAdjust(',
 'static int AddIBBuffToStore(','static int RemoveIBBuffFromStore(','void RestorePhongThanIBBuffs(']
(OUT/'SpecialBuffStore.inl').write_bytes(b'\n'.join(function(name) for name in names))
book=(ROOT/'Sources/Core/Src/PhongThanEventHandbook.inl').read_bytes();start=book.index(b'static int PtHandbookGive(')
needle=b'if(e.genre==item_ibitem && e.detail==418 && e.part==0)return PhongThanPetHopeAdjust(player,quantity)?1:-8;'
end=book.index(needle,start)+len(needle)
(OUT/'SpecialBuffHandbook.inl').write_bytes(book[start:end]+b'\n return -99;\n}\n')
assert b'PtOriginalItemBuffTick(*this)' in (ROOT/'Sources/Core/Src/KPlayer.cpp').read_bytes()
assert b'PtOriginalItemBuffApply(*this)' in (ROOT/'Sources/Core/Src/KPlayer.cpp').read_bytes()
use=(ROOT/'Sources/Core/Src/KItemList.cpp').read_bytes();assert use.index(b'PhongThanUseOriginalBuffItem(m_PlayerIdx,nIdx)')<use.index(b'int consumableResult = PhongThanUseConsumable')
print('Prepared actual IBBuff persistence, hope eligibility, GM grant branch and item/stat hooks')
