"""Reviewed project rules around exact recovered PT arena Lua, no static mob edits."""
import json,re,struct
from restore_original_world import ROOT,OUT,Original
from build_authored_npcs import barrier_at,blocked
import build_offline_loose as b
import stage_missing_npcs as staging

batch=OUT/'boss_van_tien'; stage=batch/'staged/Server'; server=ROOT/'PhongThanRuntime-Loose/Server'
original=Original(); changes={}; report=[]
worlds=dict((int(a),v) for a,v in re.findall(rb'(?m)^(\d+)=([^\r\n]+)',(server/'settings/WorldSet.ini').read_bytes()))
cache={}
def walkable(world,x,y,used):
    # Source coordinates below come from original entrance/chest scripts;
    # nearest free cells are project reconstruction, never original placements.
    name=worlds[world]
    for radius in range(0,321,32):
        candidates=[(x+dx,y+dy) for dx in range(-radius,radius+1,32) for dy in range(-radius,radius+1,32) if max(abs(dx),abs(dy))==radius]
        for px,py in candidates:
            if px<0 or py<0 or any(abs(px-a)<64 and abs(py-c)<64 for a,c in used):continue
            key=(world,px//512,py//1024)
            if key not in cache:
                rel=b'maps\\'+name+b'_S\\v_%03d\\%03d_region_s.dat'%(key[2],key[1])
                local=server/rel.decode('gbk').replace('\\','/')
                got=original.get(rel)
                if not got:
                    rel=b'maps\\'+name+b'\\v_%03d\\%03d_region_c.dat'%(key[2],key[1]);got=original.get(rel)
                cache[key]=local.read_bytes() if local.exists() else (got[0] if got else None)
            data=cache[key]
            if data is not None and not blocked(barrier_at(data,px,py),px,py):used.append((px,py));return px,py
    raise ValueError(('No reviewed free cell',world,x,y))

missionids=['AEE066FB','B0B61E64','35A96016','A8B2F2ED']
portalids=['F57647B5','281C1583','479D2A84','41E3ECB2']
entrances=[(1325,3280),(1752,3567),(1650,3400),(1671,3035)]
centers=[(1475,3227),(1675,3359),(1662,3283),(1731,3154)]
exits=[(2,1650,3159),(4,1548,3232),(3,1709,3105),(1,1510,3290)]
labels=['Tho','Thuy','Hoa','Phong']
ledgerpath=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json'
ledger=json.loads(ledgerpath.read_text('utf8')); configs=[]
portalrows=[]
for i,(scope,review) in enumerate(zip(('崇城大营','蚩尤墓','玉虚宫','封神台'),('D23D5ED0','A49BB2A3','2945F0D6','2A6CCBCB')),1):
    proof=original.get(('script/'+scope+'/传送门.lua').encode('gbk'))[1]
    ex,x,y=exits[i-1]
    portalrows.append(dict(world=1000+ex,world_registered=True,content_map=ex,name='万仙阵入口'+labels[i-1],existing=[],positions=[[x//8,y//16]],templates=[247],map_scripts=[proof],translated_names=['Cong Van Tien Tran '+labels[i-1]],coordinate_evidence=[dict(source='original portal review Lua',id=review,map=ex,x=x,y=y)],scripts=[proof],source_lines=[]))
input_path=OUT/'boss_van_tien_input.json';input_path.write_text(json.dumps(dict(rows=portalrows),ensure_ascii=False,indent=2),'utf8')
staging.BATCH=batch;staging.STAGE=stage;staging.stage(input_path.name)
baseplan=json.loads((batch/'plan.json').read_text('utf8'))
assert len(baseplan['ready'])==4 and not baseplan['blocked'],baseplan['blocked']
for f in baseplan['files']:changes[f['relative']]=(stage/f['relative']).read_bytes()

for i in range(1,5):
    world=1066+i; content=66+i; level=20*i+20; first=124+5*i; boss=first+4; label=labels[i-1]
    used=[];points=[]
    # Four named guardians and a central boss, at audited free cells.
    cx,cy=centers[i-1]
    for dx,dy in [(-80,-60),(80,-60),(-80,60),(80,60),(0,0)]:
        points.append(walkable(world,(cx+dx)*32,(cy+dy)*32,used))
    mobs=[]
    # Retain original guardian templates/resources; 24 lesser copies are authored.
    for n in range(24):
        px,py=walkable(world,(cx-100+(n%6)*40)*32,(cy-80+(n//6)*50)*32,used)
        mobs.append((first+n%4,px,py))
    entry=walkable(world,entrances[i-1][0]*32,entrances[i-1][1]*32,[])
    ex,ey,ez=exits[i-1];expos=walkable(1000+ex,ey*32,ez*32,[])
    cfg=f'''PT_ID={i}
PT_MAP={content}
PT_LEVEL={level}
PT_FIRST={first}
PT_BOSS={boss}
PT_LABEL="{label}"
PT_REWARD_TASK={4929+i}
PT_ENTRY_TASK={4933+i}
PT_CLAIM_TASK={4937+i}
PT_REWARD_DETAIL={min(62+i,65)}
PT_ENTRY_X={entry[0]//32}
PT_ENTRY_Y={entry[1]//32}
PT_EXIT_MAP={ex}
PT_EXIT_X={expos[0]//32}
PT_EXIT_Y={expos[1]//32}
PT_POINTS={{'''+','.join('{'+str(x)+','+str(y)+'}' for x,y in points)+'}\nPT_MOBS={'+','.join('{%d,%d,%d}'%v for v in mobs)+'}\n'
    configs.append(cfg)
    common_functions='''
function pt_each(fn)
    local cursor=0
    local old=PlayerIndex
    for n=1,500 do
        local player
        cursor,player=GetNextPlayer(PT_ID,cursor,0)
        if cursor==0 then break end
        if player>0 then PlayerIndex=player;fn() end
    end
    PlayerIndex=old
end
function pt_fight() SetFightState(1) end
function pt_exit()
    LockCamp(0);SetFightState(0);SetLogoutRV(0)
    NewWorld(PT_EXIT_MAP,PT_EXIT_X,PT_EXIT_Y)
end
function pt_spawn(tid,x,y,death)
    local npc=AddNpc(tid,PT_LEVEL,SubWorld,x,y,1)
    if npc==0 then return 0 end
    local owned=AddMSNpc(PT_ID,npc,0)
    if owned==nil or owned<=0 then DelNpc(npc);return 0 end
    if death~=nil then
        if SetNpcDeathScript(npc,death)~=1 then DelNpc(npc);return 0 end
    end
    return npc
end
function pt_progress()
    if GetGlobalValue(PT_ID)~=2 then return end
    for n=1,4 do
        if GetMissionV(10+n)==0 and GetMissionV(20+n)==0 then
            local p=PT_POINTS[n]
            local npc=pt_spawn(PT_FIRST+n-1,p[1],p[2],"\\\\script\\\\phongthan\\\\van_tien\\\\guard"..PT_ID..".lua")
            if npc>0 then SetMissionV(20+n,npc) end
        end
    end
    if GetMissionV(11)+GetMissionV(12)+GetMissionV(13)+GetMissionV(14)==4 and GetMissionV(30)==0 then
        local p=PT_POINTS[5]
        local npc=pt_spawn(PT_BOSS,p[1],p[2],"\\\\script\\\\phongthan\\\\van_tien\\\\boss"..PT_ID..".lua")
        if npc>0 then SetMissionV(30,npc);Msg2MSAll(PT_ID,"Da ha bon ho phap. Thong Thien Giao Chu xuat hien!") end
    end
end
'''
    common=cfg+common_functions
    base=(OUT/'priority_events'/f'{missionids[i-1]}.lua').read_bytes()
    mission=common+f'''
function InitMission()
    SetMissionV(1,floor(SystemTime()/60)*4+PT_ID)
    for n=10,31 do SetMissionV(n,0) end
    SetGlobalValue(PT_ID,1)
    StartMissionTimer(PT_ID,{3000+i*2-1},18*5*60)
    AddGlobalCountNews("Van Tien Tran ("..PT_LABEL..") mo dang ky 5 phut. Tran dau 30 phut.",1)
end
function RunMission()
    if GetGlobalValue(PT_ID)~=1 then return end
    SetGlobalValue(PT_ID,2)
    pt_each(pt_fight)
    for n=1,getn(PT_MOBS) do local p=PT_MOBS[n];pt_spawn(p[1],p[2],p[3],nil) end
    pt_progress()
    StartMissionTimer(PT_ID,3009,18*30)
    Msg2MSAll(PT_ID,"Tran dau bat dau! Ha bon ho phap de goi Thong Thien Giao Chu.")
end
function EndMission()
    SetGlobalValue(PT_ID,0)
    StopMissionTimer(PT_ID,{3000+i*2-1})
    StopMissionTimer(PT_ID,{3000+i*2})
    StopMissionTimer(PT_ID,3009)
    pt_each(pt_exit)
    AddGlobalCountNews("Van Tien Tran ("..PT_LABEL..") ket thuc.",1)
end
function OnLeave(role)
    PlayerIndex=role;LockCamp(0);SetFightState(0);SetLogoutRV(0)
end
'''
    changes[f'script/phongthan/van_tien/mission{i:02d}.lua']=base+b'\n-- Project lifecycle reconstruction; original payload retained above.\n'+mission.encode('ascii')
    changes[f'script/phongthan/van_tien/open{i}.lua']=(cfg+f'''
function main()
    SubWorld=SubWorldID2Idx(PT_MAP)
    if SubWorld<0 or IsMission(PT_ID)==1 then return end
    OpenMission(PT_ID)
end
''').encode('ascii')
    changes[f'script/timertask/task{3000+i*2-1}.lua']=(cfg+f'''
function OnTimer()
    StopMissionTimer(PT_ID,{3000+i*2-1})
    if GetGlobalValue(PT_ID)~=1 then return end
    StartMissionTimer(PT_ID,{3000+i*2},18*30*60)
    RunMission(PT_ID)
end
''').encode('ascii')
    changes[f'script/timertask/task{3000+i*2}.lua']=(cfg+'''function OnTimer() CloseMission(PT_ID) end
''').encode('ascii')
    guard=common+'''
function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 then return end
    local slot=GetNpcTemplateID(npc)-PT_FIRST+1
    if slot<1 or slot>4 or GetMissionV(20+slot)~=npc or GetMissionV(10+slot)~=0 then return end
    SetMissionV(10+slot,1)
    Msg2MSAll(PT_ID,"Da ha ho phap "..slot.."/4.")
    pt_progress()
end
'''
    changes[f'script/phongthan/van_tien/guard{i}.lua']=guard.encode('ascii')
    death=(OUT/'priority_events/645AF936.lua').read_bytes()
    death+=b'\n-- Deterministic deferred reward replaces the empty original chest reward.\n'
    death+=(common+'''
GetWorldPos=GetContentWorldPos
function pt_award()
    calc_task(PT_MAP)
    if GetTask(PT_REWARD_TASK)==0 then SetTask(PT_REWARD_TASK,GetMissionV(1)) end
    Msg2Player("Da thang Van Tien Tran. Nhan thuong tai cong vao sau khi ra ngoai.")
end
function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 or GetMissionV(30)~=npc or GetMissionV(31)==1 then return end
    SetMissionV(31,1)
    pt_each(pt_award)
    Msg2MSAll(PT_ID,"Thong Thien Giao Chu da bi danh bai. Thuong duoc luu de nhan tai cong vao.")
end
''').encode('ascii')
    changes[f'script/phongthan/van_tien/boss{i}.lua']=death
    # Preserve the source portal bytes and append explicit project entry/reward callbacks.
    source=(OUT/'van_tien'/f'{portalids[i-1]}.lua').read_bytes()
    row=baseplan['ready'][i-1]
    relative=row['script_relative']
    portal=source+b'\n-- Project no-fee recovery mode; no currency or tickets consumed.\n'
    portal+=(cfg+f'''
function no() CloseDialog() end
function main()
    Say("Van Tien Tran ("..PT_LABEL.."): 02:20, 10:20, 13:20, 18:20, 21:20, 23:20. Dang ky 5 phut, chien dau 30 phut. Ban phuc hoi mien phi.",3,"Vao tran/pt_enter","Nhan thuong/pt_claim","Thoat/no")
end
function pt_enter()
    SubWorld=SubWorldID2Idx(PT_MAP)
    if SubWorld<0 then CloseDialog();return end
    local state=GetGlobalValue(PT_ID)
    if state==0 then Say("Chua den gio mo tran.",1,"Thoat/no");return end
    if GetLevel()<{30 if i==1 else 20*i+11} {('or GetLevel()>'+str(20*i+30)) if i<4 else ''} then
        Say("Cap do khong phu hop voi tran nay.",1,"Thoat/no");return
    end
    if GetTask(PT_REWARD_TASK)>0 then Say("Hay nhan thuong tran truoc truoc khi vao tran moi.",1,"Thoat/no");return end
    if state==2 and GetTask(PT_ENTRY_TASK)~=GetMissionV(1) then Say("Da het gio dang ky.",1,"Thoat/no");return end
    if NewWorld(PT_MAP,PT_ENTRY_X,PT_ENTRY_Y)~=1 then
        Say("Chua the chuyen map. Hay thu lai.",1,"Thoat/no");return
    end
    AddMSPlayer(PT_ID,1)
    SetTask(PT_ENTRY_TASK,GetMissionV(1))
    SetLogoutRV(1);LockCamp(1)
    if state==1 then SetFightState(0) else SetFightState(1) end
    CloseDialog()
end
function pt_claim()
    local reward=GetTask(PT_REWARD_TASK)
    if reward==0 then Say("Chua co thuong cho ban.",1,"Thoat/no");return end
    if GetTask(PT_CLAIM_TASK)==reward then SetTask(PT_REWARD_TASK,0);CloseDialog();return end
    if QuestExchange(PT_CLAIM_TASK,GetTask(PT_CLAIM_TASK),reward,{{}},{{{{3,PT_REWARD_DETAIL,0,0,0,0,2}}}})~=1 then
        Say("Hay de trong 2 o F4. Thuong van duoc giu, khong bi mat.",1,"Thoat/no");return
    end
    SetTask(PT_REWARD_TASK,0)
    Say("Da nhan 2 Linh Phu thuong Van Tien Tran.",1,"Thoat/no")
end
''').encode('ascii')
    changes[relative]=portal
    row['lua_sha256']=b.sha(portal)
    report.append(dict(arena=label,runtime_map=world,mission=i,boss=boss,guardians=list(range(first,first+4)),positions=points,mobs=mobs,entry=entry,portal=relative,source_mission=missionids[i-1],source_portal=portalids[i-1],original_placements=False,free_entry=True,reward='2 next-tier Linh Phu; wind returns wind Linh Phu'))

# Heartbeat uses the same real Lua spawn APIs in its current mission context.
heartbeat=common_functions+'''function OnTimer()
    local world=SubWorldIdx2ContentID(SubWorld)
    local id=world-66
    if id<1 or id>4 then return end
'''
for i,cfg in enumerate(configs,1):heartbeat+=f'    if id=={i} then\n'+cfg+'    end\n'
heartbeat+='    pt_progress()\nend\n'
changes['script/timertask/task3009.lua']=heartbeat.encode('ascii')
schedule=(server/'settings/phongthan/OriginalWorldSchedule.txt').read_bytes()
for hour in (2,10,13,18,21,23):
    for i in range(1,5):schedule+=f'{hour}\t20\t\\script\\phongthan\\van_tien\\open{i}.lua\r\n'.encode('ascii')
changes['settings/phongthan/OriginalWorldSchedule.txt']=schedule
changes['settings/phongthan/OriginalWorldSchedule.ini']=b'[Schedule]\r\nEnabled=1\r\nUtcOffsetMinutes=420\r\n'
files=[]
for relative,data in changes.items():
    if relative.startswith('script/timertask/'):
        data+=b'\n-- Native mission timers dispatch this callback, unlike player timers.\nOnMissionTimer=OnTimer\n'
    p=stage/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
    target=server/relative
    files.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None))
meta_rel='Deploy/ProjectContent/NpcRestoration/recovered_client_npcs.json';meta=json.dumps(ledger,ensure_ascii=False,indent=2).encode('utf8')
p=batch/'staged/Project'/meta_rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(meta)
plan=dict(ready=baseplan['ready'],blocked=[],files=files,metadata=[],binaries=[dict(relative='CoreServer.dll',sha256=b.sha((ROOT/'PhongThanSource/Output/Server/CoreServer.dll').read_bytes()),before_sha256=b.sha((server/'CoreServer.dll').read_bytes()))])
(batch/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
(batch/'arenas.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),'utf8')
(batch/'lifecycle_fixtures.txt').write_text(stage.relative_to(ROOT).as_posix()+'\n'+'\n'.join(a['portal'] for a in report)+'\n','ascii')
print('STAGED',len(files),'files; 12 bosses / 39 original boss slots; 4 arenas / 6 daily windows')
