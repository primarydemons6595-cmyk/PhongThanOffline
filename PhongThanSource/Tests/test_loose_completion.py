import sys
import json
import tempfile
import struct
import argparse
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parents[1] / 'Tools'))
import complete_offline_loose as m
import build_offline_loose as b
import build_loose_id_store as ids
import finalize_named_loose as named


class CompletionTests(unittest.TestCase):
    def test_named_inventory_fail_closed(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);out=root/'out';report=out/'completion';report.mkdir(parents=True)
            recovery=root/'PakRecoveryWork';reports=recovery/'05_Reports';reports.mkdir(parents=True)
            runtime=root/'PhongThanRuntime-Staging';runtime.mkdir()
            (out/'summary.json').write_text(json.dumps(dict(input_runtime=str(runtime),
                input_recovery=str(recovery),roles={'Server':{}})),encoding='utf-8')
            name='script\\known.lua';eid='%08X'%b.pak_id(name.encode('ascii'))
            b.csv_write(out/'Server_manifest.csv',[dict(path=name,status='BLOCKED',id='')],['path','status','id'])
            for suffix in ('additional_names','historical_restoration','source_contract'):
                b.csv_write(report/('Server_'+suffix+'.csv'),[],['path','status'])
            folder=recovery/'04_Unresolved/oracle';folder.mkdir(parents=True)
            file=folder/('ID_'+eid+'.lua');file.write_bytes(b'original')
            entry=dict(IdHex=eid,Status='OK',SHA256=b.sha(b'original'),PakName='oracle.pak',
                ExtractedPath=file.name,EntryIndex=1)
            fields=list(entry)
            inventory=reports/'PAK_INVENTORY.csv'
            b.csv_write(inventory,[entry],fields)
            result=named.run(out,root,False)
            self.assertEqual(result['roles']['Server']['inventory_additions'],1)
            target=out/'Server/script/known.lua'
            self.assertFalse(target.exists())
            result=named.run(out,root,True,True)
            self.assertEqual(result['roles']['Server']['sha_verified_files'],1)
            self.assertEqual(target.read_bytes(),b'original')
            result=named.run(out,root,True,False,True)
            self.assertEqual(result['roles']['Server']['sha_verified_files'],1)
            self.assertEqual(result['roles']['Server']['verification_scope'],'SUPPLEMENTS_ONLY')
            target.write_bytes(b'changed')
            with self.assertRaises(ValueError):named.run(out,root,True)
            # Multiple original versions cannot be selected by directory order.
            other=dict(entry,SHA256=b.sha(b'other'))
            b.csv_write(inventory,[entry,other],fields)
            result=named.run(out,root,False)
            self.assertEqual(result['roles']['Server']['inventory_additions'],0)
            blocked=list(b.csv_read(report/'Server_named_unresolved.csv'))
            self.assertEqual(blocked[0]['error'],'AMBIGUOUS_HISTORICAL_VERSION')

    def test_c_bytes_and_ansi(self):
        self.assertEqual(m.c_bytes(rb'\\settings\\npcres\\\xC8\xCB.txt'), b'\\settings\\npcres\\\xc8\xcb.txt')
        self.assertEqual(m.c_bytes(rb'\101\x42\\C'), b'AB\\C')
        for bad in (rb'\x', rb'\x100', rb'\777', rb'\q'):
            with self.assertRaises(ValueError): m.c_bytes(bad)
        raw, name = m.raw_path(b'\\spr\\\xc8\xcb.spr')
        self.assertEqual(name.encode('mbcs'), raw.lstrip(b'\\'))
        for bad in (b'\\spr\\%s.spr', b'\\spr\\..\\a.spr', b'\\settings\\folder'):
            with self.assertRaises(ValueError): m.raw_path(bad)

    def test_recovery_and_no_overwrite(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp); rt=root/'PhongThanRuntime-Staging'; recovery=root/'PakRecoveryWork'
            reports=recovery/'05_Reports';reports.mkdir(parents=True)
            src=root/'PhongThanSource/Sources';src.mkdir(parents=True)
            original=b'const char* x="\\\\settings\\\\new.txt"; // \xc8\xcb\n'
            (src/'example.cpp').write_bytes(original)
            old=recovery/'03_RecoveredLoose/script/old.lua';old.parent.mkdir(parents=True);old.write_bytes(b'authentic')
            b.csv_write(reports/'PATH_DICTIONARY.csv', [], ['CanonicalPath','OriginalSpelling','ReferencedBy'])
            oldid=b.pak_id(b'\\script\\old.lua')
            b.csv_write(reports/'EXACT_MAPPINGS.csv', [dict(OutputPath='script\\old.lua', HashSpelling='\\script\\old.lua',
                IdHex='%08X'%oldid,SHA256=b.sha(b'authentic'),PakName='old.pak',EntryIndex='2')],
                ['OutputPath','HashSpelling','IdHex','SHA256','PakName','EntryIndex'])
            eid=b.pak_id(b'\\settings\\new.txt');out=root/'out';out.mkdir()
            summary=dict(input_runtime=str(rt),input_recovery=str(recovery),roles={})
            for role in ('Server','Client'):
                r=rt/role;(r/'data').mkdir(parents=True)
                (r/'package.ini').write_text('[Package]\nPath=\\data\n0=a.pak\n')
                (r/'data/a.pak').write_bytes(struct.pack('<4sIIIII8s',b'PACK',1,32,48,0,0,b'\0'*8)+
                    struct.pack('<IIII',eid,48,7,7)+b'current')
                b.csv_write(out/(role+'_manifest.csv'),[dict(path='script\\old.lua',status='BLOCKED',sha256='')],
                    ['path','status','sha256'])
                summary['roles'][role]={}
            (out/'summary.json').write_text(json.dumps(summary))
            args=argparse.Namespace(output=out,workspace=root,materialize=False)
            plan=m.run(args)
            self.assertEqual(plan['roles']['Server']['historical_known_paths'],1)
            self.assertFalse((out/'Server/script/old.lua').exists())
            args.materialize=True
            result=m.run(args)
            self.assertEqual(result['roles']['Client']['materialized'],2)
            self.assertEqual((out/'Server/settings/new.txt').read_bytes(),b'current')
            self.assertEqual((out/'Server/script/old.lua').read_bytes(),b'authentic')
            (out/'Client/settings/new.txt').write_bytes(b'user edit')
            result=m.run(args)
            self.assertEqual(result['roles']['Client']['materialize_errors'],1)
            self.assertEqual((out/'Client/settings/new.txt').read_bytes(),b'user edit')
            self.assertEqual((src/'example.cpp').read_bytes(), original)
            stored=ids.run(args)
            self.assertEqual(stored['roles']['Server']['resolved'],1)
            self.assertEqual(stored['roles']['Client']['blocked'],0)
            file=out/'Client/_loose_id'/('%08X'%eid)[:2]/('%08X.bin'%eid)
            self.assertEqual(file.read_bytes(),b'current')
            self.assertEqual((out/'Server/_loose_id/READY').read_bytes(),b'PT_LOOSE_ID_V1\n')
            file.write_bytes(b'changed ID')
            # Hard-linked aliases intentionally share edits. Verification must
            # refuse them, never silently replace caller-owned bytes.
            stored=ids.run(args)
            self.assertGreater(stored['roles']['Client']['blocked'],0)
            self.assertFalse((out/'Client/_loose_id/READY').exists())


if __name__=='__main__': unittest.main()
