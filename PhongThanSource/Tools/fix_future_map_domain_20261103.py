"""Use the explicit runtime-map API for authored quest travel tuples."""
from pathlib import Path
import hashlib,json
ROOT=Path(__file__).resolve().parents[2];r=ROOT/'PhongThanSource';s=ROOT/'PhongThanRuntime-Loose/Server';pkg=r/'Deploy/ProjectContent/ThuongChuTgpt20261103';m=json.loads((pkg/'manifest.json').read_text('utf8'))
canonical=r/'Tools/giapsi/runtime.lua';d=canonical.read_bytes();assert d.count(b'NewWorld(')==3;d=d.replace(b'NewWorld(',b'NewRuntimeWorld(');canonical.write_bytes(d)
files=[]
# Every bound GiapSi script embeds PTGApply. Fix this API boundary across those
# scripts without decoding their other quest wrappers or modifying quest data.
for p in (s/'script/phongthan/giapsi').glob('*.lua'):
 d=p.read_bytes();old=d
 if b'function PTGApply(i)' not in d:continue
 d=d.replace(b'if a.travel then NewWorld(a.travel[1]',b'if a.travel then NewRuntimeWorld(a.travel[1]')
 d=d.replace(b'then NewWorld(1063,',b'then NewRuntimeWorld(1063,').replace(b'else NewWorld(1062,',b'else NewRuntimeWorld(1062,')
 if old==d:continue
 path=p.relative_to(s).as_posix();target=pkg/'compiled/Server'/path;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(d)
 row=dict(relative=path,sha256=hashlib.sha256(d).hexdigest(),before_sha256=hashlib.sha256(old).hexdigest(),bytes=len(d));m['files'].append(row);files.append(row)
(pkg/'manifest.json').write_text(json.dumps(m,ensure_ascii=False,indent=2),'utf8')
(ROOT/'analysis/thuongchu_tgpt_20261103/future-map-domain.json').write_text(json.dumps(files,indent=2),'utf8')
print('Staged explicit runtime-domain travel in',len(files),'quest scripts')
