"""Validate every catalog key and exercise the real card Lua in Lua 4."""
from pathlib import Path
import json,os,subprocess
from build_level_gift import ROOT,LIVE,PACKAGE,REPORT,EXTRAS,enc
from extract_client_coordinate_dictionary import decode_tcvn3
tables={0:{2:'armor',5:'boot',6:'belt',7:'helm',9:'pendant',10:'horse',0:'meleeweapon'},3:'material',8:'ibitem'}
verified=[]
for role in ('Server','Client'):
 def resolve(row):
  g,d,p,l,s,luck=row[:6];fn=tables[g][d] if g==0 else tables[g]
  records=[r.split(b'\t') for r in (LIVE/role/f'settings/item/001/{fn}.txt').read_bytes().splitlines()[1:]]
  matches=[r for r in records if len(r)>12 and (r[2:4]==[str(d).encode(),str(p).encode()] and (g!=0 or r[11]==str(l).encode()) if g in (0,8) else r[2]==str(d).encode())]
  assert len(matches)==1,(role,row,len(matches))
  r=matches[0]
  if g==0:
   if d!=10:assert r[9] in (str(s).encode(),b'1000'),(role,row,r[9])
   req=dict(zip(r[44:56:2],r[45:56:2]))
   if d in (7,9,2,6,5):assert r[69] in (b'2',b'4'),(row,r[69])
   if d==10:assert req[b'36']==(b'60' if l==6 else b'90') and req[b'37'] in (b'0',b'1',b'2')
  verified.append(dict(role=role,tuple=row[:7],name=decode_tcvn3(r[0])))
 for stage in range(7):
  for row in EXTRAS[stage]:resolve(row)
 for c in range(3):
  for d,l,s in [(9,4,2+c*5),(7,4,2+c*5),(6,4,2+c*5),(5,4,2+c*5),(2,4,2+c*5),(7,8,4+c*5),(9,8,4+c*5)]:resolve([0,d,3+c,l,s,0,1])
  for l in (6,8):resolve([0,10,24+c,l,5,0,1])
 for p in (27,28,29,30):resolve([0,0,p,8,5,0,1])
 # IB 9700 goes through the scripted IB path, with no native effect/buff or timer.
 rows=[r.split(b'\t') for r in (PACKAGE/'compiled'/role/'settings/item/001/ibitem.txt').read_bytes().splitlines()]
 new=[r for r in rows if len(r)>3 and r[2]==b'9700'];assert len(new)==1
 r=new[0];assert r[1:4]==[b'8',b'9700',b'2'] and r[10]==b'0' and r[30]==b'0' and r[70]==b'1'
 assert all(r[k]==b'0' for k in (13,14,22,23,24,25,27,28,29)) and r[26]==b'1'
 assert r[12]==b'\\script\\phongthan\\rewards\\level_gift.lua'
