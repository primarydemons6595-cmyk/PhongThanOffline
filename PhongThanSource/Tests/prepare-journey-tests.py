from pathlib import Path
import ast,json,re,sys
ROOT=Path(__file__).resolve().parents[2];S=ROOT/'PhongThanRuntime-Loose/Server'
CONTENT=ROOT/'PhongThanSource/Deploy/ProjectContent/JourneyRepair20261031/compiled/Server'
OUT=ROOT/'analysis/new_errors_20261031';OUT.mkdir(exist_ok=True)
def expand(rel,chain=()):
 assert rel not in chain
 p=CONTENT/rel
 if not p.exists():p=S/rel
 return re.sub(rb'Include\("([^"\r\n]+)"\)',lambda m:expand(m[1].decode('latin1').replace('\\\\','/').replace('\\','/').lstrip('/'),chain+(rel,)),p.read_bytes())
tree=ast.parse((ROOT/'PhongThanSource/Tools/test_quest_service_repair.py').read_text('utf8'))
pre=next(ast.literal_eval(n.value) for n in tree.body if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='pre' for t in n.targets))
pre=pre.replace(b'tasks',b'taskstate').replace(b'function GetMorphType() return 364 end',b'function GetMorphType() return morph or -1 end').replace(b'function PolyMorph() end',b'function PolyMorph(id) morph=id end')
merchants=json.loads((ROOT/'analysis/bot_aigent_20261029/commerce_manifest.json').read_text('utf8'))['merchants']
source=pre+b'\nbase=globals();envs={}\n'
for m in merchants:
 source+=f'\nenvs[{m["shop"]}]={{}};for k,v in base do envs[{m["shop"]}][k]=v end;globals(envs[{m["shop"]}])\n'.encode()+expand(m['script'])+b'\nglobals(base)\n'
source+=b'''
function selectshop(shop)
 globals(base.envs[shop]);world=PT_TRADE_WORLD;px=100;py=100;nx=100;ny=100;morph=364;buff=1;level=200;cash=10000000;xp=0;main(45)
end
for origin=1,9 do for kind=1,10 do
 base.taskstate={};for shop=1,9 do base.envs[shop].taskstate=base.taskstate end
 base.selectshop(origin);morph=-1;buff=0;items={[136]=1};yes_2(kind);check(GetTask(60)==origin and GetTask(332)==mod(kind-1,5)+1 and morph==364);main(45);RandomTask()
 local wanted=GetTask(334);local quantity=GetTask(335);local seller=floor((wanted-1)/3)+1
 check(seller~=origin and GetTask(333)==origin)
 complete_task();check(GetTask(333)==origin and xp==0)
 base.selectshop(seller);main(45);check(pt_trade_active())
 local before=cash;dobuy(wanted,quantity);check(GetTask(110)==wanted and GetTask(111)==quantity and cash==before-quantity*10)
 -- Reopen the origin in its own script VM; cargo belongs to the character.
 base.selectshop(origin);main(45);check(pt_trade_active());complete_task()
 check(GetTask(333)==0 and GetTask(803)==1 and xp>0 and GetTask(111)==0)
 local earned=xp;complete_task();check(xp==earned and GetTask(803)==1)
end end
print("PASS actual merchant VMs: 90 cross-shop purchases/returns, all rentals, missing goods/replay guards, exact cargo and cash")
'''
(OUT/'commerce-test.lua').write_bytes(source)
bootstrap=b'''
world=1;dialogs=0;notices=0;options={}
function GetWorldPos() return world,0,0 end
function GetContentWorldPos() return world,0,0 end
function Say(text,count,...) dialogs=dialogs+1;assert(count<=20 and count==getn(arg));options=arg end
function Talk(...) notices=notices+1 end
function SetExeState(...) end
function CloseDialog() end
function GetMorphType() return morph or -1 end
function IsPlayerInsideWeapon(...) return 0 end
function FindAValidIBItem(g,d,p,n) assert(g==8 and d==expectedDetail and p==2);return missingItem and 0 or 1 end
function NewWorld(m,x,y) travels=(travels or 0)+1;lastMap=m end
function CostIBItem(i) assert(i==1);costs=(costs or 0)+1;return 1 end
function SetFightState(...) end
function SetRevPos(...) end
function Msg2Player(...) end
function GetPK() return pk or 0 end
function GetCamp() return camp or 1 end
'''
for name in ['wildsuper','bigwildsuper']:
 raw=(CONTENT/f'script/item/ibitem/{name}.lua').read_bytes()
 test=b'''
main(1);assert(dialogs==1 and getn(options)==15)
pt_wild_page2();assert(dialogs==2 and getn(options)==14)
local callbacks={};for i=1,getn(options) do callbacks[strsub(options[i],strfind(options[i],"/")+1)]=1 end
assert(callbacks.v21 and callbacks.v22 and callbacks.v37 and callbacks.v38 and callbacks.v58)
local calls={v21,v22,v37,v38};local destinations={18,19,65,64}
for i=1,4 do local old=costs or 0;calls[i]();assert(lastMap==destinations[i] and costs==old+1 and travels==i) end
pk=88;local old=costs;v38();assert(costs==old);pk=0
morph=364;v21();assert(costs==old);morph=-1
missingItem=1;v21();assert(costs==old);missingItem=nil
local forbidden={64,66,71,72};for i=1,4 do world=forbidden[i];local before=dialogs;local oldNotices=notices;pt_wild_page2();assert(dialogs==before and notices==oldNotices+1) end
print("PASS 25 original teleport choices: both pages reachable within engine limit; four missing maps and forbidden-map guards")
'''
 (OUT/(name+'-test.lua')).write_bytes(bootstrap+f'\nexpectedDetail={35 if name=="wildsuper" else 159}\n'.encode()+raw+test)
print('Prepared journey Lua4 fixtures with isolated merchant environments')
doctor=b'''
state={};faction=0;world=14;dialogs=0;options={}
function GetContentWorldPos() return world,0,0 end
function GetWorldPos() return world+1000,0,0 end
function GetPlayerType() return faction end
function GetLevel() return 200 end
function GetTask(k) return state[k] or 0 end
function SetTask(k,v) state[k]=v end
function HaveNormalItem(...) return 0 end
function HaveEventItem(...) return 0 end
function AddNote(...) end
function PtTwoFactionNote(...) end
function GetTrade() return 0 end
function GetLockState() return 0 end
function SystemTime() return 1793200000 end
function CloseDialog() end
function SayButtons(t,n,...)
 dialogs=dialogs+1
 if type(arg[1])=="table" then options=arg[1] else options=arg end
 assert(n==getn(options) and n<=6)
end
'''
doctor+=expand('script/phongthan/giapsi/npc_doctor_dong.lua')+b'''
for f=0,2 do faction=f;state={};local old=dialogs;main();assert(dialogs>old) end
faction=0;state={[4550]=4};main();local found=0
for i=1,getn(options) do if strfind(options[i],"/ptg_a") then found=1 end end
assert(found==1)
for f=1,2 do faction=f;local tested=0
 for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i];if a.f==f and a.role=="doctor_dong" then
  state={};state[a.task]=a.from;if a.mainPhase then state[PT2MainTask()]=a.mainPhase end
  main();local found=0;for j=1,getn(options) do if strfind(options[j],"/pt2_view") then found=1 end end;assert(found==1);tested=tested+1
 end end
 assert(tested>0)
end
print("PASS Dong Quan doctor: visible fallback for all three factions; existing Giap/Dao/Di quest phases reachable")
'''
(OUT/'doctor-test.lua').write_bytes(doctor)
