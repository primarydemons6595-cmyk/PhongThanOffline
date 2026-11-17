import importlib.util
import struct
import tempfile
import unittest
import json
from pathlib import Path

spec = importlib.util.spec_from_file_location('loose', Path(__file__).parents[1] / 'Tools/build_offline_loose.py')
m = importlib.util.module_from_spec(spec)
spec.loader.exec_module(m)


def make_pak(path, payload, eid=1, method=0):
    path.write_bytes(struct.pack('<4sIIIII8s', b'PACK', 1, 32, 48, 0, 0, b'\0'*8) +
                     struct.pack('<IIII', eid, 48, len(payload), (method << 24) | len(payload)) + payload)


class OfflineLooseTests(unittest.TestCase):
    def test_hash_and_path(self):
        self.assertEqual(m.pak_id(b'\\settings\\summonskill.txt'), 0x27955344)
        self.assertEqual(m.pak_id(b'/SETTINGS/summonskill.txt'), 0x27955344)
        for path in ('../bad', 'C:\\bad', '\\script\\..\\bad', 'ui\\x:ads', 'ui\\CON.txt', 'ui\\bad?.ini'):
            with self.assertRaises(ValueError):
                m.logical(path)

    def test_priority_decode_and_bounds(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            a, b = root / 'a.pak', root / 'b.pak'
            make_pak(a, b'first')
            make_pak(b, b'later')
            winners, entries = m.pak_index([a, b])
            self.assertEqual(len(entries), 2)
            self.assertEqual(winners[1]['pak'], a)
            self.assertEqual(m.entry_payload(winners[1], {}, root)[0], b'first')
            stale = {('a.pak', 0): {'SHA256': 'wrong', 'IdDecimal': '999'}}
            self.assertEqual(m.entry_payload(winners[1], stale, root)[0], b'first')
            make_pak(a, b'bad', method=0x11)
            winners, _ = m.pak_index([a])
            with self.assertRaises(ValueError):
                m.entry_payload(winners[1], {}, root)
            a.write_bytes(struct.pack('<4sII', b'PACK', 999, 12))
            with self.assertRaises(ValueError):
                m.pak_index([a])

    def test_native_duplicate_midpoint(self):
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / 'a.pak'
            path.write_bytes(struct.pack('<4sIIIII8s', b'PACK', 3, 32, 80, 0, 0, b'\0'*8) + b''.join(
                struct.pack('<IIII', 1, 80 + n, 1, 1) for n in range(3)) + b'abc')
            winners, _ = m.pak_index([path])
            self.assertEqual(winners[1]['entry'], 1)
            self.assertEqual(m.entry_payload(winners[1], {}, Path(d))[0], b'b')

    def test_native_invalid_archive_fallback(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            a, b = root / 'a.pak', root / 'b.pak'
            make_pak(a, b'bad', eid=0)
            make_pak(b, b'good', eid=1)
            warnings = []
            winners, _ = m.pak_index([a, b], strict=False, diagnostics=warnings)
            self.assertEqual(winners[1]['pak'], b)
            self.assertEqual(warnings, [{'pak': 'a.pak', 'error': 'NATIVE_OPEN_ENTRY_REJECT'}])

    def test_native_unsorted_search_fallback(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            a, b = root / 'a.pak', root / 'b.pak'
            a.write_bytes(struct.pack('<4sIIIII8s', b'PACK', 3, 32, 80, 0, 0, b'\0'*8) + b''.join(
                struct.pack('<IIII', eid, 80 + n, 1, 1) for n, eid in enumerate((2, 1, 3))) + b'abc')
            make_pak(b, b'fallback', eid=2)
            winners, _ = m.pak_index([a, b])
            self.assertEqual(winners[1]['pak'], a)
            self.assertEqual(winners[3]['pak'], a)
            self.assertEqual(winners[2]['pak'], b)

    def test_nrv_corrupt(self):
        for blob in (b'', b'\xff', b'\0' * 20):
            with self.assertRaises(ValueError):
                m.nrv2b(blob, 2)

    def test_overlap_copy_equivalence(self):
        for off in range(1, 9):
            for count in range(1, 40):
                a = bytearray(b'abcdefgh')
                b = bytearray(a)
                for _ in range(count):
                    a.append(a[-off])
                m.copy_match(b, off, count)
                self.assertEqual(a, b)
        for off in (1, 7, 65535, 70000):
            for count in (2, 65536, 150000):
                a = bytearray(bytes(range(256)) * 300)
                b = bytearray(a)
                for _ in range(count):
                    a.append(a[-off])
                m.copy_match(b, off, count)
                self.assertEqual(a, b)

    def test_frame_sprite_contract(self):
        header = struct.pack('<4s14H', b'SPR\0', 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0)
        for palette, pixels in ((True, b'\1\xff\0'), (False, b'\1\xff\xff\0\0')):
            body = struct.pack('<4H', 1, 1, 0, 0) + pixels
            pal = b'\xff\0\0' if palette else b''
            packed = header + pal + struct.pack('<ii', len(body), -len(body)) + body
            blob, how = m.frame_sprite(packed, len(packed))
            self.assertEqual(blob[-len(body):], body)
            self.assertEqual(how, 'PAK_FRAME_EXPANDED_INDEXED' if palette else 'PAK_FRAME_EXPANDED_RGB')
            self.assertTrue(m.sprite_blob_valid(blob, palette))
            self.assertEqual(m.frame_sprite(packed, len(packed)+768)[0], blob)
            with self.assertRaises(ValueError):
                m.frame_sprite(packed[:-1], len(packed))

    def test_retry_frames_only(self):
        import argparse
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            recovery = root / 'PakRecoveryWork'
            reports = recovery / '05_Reports'
            reports.mkdir(parents=True)
            (root / 'PhongThanSource/Sources').mkdir(parents=True)
            (root / 'PhongThanSource/Sources/example.cpp').write_text('const char* p="\\\\spr\\\\tiny.spr";')
            m.csv_write(reports / 'EXACT_MAPPINGS.csv', [], ['CanonicalPath'])
            m.csv_write(reports / 'UNRESOLVED_IDS.csv', [], ['PakName', 'IdHex', 'SHA256'])
            header = struct.pack('<4s14H', b'SPR\0', 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0)
            body = struct.pack('<4H', 1, 1, 0, 0) + b'\1\xff\0'
            packed = header + b'\xff\0\0' + struct.pack('<ii', len(body), -len(body)) + body
            eid = m.pak_id(b'\\spr\\tiny.spr')
            for role in ('Server', 'Client'):
                rt = root / 'PhongThanRuntime-Staging' / role
                (rt / 'data').mkdir(parents=True)
                (rt / 'package.ini').write_text('[Package]\nPath=\\data\n0=a.pak\n')
                make_pak(rt / 'data/a.pak', packed, eid, method=0x11)
            args = argparse.Namespace(workspace=root, runtime=None, recovery=None, output=root / 'out', materialize=True)
            summary = m.run(args)
            for role in ('Server', 'Client'):
                rows = list(m.csv_read(args.output / (role + '_manifest.csv')))
                rows[0].update(status='BLOCKED', bytes=0, sha256='')
                m.csv_write(args.output / (role + '_manifest.csv'), rows, list(rows[0]))
                m.csv_write(args.output / (role + '_blocked.csv'), [{'path':'spr\\tiny.spr', 'error':'Unsupported compression: 11'}], ['path','error'])
                m.csv_write(args.output / (role + '_unknown_ids.csv'), [{'id':'%08X'%eid, 'pak':'a.pak','entry':0,'bytes':len(packed)}], ['id','pak','entry','bytes'])
                m.csv_write(args.output / (role + '_source_gaps.csv'), list(m.csv_read(args.output / 'source_references.csv')), ['path','caller','line','kind'])
                (args.output / role / 'spr/tiny.spr').unlink()
                summary['roles'][role].update(resolved=0, blocked=1, bytes=0, unknown_winning_ids=1, source_gaps=1)
            (args.output / 'summary.json').write_text(json.dumps(summary))
            repaired = m.retry_frame_exports(args)
            for role in ('Server', 'Client'):
                self.assertEqual(repaired['roles'][role]['frame_retry_resolved'], 1)
                self.assertEqual(repaired['roles'][role]['blocked'], 0)
                self.assertEqual(repaired['roles'][role]['unknown_winning_ids'], 0)
                self.assertEqual(repaired['roles'][role]['source_gaps'], 0)
                self.assertTrue(m.sprite_blob_valid((args.output / role / 'spr/tiny.spr').read_bytes(), True))

    def test_output_protection(self):
        import argparse
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            for output in (root, root / 'PhongThanRuntime-Staging' / 'out', root / 'PhongThanSource'):
                args = argparse.Namespace(workspace=root, runtime=None, recovery=None, output=output, materialize=False)
                with self.assertRaises(ValueError):
                    m.run(args)

    def test_end_to_end_export_precedence(self):
        import argparse
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            recovery = root / 'PakRecoveryWork'
            reports = recovery / '05_Reports'
            reports.mkdir(parents=True)
            (root / 'PhongThanSource/Sources').mkdir(parents=True)
            (root / 'PhongThanSource/Sources/example.cpp').write_text('const char* p="\\\\settings\\\\example.txt";')
            m.csv_write(reports / 'PAK_INVENTORY.csv', [], ['PakName', 'EntryIndex'])
            m.csv_write(reports / 'EXACT_MAPPINGS.csv', [], ['CanonicalPath'])
            m.csv_write(reports / 'UNRESOLVED_IDS.csv',
                [{'PakName': 'a.pak', 'IdHex': '%08X' % m.pak_id(b'\\settings\\example.txt'), 'SHA256': m.sha(b'old')}],
                ['PakName', 'IdHex', 'SHA256'])
            for role in ('Client', 'Server'):
                runtime = root / 'PhongThanRuntime-Staging' / role
                (runtime / 'data').mkdir(parents=True)
                (runtime / 'settings').mkdir()
                (runtime / 'settings/example.txt').write_bytes(b'loose')
                (runtime / 'package.ini').write_text('[Package]\nPath=\\data\n0=a.pak\n1=b.pak\n')
                eid = m.pak_id(b'\\settings\\example.txt')
                make_pak(runtime / 'data/a.pak', b'first', eid)
                make_pak(runtime / 'data/b.pak', b'later', eid)
            args = argparse.Namespace(workspace=root, runtime=None, recovery=None,
                                      output=root / 'export', materialize=True)
            result = m.run(args)
            self.assertFalse(result['pak_free_ready'])
            self.assertEqual(result['roles']['Server']['source04_mapped'], 1)
            self.assertEqual(result['roles']['Client']['source04_mapped'], 1)
            loose_proof = list(m.csv_read(root / 'export/Client_source04_mappings.csv'))[0]
            self.assertEqual(loose_proof['historical04_payload'], 'DIFFERENT_VERSION_CURRENT_RUNTIME_LOOSE_USED')
            proof = list(m.csv_read(root / 'export/Server_source04_mappings.csv'))[0]
            self.assertEqual(proof['historical04_payload'], 'DIFFERENT_VERSION_CURRENT_PAK_USED')
            self.assertEqual((root / 'export/Client/settings/example.txt').read_bytes(), b'loose')
            self.assertEqual((root / 'export/Server/settings/example.txt').read_bytes(), b'first')
            self.assertFalse(list((root / 'export').rglob('*.pak')))
            with self.assertRaises(ValueError):
                m.run(args)  # Never overwrite a previous export.
            args.resume = True
            resumed = m.run(args)
            self.assertEqual(resumed['roles']['Server']['resolved'], 1)
            (root / 'export/Client/settings/example.txt').write_bytes(b'tampered')
            with self.assertRaises(ValueError):
                m.run(args)
            (root / 'export/Client/settings/example.txt').write_bytes(b'loose')
            args.resume = False
            args.from_plan = root / 'export'
            args.output = root / 'from_plan'
            exported = m.export_plan(args)
            self.assertEqual(exported['exported_files'], {'Server': 1, 'Client': 1})
            self.assertEqual(exported['export_failures'], 0)
            self.assertTrue((args.output / 'Server_source04_mappings.csv').exists())
            (root / 'PhongThanRuntime-Staging/Client/settings/example.txt').write_bytes(b'changed')
            args.output = root / 'changed_snapshot'
            failed = m.export_plan(args)
            self.assertEqual(failed['export_failures'], 1)
            self.assertFalse((args.output / 'Client/settings/example.txt').exists())


if __name__ == '__main__':
    unittest.main()
