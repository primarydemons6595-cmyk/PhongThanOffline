"""Restore original preparation-phase population and open one normal recovery round."""
import ctypes,json,re,shutil,time
from restore_original_world import ROOT
import build_offline_loose as b
folder=ROOT/'analysis/vtt_client_services_20261019';server=ROOT/'PhongThanRuntime-Loose/Server'
if (folder/'activation.json').exists():raise ValueError('Recovery already activated')
k=ctypes.windll.kernel32;k.CreateFileW.argtypes=[ctypes.c_wchar_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p,ctypes.c_uint32,ctypes.c_uint32,ctypes.c_void_p];k.CreateFileW.restype=ctypes.c_void_p;k.CloseHandle.argtypes=[ctypes.c_void_p]
h=k.CreateFileW(str(server/'CoreServer.dll'),0x40000000,0,None,3,0,None)
if h in (None,ctypes.c_void_p(-1).value):raise ValueError('Main server must be stopped')
k.CloseHandle(h)
changes={};backup=folder/'backup_activation'
helper=b'''
-- Original InitMission revived the arena population during preparation.
-- Track each owned normal monster to avoid duplicate spawns on RunMission.
function pt_spawn_mobs()
    for n=1,getn(PT_MOBS) do
        if GetMissionV(39+n)==0 then
            local p=PT_MOBS[n]
            local npc=pt_spawn(p[1],p[2],p[3],"\\\\script\\\\phongthan\\\\van_tien\\\\mob"..p[1]..".lua")
            if npc>0 then SetMissionV(39+n,npc) end
        end
    end
end
'''
for ident in range(1,5):
 rel=f'script/phongthan/van_tien/mission{ident:02d}.lua';saved=backup/rel;data=(saved if saved.exists() else server/rel).read_bytes()
 assert b'function pt_spawn_mobs()' not in data
 data=data.replace(b'function pt_each(fn)',helper+b'\nfunction pt_each(fn)',1)
 data=data.replace(b'for n=10,31 do SetMissionV(n,0) end',b'for n=10,64 do SetMissionV(n,0) end')
 needle=b'    SetGlobalValue(PT_ID,1)\n';assert data.count(needle)==1
 data=data.replace(needle,needle+b'    pt_spawn_mobs()\n',1)
 data,count=re.subn(rb'    for n=1,getn\(PT_MOBS\) do local p=PT_MOBS\[n\];pt_spawn\(p\[1\],p\[2\],p\[3\],"[^\r\n]+"\) end',b'    pt_spawn_mobs()',data)
 assert count==1
 changes[rel]=data
rel='script/servertimer.lua';saved=backup/rel;normal=(saved if saved.exists() else server/rel).read_bytes();expires=int(time.time())+3600
changes[rel]=normal+f'''
-- One recovery round, ordinary five-minute registration; expires UTC {expires}.
pt_vtt_services_previous=main
pt_vtt_services_done=0
function main()
    pt_vtt_services_previous()
    if pt_vtt_services_done==1 or SystemTime()>{expires} then return end
    pt_vtt_services_done=1
    local old=SubWorld
    for id=1,4 do
        SubWorld=SubWorldID2Idx(66+id)
        if SubWorld>=0 and IsMission(id)==0 then OpenMission(id) end
    end
    SubWorld=old
end
'''.encode('ascii')
backup.mkdir(exist_ok=True)
manifest=ROOT/'PhongThanRuntime-Loose/completion/Server_named_effective_manifest.csv'
if not (backup/manifest.name).exists():shutil.copy2(manifest,backup/manifest.name)
rows=list(b.csv_read(manifest));byid={}
for row in rows:byid.setdefault(row['id'],[]).append(row)
for rel,data in changes.items():
 target=server/rel;saved=backup/rel;saved.parent.mkdir(parents=True,exist_ok=True)
 if not saved.exists():shutil.copy2(target,saved)
 target.write_bytes(data)
 compiled=ROOT/'PhongThanSource/Deploy/ProjectContent/NpcRestoration/compiled'/rel;compiled.write_bytes(data)
 matches=[row for row in rows if row['path'].lower()==rel.replace('/','\\').lower()]
 assert matches,rel
 for row in matches:row.update(sha256=b.sha(data),bytes=str(len(data)),origin='VTT_PREPARATION_POPULATION_REPAIR',evidence='Original InitMission revived monsters during registration; tracked slots prevent duplicate normal spawns')
b.csv_write(manifest,rows,['path','id','status','origin','pak','entry','sha256','bytes','evidence','error'])
# No game client process may be overwritten while running; a file lock fails safely.
target=ROOT/'PhongThanRuntime-Loose/Client/Game.exe';source=ROOT/'PhongThanSource/Output/Client/Game.exe'
shutil.copy2(target,backup/'Game.exe');before=b.sha(target.read_bytes());shutil.copy2(source,target)
result=dict(applied=True,normal_mobs_in_registration=True,tracked_mob_slots=[40,63],client_game_before=before,client_game_after=b.sha(target.read_bytes()),server_sha256=b.sha((server/'CoreServer.dll').read_bytes()),recovery_expires_utc=expires,files={rel:b.sha(data) for rel,data in changes.items()})
(folder/'activation.json').write_text(json.dumps(result,indent=2),'utf8');print(json.dumps(result))
