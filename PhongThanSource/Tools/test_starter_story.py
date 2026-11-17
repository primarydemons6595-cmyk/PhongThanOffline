"""Run starter dialogue/reward checks with the actual Lua4 engine runner."""
from pathlib import Path
import json,os,subprocess
from extract_client_coordinate_dictionary import encode_tcvn3
ROOT=Path(__file__).resolve().parents[2]
SRC=ROOT/'PhongThanSource'
REPORT=ROOT/'analysis/starter_story_20261029'
def lua(v):
    if v is None:return 'nil'
    if isinstance(v,str):return json.dumps(v,ensure_ascii=False)
    if isinstance(v,int):return str(v)
    if isinstance(v,list):return '{'+','.join(lua(x) for x in v)+'}'
    return '{'+','.join('['+lua(k)+']='+lua(x) for k,x in v.items())+'}'
def steps(path):
    return json.loads(subprocess.check_output(['node','-e',"process.stdout.write(JSON.stringify(require('./"+path+"')))"],cwd=ROOT))['steps']
source='PTG_STEPS='+lua(steps('PhongThanSource/Tools/giapsi/quests.cjs'))+'\n'
source+='PT2_STEPS='+lua(steps('PhongThanSource/Tools/two_faction_quests.cjs'))+'\n'
for path in ['Tools/giapsi/runtime.lua','Tools/two_faction_runtime.lua','Tools/starter_story_generated.lua','Tests/fixtures/StarterStoryChecks.lua']:
    source+='\n'+(SRC/path).read_text('utf-8')
data=bytearray()
for c in source:
    try:data.extend(encode_tcvn3(c))
    except KeyError:data.extend(encode_tcvn3(c.lower()))
fixture=REPORT/'behavior.lua';fixture.write_bytes(data)
listing=REPORT/'behavior-list.txt';listing.write_text(str(fixture)+'\n','ascii')
env=os.environ.copy();env['PATH']=str(ROOT/'PhongThanRuntime-Loose/Client')+';'+env['PATH']
runner=SRC/'Output/Tools/LuaStageSyntaxTests.exe'
for name in ['syntax','behavior']:
    result=subprocess.run([str(runner),str(REPORT/(name+'-list.txt'))],env=env,cwd=ROOT,capture_output=True)
    (REPORT/(name+'.log')).write_bytes(result.stdout+result.stderr)
    print((result.stdout+result.stderr).decode('ascii','replace'),end='')
    if result.returncode:raise SystemExit(result.returncode)
