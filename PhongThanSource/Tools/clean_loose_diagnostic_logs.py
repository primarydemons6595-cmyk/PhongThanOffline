"""Remove disposable Loose diagnostics only, recording a manifest before deletion."""
import argparse
import json
import os
import re
import subprocess
from datetime import datetime
from pathlib import Path

WORKSPACE = Path(__file__).resolve().parents[2]
LOOSE = WORKSPACE / 'PhongThanRuntime-Loose'
REPORT = WORKSPACE / 'analysis' / 'quiet_logs_20261031'
SKIP = {'characterstore', 'nativedroppersistencev1', 'userdata', 'accountstore',
        'localai', 'node_modules', '.git', 'bin', 'obj', 'wpf', 'settings',
        'script', 'scripts', 'data', 'native', 'assets', 'database', 'databases'}
LOG = re.compile(r'\.(log|jsonl|trace)(\.\d+)?$', re.I)
ROOT_LOGS = {'ibshop_loader_diag.log', 'player_chat_trace.log', 'pt_buff_cancel.log',
            'pt_consumable_client_trace.log', 'summon_trace.log', 'travel_item_diag.log'}

def stopped():
    command = "$r='" + str(LOOSE).replace("'", "''") + "\\'; @(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($r,[StringComparison]::OrdinalIgnoreCase) -and $_.Name -match '^(Game|GameServer|Goddess|Bishop|PhongThan.*)\\.exe$'}).Count"
    count = subprocess.check_output(['powershell', '-NoProfile', '-Command', command], text=True).strip()
    if count != '0':
        raise RuntimeError('Close Loose applications before cleaning logs: ' + count)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    stopped()
    selected = []
    for role in ['Server', 'Client', 'ManagerStudio']:
        root = LOOSE / role
        for parent, dirs, files in os.walk(root):
            dirs[:] = [d for d in dirs if d.lower() not in SKIP and not (Path(parent)/d).is_junction() and not (Path(parent)/d).is_symlink()]
            for name in files:
                low = name.lower()
                if low == 'studio_history.jsonl':
                    continue  # Editor undo history is user data.
                if LOG.search(low) or (low.endswith('.csv') and any(x in low for x in ['diag', 'trace', 'audit'])) or low in {'movement_trace.flag', 'escort_click_trace.flag'}:
                    p = Path(parent)/name
                    if not p.is_symlink() and not p.is_junction():
                        selected.append(p)
    selected.extend(p for p in WORKSPACE.iterdir() if p.is_file() and p.name in ROOT_LOGS)
    records = []
    for p in selected:
        resolved = p.resolve(strict=True)
        assert resolved.is_relative_to(LOOSE.resolve()) or (resolved.parent == WORKSPACE and resolved.name in ROOT_LOGS), resolved
        s = p.stat()
        records.append(dict(path=str(p.relative_to(WORKSPACE)), bytes=s.st_size, mtime_ns=s.st_mtime_ns))
    report = dict(scope='LOOSE_ONLY', applied=args.apply, files=len(records), bytes=sum(r['bytes'] for r in records), entries=records)
    REPORT.mkdir(exist_ok=True)
    manifest = REPORT / ('log_cleanup_'+datetime.now().strftime('%Y%m%d_%H%M%S')+'.json')
    manifest.write_text(json.dumps(report, indent=2), encoding='utf8')
    if args.apply:
        stopped()
        for p, record in zip(selected, records):
            s=p.stat()
            if s.st_size != record['bytes'] or s.st_mtime_ns != record['mtime_ns']:
                raise RuntimeError('File changed since inventory: '+str(p))
            p.unlink()  # Literal verified file path, never a recursive delete.
        for role in ['Server/Logs', 'Server/payserver_log', 'Server/EventService/logs']:
            root=LOOSE/role
            if root.exists():
                for parent, dirs, files in os.walk(root, topdown=False, followlinks=False):
                    p=Path(parent)
                    assert p.resolve().is_relative_to(LOOSE.resolve())
                    if not p.is_junction() and not p.is_symlink() and not any(p.iterdir()):
                        p.rmdir()
    print(json.dumps({k:v for k,v in report.items() if k != 'entries'}))
    print('Manifest: '+str(manifest))

if __name__ == '__main__':
    main()