(REPORT/'catalog-verification.json').write_text(json.dumps(verified,ensure_ascii=False,indent=2),'utf8')
runtime=(PACKAGE/'compiled/Server/script/phongthan/rewards/level_gift.lua').read_bytes()
fixture=REPORT/'behavior.lua';fixture.write_bytes(runtime+b'\n'+(ROOT/'PhongThanSource/Tests/fixtures/LevelGiftChecks.lua').read_bytes())
integration=REPORT/'starter-integration.lua'
starter=(PACKAGE/'compiled/Server/script/phongthan/escort/lequan.lua').read_bytes()
starter=starter[:starter.index(b'PTLG_LEQUAN_MAIN=main')]
integration.write_bytes(runtime+b'\n'+(ROOT/'PhongThanSource/Tests/fixtures/LevelGiftChecks.lua').read_bytes()+b'\n'+starter+b'''
function AddNormalItemPile(g,d,p,l,s,luck) return AddNormalItem(g,d,p,l,s,luck,0) end
function GetItemCount(g,d,l,s,p) if g==6 and d==9173 then return 0 end;return card end
function LeQuanStarterXu() error("Old 100 Xu grant must never be called") end
for c=0,2 do
 reset(c);capacity=3;PhongThanClaimStarter();check(GetTask(4700)==1 and GetTask(4702)==3 and card==0)
 capacity=1000;PhongThanClaimStarter();check(GetTask(4700)==2 and GetTask(4704)==1 and card==1)
 local before=getn(items);PhongThanClaimStarter();check(getn(items)==before and card==1)
end
reset(1);tasks[4700]=2;tasks[4703]=1;PhongThanClaimStarter();check(card==1 and GetTask(4703)==1 and getn(items)==0)
print("PASS STARTER_CARD_INTEGRATION checks="..checks.." old_xu_calls=0 legacy_completed=card_only")
''')
teacher=REPORT/'teacher-behavior.lua'
teacher.write_bytes(b'''
checks=0;PlayerIndex=1;PlayerId=100;world=1003;px=100;py=100;career=1;sales=0;menus=0;trade=0;locked=0
function check(v) checks=checks+1;if not v then error("Teacher check #"..checks) end end
function GetWorldPos() return world,px,py end
function GetContentWorldPos() return world-1000,px,py end
function SubWorldIdx2ContentID(n) return n end
function GetNpcID(n) if n==7 then return 123 end;return 0 end
function GetNpcTemplateID(n) if n==7 then return 154 end;return 0 end
function GetNpcPos() return 1003,100,100 end
function GetTask() return 0 end
function GetLevel() return 80 end
function GetPlayerType() return career end
function GetByte() return 0 end
function IsMantleMaster() return 0 end
function GetTrade() return trade end
function GetLockState() return locked end
function SayTask(t,options) menus=menus+1;check(options[1][2]=="jineng" and options[1].show==1) end
function MsgBox(t,yes,no) check(t==10553 and yes=="yes_2");menus=menus+1 end
function Talk() end
function CloseDialog() end
function Sale(n) check(n==9);sales=sales+1 end
'''+(PACKAGE/'compiled/Server/script/phongthan/npc_recovered_batch/1003_176efdac.lua').read_bytes()+b'''
main(7);check(menus==1);jineng();check(menus==2);yes_2();check(sales==1)
career=0;yes_2();jineng();check(sales==1);career=2;yes_2();check(sales==1);career=1
px=113;yes_2();check(sales==1);px=100;world=1002;yes_2();check(sales==1);world=1003
trade=1;yes_2();check(sales==1);trade=0;locked=1;yes_2();check(sales==1);locked=0
PlayerId=101;yes_2();check(sales==1);PlayerId=100;yes_2();check(sales==2)
check(type(mission_PR)=="function" and type(songxin)=="function")
print("PASS ORIGINAL_NGOC_HU_TEACHER checks="..checks.." shop=9 original_quests=retained")
''')
goods=[r.split(b'\t') for r in (LIVE/'Server/settings/goods.txt').read_bytes().splitlines()]
for role in ('Server','Client'):
 shop=(LIVE/role/'settings/buysell.txt').read_bytes().splitlines()[9].split(b'\t')
 assert [int(n) for n in shop if n and int(n)>0]==[452,453,454,455,456,457,458,459,502]
 books=[r.split(b'\t') for r in (LIVE/role/'settings/item/001/skillbook.txt').read_bytes().splitlines()[1:]]
 for n in range(452,460):
  g,d,p,s,l=map(int,goods[n][:5]);assert g==7
  matches=[r for r in books if r[1:4]==[b'7',str(d).encode(),str(p).encode()]]
  assert len(matches)==1 and int(matches[0][9])>0
listing=REPORT/'behavior-list.txt';listing.write_text(''.join(str(p)+'\n' for p in (fixture,integration,teacher)),'ascii')
env=os.environ.copy();env['PATH']=str(LIVE/'Client')+';'+env['PATH']
r=subprocess.run([str(ROOT/'PhongThanSource/Output/Tools/LuaStageSyntaxTests.exe'),str(listing)],env=env,cwd=ROOT,capture_output=True)
(REPORT/'behavior.log').write_bytes(r.stdout+r.stderr)
print('Catalog identities verified:',len(verified));print((r.stdout+r.stderr).decode('ascii','replace'))
raise SystemExit(r.returncode)
