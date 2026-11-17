"""Patch only the two bound NPCs, preserving their other quest wrappers."""
from pathlib import Path
import hashlib,json
from extract_client_coordinate_dictionary import encode_tcvn3,decode_tcvn3

ROOT=Path(__file__).resolve().parents[2]
server=ROOT/'PhongThanRuntime-Loose/Server'
out=ROOT/'analysis/giapsi_future_portal_20261102'
out.mkdir(exist_ok=True)
source=(ROOT/'PhongThanSource/Tools/giapsi/runtime.lua').read_text('utf8')
helper=source[source.index('function PTGFutureTravelReady()'):source.index('function PTGMenu()')]
menu=source[source.index('    if PTGFutureTravelReady() then'):source.index('    if PTSAddChoices then')]
receipt=[]
for name in ('tay','dac_future'):
    relative=f'script/phongthan/giapsi/npc_{name}.lua'
    path=server/relative
    before=path.read_bytes();text=before
    if b'function PTGFutureTravelReady()' in text:
        raise RuntimeError('Already patched: '+relative)
    assert text.count(b'function PTGMenu()')==1
    text=text.replace(b'function PTGMenu()',encode_tcvn3(helper)+b'function PTGMenu()',1)
    start=text.index(b'function PTGMenu()')
    pos=text.index(b'    local options={}',start)
    end=text.index(b'\n',pos)+1
    after=text[:end]+encode_tcvn3(menu)+text[end:]
    (out/f'npc_{name}.before.lua').write_bytes(before)
    (out/f'npc_{name}.lua').write_bytes(after)
    receipt.append(dict(relative=relative,before_sha256=hashlib.sha256(before).hexdigest(),sha256=hashlib.sha256(after).hexdigest()))
(out/'plan.json').write_text(json.dumps(receipt,indent=2),'utf8')
print('Staged two NPC scripts; live files unchanged.')
