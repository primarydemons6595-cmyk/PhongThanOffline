"""Targeted recovery of logged names and one source-proven ID dependency."""
import argparse
import json
import re
from collections import Counter
from pathlib import Path
import build_offline_loose as b
import build_named_loose_index as index


def run(workspace):
    root = workspace.resolve() / 'PhongThanRuntime-Loose'
    records = []
    original = workspace / 'Client_Original'
    listed = [original / 'data' / line.split('=', 1)[1].strip()
              for line in (original / 'package.ini').read_text('ascii').splitlines()
              if line.split('=', 1)[0].isdigit()]
    original_winners, _ = b.pak_index([p for p in listed if p.is_file()], strict=False)
    for role in ('Client', 'Server'):
        staging = workspace / 'PhongThanRuntime-Staging' / role
        winners, _ = b.pak_index(b.package_chain(staging), strict=False)
        requested = {}
        log = root / role / 'loose_missing_files.log'
        log_bytes = log.read_bytes()
        for line in log_bytes.splitlines():
            match = re.search(rb'caller=FindElemFile/name id=([0-9A-F]{8}) path=(.*?) result=MISSING', line)
            if match:
                eid = int(match[1], 16)
                raw = match[2]
                if b.pak_id(raw) != eid:
                    raise ValueError('Logged path/hash mismatch')
                try:
                    name = b.logical(raw.decode('mbcs', errors='strict'))
                except (ValueError, UnicodeError):
                    records.append(dict(role=role, path=raw.decode('mbcs', errors='replace'),
                                        id='%08X' % eid, status='INVALID_RUNTIME_PATH_NO_GUESSED_REPLACEMENT'))
                    continue
                if '__loose_probe' not in name and not name.lower().startswith('userdata\\'):
                    requested[eid] = (name, 'RUNTIME_LOGGED_NAME')
        named_ids = {int(raw, 16) for raw in re.findall(rb'caller=FindElemFile/name id=([0-9A-F]{8})', log_bytes)}
        only_ids = {int(raw, 16) for raw in re.findall(rb'caller=FindElemFile/ID id=([0-9A-F]{8})', log_bytes)} - named_ids
        for eid in only_ids - {0xDEADBEEF, 0x2DF60386}:
            entry = winners.get(eid)
            if entry is None:
                records.append(dict(role=role, path='<ID_ONLY>', id='%08X' % eid,
                                    status='MISSING_ID_ONLY_PAYLOAD_AND_ORIGINAL_NAME'))
                continue
            payload, _ = b.entry_payload(entry, {}, None)
            if payload[:3] == b'SPR' and (b.sprite_blob_valid(payload, True) or b.sprite_blob_valid(payload, False)):
                requested[eid] = ('spr\\recovered_by_id\\%08X.spr' % eid,
                                  'LOGGED_ID_ONLY_VALIDATED_SPR_OPERATIONAL_NAME_NOT_ORIGINAL_NAME_CALLER_UNKNOWN')
            else:
                records.append(dict(role=role, path='<ID_ONLY>', id='%08X' % eid,
                                    status='ID_ONLY_PAYLOAD_TYPE_UNRESOLVED'))
        render_log = root / role / 'invalid_spr_frames.log'
        if render_log.is_file():
            for raw in re.findall(rb'image=(.*?) frame=', render_log.read_bytes()):
                if not raw.lower().endswith(b'.spr'):
                    continue
                try:
                    name = b.logical(raw.decode('mbcs', errors='strict'))
                except (ValueError, UnicodeError):
                    continue
                if not (root / role / Path(name.replace('\\', '/'))).is_file():
                    requested[b.pak_id(raw)] = (name, 'RENDER_LOG_EXACT_MISSING_SPR_PATH')
        # Source InitVngSetAttrib uses this exact ID, with no known original name.
        # The explicit operational filename is NOT claimed as a recovered name.
        requested[0x2DF60386] = ('settings\\item\\PhongThanSetAttributes.txt', 'USER_NAMED_FILE_ORIGINAL_PAYLOAD_SOURCE_ID_2DF60386_NOT_ORIGINAL_FILENAME')
        manifest = root / 'completion' / (role + '_named_effective_manifest.csv')
        rows = list(b.csv_read(manifest))
        old_alias = root / role / 'recovered_by_id/2DF60386.txt'
        if old_alias.is_file():
            assert b.sha(old_alias.read_bytes()) == '455cdcc29ef6a5c5ec9a1e7c642d0576aaa2476ff6447f1ecb47945cb46456b4'
            saved = root / 'completion' / (role + '_2DF60386_before_rename.txt')
            if not saved.exists():
                saved.write_bytes(old_alias.read_bytes())
            rows = [r for r in rows if r['path'].lower() != 'recovered_by_id\\2df60386.txt']
        for eid, (name, evidence) in requested.items():
            target = root / role / Path(name.replace('\\', '/'))
            entry = winners.get(eid)
            historical = entry is None and eid in original_winners
            if historical:
                entry = original_winners[eid]
            loose = staging / Path(name.replace('\\', '/'))
            source = None
            if loose.is_file() and (role == 'Client' or entry is None):
                data = loose.read_bytes()
                source = str(loose)
                pak, number = '', ''
            elif entry:
                try:
                    data, _ = b.entry_payload(entry, {}, None)
                except ValueError as exc:
                    records.append(dict(role=role, path=name, id='%08X' % eid,
                                        status='PAYLOAD_DECODE_UNRESOLVED', reason=str(exc)))
                    continue
                source = str(entry['pak'])
                pak, number = entry['pak'].name, str(entry['entry'])
            else:
                records.append(dict(role=role, path=name, id='%08X' % eid, status='MISSING_AVAILABLE_PAYLOAD'))
                continue
            sha = b.sha(data)
            if target.exists() and target.read_bytes() != data:
                raise ValueError('Refuse differing existing file: ' + str(target))
            same = [r for r in rows if r['path'].lower() == name.lower()]
            if any(r['sha256'].lower() != sha for r in same):
                raise ValueError('Manifest hash conflict: ' + name)
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            assert b.sha(target.read_bytes()) == sha
            row = dict(path=name, id='%08X' % eid, status='RESOLVED',
                       origin='RUNTIME_MISSING_TARGETED_RECOVERY', pak=pak, entry=number,
                       sha256=sha, bytes=str(len(data)), evidence=evidence, error='')
            if historical and source != str(loose):
                row.update(origin='ORIGINAL_AVAILABLE_PACKAGE_HISTORICAL_RECOVERY',
                           evidence=evidence + '; exact path hash; original available package payload, incomplete original chain, NOT certified current active-version winner')
            rows = [r for r in rows if r['path'].lower() != name.lower()] + [row]
            records.append(dict(role=role, source=source, **row))
        backup = manifest.with_suffix('.csv.before_logged_recovery')
        if not backup.exists():
            backup.write_bytes(manifest.read_bytes())
        b.csv_write(manifest, rows, ['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
        if old_alias.is_file():
            new_alias = root / role / 'settings/item/PhongThanSetAttributes.txt'
            assert new_alias.read_bytes() == old_alias.read_bytes()
            old_alias.unlink()  # Byte-identical new named file plus completion backup.
        old_index = root / role / 'loose_index.bin'
        backup_index = old_index.with_suffix('.bin.before_logged_recovery')
        if not backup_index.exists():
            backup_index.write_bytes(old_index.read_bytes())
    index.run(root)
    for record in records:
        if record['status'] == 'MISSING_AVAILABLE_PAYLOAD':
            record['original_available_packages_match'] = int(record['id'], 16) in original_winners
            record['original_package_chain_incomplete'] = [str(p) for p in listed if not p.is_file()]
    records = list({(r['role'], r['id'], r['path']): r for r in records}.values())
    report = root / 'completion' / 'logged_missing_recovery.json'
    report.write_text(json.dumps(records, ensure_ascii=True, indent=2), encoding='utf-8')
    for role in ('Client', 'Server'):
        print(role, dict(Counter(r['status'] for r in records if r['role'] == role)), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workspace', type=Path, required=True)
    run(parser.parse_args().workspace)
