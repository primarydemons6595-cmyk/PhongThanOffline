from pathlib import Path
import hashlib,json,sys
ROOT=Path(__file__).resolve().parents[2]
PACKAGE=ROOT/'PhongThanSource/Deploy/ProjectContent/F2Requested20261031'
manifest=json.loads((PACKAGE/'manifest.json').read_text('utf8'))
base=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else PACKAGE/'compiled'
for f in manifest['files']:
    assert hashlib.sha256((base/f['path']).read_bytes()).hexdigest()==f['sha256'],f['path']
for role in ['Server','Client']:
    def read(rel): return (base/role/rel).read_bytes()
    rel='settings/item/001/ibitem.txt'
    rows={tuple(c[1:4]):c for c in (l.split(b'\t') for l in read(rel).splitlines()[1:]) if len(c)>13}
    original={tuple(c[1:4]):c for c in (l.split(b'\t') for l in (PACKAGE/'source'/role/rel).read_bytes().splitlines()[1:]) if len(c)>13}
    targets={str(i).encode() for i in manifest['gifts']}|{b'30',b'391',b'9444'}
    for key,before in original.items():
        after=rows[key]
        if key[1] in targets and key[2]==b'2':
            assert all(a==b for n,(a,b) in enumerate(zip(before,after)) if n not in [8,12])
        else: assert before==after,key
    assert rows[(b'8',b'9444',b'2')][9]==b'1'
    assert rows[(b'8',b'9445',b'0')][14]==b'604800'
    for detail,uses in [(30,10),(258,10),(330,5),(334,1),(133,1)]:
        part=b'2' if detail in [30,258] else b'0'
        assert int(rows[(b'8',str(detail).encode(),part)][9])==uses
    goods=read('settings/ibshop/ibshopgoods.txt').splitlines()[1:]
    matches=[n for n,l in enumerate(goods) if l.split(b'\t')[1:4]==[b'8',b'9444',b'2']]
    assert len(matches)==1
    assert goods[matches[0]].split(b'\t')[7:9]==[b'10',b'10']
    category=read('settings/ibshop/micsgoods.txt').splitlines()[1:]
    assert str(matches[0]).encode() in [l.split(b'\t')[0] for l in category]
    assert any(l.split(b'\t')[1:4]==[b'8',b'1408',b'2'] for l in goods)
    for detail in [295,213,288,287,448,449,1408,285,286,133,258,30,334,330]:
        assert any(l.split(b'\t')[1:3]==[b'8',str(detail).encode()] for l in goods)
    effects=[l.split(b'\t') for l in read('settings/phongthan/ConsumableEffects.txt').splitlines()[1:]]
    for detail,seconds in [(133,3600),(330,3600),(334,86400),(782,3600)]:
        effect=next(c for c in effects if c[0]==str(detail).encode())
        assert effect[2:4]==[b'5',str(seconds).encode()]
        assert ('[BuffInfo_%d]'%detail).encode() in (base/'Client/Ui/ui3/PtNativeTimedItems.ini').read_bytes()
source=(ROOT/'PhongThanSource/Sources/Core/Src/KPlayer.cpp').read_bytes()
assert b'[BuffInfo_9445]' in (base/'Client/Ui/ui3/PtConsumableBuff.ini').read_bytes()
for signature in [b'void\tKPlayer::AddSelfExp(',b'void\tKPlayer::DirectAddExp(']:
    start=source.index(signature)
    assert b'if(nExp>0 && PtOriginalExperienceLocked(*this))return;' in source[start:start+230]
assert b'if GetMorphType()==364 or IsPlayerInsideWeapon' not in (base/'Server/script/phongthan/travel_items.lua').read_bytes()
print('PASS content: %d hashes, original unrelated rows preserved, exact use counts, original seven-day lock/buff, F2 product/category, four timed buffs and both character-XP entry guards'%len(manifest['files']))
