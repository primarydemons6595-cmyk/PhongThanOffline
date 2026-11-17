from pathlib import Path
import json,os,subprocess
ROOT=Path(__file__).resolve().parents[2];P=ROOT/'PhongThanSource/Deploy/ProjectContent/MasterAlias20261030';A=ROOT/'analysis/master_alias_20261030'
manifest=json.loads((P/'manifest.json').read_text());fixtures=[]
for i,e in enumerate(manifest['files']):
 data=(P/'compiled'/e['role']/e['relative']).read_bytes()
 pre=b'''function Include() end
function GetContentWorldPos() return 3,100,100 end
function GetWorldPos() return 1003,100,100 end
function SubWorldIdx2ContentID(n) return n end
master=0
function IsMantleMaster() return master end
'''
 checks=b'\nassert(IsMaster()==0);master=1;assert(IsMaster()==1);master=0;assert(IsMaster()==0)\n'
 if e['relative'].endswith('1004_94a2d951.lua'):
  checks+=b'''function GetLevel() return 90 end
function GetTask(n) if n==897 or n==898 then return 1 end;return 0 end
function GetByte() return 0 end
function SayTask(text,rows) assert(rows[4].show==1) end
function CloseDialog() end
function GetContentWorldPos() return 4,100,100 end
pt_original_main()
print("PASS REAL_DI_NHAN_MAIN level90 active_quest=1 missing_api=0")
'''
 f=A/('fixture-%02d.lua'%i);f.write_bytes(pre+data+checks);fixtures.append(f)
listing=A/'fixtures.txt';listing.write_text(''.join(str(f)+'\n' for f in fixtures),'ascii')
env=os.environ.copy();env['PATH']=str(ROOT/'PhongThanRuntime-Loose/Client')+';'+env['PATH']
r=subprocess.run([str(ROOT/'PhongThanSource/Output/Tools/LuaStageSyntaxTests.exe'),str(listing)],capture_output=True,env=env,cwd=ROOT)
(A/'tests.log').write_bytes(r.stdout+r.stderr);print((r.stdout+r.stderr).decode('ascii','replace'));raise SystemExit(r.returncode)
