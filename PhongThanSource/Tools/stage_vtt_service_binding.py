"""Stage VTT service settings, original boss chest path and atomic admission."""
import json,re,hashlib
from restore_original_world import ROOT,Original
OUT=ROOT/'analysis/event_service_20261019';stage=OUT/'staged';server=ROOT/'PhongThanRuntime-Loose/Server';o=Original()
plan=json.loads((OUT/'escort_publication_plan.json').read_text('utf8'))
def put(relative,data):
    p=stage/'Server'/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data);target=server/relative
    row=dict(relative='Server/'+relative,sha256=hashlib.sha256(data).hexdigest(),before_sha256=hashlib.sha256(target.read_bytes()).hexdigest() if target.exists() else None)
    plan['files']=[r for r in plan['files'] if r['relative']!=row['relative']]+[row]
portals=['1002_f57647b5','1004_281c1583','1003_479d2a84','1001_41e3ecb2']
for i,file in enumerate(portals,1):
    relative='script/phongthan/npc_recovered_batch/'+file+'.lua';data=(server/relative).read_bytes()
    appendix=f'''
-- Atomic native admission; no consumption before a successful local transfer.
function main()
    Say("Van Tien Tran: can 1 Linh Phu dung loai cho moi luot moi. Vao lai cung luot khong thu them. Lich va ve do Studio quan ly. Thuong theo NPC nhiem vu va drop goc.",3,"Vao tran/pt_enter","Thuong ton tu ban cu/pt_claim","Ket thuc doi thoai/no")
end
function pt_enter()
    SubWorld=SubWorldID2Idx(PT_MAP)
    local result=EventEnterVanTien(PT_ID,PT_ENTRY_X,PT_ENTRY_Y)
    if result~=1 then
        local message="Chua den gio dang ky, het cho hoac dieu kien cap do chua phu hop."
        if result==-1 then message="Thieu Linh Phu dung loai trong F4. Chua tru ve."
        elseif result==-2 then message="Chua the xu ly hanh trang. Hay ket thuc giao dich va thu lai." end
        Say(message,1,"Ket thuc doi thoai/no");return
    end
    SetLogoutRV(1);LockCamp(1)
    local phase=GetGlobalValue(PT_ID)
    if phase==1 then SetFightState(0);ShowMissionCountdown(PT_ID,{2999+i*2})
    else SetFightState(1);ShowMissionCountdown(PT_ID,{3000+i*2}) end
    CloseDialog()
end
pt_old_pending_claim=pt_claim
function pt_claim()
    local reward=GetTask(PT_REWARD_TASK)
    local now=floor(SystemTime()/60)*4
    -- Legacy authored pending payments must match the old admission generation.
    -- Never interpret arbitrary IBBuff values as a reward or clear those slots.
    if reward<now-172800 or reward>now+4 or GetTask({4933+i})~=reward then
        Say("Thuong tran hien tai theo nhiem vu goc tai Thien Hung va vat pham roi. Khong phat them 2 Linh Phu.",1,"Ket thuc doi thoai/no");return
    end
    return pt_old_pending_claim()
end
'''.encode('ascii')
    put(relative,data+appendix)
    mission=f'script/phongthan/van_tien/mission{i:02d}.lua';data=(server/mission).read_bytes()
    pattern=f'StartMissionTimer(PT_ID,{2999+i*2},18*5*60)'.encode();assert pattern in data
    replacement=f'local seconds=300\n    if EventServiceOwns(PT_ID)==1 then seconds=EventServiceValue(PT_ID,1) end\n    StartMissionTimer(PT_ID,{2999+i*2},18*seconds)'.encode()
    data=data.replace(pattern,replacement).replace(b'mo dang ky 5 phut. Tran dau 30 phut.',b'mo dang ky. Thoi luong theo cau hinh Event Phong Than.')
    put(mission,data)
    timer=f'script/timertask/task{2999+i*2}.lua';data=(server/timer).read_bytes();pattern=f'StartMissionTimer(PT_ID,{3000+i*2},18*30*60)'.encode();assert pattern in data
    data=data.replace(pattern,f'local seconds=1800\n    if EventServiceOwns(PT_ID)==1 then seconds=EventServiceValue(PT_ID,2) end\n    StartMissionTimer(PT_ID,{3000+i*2},18*seconds)'.encode());put(timer,data)
    boss=f'script/phongthan/van_tien/boss{i}.lua';data=(server/boss).read_bytes();prefix=o.get('script/万仙阵/通天教主.lua'.encode('gbk'))[0];assert data.startswith(prefix)
    data=data[:len(prefix)]+b'\npt_original_boss_death=OnDeath\n'+data[len(prefix):]
    data+=b'''
-- Restore original boss quest credit and four original native-drop chests.
pt_boss_native_add=AddNpc
function AddNpc(tid,level,world,x,y,barrier)
    local n=0
    if barrier==nil then n=pt_boss_native_add(tid,level,world,x,y)
    else n=pt_boss_native_add(tid,level,world,x,y,barrier) end
    if n>0 and (tid==410 or tid==413 or tid==414 or tid==415) then
        if AddMSNpc(PT_ID,n,0)==0 then DelNpc(n);return 0 end
        SetNpcDeathScript(n,"\\\\script\\\\phongthan\\\\van_tien\\\\chest.lua")
    end
    return n
end
function OnDeath(npc)
    if GetGlobalValue(PT_ID)~=2 or GetMissionV(30)~=npc or GetMissionV(31)==1 then return end
    SetMissionV(31,1)
    pt_original_boss_death()
    Msg2MSAll(PT_ID,"Thong Thien Giao Chu da bi danh bai. Bao ruong goc da xuat hien; thuong nhiem vu tra tai Thien Hung.")
end
'''
    put(boss,data)
put('script/phongthan/van_tien/chest.lua',o.get('script/万仙阵/宝箱.lua'.encode('gbk'))[0])
relative='script/servertimer.lua';data=(server/relative).read_bytes();data=data.replace(b'if diff==10 or diff==5 or diff==1 then',b'if EventServiceOwns(1)==0 and (diff==10 or diff==5 or diff==1) then')
data=data.replace(b'if mod(minute-PT_NEWS_VTT[i]+1440,1440)==5 then',b'if EventServiceOwns(1)==0 and mod(minute-PT_NEWS_VTT[i]+1440,1440)==5 then')
data=data.replace(b'if (diff==5 or diff==1) and GetGlobalValue(row[3])==-1 then',b'if EventServiceOwns(1000+row[3])==0 and (diff==5 or diff==1) and GetGlobalValue(row[3])==-1 then');put(relative,data)
(OUT/'escort_publication_plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8');print('VTT service binding staged; files',len(plan['files']))
