import importlib.util
import struct
import unittest
from pathlib import Path

workspace = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('yenson_builder',
                                              workspace / 'PhongThanSource/Tools/build_yenson_population.py')
builder = importlib.util.module_from_spec(spec)
spec.loader.exec_module(builder)


class YensonTests(unittest.TestCase):
    def test_short_header_rejected(self):
        with self.assertRaises(ValueError):
            builder.sections(b'')

    def test_section_bounds_rejected(self):
        data = bytearray(52)
        struct.pack_into('<I', data, 0, 6)
        struct.pack_into('<II', data, 4, 0, 1)
        with self.assertRaises(ValueError):
            builder.sections(data)

    def test_empty_region_not_walkability_evidence(self):
        data = bytearray(52)
        struct.pack_into('<I', data, 0, 6)
        self.assertEqual(builder.sections(data)[0], b'')

    def test_plan_scoped_bounded_and_unique(self):
        _, plan, report = builder.build(workspace)
        self.assertEqual(report['classification'], 'AUTHORED_RECONSTRUCTION')
        self.assertFalse(report['original_placement'])
        self.assertEqual(len(plan), 398)
        self.assertEqual(len({p['relative'] for p in plan}), len(plan))
        self.assertEqual(len({(p['x'], p['y']) for p in plan}), len(plan))
        for p in plan:
            self.assertIn(p['template'], (0, 7))
            self.assertTrue(p['relative'].startswith('settings/phongthan/npc_regions/1007/'))
            self.assertEqual(len(p['payload']), 72)
            self.assertEqual(struct.unpack_from('<III', p['payload']), (1, 0, 0))
            self.assertEqual(p['x'] % 32, 16)
            self.assertEqual(p['y'] % 32, 16)


if __name__ == '__main__':
    unittest.main()
