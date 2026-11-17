"""Wire the existing authoritative skill-book generator into NPC shops."""
from repair_npc_dialog_exit import change
from pathlib import Path
p=Path(__file__).resolve().parents[1]/'Sources/Core/Src/KBuySell.cpp'
data=p.read_bytes().replace(b'\r\n',b'\n')
data=data.replace(b'#include "PhongThanSkillBookKey.h"\n',b'')
data=data.replace(b'#include "KCore.h"',b'#include "KCore.h"\n#include "PhongThanSkillBookKey.h"',1)
if data.count(b'case item_skillbook:')>1:
    data=data.replace(b'\tcase item_skillbook:\n\t\treturn ItemGen.Gen_SkillBook(itemKey.nDetailType, itemKey.nParticularType, itemKey.nLevel, pItem);\n',b'')
p.write_bytes(data)
if b'case item_skillbook:' not in data:
    change('Sources/Core/Src/KBuySell.cpp','\tcase item_ibitem:\n\t\treturn ItemGen.Gen_IBItem',
        '\tcase item_skillbook:\n\t\treturn ItemGen.Gen_SkillBook(itemKey.nDetailType, itemKey.nParticularType, itemKey.nLevel, pItem);\n\tcase item_ibitem:\n\t\treturn ItemGen.Gen_IBItem')
print('NPC shops now generate skill books with their exact detail + skill ID.')
change('Sources/Core/Src/KBuySell.cpp',
       'ItemGen.Gen_SkillBook(itemKey.nDetailType, itemKey.nParticularType, itemKey.nLevel, pItem);',
       'ItemGen.Gen_SkillBook(itemKey.nDetailType, PhongThanCurrentSkillBookKey(itemKey.nDetailType,itemKey.nParticularType), itemKey.nLevel, pItem);')
