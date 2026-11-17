"""Verify mount-affecting attribute IDs against the original client registry.

Reads the original PE without running it; catches both misleading tooltip
labels and server handlers being assigned to the wrong numerical attribute.
"""
from pathlib import Path
import re,struct
import pefile

root=Path(__file__).resolve().parents[2]
pe=pefile.PE(str(root/'Client_Original/game.dat'))
data=pe.get_memory_mapped_image();base=pe.OPTIONAL_HEADER.ImageBase
source=(root/'PhongThanSource/Sources/Core/Src/KMagicAttribRegistry.inc').read_text()
registry={int(i):(symbol,key) for i,symbol,key in re.findall(r'MAGIC_ATTRIB_ENTRY\((\d+), (\w+), "([^"]+)"\)',source)}
anchor=0x9caee4
assert data[struct.unpack_from('<I',data,anchor-base)[0]-base:][:14]==b'attackspeed_v\0'
for i in [86,111,114,115,126,*range(136,168)]:
    pointer=struct.unpack_from('<I',data,anchor+(i-115)*164-base)[0]
    original=data[pointer-base:pointer-base+100].split(b'\0')[0].decode('ascii')
    assert registry[i][1]==original,(i,registry[i][1],original)
assert registry[146]==('magic_deadlystrikeenhance_p','deadlystrikeenhance_p')
assert registry[151]==('magic_adddefense_p','adddefense_p')
assert registry[126][1]=='addphysicsdamage_p' # STCB remains unchanged.
assert len(set(symbol for symbol,key in registry.values()))==len(registry)
print('PASS: mount IDs and 136..167 registry permutation match original game.dat; STCB unchanged.')
