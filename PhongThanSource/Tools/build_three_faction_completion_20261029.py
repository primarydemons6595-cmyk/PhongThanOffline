"""Stage quest additions against current Loose bytes; do not modify the runtime."""
import hashlib
import json
import re
import subprocess
from pathlib import Path

from extract_client_coordinate_dictionary import encode_tcvn3

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'PhongThanSource'
RUNTIME = ROOT / 'PhongThanRuntime-Loose'
PACKAGE = SOURCE / 'Deploy/ProjectContent/ThreeFactionQuests20261029'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    changed = {}
    counts = dict(giap_menu=0, two_menu=0, giap_marker=0, two_marker=0)
    giap_old = b'if PTGReady(a) and (getn(a.needs)==0 or PTGHave(a.needs)==1) then'
    two_old = b'if PT2Ready(a) and PT2Have(a.needs)==1 then tinsert(options'
    giap_marker = b'    return best,level\nend\nfunction PTGEvent'
    giap_new_marker = (b'    if PTG_OLD_MARK then\n        local state,oldLevel=PTG_OLD_MARK()\n'
        b'        if state and state>best then return state,oldLevel end\n    end\n'
        b'    return best,level\nend\nfunction PTGEvent')
    two_marker = b' if best==0 and PT2_OLD_MARK then return PT2_OLD_MARK() end'
    two_new_marker = (b' if PT2_OLD_MARK then\n  local state,oldLevel=PT2_OLD_MARK()\n'
        b'  if state and state>best then return state,oldLevel end\n end')
    candidates = subprocess.check_output(['rg','-l','--no-ignore','--glob','*.lua','-F','-e','PTGReady(a)','-e','PT2Ready(a)',str(RUNTIME / 'Server/script')]).decode('utf-8').splitlines()
    for path in map(Path, candidates):
        old = path.read_bytes()
        data = old.replace(b'\r\n', b'\n')
        original = data
        for key, before, after in (
            ('giap_menu', giap_old, b'if PTGReady(a) then'),
            ('two_menu', two_old, b'if PT2Ready(a) then tinsert(options'),
            ('giap_marker', giap_marker, giap_new_marker),
            ('two_marker', two_marker, two_new_marker),
        ):
            count = data.count(before)
            counts[key] += count
            data = data.replace(before, after)
        if data != original:
            changed[('Server', path.relative_to(RUNTIME / 'Server').as_posix())] = data
    assert all(counts.values()), counts

    audit = json.loads((ROOT / 'analysis/two_faction_quests_20261027/audit.json').read_text('utf-8'))
    vantrung = next(e['runtime_path'] for e in audit['scripts'] if e['path'].endswith('玉虚宫\\云中子.lua'))
    books = [
        dict(f=0, map=1002, level=6, task=1035, id=3160, detail=24, skill=27, gifts=[],
             path='script/phongthan/npc_restore/1002_00.lua', npc='Tô Hộ', book='Tế Huyết Trảm',
             reference='7269c8d467302031a5b211d2ba7841d738b2049d4dbff5f27ff6984d1cba7937'),
        dict(f=1, map=1003, level=3, task=1073, id=3161, detail=0, skill=3, gifts=[[7,1,4,0,0,0,1]],
             path=vantrung, npc='Vân Trung Tử', book='Chưởng Tâm Lôi',
             reference='84d9360d5fb5df66bcf2ec51e52ee3dacf836e5e756ac274a318b00de23aeba1'),
        dict(f=2, map=1004, level=4, task=1090, id=3162, detail=49, skill=450, gifts=[],
             path='script/phongthan/npc_restore/1004_00.lua', npc='Thiếu Hạo', book='Lực Sĩ Tế',
             reference='e16dfc06f00f4d444bfb6c90aec334f643967fc362d2edc0760a7ffbd2c0d26b'),
    ]
    library = json.loads((SOURCE / 'Docs/NPC_LUA_LIBRARY.json').read_text('utf-8'))
    provenance = []
    body = (SOURCE / 'Tools/three_faction_book_quest.lua').read_text('utf-8')
    for book in books:
        evidence = next(e for e in library['entries'] if e['evidence']['sha256'] == book['reference'])
        raw = (SOURCE / 'SourceMigration/NpcLuaLibrary' / evidence['object']).read_bytes()
        assert sha(raw) == book['reference']
        provenance.append(dict(quest=book, source=evidence))
        data = {k: book[k] for k in ('f','map','level','task','id','detail','skill','gifts')}
        data['note'] = ('<color=yellow>Cần Mẫn<color><enter>Đến Võ Sư mua sách <color=yellow>' +
            book['book'] + '<color>, rồi mang về cho <color=green>' + book['npc'] +
            '<color> xem. Có thể học sách trước khi phục mệnh. Giữ sách sau khi hoàn thành.<enter>' +
            '<color=green>Phần thưởng: 400 kinh nghiệm' +
            (' và sách Lưu Tinh Thạch' if book['f'] == 1 else '') + '.<color>')
        # Lua 4 has no JSON decoder; produce its literal data table explicitly.
        gifts = '{' + ','.join('{' + ','.join(map(str, row)) + '}' for row in data['gifts']) + '}'
        fields = ','.join(k + '=' + str(v) for k, v in data.items() if isinstance(v, int))
        literal = '{' + fields + ',gifts=' + gifts + ',note=' + json.dumps(data['note'], ensure_ascii=False) + '}'
        key = ('Server', book['path'])
        base = changed.get(key, (RUNTIME / key[0] / key[1]).read_bytes())
        assert b'-- PT_THREE_FACTION_BOOK_BEGIN' not in base
        changed[key] = base + encode_tcvn3('\n-- PT_THREE_FACTION_BOOK_BEGIN\nPT3_BOOK=' + literal + '\n' + body)

    names = ('Giáp Sĩ', 'Đạo Sĩ', 'Dị Nhân')
    for scheme in ('ui3','ui4'):
        relative = f'Ui/{scheme}/taskinfo.ini'
        data = (RUNTIME / 'Client' / relative).read_bytes()
        for book in books:
            assert ('[Task_%d]' % book['id']).encode() not in data
            data += encode_tcvn3('\r\n[Task_%d]\r\nTitle=$ Cần Mẫn - %s\r\n' % (book['id'], names[book['f']]))
        changed[('Client', relative)] = data
    files = []
    for (role, relative), data in sorted(changed.items()):
        target = PACKAGE / 'compiled' / role / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
        files.append(dict(role=role, relative=relative, sha256=sha(data), before=sha((RUNTIME / role / relative).read_bytes())))
    (PACKAGE / 'manifest.json').write_text(json.dumps(dict(schema=1, files=files), ensure_ascii=False, indent=2)+'\n','utf-8')
    (PACKAGE / 'provenance.json').write_text(json.dumps(dict(books=provenance, changes=counts), ensure_ascii=False, indent=2)+'\n','utf-8')
    print(json.dumps(dict(files=len(files), changes=counts, books=len(books))))


if __name__ == '__main__':
    main()
