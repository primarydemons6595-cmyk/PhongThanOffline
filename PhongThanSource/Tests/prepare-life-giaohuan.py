from pathlib import Path
import json
root=Path(__file__).resolve().parents[1];work=root.parent/'analysis/life_giaohuan_20261030';work.mkdir(exist_ok=True)
content=root/'Deploy/ProjectContent/LifeGiaoHuan20261030';manifest=json.loads((content/'manifest.json').read_text('utf8'))
bootstrap=(root/'Tests/fixtures/giao-huan-merchant.lua').read_bytes()
for mapid,faction,relative,home in manifest['merchants']:
    raw=(content/'compiled/Server'/relative).read_bytes()
    module=raw.split(b'-- PT_GIAO_HUAN_MERCHANT_20261030\n')[1]
    (work/f'giao-{mapid}-{Path(relative).stem}.lua').write_bytes(bootstrap+b'\n'+module+b'\nTestGiaoMerchant()\n')
for mid in [1003,1004]:
    raw=(content/f'compiled/Server/script/phongthan/npc_restore/{mid}_15.lua').read_bytes()
    assert b'PT2_ROLE=' not in raw and b'pt2_giao_take' not in raw
source=(root/'Sources/GameClient/Ui/ShortcutKey.cpp').read_bytes()
start=source.index(b'char *l_WindowList[]');end=source.index(b'bool UiCloseWndsInGame(',start)
(root/'Output/Tools/LifeShortcutLookup.inl').write_bytes(source[start:end])
assert b'case 22:\n                PhongThanToggleLifeSkills();' in source
autoexec=(root.parent/'PhongThanRuntime-Loose/Client/Ui/autoexec.lua').read_bytes()
assert b'AddCommand("L", "", "Open([[liveSkill]])")' in autoexec
print('PASS merchant fixtures, teacher quest removal and actual L wiring prepared')
