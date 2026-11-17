from restore_original_world import ROOT
p=ROOT/'PhongThanSource/Tests/Native/VanTienLifecycleTests.cpp';s=p.read_text('utf8')
if 'static int EventEnter(' not in s:
    at=s.index('static int Fight(')
    s=s[:at]+'''static int EventEnter(Lua_State* L){
 int p=Player(L),id=Num(L,1),generation=value[arena][1];
 if(id!=arena || state[id]==0 || (state[id]==2 && tasks[p][4789+id]!=generation))return Ret(L,0);
 if(moveFailure)return Ret(L,0);playerMap[p]=arena;member[p]=arena;tasks[p][4789+id]=generation;++moves;return Ret(L,1);
}
'''+s[at:]
    s=s.replace('{"GetGlobalValue",GetState}', '{"EventServiceOwns",Zero},{"EventServiceValue",Zero},{"EventEnterVanTien",EventEnter},{"SetNpcName",Noop},{"SetNpcScript",Noop},{"Msg2SubWorld",Noop},{"AddGlobalCountNews",Noop},{"GetTeam",Zero},{"GetGlobalValue",GetState}',1)
    s=s.replace('!tasks[1][4933+arena]', '!tasks[1][4789+arena]')
    s=s.replace('tasks[1][4933+arena]==generation', 'tasks[1][4789+arena]==generation')
    old='CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]+1));CHECK(!tasks[1][4929+arena]);CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(tasks[1][4929+arena]==generation);CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(tasks[1][4929+arena]==generation);'
    new='CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]+1));CHECK(!tasks[1][4929+arena]);int beforeChests=npcs;CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(!tasks[1][4929+arena] && npcs==beforeChests+4);CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(npcs==beforeChests+4);'
    assert old in s;s=s.replace(old,new)
    s=s.replace('int oldAwards=awards;room=0;', 'int oldAwards=awards;CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards);tasks[1][4929+arena]=generation;tasks[1][4933+arena]=generation;room=0;')
    s=s.replace('boss_deferred_rewards_cleanup=PASS', 'boss_original_chests_legacy_pending_cleanup=PASS')
    p.write_text(s,'utf8')
print('VTT lifecycle contract updated for native entry / original chests / legacy pending only')
