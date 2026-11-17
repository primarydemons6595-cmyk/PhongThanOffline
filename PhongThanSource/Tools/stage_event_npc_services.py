"""Stage connected five-city commerce and original roulette NPC services."""
import json
from roulette_level_policy import apply_level_20
from restore_original_world import ROOT,OUT,Original
import stage_missing_npcs as staging
import build_offline_loose as b

BATCH=OUT/'event_services';STAGE=BATCH/'staged/Server'
SHOPS={1002:(1,'B97655E3'),1003:(2,'542AEE83'),1004:(3,'F7B06834'),1021:(4,'C44653BE'),1020:(5,'B794E328')}

def commerce_appendix(world,shop):
    return f'''
-- Connected five-city restoration; original payload is retained above.
-- Keep generated delivery requests within the five currently restored shops.
function pt_trade_active()
    return pt_runtime_world_pos()=={world} and GetTask(60)==1 and
        GetTask(332)>=1 and GetTask(332)<=5 and HaveIBBuff(160)==1
end
pt_original_buyover=buyover
function buyover(slot,goods,quantity)
    if not pt_trade_active() or quantity~=floor(quantity) or quantity<1 or quantity>1000 or
        slot<110 or slot>118 or mod(slot,2)~=0 or
        goods<{(shop-1)*3+1} or goods>{shop*3} then return end
    return pt_original_buyover(slot,goods,quantity)
end
pt_original_dosell=dosell
function dosell(slot,quantity)
    if not pt_trade_active() or quantity~=floor(quantity) or quantity<1 or quantity>1000 or
        slot<110 or slot>118 or mod(slot,2)~=0 then return end
    return pt_original_dosell(slot,quantity)
end
pt_original_complete_task=complete_task
function complete_task()
    if not pt_trade_active() or GetTask(333)~={shop} or GetTask(334)<1 or
        GetTask(334)>15 or GetTask(335)<1 or GetTask(335)>1000 then return end
    return pt_original_complete_task()
end
pt_original_yes_2=yes_2
function yes_2(kind)
    if pt_runtime_world_pos()~={world} or GetTask(60)~=0 or kind<1 or kind>10 or kind~=floor(kind) then return end
    if HaveIBBuff(160)==0 and GetIBBuffCount()>=16 then
        Msg2Player("Hay giam bot trang thai truoc khi thue lac da."); return
    end
    return pt_original_yes_2(kind)
end
function paoshangrenwu()
    if not pt_trade_active() or GetTask(333)~=0 or GetLevel()<25 then return end
    local count=GetTask(766)
    if count<0 or count>=20 then return end
    if count>=5 then
        local item=FindAValidIBItem(8,137,2,0)
        if item<=0 or CostIBItem(item)~=1 then return end
    end
    local available={{}}
    for good=1,15 do
        if good<{(shop-1)*3+1} or good>{shop*3} then tinsert(available,good) end
    end
    local good=available[random(1,getn(available))]
    local kind=mod(good-1,3)
    local quantity=1
    if kind==0 then quantity=20 elseif kind==1 then quantity=2 end
    SetTask(333,{shop});SetTask(334,good);SetTask(335,quantity)
    if count==0 then SetTask(769,SystemTime()) end
    MsgBox("Hay mua "..GetGoodsName(good).." ("..quantity..") tai Thuong Diem khac, roi mang ve giao cho ta.","no")
end
'''.encode('ascii')

ROULETTE_APPENDIX=b'''
-- A reward is claimed only during the server's current roulette completion.
pt_original_Finished=Finished
function Finished()
    if ConsumeRouletteResult()~=1 then return end
    return pt_original_Finished()
end
'''

