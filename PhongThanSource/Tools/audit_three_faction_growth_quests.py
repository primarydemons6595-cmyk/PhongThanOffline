"""Export all six growth tables and existing explicit quest graphs for review.

Presence in either list is not a claim of complete gameplay coverage.
"""
import hashlib
import json
import re
import subprocess
from pathlib import Path

from extract_client_coordinate_dictionary import decode_tcvn3

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'analysis/three_faction_completion_20261029'


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    raw = (ROOT / 'PhongThanSource/Headers/PhongThanLevelGrowth.h').read_bytes()
    rows = []
    pattern = r'static const PTLevelGrowth ptGrowth([01])_([012])\[\] = \{(.*?)\n\};'
    for advanced, faction, body in re.findall(pattern, raw.decode('ascii'), re.S):
        for level, line in enumerate(body.strip().splitlines(), 1):
            texts = [decode_tcvn3(bytes(int(x, 16) for x in re.findall(r'\\x([0-9A-F]{2})', value)))
                     for value in re.findall(r'"([^"]*)"', line)]
            assert len(texts) == 3
            rows.append(dict(advanced=int(advanced), faction=int(faction), level=level,
                tip=texts[0], description=texts[1], mission=texts[2],
                damaged_text=any('?' in text for text in texts)))
    assert len(rows) == 840
    (OUT / 'growth-catalog.json').write_text(json.dumps(dict(rows=rows,
        header_sha256=hashlib.sha256(raw).hexdigest()), ensure_ascii=False, indent=2)+'\n','utf-8')
    expression = "process.stdout.write(JSON.stringify({giap:require('./PhongThanSource/Tools/giapsi/quests.cjs').steps,two:require('./PhongThanSource/Tools/two_faction_quests.cjs').steps}))"
    graph = json.loads(subprocess.check_output(['node','-e',expression], cwd=ROOT))
    chapters = []
    for faction in range(3):
        steps = graph['giap'] if faction == 0 else [s for s in graph['two'] if s['f'] == faction]
        groups = {}
        for step in steps:
            key = (step['title'].split(':')[0], step['level'], step['task'])
            groups.setdefault(key, []).append(dict(role=step['role'], from_phase=step['from'], to_phase=step['to']))
        chapters.append(dict(faction=faction, explicit_transitions=len(steps), chapters=[dict(
            title=key[0], level=key[1], task=key[2], steps=value) for key,value in groups.items()]))
    (OUT / 'implemented-graphs.json').write_text(json.dumps(chapters,ensure_ascii=False,indent=2)+'\n','utf-8')
    print(json.dumps(dict(growth_rows=len(rows), mission_rows=sum(bool(r['mission']) for r in rows),
        damaged_rows=sum(r['damaged_text'] for r in rows), graph_transitions=[c['explicit_transitions'] for c in chapters])))


if __name__ == '__main__':
    main()
