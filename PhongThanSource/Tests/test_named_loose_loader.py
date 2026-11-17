import os
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parents[1]/'Tools'))
import build_offline_loose as b
import build_named_loose_index as index

PROBE=Path(__file__).parents[1]/'Output/Tools/LooseLoaderProbe.exe'


class NamedLooseTests(unittest.TestCase):
    def test_index_paths_collision_and_scope(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)/'PhongThanRuntime-Loose';(root/'completion').mkdir(parents=True)
            for role in ('Client','Server'):
                folder=root/role/'settings';folder.mkdir(parents=True)
                (folder/'a.ini').write_bytes(b'[T]\nValue=original\n')
                b.csv_write(root/'completion'/(role+'_named_effective_manifest.csv'),[
                    dict(path='settings\\a.ini',id='12345678',status='RESOLVED',sha256=b.sha(b'original'))],
                    ['path','id','status','sha256'])
            result=index.run(root)
            self.assertEqual(result['Client']['ids'],2)
            data=(root/'Client/loose_index.bin').read_bytes()
            self.assertEqual(data[:8],b'PTLOOSE1')
            with self.assertRaises(ValueError):index.run(root/'Client')

    @unittest.skipUnless(PROBE.is_file(),'Build-LooseLoaderProbe.ps1 required')
    def test_real_engine_no_fallback_and_invalid_indices(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);raw=b'\\settings\\test.ini';eid=b.pak_id(raw)
            payload=b'[Test]\nValue=LOOSE\n';packed=b'[Test]\nValue=PACKED\n'
            (root/'settings').mkdir();(root/'settings/test.ini').write_bytes(payload)
            indexbytes=b'PTLOOSE1'+struct.pack('<IIH',1,eid,len(raw))+raw
            (root/'loose_index.bin').write_bytes(indexbytes)
            (root/'loose_only.flag').write_bytes(b'PT_LOOSE_ONLY_V1\n')
            (root/'package.ini').write_bytes(b'[Package]\nPath=\\data\n0=conflict.pak\n')
            (root/'data').mkdir()
            pak=struct.pack('<4sIIIII8s',b'PACK',1,32,48,0,0,b'\0'*8)
            pak+=struct.pack('<IIII',eid,48,len(packed),len(packed))+packed
            (root/'data/conflict.pak').write_bytes(pak)
            def probe(mode):
                result=subprocess.run([str(PROBE),str(root),raw.decode('ascii'),mode],capture_output=True,text=True)
                self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            probe('loose')
            self.assertIn('no_pak_fallback=1',(root/'loose_missing_files.log').read_text())
            for bad in (b'bad',indexbytes[:-1],indexbytes+b'extra',
                        b'PTLOOSE1'+struct.pack('<IIH',1,eid,9)+b'\\..\\x.ini',
                        b'PTLOOSE1'+struct.pack('<IIH',1,eid,9)+b'\\x\0xx.ini'):
                (root/'loose_index.bin').write_bytes(bad);probe('reject')
            (root/'loose_only.flag').unlink()
            (root/'settings/test.ini').unlink()
            probe('pak')


if __name__=='__main__':unittest.main()
