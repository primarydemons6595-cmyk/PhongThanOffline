"""Exercise staged original merchant callbacks and daily acceptance in Lua 4."""
import os,re,subprocess,json
from build_quest_service_repair import ROOT,LIVE,PACKAGE,REPORT,enc
STAGE=PACKAGE/'compiled/Server'
def expand(rel,chain=()):
    assert rel not in chain,(rel,chain)
    p=STAGE/rel
    if not p.exists():p=LIVE/rel
    data=p.read_bytes()
    def inc(m):
        path=m[1].decode('latin1').replace('\\\\','/').replace('\\','/').lstrip('/')
        return expand(path,chain+(rel,))
    return re.sub(rb'Include\("([^"\r\n]+)"\)',inc,data)
pre=b'''
checks=0;PlayerIndex=7;PlayerId=123;tasks={};cash=10000000;level=80;world=1002;px=100;py=100;nx=100;ny=100;buff=0;stock=1000;xp=0;notes=0;dialogs=0;notices=0;own=999
function check(v) checks=checks+1;if not v then error("check failed #"..checks) end end
function GetWorldPos() return world,px,py end
RuntimeWorldPos=GetWorldPos
function GetContentWorldPos() return world-1000,px,py end
function SubWorldIdx2ContentID(n) return n end
function GetTask(n) return tasks[n] or 0 end
function SetTask(n,v) tasks[n]=v end
function GetLevel() return level end
function GetNpcIdx() return own end
function GetNpcTemplateID(n) if n==45 then return 158 end;return 0 end
function GetNpcID(n) if n==45 then return 9999 end;return 0 end
function GetNpcPos(n) if n==45 then return world,nx,ny end;return world,px,py end
function HaveIBBuff(n) if n==160 then return buff end;return 0 end
function GetIBBuffCount() return 0 end
function AddIBBuff(n) if n==160 then buff=1 end;return 1 end
function GetGoodsName(n) return "Good"..n end
function GetGoodsWeight(n) return 1 end
function GetGoodsPurchasePrice() return 10 end
function GetGoodsSalePrice() return 8 end
function GetGoodsNum() return stock end
function ChangeGoodsNum(s,g,n) stock=stock+n end
function PlayerSellGoods() end
function SystemTime() return 1793200000 end
function CloseDialog() end
function MsgBox() dialogs=dialogs+1 end
function Say() dialogs=dialogs+1 end
function SayTask(t,opts) dialogs=dialogs+1;for i=1,getn(opts) do if opts[i][2]=="RandomTask" then check(opts[i].show==1) end end end
function Talk() notices=notices+1 end
function Msg2Player() end
function DailyJournal() notes=notes+1 end
function FindAValidIBItem() return 1 end
function CostIBItem() return 1 end
function DailyHopeFlush() return 0 end
function DailyHope() return 1,0 end
function GetName() return "Tester" end
function GetTGuardIndexByPlayerName() return 0 end
function GetMorphType() return 364 end
function GetCreatureInfo() return 0,-1 end
function SetCreatureType() end
function PolyMorph() end
function SetDeathScript() end
function GetCash() return cash end
function Pay(n) cash=cash-n;return 1 end
function Earn(n) cash=cash+n;return 1 end
function AddOwnExp(n) xp=xp+n end
function AddNormalItem() return 1 end
function GetTrade() return trade or 0 end
function GetLockState() return locked or 0 end
function HaveNormalItem(g,d) return items and (items[d] or 0) or 0 end
function DailyWorldPos() return world,px,py end
function DelNpc() end
function GetNpcLife() return 0 end
function NewWorld(m,x,y) world=m+1000;px=x;py=y end
function DailyExchange(t,a,b,need,gift,exp,fee,money)
 if reject or GetTask(t)~=a or cash<fee or GetTrade()~=0 or GetLockState()~=0 then return 0 end
 for i=1,getn(need) do if HaveNormalItem(need[i][1],need[i][2])<need[i][7] then return 0 end end
 for i=1,getn(need) do items[need[i][2]]=items[need[i][2]]-need[i][7] end
 cash=cash-fee+money;xp=xp+exp;SetTask(t,b);return 1
end
'''
merchants=json.loads((ROOT/'analysis/bot_aigent_20261029/commerce_manifest.json').read_text('utf8'))['merchants']
pre=pre.replace(b'tasks',b'taskstate')
source=pre
for m in merchants:
    source+=b'\nGetWorldPos=RuntimeWorldPos\n'+re.sub(rb'\btasks\b',b'menu_tasks',expand(m['script']))
    source+=f'\nworld={m["map"]};shop={m["shop"]};first={(m["shop"]-1)*3+1}\n'.encode()
    source+=b'''
 tasks={};buff=0;cash=10000000;level=80;px=nx;py=ny;xp=0
 local before=dialogs;main(45);check(dialogs>before);check(PTTradeNear())
 yes_2(1);check(GetTask(60)==shop and buff==1 and GetTask(332)==1)
 main(45);check(pt_trade_active())
 RandomTask();check(GetTask(333)==shop);local required=GetTask(334);local qty=GetTask(335)
 check(required>=1 and required<=27 and floor((required-1)/3)+1~=shop)
 local oldxp=xp;complete_task();check(xp==oldxp and GetTask(333)==shop)
 tasks[110]=required;tasks[111]=qty;tasks[120]=qty
 complete_task();check(xp>oldxp and GetTask(333)==0 and GetTask(803)==1)
 oldxp=xp;complete_task();check(xp==oldxp and GetTask(803)==1)
 tasks[110]=0;tasks[111]=0;tasks[120]=0
 local money=cash;buyover(110,first,2);check(GetTask(111)==2 and cash==money-20)
 dosell(110,1);check(GetTask(111)==1 and cash==money-12)
 px=nx+13;money=cash;buyover(110,first,1);check(cash==money);px=nx
 PlayerId=124;check(not PTTradeNear());PlayerId=123
 main(own);check(not PTTradeNear() or GetNpcTemplateID(own)~=158)
 print("PASS original merchant journey shop="..shop)
'''
source+=b'\n'+expand('script/phongthan/daily/tu_linh.lua')+b'\n'+expand('script/phongthan/daily/thien_cuong.lua')
source+=b'''
function reset() tasks={};items={};cash=10000000;world=1021;px=54912/32;py=100096/32;trade=0;locked=0;reject=nil;level=80 end
reset();local old=notices;pt_daily_tu_take();check(notices>old and GetTask(4412)==0 and cash==10000000)
items[82]=1;px=px+20;old=notices;pt_daily_tu_take();check(notices>old and items[82]==1);px=px-20
cash=0;old=notices;pt_daily_tu_take();check(notices>old and items[82]==1);cash=10000000
reject=1;old=notices;pt_daily_tu_take();check(notices>old and items[82]==1 and PTDailyGet(5,2)==0);reject=nil
pt_daily_tu_take();check(PTDailyGet(5,0)==1 and items[82]==0 and PTDailyGet(5,2)==1)
local paid=cash;pt_daily_tu_take();check(cash==paid and PTDailyGet(5,2)==1)
reset();world=1020;px=44672/32;py=96512/32;items[82]=1;pt_daily_tu_take();check(PTDailyGet(5,0)==1)
reset();world=1001;px=47216/32;py=99056/32
old=notices;pt_daily_cuong_take();check(notices>old and PTDailyGet(10,0)==0)
items[114]=1;items[115]=1;locked=1;old=notices;pt_daily_cuong_take();check(notices>old and items[114]==1);locked=0
reject=1;pt_daily_cuong_take();check(PTDailyGet(10,2)==0 and items[115]==1);reject=nil
pt_daily_cuong_take();check(PTDailyGet(10,0)==1 and items[114]==0 and items[115]==0 and PTDailyGet(10,2)==1)
paid=cash;pt_daily_cuong_take();check(cash==paid and PTDailyGet(10,2)==1)
print("PASS QUEST_SERVICE_REPAIR checks="..checks.." original_merchants=9 daily_acceptance=2")
'''
# Keep test storage separate from the original merchant's global menu `tasks`.
source=source.replace(b' tasks={}',b' taskstate={}').replace(b' tasks[',b' taskstate[').replace(b';tasks[',b';taskstate[')
fixture=REPORT/'behavior.lua';fixture.write_bytes(source)
listing=REPORT/'behavior-list.txt';listing.write_text(str(fixture)+'\n','ascii')
env=os.environ.copy();env['PATH']=str(ROOT/'PhongThanRuntime-Loose/Client')+';'+env['PATH']
r=subprocess.run([str(ROOT/'PhongThanSource/Output/Tools/LuaStageSyntaxTests.exe'),str(listing)],capture_output=True,env=env,cwd=ROOT)
(REPORT/'behavior.log').write_bytes(r.stdout+r.stderr)
print((r.stdout+r.stderr).decode('ascii','replace'))
raise SystemExit(r.returncode)
