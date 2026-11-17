"""Offline fixtures only: Studio edits must survive named-index rebuilds."""
import json
import struct
import tempfile
import unittest
from pathlib import Path
import build_offline_loose as b
import build_named_loose_index as index


class StudioIndexTests(unittest.TestCase):
    def fixture(self, base):
        root = Path(base) / 'PhongThanRuntime-Loose'
        (root / 'completion').mkdir(parents=True)
        for role in ('Client', 'Server'):
            (root / role / 'script').mkdir(parents=True)
            (root / role / 'script' / 'existing.lua').write_bytes(b'old')
            b.csv_write(root / 'completion' / (role + '_named_effective_manifest.csv'),
                        [dict(status='RESOLVED', path=r'script\existing.lua', id='12345678', sha256=b.sha(b'old'))],
                        ['status', 'path', 'id', 'sha256'])
        return root

    def test_edit_and_new_script_keep_original_id(self):
        with tempfile.TemporaryDirectory(prefix='StudioOverlay_') as base:
            root = self.fixture(base)
            (root / 'Server/script/existing.lua').write_bytes(b'edited')
            (root / 'Server/script/new.lua').write_bytes(b'new')
            changes = [dict(Side='Server', Path=r'script\existing.lua', Sha256=b.sha(b'edited'), Ids=[0x12345678]),
                       dict(Side='Server', Path=r'script\new.lua', Sha256=b.sha(b'new'), Ids=[])]
            (root / 'completion/studio_effective_overrides.json').write_text(json.dumps(changes), encoding='utf-8')
            index.run(root)
            raw = (root / 'Server/loose_index.bin').read_bytes()
            self.assertEqual(raw[:8], b'PTLOOSE1')
            count, = struct.unpack_from('<I', raw, 8)
            pos = 12
            entries = {}
            for _ in range(count):
                eid, length = struct.unpack_from('<IH', raw, pos)
                pos += 6
                entries[eid] = raw[pos:pos + length]
                pos += length
            self.assertEqual(entries[0x12345678], br'\script\existing.lua')
            self.assertEqual(entries[b.pak_id(br'\script\new.lua')], br'\script\new.lua')
            self.assertEqual(pos, len(raw))

    def test_untracked_external_edit_fails_closed(self):
        with tempfile.TemporaryDirectory(prefix='StudioOverlay_') as base:
            root = self.fixture(base)
            (root / 'completion/studio_effective_overrides.json').write_text(json.dumps([
                dict(Side='Client', Path=r'script\existing.lua', Sha256=b.sha(b'not current'), Ids=[0x12345678])]), encoding='utf-8')
            with self.assertRaisesRegex(ValueError, 'changed outside Studio'):
                index.run(root)


if __name__ == '__main__':
    unittest.main()