def run():
    report=json.loads((OUT/'missing_npc_batch.json').read_text('utf8'))
    astro=[n for n in report['rows'] if n['world'] in (1020,1021) and n['name']=='太岁师']
    assert len(astro)==2
    for n in astro:
        n['templates']=[202]
        n['appearance_status']='PROJECT_REUSED_ORIGINAL_SAGE_RESOURCE_NOT_ORIGINAL_IDENTITY'
        n['appearance_evidence']='Authored sage appearance: original template 202, same reviewed resource as authored_profiles.json'
    (OUT/'event_service_candidates.json').write_text(json.dumps(dict(rows=astro),ensure_ascii=False,indent=2),'utf8')
    staging.BATCH=BATCH;staging.STAGE=STAGE;staging.stage('event_service_candidates.json')
    plan=json.loads((BATCH/'plan.json').read_text('utf8'));assert len(plan['ready'])==2
    changes={f['relative']:(STAGE/f['relative']).read_bytes() for f in plan['files']}
    for npc in plan['ready']:
        relative=npc['script_relative'];changes[relative]=apply_level_20(changes[relative]+ROULETTE_APPENDIX)
        npc['lua_sha256']=b.sha(changes[relative]);npc['business_status']='ORIGINAL_ROULETTE_SERVER_CLAIM_GUARD_TEST_REQUIRED'
    original=Original();server=ROOT/'PhongThanRuntime-Loose/Server';project=ROOT/'PhongThanSource'
    document='Docs/NPC_RESTORATION_DEPLOYMENT.json';deployment=json.loads((project/document).read_text('utf8'))
    proofs={n['id']:n for n in json.loads((OUT/'priority_events/original_scripts.json').read_text('utf8'))['found']}
    upgraded=[]
    for world,(shop,eid) in SHOPS.items():
        npc=next(n for n in deployment['rows'] if n['world']==world and n['name']=='Thuong Diem')
        proof=proofs[eid];raw=original.get(bytes.fromhex(proof['raw_hex']))[0]
        wrapper=raw+b'\npt_original_main=main\npt_runtime_world_pos=GetWorldPos\nGetWorldPos=GetContentWorldPos\nSubWorldIdx2ID=SubWorldIdx2ContentID\n'+f'''function main()
    if pt_runtime_world_pos()~={world} then CloseDialog();return end
    return pt_original_main()
end
'''.encode('ascii')+commerce_appendix(world,shop)
        relative=npc['lua']['relative'];changes[relative]=wrapper
        npc['lua_original']=proof;npc['lua_status']='ORIGINAL_COMMERCE_WITH_CONNECTED_DELIVERY_AND_TRANSACTION_GUARDS'
        npc['lua'].update(sha256=b.sha(wrapper),bytes=len(wrapper))
        npc['service_scope']='FIVE_CITY_15_GOODS; other original goods remain in native table but are not assigned as unreachable delivery requests'
        upgraded.append(dict(world=world,shop=shop,relative=relative,original_lua=proof))
    files=[]
    for relative,data in changes.items():
        p=STAGE/relative;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data);target=server/relative
        files.append(dict(relative=relative,sha256=b.sha(data),before_sha256=b.sha(target.read_bytes()) if target.exists() else None))
    metadata= json.dumps(deployment,ensure_ascii=False,indent=2).encode('utf8')
    p=BATCH/'staged/Project'/document;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(metadata)
    plan.update(files=files,upgraded=upgraded,metadata=[dict(relative=document,sha256=b.sha(metadata),before_sha256=b.sha((project/document).read_bytes()))])
    (BATCH/'plan.json').write_text(json.dumps(plan,ensure_ascii=False,indent=2),'utf8')
    (BATCH/'dispatch_fixtures.txt').write_text(''.join(str((STAGE/n['script_relative']).relative_to(ROOT)).replace('\\','/')+' '+str(n['world'])+'\n' for n in plan['ready'])+''.join(str((STAGE/n['relative']).relative_to(ROOT)).replace('\\','/')+' '+str(n['world'])+'\n' for n in upgraded),'ascii')
    print(json.dumps(dict(new_npcs=len(plan['ready']),upgraded=len(upgraded),files=len(files))))

if __name__=='__main__':run()
